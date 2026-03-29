/*
 * File: client.c
 * Purpose: A simple database client implementation. 
 *          Connects to the database server, sends requests, 
 *          and receives responses.
 * Author: Sean Balbale
 * Date: 4/3/2026
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 5555
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_BODY 2048
#define MAX_RESP_BODY 8192
#define LINE_BUF 512

#define EXEC_REQ 10
#define READ_REQ 0
#define WRITE_REQ 1
#define COMMIT_REQ 2
#define SHUTDOWN_REQ 3

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

static void trim_newline(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
    {
        s[n - 1] = '\0';
        n--;
    }
}

static int parse_extra_count(const char *cmd_line)
{
    char cmd[8];
    char a[64];
    char b[64];
    char c[64];
    int n;

    cmd[0] = '\0';
    n = sscanf(cmd_line, "%7s %63s %63s %63s", cmd, a, b, c);
    if (n <= 0)
    {
        return 0;
    }

    if (strcmp(cmd, "CR") == 0)
    {
        int attrs = 0;
        if (sscanf(cmd_line, "CR %63s %d", a, &attrs) == 2)
        {
            return attrs;
        }
    }
    else if (strcmp(cmd, "IN") == 0 || strcmp(cmd, "RM") == 0 || strcmp(cmd, "UP") == 0)
    {
        int rows = 0;
        if (sscanf(cmd_line, "%*s %63s %d", a, &rows) == 2)
        {
            return rows;
        }
    }
    else if (strcmp(cmd, "PJ") == 0 || strcmp(cmd, "SL") == 0)
    {
        int rows = 0;
        if (sscanf(cmd_line, "%*s %63s %63s %d", a, b, &rows) == 3)
        {
            return rows;
        }
    }
    else if (strcmp(cmd, "NJ") == 0)
    {
        int rows = 0;
        if (sscanf(cmd_line, "NJ %63s %63s %63s %d", a, b, c, &rows) == 4)
        {
            return rows;
        }
    }

    return 0;
}

static int extract_tid_client(const char *tidtok)
{
    if (tidtok[0] != 'T')
    {
        return -1;
    }
    if (tidtok[1] < '0' || tidtok[1] > '9')
    {
        return -1;
    }
    return tidtok[1] - '0';
}

static int connect_server(void)
{
    int fd;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &addr.sin_addr) <= 0)
    {
        close(fd);
        return -1;
    }

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        close(fd);
        return -1;
    }

    return fd;
}

static int rpc_call(int fd, request_t *req, response_t *resp)
{
    if (!send_all(fd, req, sizeof(*req)))
    {
        return 0;
    }
    if (!recv_all(fd, resp, sizeof(*resp)))
    {
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    int client_id;
    int fd;
    char line[LINE_BUF];

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <client-id>\n", argv[0]);
        return 1;
    }

    client_id = atoi(argv[1]);
    if (client_id < 1 || client_id > 9)
    {
        fprintf(stderr, "invalid client id\n");
        return 1;
    }

    fd = connect_server();
    if (fd < 0)
    {
        fprintf(stderr, "could not connect to server\n");
        return 1;
    }

    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        char local[LINE_BUF];
        char tidtok[32];
        char cmd[8];
        char *rest;
        int txn_client;
        int extra;
        int i;

        trim_newline(line);
        if (line[0] == '\0')
        {
            continue;
        }
        if (line[0] != 'T')
        {
            continue;
        }

        strncpy(local, line, sizeof(local) - 1);
        local[sizeof(local) - 1] = '\0';

        if (sscanf(local, "%31s %7s", tidtok, cmd) < 2)
        {
            continue;
        }

        txn_client = extract_tid_client(tidtok);
        rest = strchr(local, ' ');
        if (rest == NULL)
        {
            continue;
        }
        rest++;

        extra = parse_extra_count(rest);

        if (txn_client != client_id)
        {
            for (i = 0; i < extra; i++)
            {
                if (fgets(line, sizeof(line), stdin) == NULL)
                {
                    break;
                }
            }
            continue;
        }

        if (strcmp(cmd, "C") == 0)
        {
            request_t req;
            response_t resp;
            memset(&req, 0, sizeof(req));
            req.type = COMMIT_REQ;
            req.tid = atoi(tidtok + 1);

            if (!rpc_call(fd, &req, &resp))
            {
                break;
            }
        }
        else
        {
            request_t req;
            response_t resp;
            size_t used;

            memset(&req, 0, sizeof(req));
            req.type = EXEC_REQ;
            req.tid = atoi(tidtok + 1);

            snprintf(req.buffer, sizeof(req.buffer), "%s\n", rest);
            used = strlen(req.buffer);

            for (i = 0; i < extra; i++)
            {
                char extra_line[LINE_BUF];
                if (fgets(extra_line, sizeof(extra_line), stdin) == NULL)
                {
                    break;
                }
                if (used + strlen(extra_line) + 1 < sizeof(req.buffer))
                {
                    strcpy(req.buffer + used, extra_line);
                    used += strlen(extra_line);
                }
            }

            req.datalen = (int)strlen(req.buffer);

            if (!rpc_call(fd, &req, &resp))
            {
                break;
            }

            if (resp.status && resp.datalen > 0)
            {
                fwrite(resp.buffer, 1, (size_t)resp.datalen, stdout);
                fflush(stdout);
            }
        }
    }

    {
        request_t req;
        response_t resp;
        memset(&req, 0, sizeof(req));
        req.type = SHUTDOWN_REQ;
        req.tid = client_id;
        rpc_call(fd, &req, &resp);
    }

    close(fd);
    return 0;
}
