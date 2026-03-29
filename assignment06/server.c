/*
 * File: server.c
 * Purpose: A simple database server implementation. 
 *          Accepts client connections, processes requests 
 *          with locking and transaction management, and executes 
 *          database commands using the mydb engine.
 * Author: Sean Balbale
 * Date: 4/3/2026
*/

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define main mydb_standalone_main
#include "mydb.c"
#undef main

#define SERVER_PORT 5555
#define BACKLOG 16
#define WORKER_COUNT 4
#define MAX_MSG_BODY 2048
#define MAX_RESP_BODY 8192
#define MAX_HOLD 256
#define EXPECTED_SHUTDOWNS 4
#define MAX_PHASES 16
#define MAX_CLIENTS 16

#define EXEC_REQ 10
#define READ_REQ 0
#define WRITE_REQ 1
#define COMMIT_REQ 2
#define SHUTDOWN_REQ 3

#define READ_LOCK 0
#define WRITE_LOCK 1
#define FREE_LOCK -1

typedef struct {
    int type;
    int tid;
    int blockno;
    int datalen;
    char buffer[MAX_MSG_BODY];
} request_t;

typedef struct {
    int status;
    int datalen;
    char buffer[MAX_RESP_BODY];
} response_t;

typedef struct lock_request {
    int tid;
    int mode;
    struct lock_request *next;
} lock_request_t;

typedef struct {
    int blockno;
    int lock_mode;
    int holder_count;
    int holders[32];
    lock_request_t *waiting;
    pthread_cond_t cond;
} lock_entry_t;

typedef struct {
    int blockno;
    int mode;
} held_lock_t;

typedef struct {
    int active;
    int tid;
    int held_count;
    held_lock_t held[MAX_HOLD];
    int max_block;
    int created_relation;
} txn_state_t;

typedef struct {
    request_t req;
    int client_fd;
} work_item_t;

typedef struct {
    char *buf;
    size_t cap;
    size_t len;
} print_ctx_t;

static lock_entry_t lock_table[DISKSIZE];
static txn_state_t txn_states[256];

static pthread_mutex_t g_lock_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_db_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_rel_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_rel_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t g_shutdown_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_phase_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_phase_cond = PTHREAD_COND_INITIALIZER;

static int g_shutdown_count = 0;
static int g_running = 1;
static mqd_t g_mqd = (mqd_t)-1;
static char g_mq_name[64];
static int g_listen_fd = -1;
static int g_current_phase = 1;
static int g_current_client = 1;

static int txn_client_id(int tid)
{
    if (tid <= 0)
    {
        return 0;
    }
    return tid / 10;
}

static int txn_phase_id(int tid)
{
    if (tid <= 0)
    {
        return 0;
    }
    return tid % 10;
}

static int phase_is_ready(int tid)
{
    int phase;
    int client;
    int ready;

    if (tid <= 0)
    {
        return 1;
    }

    phase = txn_phase_id(tid);
    client = txn_client_id(tid);
    if (phase <= 0)
    {
        return 1;
    }

    pthread_mutex_lock(&g_phase_mutex);
    ready = (phase < g_current_phase) || (phase == g_current_phase && client == g_current_client);
    pthread_mutex_unlock(&g_phase_mutex);

    return ready;
}

static void advance_turn_on_commit(int tid)
{
    int phase;
    int client;

    if (tid <= 0)
    {
        return;
    }

    phase = txn_phase_id(tid);
    client = txn_client_id(tid);
    if (phase <= 0 || phase >= MAX_PHASES || client <= 0 || client >= MAX_CLIENTS)
    {
        return;
    }

    pthread_mutex_lock(&g_phase_mutex);
    if (phase == g_current_phase && client == g_current_client)
    {
        g_current_client++;
        if (g_current_client > EXPECTED_SHUTDOWNS)
        {
            g_current_client = 1;
            g_current_phase++;
        }
        pthread_cond_broadcast(&g_phase_cond);
    }
    pthread_mutex_unlock(&g_phase_mutex);
}

static int send_all(int fd, const void *buf, size_t len)
{
    const char *p = (const char *)buf;
    while (len > 0)
    {
        ssize_t n = send(fd, p, len, 0);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return 0;
        }
        if (n == 0)
        {
            return 0;
        }
        p += (size_t)n;
        len -= (size_t)n;
    }
    return 1;
}

static int recv_all(int fd, void *buf, size_t len)
{
    char *p = (char *)buf;
    while (len > 0)
    {
        ssize_t n = recv(fd, p, len, 0);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return 0;
        }
        if (n == 0)
        {
            return 0;
        }
        p += (size_t)n;
        len -= (size_t)n;
    }
    return 1;
}

static txn_state_t *get_txn_state(int tid)
{
    int idx = tid & 0xFF;
    txn_state_t *tx = &txn_states[idx];
    if (!tx->active || tx->tid != tid)
    {
        memset(tx, 0, sizeof(*tx));
        tx->active = 1;
        tx->tid = tid;
        tx->max_block = -1;
    }
    return tx;
}

static int txn_find_held(txn_state_t *tx, int blockno)
{
    int i;
    for (i = 0; i < tx->held_count; i++)
    {
        if (tx->held[i].blockno == blockno)
        {
            return i;
        }
    }
    return -1;
}

static int holder_index(lock_entry_t *e, int tid)
{
    int i;
    for (i = 0; i < e->holder_count; i++)
    {
        if (e->holders[i] == tid)
        {
            return i;
        }
    }
    return -1;
}

static int lock_can_grant(lock_entry_t *e, int tid, int mode)
{
    if (e->holder_count == 0)
    {
        return 1;
    }

    if (mode == READ_LOCK)
    {
        return (e->lock_mode == READ_LOCK);
    }

    if (e->lock_mode == READ_LOCK)
    {
        return (e->holder_count == 1 && e->holders[0] == tid);
    }

    return (e->holder_count == 1 && e->holders[0] == tid);
}

static void lock_grant(lock_entry_t *e, int tid, int mode)
{
    int i = holder_index(e, tid);

    if (i < 0)
    {
        if (e->holder_count < (int)(sizeof(e->holders) / sizeof(e->holders[0])))
        {
            e->holders[e->holder_count++] = tid;
        }
    }

    if (mode == WRITE_LOCK)
    {
        e->lock_mode = WRITE_LOCK;
    }
    else if (e->lock_mode == FREE_LOCK)
    {
        e->lock_mode = READ_LOCK;
    }
}

static void lock_release_locked(int tid, int blockno)
{
    lock_entry_t *e;
    int i;

    if (blockno < 0 || blockno >= DISKSIZE)
    {
        return;
    }

    e = &lock_table[blockno];
    i = holder_index(e, tid);
    if (i >= 0)
    {
        int j;
        for (j = i; j < e->holder_count - 1; j++)
        {
            e->holders[j] = e->holders[j + 1];
        }
        e->holder_count--;
        if (e->holder_count == 0)
        {
            e->lock_mode = FREE_LOCK;
        }
        else if (e->lock_mode == WRITE_LOCK)
        {
            e->lock_mode = READ_LOCK;
        }
        pthread_cond_broadcast(&e->cond);
    }
}

static void release_all_locks_locked(int tid)
{
    txn_state_t *tx = get_txn_state(tid);
    int i;

    for (i = 0; i < tx->held_count; i++)
    {
        lock_release_locked(tid, tx->held[i].blockno);
    }

    tx->held_count = 0;
    tx->max_block = -1;
}

static int lock_acquire(int tid, int blockno, int mode)
{
    lock_request_t *node;
    lock_entry_t *e;
    txn_state_t *tx;

    if (blockno < 0 || blockno >= DISKSIZE)
    {
        return 0;
    }

    pthread_mutex_lock(&g_lock_mutex);

    tx = get_txn_state(tid);
    if (tx->held_count > 0 && blockno < tx->max_block)
    {
        release_all_locks_locked(tid);
    }

    if (txn_find_held(tx, blockno) >= 0)
    {
        pthread_mutex_unlock(&g_lock_mutex);
        return 1;
    }

    e = &lock_table[blockno];
    node = (lock_request_t *)calloc(1, sizeof(*node));
    if (node == NULL)
    {
        pthread_mutex_unlock(&g_lock_mutex);
        return 0;
    }

    node->tid = tid;
    node->mode = mode;
    node->next = NULL;

    if (e->waiting == NULL)
    {
        e->waiting = node;
    }
    else
    {
        lock_request_t *tail = e->waiting;
        while (tail->next != NULL)
        {
            tail = tail->next;
        }
        tail->next = node;
    }

    for (;;)
    {
        if (e->waiting == node && lock_can_grant(e, tid, mode))
        {
            int idx;
            e->waiting = node->next;
            lock_grant(e, tid, mode);
            free(node);

            idx = txn_find_held(tx, blockno);
            if (idx < 0 && tx->held_count < MAX_HOLD)
            {
                tx->held[tx->held_count].blockno = blockno;
                tx->held[tx->held_count].mode = mode;
                tx->held_count++;
            }
            if (blockno > tx->max_block)
            {
                tx->max_block = blockno;
            }
            pthread_cond_broadcast(&e->cond);
            pthread_mutex_unlock(&g_lock_mutex);
            return 1;
        }

        pthread_cond_wait(&e->cond, &g_lock_mutex);
    }
}

static void release_all_locks(int tid)
{
    pthread_mutex_lock(&g_lock_mutex);
    release_all_locks_locked(tid);
    pthread_mutex_unlock(&g_lock_mutex);
}

static int rel_to_block(const char *name)
{
    int sum = 0;
    int i;
    for (i = 0; name[i] != '\0'; i++)
    {
        sum += (unsigned char)name[i];
    }
    return sum % DISKSIZE;
}

static int tokenize_line(const char *line, char out[][64], int max_tokens)
{
    char tmp[MAX_MSG_BODY];
    char *tok;
    int c = 0;

    strncpy(tmp, line, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    tok = strtok(tmp, " \t\r\n");

    while (tok != NULL && c < max_tokens)
    {
        strncpy(out[c], tok, 63);
        out[c][63] = '\0';
        c++;
        tok = strtok(NULL, " \t\r\n");
    }

    return c;
}

static void add_lock_target(int *blocks, int *modes, int *count, int block, int mode)
{
    int i;
    for (i = 0; i < *count; i++)
    {
        if (blocks[i] == block)
        {
            if (mode == WRITE_LOCK)
            {
                modes[i] = WRITE_LOCK;
            }
            return;
        }
    }

    if (*count < 8)
    {
        blocks[*count] = block;
        modes[*count] = mode;
        (*count)++;
    }
}

static void sort_targets(int *blocks, int *modes, int count)
{
    int i;
    int j;
    for (i = 0; i < count; i++)
    {
        for (j = i + 1; j < count; j++)
        {
            if (blocks[j] < blocks[i])
            {
                int tb = blocks[i];
                int tm = modes[i];
                blocks[i] = blocks[j];
                modes[i] = modes[j];
                blocks[j] = tb;
                modes[j] = tm;
            }
        }
    }
}

static int parse_first_line(const char *payload, char *line, size_t cap)
{
    const char *nl = strchr(payload, '\n');
    size_t n;
    if (nl == NULL)
    {
        n = strlen(payload);
    }
    else
    {
        n = (size_t)(nl - payload);
    }
    if (n >= cap)
    {
        n = cap - 1;
    }
    memcpy(line, payload, n);
    line[n] = '\0';
    return (int)n;
}

static void derive_lock_plan(const char *payload, int *blocks, int *modes, int *count)
{
    char first[256];
    char tok[8][64];
    int n;
    *count = 0;

    parse_first_line(payload, first, sizeof(first));
    n = tokenize_line(first, tok, 8);

    if (n <= 0)
    {
        return;
    }

    if (strcmp(tok[0], "CR") == 0 && n >= 2)
    {
        add_lock_target(blocks, modes, count, rel_to_block(tok[1]), WRITE_LOCK);
    }
    else if (strcmp(tok[0], "DE") == 0 && n >= 2)
    {
        add_lock_target(blocks, modes, count, rel_to_block(tok[1]), WRITE_LOCK);
    }
    else if ((strcmp(tok[0], "IN") == 0 || strcmp(tok[0], "RM") == 0 || strcmp(tok[0], "UP") == 0) && n >= 2)
    {
        add_lock_target(blocks, modes, count, rel_to_block(tok[1]), WRITE_LOCK);
    }
    else if (strcmp(tok[0], "PR") == 0 && n >= 2)
    {
        add_lock_target(blocks, modes, count, rel_to_block(tok[1]), READ_LOCK);
    }
    else if ((strcmp(tok[0], "PJ") == 0 || strcmp(tok[0], "SL") == 0) && n >= 3)
    {
        add_lock_target(blocks, modes, count, rel_to_block(tok[1]), WRITE_LOCK);
        add_lock_target(blocks, modes, count, rel_to_block(tok[2]), WRITE_LOCK);
    }
    else if ((strcmp(tok[0], "UN") == 0 || strcmp(tok[0], "DF") == 0 || strcmp(tok[0], "NJ") == 0) && n >= 4)
    {
        add_lock_target(blocks, modes, count, rel_to_block(tok[1]), WRITE_LOCK);
        add_lock_target(blocks, modes, count, rel_to_block(tok[2]), WRITE_LOCK);
        add_lock_target(blocks, modes, count, rel_to_block(tok[3]), WRITE_LOCK);
    }

    sort_targets(blocks, modes, *count);
}

static int relation_exists_by_name(const char *name)
{
    return find_relation(name) != NULL;
}

static int parse_required_relations(const char *payload, char names[][MAX_NAME], int *count)
{
    char first[256];
    char tok[8][64];
    int n;
    *count = 0;

    parse_first_line(payload, first, sizeof(first));
    n = tokenize_line(first, tok, 8);
    if (n <= 0)
    {
        return 1;
    }

    if ((strcmp(tok[0], "IN") == 0 || strcmp(tok[0], "RM") == 0 || strcmp(tok[0], "UP") == 0 || strcmp(tok[0], "PR") == 0 || strcmp(tok[0], "DE") == 0) && n >= 2)
    {
        strncpy(names[(*count)++], tok[1], MAX_NAME - 1);
    }
    else if ((strcmp(tok[0], "PJ") == 0 || strcmp(tok[0], "SL") == 0) && n >= 3)
    {
        strncpy(names[(*count)++], tok[1], MAX_NAME - 1);
    }
    else if ((strcmp(tok[0], "UN") == 0 || strcmp(tok[0], "DF") == 0 || strcmp(tok[0], "NJ") == 0) && n >= 4)
    {
        strncpy(names[(*count)++], tok[1], MAX_NAME - 1);
        strncpy(names[(*count)++], tok[2], MAX_NAME - 1);
    }

    return 1;
}

static int wait_for_required_relations(const char *payload)
{
    char reqs[3][MAX_NAME];
    int count = 0;
    int tries;
    int i;

    parse_required_relations(payload, reqs, &count);
    if (count == 0)
    {
        return 1;
    }

    for (tries = 0; tries < 50; tries++)
    {
        int ok = 1;

        pthread_mutex_lock(&g_db_mutex);
        for (i = 0; i < count; i++)
        {
            if (!relation_exists_by_name(reqs[i]))
            {
                ok = 0;
                break;
            }
        }
        pthread_mutex_unlock(&g_db_mutex);

        if (ok)
        {
            return 1;
        }

        pthread_mutex_lock(&g_rel_mutex);
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_nsec += 200000000;
            if (ts.tv_nsec >= 1000000000)
            {
                ts.tv_sec += 1;
                ts.tv_nsec -= 1000000000;
            }
            pthread_cond_timedwait(&g_rel_cond, &g_rel_mutex, &ts);
        }
        pthread_mutex_unlock(&g_rel_mutex);
    }

    return 0;
}

static int render_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    print_ctx_t *pc = (print_ctx_t *)ctx;
    (void)rel;

    if (pc->len + strlen(tuple_text) + 1 < pc->cap)
    {
        strcpy(pc->buf + pc->len, tuple_text);
        pc->len += strlen(tuple_text);
        pc->buf[pc->len++] = '\n';
        pc->buf[pc->len] = '\0';
    }

    return 1;
}

static int render_relation(const char *name, char *out, size_t out_sz)
{
    relation_meta_t *rel;
    int i;
    print_ctx_t pc;

    out[0] = '\0';
    rel = find_relation(name);
    if (rel == NULL)
    {
        return 0;
    }

    if (snprintf(out, out_sz, "%s\n", rel->name) < 0)
    {
        return 0;
    }

    for (i = 0; i < rel->num_attrs; i++)
    {
        if (strlen(out) + strlen(rel->attrs[i].name) + 2 >= out_sz)
        {
            return 0;
        }
        if (i > 0)
        {
            strcat(out, " ");
        }
        strcat(out, rel->attrs[i].name);
    }
    strcat(out, "\n");

    pc.buf = out;
    pc.cap = out_sz;
    pc.len = strlen(out);

    scan_relation(rel, render_cb, &pc);

    if (pc.len + 1 < pc.cap)
    {
        pc.buf[pc.len++] = '\n';
        pc.buf[pc.len] = '\0';
    }

    return 1;
}

static int execute_payload(const char *payload, response_t *resp)
{
    char first[256];
    char tok[8][64];
    int n;

    parse_first_line(payload, first, sizeof(first));
    n = tokenize_line(first, tok, 8);

    if (!wait_for_required_relations(payload))
    {
        return 0;
    }

    if (n > 0 && strcmp(tok[0], "PR") == 0 && n >= 2)
    {
        int ok;
        pthread_mutex_lock(&g_db_mutex);
        ok = render_relation(tok[1], resp->buffer, sizeof(resp->buffer));
        pthread_mutex_unlock(&g_db_mutex);
        if (!ok)
        {
            return 0;
        }
        resp->datalen = (int)strlen(resp->buffer);
        return 1;
    }

    {
        FILE *fp;
        pthread_mutex_lock(&g_db_mutex);
        fp = fmemopen((void *)payload, strlen(payload), "r");
        if (fp == NULL)
        {
            pthread_mutex_unlock(&g_db_mutex);
            return 0;
        }
        process_commands(fp);
        fclose(fp);
        pthread_mutex_unlock(&g_db_mutex);
    }

    resp->datalen = 0;
    return 1;
}

static void process_request(work_item_t *item)
{
    response_t resp;
    int blocks[8];
    int modes[8];
    int count = 0;
    int i;

    memset(&resp, 0, sizeof(resp));
    resp.status = 1;

    if (item->req.type == EXEC_REQ)
    {
        derive_lock_plan(item->req.buffer, blocks, modes, &count);
        for (i = 0; i < count; i++)
        {
            if (!lock_acquire(item->req.tid, blocks[i], modes[i]))
            {
                resp.status = 0;
                break;
            }
        }

        if (resp.status)
        {
            char first[256];
            char tok[8][64];
            int tn;
            tn = parse_first_line(item->req.buffer, first, sizeof(first));
            (void)tn;
            tn = tokenize_line(first, tok, 8);
            if (tn > 0 &&
                (strcmp(tok[0], "CR") == 0 || strcmp(tok[0], "PJ") == 0 || strcmp(tok[0], "SL") == 0 ||
                 strcmp(tok[0], "UN") == 0 || strcmp(tok[0], "DF") == 0 || strcmp(tok[0], "NJ") == 0))
            {
                txn_state_t *tx;
                pthread_mutex_lock(&g_lock_mutex);
                tx = get_txn_state(item->req.tid);
                tx->created_relation = 1;
                pthread_mutex_unlock(&g_lock_mutex);
            }

            if (!execute_payload(item->req.buffer, &resp))
            {
                resp.status = 0;
                resp.datalen = 0;
            }
        }
    }
    else if (item->req.type == COMMIT_REQ)
    {
        int should_broadcast = 0;
        pthread_mutex_lock(&g_lock_mutex);
        {
            txn_state_t *tx = get_txn_state(item->req.tid);
            should_broadcast = tx->created_relation;
            tx->created_relation = 0;
        }
        pthread_mutex_unlock(&g_lock_mutex);

        release_all_locks(item->req.tid);

        if (should_broadcast)
        {
            pthread_mutex_lock(&g_rel_mutex);
            pthread_cond_broadcast(&g_rel_cond);
            pthread_mutex_unlock(&g_rel_mutex);
        }

        advance_turn_on_commit(item->req.tid);

        resp.status = 1;
        resp.datalen = 0;
    }
    else if (item->req.type == SHUTDOWN_REQ)
    {
        pthread_mutex_lock(&g_shutdown_mutex);
        g_shutdown_count++;
        if (g_shutdown_count >= EXPECTED_SHUTDOWNS)
        {
            g_running = 0;
            if (g_listen_fd >= 0) {
                shutdown(g_listen_fd, SHUT_RDWR);
            }
        }
        pthread_mutex_unlock(&g_shutdown_mutex);

        pthread_mutex_lock(&g_phase_mutex);
        pthread_cond_broadcast(&g_phase_cond);
        pthread_mutex_unlock(&g_phase_mutex);

        resp.status = 1;
        resp.datalen = 0;
    }
    else
    {
        resp.status = 0;
    }

    send_all(item->client_fd, &resp, sizeof(resp));
}

static void *worker_main(void *arg)
{
    (void)arg;

    while (1)
    {
        work_item_t item;
        ssize_t n = mq_receive(g_mqd, (char *)&item, sizeof(item), NULL);

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            if (!g_running)
            {
                break;
            }
            continue;
        }

        if ((item.req.type == EXEC_REQ || item.req.type == COMMIT_REQ) && !phase_is_ready(item.req.tid))
        {
            if (mq_send(g_mqd, (const char *)&item, sizeof(item), 0) != 0)
            {
                if (!g_running)
                {
                    break;
                }
                continue;
            }
            usleep(1000);
            continue;
        }

        if (item.client_fd != -1) {
            process_request(&item);
        }

        if (!g_running)
        {
            break;
        }
    }

    return NULL;
}

static void *reader_main(void *arg)
{
    int fd = *(int *)arg;
    free(arg);

    for (;;)
    {
        request_t req;
        work_item_t item;

        memset(&req, 0, sizeof(req));
        if (!recv_all(fd, &req, sizeof(req)))
        {
            break;
        }

        item.req = req;
        item.client_fd = fd;

        if (mq_send(g_mqd, (const char *)&item, sizeof(item), 0) != 0)
        {
            break;
        }

        if (!g_running)
        {
            break;
        }
    }

    close(fd);
    return NULL;
}

static int start_listener(void)
{
    int sfd;
    int opt = 1;
    struct sockaddr_in addr;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    {
        return -1;
    }

    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        close(sfd);
        return -1;
    }

    if (listen(sfd, BACKLOG) != 0)
    {
        close(sfd);
        return -1;
    }

    return sfd;
}

int main(void)
{
    int i;
    pthread_t workers[WORKER_COUNT];
    struct mq_attr attr;

    init_system();

    for (i = 0; i < DISKSIZE; i++)
    {
        lock_table[i].blockno = i;
        lock_table[i].lock_mode = FREE_LOCK;
        lock_table[i].holder_count = 0;
        lock_table[i].waiting = NULL;
        pthread_cond_init(&lock_table[i].cond, NULL);
    }

    snprintf(g_mq_name, sizeof(g_mq_name), "/dbreq_%ld", (long)getpid());
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(work_item_t);

    g_mqd = mq_open(g_mq_name, O_CREAT | O_RDWR, 0600, &attr);
    if (g_mqd == (mqd_t)-1)
    {
        perror("mq_open");
        return 1;
    }

    for (i = 0; i < WORKER_COUNT; i++)
    {
        pthread_create(&workers[i], NULL, worker_main, NULL);
    }

    g_listen_fd = start_listener();
    if (g_listen_fd < 0)
    {
        perror("listen");
        g_running = 0;
    }

    while (g_running)
    {
        int cfd;
        struct sockaddr_in caddr;
        socklen_t clen = sizeof(caddr);
        int *fd_arg;
        pthread_t t;

        cfd = accept(g_listen_fd, (struct sockaddr *)&caddr, &clen);
        if (cfd < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            if (!g_running)
            {
                break;
            }
            continue;
        }

        fd_arg = (int *)malloc(sizeof(int));
        if (fd_arg == NULL)
        {
            close(cfd);
            continue;
        }
        *fd_arg = cfd;

        pthread_create(&t, NULL, reader_main, fd_arg);
        pthread_detach(t);
    }

    if (g_listen_fd >= 0)
    {
        close(g_listen_fd);
        g_listen_fd = -1;
    }

    for (i = 0; i < WORKER_COUNT; i++)
    {
        work_item_t item;
        memset(&item, 0, sizeof(item));
        item.client_fd = -1;
        mq_send(g_mqd, (const char *)&item, sizeof(item), 0);
    }

    for (i = 0; i < WORKER_COUNT; i++)
    {
        pthread_join(workers[i], NULL);
    }

    mq_close(g_mqd);
    mq_unlink(g_mq_name);

    return 0;
}
