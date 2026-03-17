/*
 * File: mydb.c
 * Purpose: Block-based DBMS simulator for Assignment 5
 * Author: Sean Balbale
 * Date: 3/25/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* --- System Constants --- */
#define RECSIZE 63
#define BLKSIZE 256
#define BLKFAC 4
#define DISKSIZE 256
#define BLKMAX 16
#define OVBLKMAX 4

#define MAX_REL 64
#define MAX_ATTR 10
#define MAX_NAME 32
#define MAX_TOKEN 64
#define LINE_BUF 512

typedef struct slot
{
    char flag; /* 0 = free, 1 = used */
    char tuple[RECSIZE];
} slot_t;

typedef union block
{
    slot_t slots[BLKFAC];
    char raw[BLKSIZE];
} block_t;

typedef struct header_block
{
    int data_blocks[BLKMAX];
    int overflow_blocks[OVBLKMAX];
    int num_data_blocks;
    int num_overflow_blocks;
    int is_base; /* 1 base(hash), 0 derived(heap) */
} header_block_t;

typedef struct attr_info
{
    char name[MAX_NAME];
    char domain; /* 'S' or 'I' */
} attr_info_t;

typedef struct relation_meta
{
    int in_use;
    int is_dictionary;
    int is_base;

    char name[MAX_NAME];
    int num_attrs;
    int keysize;
    attr_info_t attrs[MAX_ATTR];

    int header_block;
} relation_meta_t;

/* --- Simulated Disk + Buffers (9-block model) --- */
static unsigned char virtual_disk[DISKSIZE][BLKSIZE];
static unsigned char bitmapblk[DISKSIZE];
static unsigned char catalog_header[BLKSIZE];
static unsigned char columns_header[BLKSIZE];
static block_t data_buffers[3];
static unsigned char header_buffers[3][BLKSIZE];

/* --- In-memory relation catalog (metadata only) --- */
static relation_meta_t g_relations[MAX_REL];

/* --- Helpers --- */
static void trim_newline(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
    {
        s[n - 1] = '\0';
        n--;
    }
}

static int parse_fields(const char *line, char out[][MAX_TOKEN], int max_fields)
{
    char tmp[LINE_BUF];
    char *tok;
    int c = 0;

    strncpy(tmp, line, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    tok = strtok(tmp, " \t\r\n");
    while (tok != NULL && c < max_fields)
    {
        strncpy(out[c], tok, MAX_TOKEN - 1);
        out[c][MAX_TOKEN - 1] = '\0';
        c++;
        tok = strtok(NULL, " \t\r\n");
    }
    return c;
}

static int str_is_int(const char *s)
{
    int i = 0;
    if (s[0] == '-' || s[0] == '+')
    {
        i = 1;
    }
    if (s[i] == '\0')
    {
        return 0;
    }
    for (; s[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)s[i]))
        {
            return 0;
        }
    }
    return 1;
}

/* --- Disk I/O --- */
static void diskread(int blocknum, unsigned char *buffer)
{
    // Ignore invalid block requests instead of crashing.
    if (blocknum < 0 || blocknum >= DISKSIZE)
    {
        return;
    }
    // Copy one full block from simulated disk to caller buffer.
    memcpy(buffer, virtual_disk[blocknum], BLKSIZE);
}

static void diskwrite(int blocknum, const unsigned char *buffer)
{
    // Ignore invalid block requests instead of crashing.
    if (blocknum < 0 || blocknum >= DISKSIZE)
    {
        return;
    }
    // Copy one full block from caller buffer to simulated disk.
    memcpy(virtual_disk[blocknum], buffer, BLKSIZE);
}

static int allocate_block(void)
{
    // First-fit allocation from the bitmap.
    for (int i = 0; i < DISKSIZE; i++)
    {
        if (bitmapblk[i] == 0)
        {
            bitmapblk[i] = 1;
            // Persist bitmap immediately so block ownership survives later operations.
            diskwrite(0, bitmapblk);
            return i;
        }
    }
    // No free blocks remain.
    return -1;
}

static void free_block(int b)
{
    if (b >= 0 && b < DISKSIZE)
    {
        // Mark block available and persist bitmap update.
        bitmapblk[b] = 0;
        diskwrite(0, bitmapblk);
    }
}

/* --- Relation Metadata --- */
static int find_relation_index(const char *name)
{
    for (int i = 0; i < MAX_REL; i++)
    {
        if (g_relations[i].in_use && strcmp(g_relations[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

static relation_meta_t *find_relation(const char *name)
{
    int idx = find_relation_index(name);
    if (idx < 0)
    {
        return NULL;
    }
    return &g_relations[idx];
}

static int relsize_of(const relation_meta_t *rel)
{
    // Relsize is payload bytes per tuple: sum of attribute sizes.
    int total = 0;
    for (int i = 0; i < rel->num_attrs; i++)
    {
        total += (rel->attrs[i].domain == 'I') ? 11 : MAX_TOKEN;
    }
    if (total > RECSIZE)
    {
        total = RECSIZE;
    }
    return total;
}

static int is_reserved_relation(const char *name)
{
    return (strcmp(name, "catalog") == 0 || strcmp(name, "columns") == 0);
}

static int create_relation_meta(const char *name, int is_base, int is_dictionary,
                                attr_info_t *attrs, int nattrs, int keysize,
                                int header_block)
{
    // Find a free metadata slot in the in-memory relation table.
    for (int i = 0; i < MAX_REL; i++)
    {
        if (!g_relations[i].in_use)
        {
            g_relations[i].in_use = 1;
            g_relations[i].is_base = is_base;
            g_relations[i].is_dictionary = is_dictionary;
            strncpy(g_relations[i].name, name, MAX_NAME - 1);
            g_relations[i].name[MAX_NAME - 1] = '\0';
            g_relations[i].num_attrs = nattrs;
            g_relations[i].keysize = keysize;
            g_relations[i].header_block = header_block;

            // Copy schema definition into the metadata record.
            for (int a = 0; a < nattrs; a++)
            {
                g_relations[i].attrs[a] = attrs[a];
            }
            return 1;
        }
    }
    // Metadata table is full.
    return 0;
}

/* --- Tuple Helpers --- */
static int tuple_key_equals(const relation_meta_t *rel, const char *tuple_text, char key_vals[][MAX_TOKEN], int key_count)
{
    // Parse tuple text and compare only the leading key attributes.
    char fields[MAX_ATTR][MAX_TOKEN];
    int got = parse_fields(tuple_text, fields, MAX_ATTR);
    if (key_count != rel->keysize || got < rel->keysize)
    {
        return 0;
    }
    for (int i = 0; i < rel->keysize; i++)
    {
        if (strcmp(fields[i], key_vals[i]) != 0)
        {
            return 0;
        }
    }
    return 1;
}

static int validate_tuple_domains(const relation_meta_t *rel, const char *tuple_line)
{
    // Validate each token against the declared schema domain.
    char f[MAX_ATTR][MAX_TOKEN];
    int got = parse_fields(tuple_line, f, MAX_ATTR);
    if (got != rel->num_attrs)
    {
        return 0;
    }
    for (int i = 0; i < rel->num_attrs; i++)
    {
        if (rel->attrs[i].domain == 'I' && !str_is_int(f[i]))
        {
            return 0;
        }
    }
    return 1;
}

static int hash_key_from_tuple(const relation_meta_t *rel, const char *tuple_line)
{
    // Hash function: ASCII sum of key fields modulo BLKMAX.
    char f[MAX_ATTR][MAX_TOKEN];
    int got = parse_fields(tuple_line, f, MAX_ATTR);
    int sum = 0;
    if (got < rel->keysize)
    {
        return 0;
    }
    for (int i = 0; i < rel->keysize; i++)
    {
        for (int j = 0; f[i][j] != '\0'; j++)
        {
            sum += (unsigned char)f[i][j];
        }
    }
    return sum % BLKMAX;
}

/* --- Header helpers --- */
static void read_header(const relation_meta_t *rel, header_block_t *hdr)
{
    // Read relation header block from simulated disk into struct form.
    memset(header_buffers[0], 0, BLKSIZE);
    diskread(rel->header_block, header_buffers[0]);
    memcpy(hdr, header_buffers[0], sizeof(*hdr));
}

static void write_header(const relation_meta_t *rel, const header_block_t *hdr)
{
    // Persist relation header struct back to its header block.
    memset(header_buffers[0], 0, BLKSIZE);
    memcpy(header_buffers[0], hdr, sizeof(*hdr));
    diskwrite(rel->header_block, header_buffers[0]);
}

static void clear_block_and_write(int blocknum)
{
    // Fresh blocks are zeroed before first use.
    memset(data_buffers[0].raw, 0, BLKSIZE);
    diskwrite(blocknum, (unsigned char *)data_buffers[0].raw);
}

/* --- Primitive access --- */
static int find_free_slot_in_block(int blocknum, int *slot_idx)
{
    // Return first free slot among the 4 fixed slots in a block.
    block_t *blk = &data_buffers[0];
    diskread(blocknum, (unsigned char *)blk->raw);
    for (int s = 0; s < BLKFAC; s++)
    {
        if (blk->slots[s].flag == 0)
        {
            *slot_idx = s;
            return 1;
        }
    }
    return 0;
}

static void write_tuple_to_slot(int blocknum, int slot_idx, const char *tuple_text)
{
    // Materialize tuple text inside slot payload and set occupancy flag.
    block_t *blk = &data_buffers[0];
    diskread(blocknum, (unsigned char *)blk->raw);
    blk->slots[slot_idx].flag = 1;
    memset(blk->slots[slot_idx].tuple, 0, RECSIZE);
    strncpy(blk->slots[slot_idx].tuple, tuple_text, RECSIZE - 1);
    diskwrite(blocknum, (unsigned char *)blk->raw);
}

static int tuple_exists_full_scan(const relation_meta_t *rel, const char *tuple_text)
{
    // Full physical scan used by duplicate suppression logic.
    header_block_t hdr;
    read_header(rel, &hdr);

    for (int i = 0; i < BLKMAX; i++)
    {
        if (hdr.data_blocks[i] == 0)
        {
            continue;
        }
        block_t *blk = &data_buffers[0];
        diskread(hdr.data_blocks[i], (unsigned char *)blk->raw);
        for (int s = 0; s < BLKFAC; s++)
        {
            if (blk->slots[s].flag == 1 && strcmp(blk->slots[s].tuple, tuple_text) == 0)
            {
                return 1;
            }
        }
    }

    if (rel->is_base)
    {
        for (int i = 0; i < OVBLKMAX; i++)
        {
            if (hdr.overflow_blocks[i] == 0)
            {
                continue;
            }
            block_t *blk = &data_buffers[0];
            diskread(hdr.overflow_blocks[i], (unsigned char *)blk->raw);
            for (int s = 0; s < BLKFAC; s++)
            {
                if (blk->slots[s].flag == 1 && strcmp(blk->slots[s].tuple, tuple_text) == 0)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

static int find_base_tuple_by_key(const relation_meta_t *rel, char key_vals[][MAX_TOKEN], int key_count,
                                  int *out_block, int *out_slot)
{
    // Locate a base-relation tuple by key and return its block/slot position.
    header_block_t hdr;
    read_header(rel, &hdr);

    if (key_count != rel->keysize)
    {
        return 0;
    }

    /* Search all hash buckets and overflow blocks. */
    for (int i = 0; i < BLKMAX; i++)
    {
        if (hdr.data_blocks[i] == 0)
        {
            continue;
        }
        block_t *blk = &data_buffers[0];
        diskread(hdr.data_blocks[i], (unsigned char *)blk->raw);
        for (int s = 0; s < BLKFAC; s++)
        {
            if (blk->slots[s].flag == 1 && tuple_key_equals(rel, blk->slots[s].tuple, key_vals, key_count))
            {
                *out_block = hdr.data_blocks[i];
                *out_slot = s;
                return 1;
            }
        }
    }

    for (int i = 0; i < OVBLKMAX; i++)
    {
        if (hdr.overflow_blocks[i] == 0)
        {
            continue;
        }
        block_t *blk = &data_buffers[0];
        diskread(hdr.overflow_blocks[i], (unsigned char *)blk->raw);
        for (int s = 0; s < BLKFAC; s++)
        {
            if (blk->slots[s].flag == 1 && tuple_key_equals(rel, blk->slots[s].tuple, key_vals, key_count))
            {
                *out_block = hdr.overflow_blocks[i];
                *out_slot = s;
                return 1;
            }
        }
    }

    return 0;
}

static int insert_base_tuple(const relation_meta_t *rel, const char *tuple_text)
{
    // Base insertion: hash bucket first, then overflow blocks as needed.
    header_block_t hdr;
    read_header(rel, &hdr);

    int bucket = hash_key_from_tuple(rel, tuple_text);
    int blocknum = hdr.data_blocks[bucket];

    if (blocknum == 0)
    {
        blocknum = allocate_block();
        if (blocknum < 0)
        {
            return 0;
        }
        clear_block_and_write(blocknum);
        hdr.data_blocks[bucket] = blocknum;
        hdr.num_data_blocks++;
        write_header(rel, &hdr);
    }

    int slot = -1;
    if (find_free_slot_in_block(blocknum, &slot))
    {
        write_tuple_to_slot(blocknum, slot, tuple_text);
        return 1;
    }

    /* Existing overflow blocks first. */
    for (int i = 0; i < OVBLKMAX; i++)
    {
        if (hdr.overflow_blocks[i] != 0 && find_free_slot_in_block(hdr.overflow_blocks[i], &slot))
        {
            write_tuple_to_slot(hdr.overflow_blocks[i], slot, tuple_text);
            return 1;
        }
    }

    /* Allocate new overflow block if capacity remains. */
    if (hdr.num_overflow_blocks < OVBLKMAX)
    {
        int ov = allocate_block();
        if (ov < 0)
        {
            return 0;
        }
        clear_block_and_write(ov);
        for (int i = 0; i < OVBLKMAX; i++)
        {
            if (hdr.overflow_blocks[i] == 0)
            {
                hdr.overflow_blocks[i] = ov;
                hdr.num_overflow_blocks++;
                break;
            }
        }
        write_header(rel, &hdr);
        write_tuple_to_slot(ov, 0, tuple_text);
        return 1;
    }

    return 0;
}

static int insert_heap_tuple(const relation_meta_t *rel, const char *tuple_text)
{
    // Heap insertion for derived relations: first fit, then allocate block.
    header_block_t hdr;
    read_header(rel, &hdr);

    int slot = -1;
    for (int i = 0; i < BLKMAX; i++)
    {
        if (hdr.data_blocks[i] == 0)
        {
            continue;
        }
        if (find_free_slot_in_block(hdr.data_blocks[i], &slot))
        {
            write_tuple_to_slot(hdr.data_blocks[i], slot, tuple_text);
            return 1;
        }
    }

    if (hdr.num_data_blocks >= BLKMAX)
    {
        return 0;
    }

    int b = allocate_block();
    if (b < 0)
    {
        return 0;
    }
    clear_block_and_write(b);

    for (int i = 0; i < BLKMAX; i++)
    {
        if (hdr.data_blocks[i] == 0)
        {
            hdr.data_blocks[i] = b;
            hdr.num_data_blocks++;
            break;
        }
    }
    write_header(rel, &hdr);

    write_tuple_to_slot(b, 0, tuple_text);
    return 1;
}

static int append_tuple(const relation_meta_t *rel, const char *tuple_text)
{
    // Dispatch insertion policy based on relation kind.
    if (rel->is_base)
    {
        // Base relations use hash + overflow strategy.
        return insert_base_tuple(rel, tuple_text);
    }
    // Derived relations use heap append strategy.
    return insert_heap_tuple(rel, tuple_text);
}

static void remove_tuple_slot(int blocknum, int slot)
{
    // Logical delete clears the slot and data bytes.
    block_t *blk = &data_buffers[0];
    diskread(blocknum, (unsigned char *)blk->raw);
    blk->slots[slot].flag = 0;
    memset(blk->slots[slot].tuple, 0, RECSIZE);
    diskwrite(blocknum, (unsigned char *)blk->raw);
}

static void update_tuple_slot(int blocknum, int slot, const char *tuple_text)
{
    write_tuple_to_slot(blocknum, slot, tuple_text);
}

/* --- Iteration --- */
typedef int (*tuple_cb_t)(const relation_meta_t *rel, const char *tuple_text, void *ctx);

static int scan_relation(const relation_meta_t *rel, tuple_cb_t cb, void *ctx)
{
    // Generic relation scan used by PR and relational operators.
    header_block_t hdr;
    read_header(rel, &hdr);

    for (int i = 0; i < BLKMAX; i++)
    {
        if (hdr.data_blocks[i] == 0)
        {
            continue;
        }
        block_t *blk = &data_buffers[2];
        diskread(hdr.data_blocks[i], (unsigned char *)blk->raw);
        for (int s = 0; s < BLKFAC; s++)
        {
            if (blk->slots[s].flag == 1)
            {
                if (!cb(rel, blk->slots[s].tuple, ctx))
                {
                    return 0;
                }
            }
        }
    }

    if (rel->is_base)
    {
        for (int i = 0; i < OVBLKMAX; i++)
        {
            if (hdr.overflow_blocks[i] == 0)
            {
                continue;
            }
            block_t *blk = &data_buffers[2];
            diskread(hdr.overflow_blocks[i], (unsigned char *)blk->raw);
            for (int s = 0; s < BLKFAC; s++)
            {
                if (blk->slots[s].flag == 1)
                {
                    if (!cb(rel, blk->slots[s].tuple, ctx))
                    {
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

/* --- Create/Delete relation physical storage --- */
static int create_relation(const char *name, int is_base, int is_dictionary,
                           attr_info_t *attrs, int nattrs, int keysize)
{
    // Create physical header block, then register relation metadata.
    if (find_relation(name) != NULL)
    {
        // Relation names must be unique.
        return 0;
    }

    // Reserve one header block for this relation.
    int hb = allocate_block();
    if (hb < 0)
    {
        return 0;
    }

    header_block_t hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.is_base = is_base;

    // Initialize header on disk.
    memset(header_buffers[1], 0, BLKSIZE);
    memcpy(header_buffers[1], &hdr, sizeof(hdr));
    diskwrite(hb, header_buffers[1]);

    if (!create_relation_meta(name, is_base, is_dictionary, attrs, nattrs, keysize, hb))
    {
        free_block(hb);
        return 0;
    }

    if (!is_dictionary)
    {
        // Persist dictionary tuples so catalog/columns live on disk like base relations.
        relation_meta_t *new_rel = find_relation(name);
        relation_meta_t *catalog = find_relation("catalog");
        relation_meta_t *columns = find_relation("columns");
        char tuple_line[LINE_BUF];

        if (new_rel != NULL && catalog != NULL && columns != NULL)
        {
            // Insert one catalog tuple for relation-level metadata.
            snprintf(tuple_line, sizeof(tuple_line), "%s %d %d %d %d %d",
                     new_rel->name,
                     new_rel->is_base ? 1 : 0,
                     relsize_of(new_rel),
                     new_rel->keysize,
                     0,
                     new_rel->header_block);
            append_tuple(catalog, tuple_line);

            // Insert one columns tuple per attribute in schema order.
            for (int i = 0; i < new_rel->num_attrs; i++)
            {
                snprintf(tuple_line, sizeof(tuple_line), "%s %s %d %d",
                         new_rel->name,
                         new_rel->attrs[i].name,
                         (new_rel->attrs[i].domain == 'I') ? 1 : 0,
                         i);
                append_tuple(columns, tuple_line);
            }
        }
    }

    return 1;
}

static int delete_relation(const char *name)
{
    // Delete all allocated blocks for the relation and clear metadata slot.
    relation_meta_t *rel = find_relation(name);
    if (rel == NULL || rel->is_dictionary)
    {
        return 0;
    }

    header_block_t hdr;
    read_header(rel, &hdr);

    for (int i = 0; i < BLKMAX; i++)
    {
        if (hdr.data_blocks[i] != 0)
        {
            // Free all primary data blocks.
            free_block(hdr.data_blocks[i]);
        }
    }
    for (int i = 0; i < OVBLKMAX; i++)
    {
        if (hdr.overflow_blocks[i] != 0)
        {
            // Free all overflow blocks.
            free_block(hdr.overflow_blocks[i]);
        }
    }
    // Free the header block last.
    free_block(rel->header_block);

    // Remove dictionary rows that describe this relation.
    {
        relation_meta_t *catalog = find_relation("catalog");
        relation_meta_t *columns = find_relation("columns");
        header_block_t hdr;

        if (catalog != NULL)
        {
            read_header(catalog, &hdr);
            for (int i = 0; i < BLKMAX; i++)
            {
                if (hdr.data_blocks[i] == 0)
                {
                    continue;
                }
                block_t *blk = &data_buffers[0];
                diskread(hdr.data_blocks[i], (unsigned char *)blk->raw);
                for (int s = 0; s < BLKFAC; s++)
                {
                    char fields[MAX_ATTR][MAX_TOKEN];
                    if (blk->slots[s].flag != 1)
                    {
                        continue;
                    }
                    if (parse_fields(blk->slots[s].tuple, fields, MAX_ATTR) > 0 && strcmp(fields[0], name) == 0)
                    {
                        // Remove matching dictionary row.
                        remove_tuple_slot(hdr.data_blocks[i], s);
                    }
                }
            }
            for (int i = 0; i < OVBLKMAX; i++)
            {
                if (hdr.overflow_blocks[i] == 0)
                {
                    continue;
                }
                block_t *blk = &data_buffers[0];
                diskread(hdr.overflow_blocks[i], (unsigned char *)blk->raw);
                for (int s = 0; s < BLKFAC; s++)
                {
                    char fields[MAX_ATTR][MAX_TOKEN];
                    if (blk->slots[s].flag != 1)
                    {
                        continue;
                    }
                    if (parse_fields(blk->slots[s].tuple, fields, MAX_ATTR) > 0 && strcmp(fields[0], name) == 0)
                    {
                        // Remove matching dictionary row.
                        remove_tuple_slot(hdr.overflow_blocks[i], s);
                    }
                }
            }
        }

        if (columns != NULL)
        {
            read_header(columns, &hdr);
            for (int i = 0; i < BLKMAX; i++)
            {
                if (hdr.data_blocks[i] == 0)
                {
                    continue;
                }
                block_t *blk = &data_buffers[0];
                diskread(hdr.data_blocks[i], (unsigned char *)blk->raw);
                for (int s = 0; s < BLKFAC; s++)
                {
                    char fields[MAX_ATTR][MAX_TOKEN];
                    if (blk->slots[s].flag != 1)
                    {
                        continue;
                    }
                    if (parse_fields(blk->slots[s].tuple, fields, MAX_ATTR) > 0 && strcmp(fields[0], name) == 0)
                    {
                        // Remove matching dictionary row.
                        remove_tuple_slot(hdr.data_blocks[i], s);
                    }
                }
            }
            for (int i = 0; i < OVBLKMAX; i++)
            {
                if (hdr.overflow_blocks[i] == 0)
                {
                    continue;
                }
                block_t *blk = &data_buffers[0];
                diskread(hdr.overflow_blocks[i], (unsigned char *)blk->raw);
                for (int s = 0; s < BLKFAC; s++)
                {
                    char fields[MAX_ATTR][MAX_TOKEN];
                    if (blk->slots[s].flag != 1)
                    {
                        continue;
                    }
                    if (parse_fields(blk->slots[s].tuple, fields, MAX_ATTR) > 0 && strcmp(fields[0], name) == 0)
                    {
                        // Remove matching dictionary row.
                        remove_tuple_slot(hdr.overflow_blocks[i], s);
                    }
                }
            }
        }
    }

    memset(rel, 0, sizeof(*rel));
    return 1;
}

/* --- Command Operations --- */
static int print_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    (void)rel; // Unused in this callback, but could be used for more complex formatting.
    (void)ctx; // Unused in this callback, but could be used to pass additional state if needed.
    printf("%s\n", tuple_text);
    return 1;
}

static int do_print(const char *name)
{
    // PR command prints schema heading and all live tuple strings.
    relation_meta_t *rel = find_relation(name);
    if (rel == NULL)
    {
        return 0;
    }

    // Print relation title line, then the column header line.
    printf("%s\n", rel->name);
    for (int i = 0; i < rel->num_attrs; i++)
    {
        if (i > 0)
        {
            printf(" ");
        }
        printf("%s", rel->attrs[i].name);
    }
    printf("\n");

    scan_relation(rel, print_cb, NULL);
    // Keep a blank line between PR outputs.
    printf("\n");
    return 1;
}

static int do_insert(const char *name, int n, FILE *in)
{
    // IN command: allowed only for base, non-dictionary relations.
    relation_meta_t *rel = find_relation(name);
    char line[LINE_BUF];

    if (rel == NULL || !rel->is_base || rel->is_dictionary)
    {
        // Consume and discard tuple lines to keep parser stream aligned.
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    for (int i = 0; i < n; i++)
    {
        char key_vals[MAX_ATTR][MAX_TOKEN];
        int b, s;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            break;
        }
        trim_newline(line);

        if (!validate_tuple_domains(rel, line))
        {
            // Skip tuples that violate schema types.
            continue;
        }

        /* Uniqueness of key */
        parse_fields(line, key_vals, MAX_ATTR);
        if (find_base_tuple_by_key(rel, key_vals, rel->keysize, &b, &s))
        {
            // Skip duplicate keys for base relations.
            continue;
        }

        // Physical insertion into hash/overflow storage.
        append_tuple(rel, line);
    }

    return 1;
}

static int do_remove(const char *name, int n, FILE *in)
{
    // RM command: each input line is a key identifying one tuple.
    relation_meta_t *rel = find_relation(name);
    char line[LINE_BUF];

    if (rel == NULL || !rel->is_base || rel->is_dictionary)
    {
        // Consume and discard key lines to keep parser stream aligned.
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    for (int i = 0; i < n; i++)
    {
        char key_vals[MAX_ATTR][MAX_TOKEN];
        int key_count, b, s;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            break;
        }
        trim_newline(line);

        key_count = parse_fields(line, key_vals, MAX_ATTR);
        if (find_base_tuple_by_key(rel, key_vals, key_count, &b, &s))
        {
            // Logical delete by clearing the matched slot.
            remove_tuple_slot(b, s);
        }
    }
    return 1;
}

static int do_update(const char *name, int n, FILE *in)
{
    // UP command: replace tuple payload at existing key location.
    relation_meta_t *rel = find_relation(name);
    char line[LINE_BUF];

    if (rel == NULL || !rel->is_base || rel->is_dictionary)
    {
        // Consume and discard replacement lines to keep parser stream aligned.
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    for (int i = 0; i < n; i++)
    {
        char key_vals[MAX_ATTR][MAX_TOKEN];
        int b, s;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            break;
        }
        trim_newline(line);

        if (!validate_tuple_domains(rel, line))
        {
            // Skip updates that violate schema types.
            continue;
        }

        parse_fields(line, key_vals, MAX_ATTR);
        if (find_base_tuple_by_key(rel, key_vals, rel->keysize, &b, &s))
        {
            // In-place replacement of tuple payload in the matched slot.
            update_tuple_slot(b, s, line);
        }
    }
    return 1;
}

static int build_projected_tuple(const relation_meta_t *src, const char *tuple_text,
                                 int proj_idx[], int nproj,
                                 char *out, size_t out_sz)
{
    // Build projected tuple text using selected source attribute indexes.
    char fields[MAX_ATTR][MAX_TOKEN];
    int got = parse_fields(tuple_text, fields, MAX_ATTR);
    out[0] = '\0';

    if (got < src->num_attrs)
    {
        return 0;
    }

    for (int i = 0; i < nproj; i++)
    {
        if (i > 0)
        {
            strncat(out, " ", out_sz - strlen(out) - 1);
        }
        strncat(out, fields[proj_idx[i]], out_sz - strlen(out) - 1);
    }
    return 1;
}

typedef struct proj_ctx
{
    relation_meta_t *dst;
    relation_meta_t *src;
    int proj_idx[MAX_ATTR];
    int nproj;
} proj_ctx_t;

static int project_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    // Projection callback suppresses duplicates in destination relation.
    char out[RECSIZE];
    proj_ctx_t *pc = (proj_ctx_t *)ctx;
    (void)rel; // Unused in this callback, but could be used for more complex logic if needed.

    if (build_projected_tuple(pc->src, tuple_text, pc->proj_idx, pc->nproj, out, sizeof(out)))
    {
        if (!tuple_exists_full_scan(pc->dst, out))
        {
            append_tuple(pc->dst, out);
        }
    }
    return 1;
}

static int do_project(const char *src_name, const char *dst_name, int n, FILE *in)
{
    // PJ command: read attribute list, build derived projection relation.
    relation_meta_t *src = find_relation(src_name);
    char line[LINE_BUF];
    int proj_idx[MAX_ATTR];
    attr_info_t attrs[MAX_ATTR];

    if (src == NULL || find_relation(dst_name) != NULL || is_reserved_relation(dst_name) || n <= 0 || n > src->num_attrs)
    {
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    for (int i = 0; i < n; i++)
    {
        int idx;
        char aname[MAX_NAME];
        if (fgets(line, sizeof(line), in) == NULL)
        {
            return 0;
        }
        trim_newline(line);
        strncpy(aname, line, sizeof(aname) - 1);
        aname[sizeof(aname) - 1] = '\0';

        idx = -1;
        for (int a = 0; a < src->num_attrs; a++)
        {
            if (strcmp(src->attrs[a].name, aname) == 0)
            {
                idx = a;
                break;
            }
        }
        if (idx < 0)
        {
            return 0;
        }
        proj_idx[i] = idx;
        attrs[i] = src->attrs[idx];
    }

    if (!create_relation(dst_name, 0, 0, attrs, n, n))
    {
        return 0;
    }

    proj_ctx_t pc;
    memset(&pc, 0, sizeof(pc));
    pc.dst = find_relation(dst_name);
    pc.src = src;
    pc.nproj = n;
    for (int i = 0; i < n; i++)
    {
        pc.proj_idx[i] = proj_idx[i];
    }

    scan_relation(src, project_cb, &pc);
    return 1;
}

static int eval_cmp(char domain, const char *lhs, const char *op, const char *rhs)
{
    // Evaluate a single boolean predicate for integer/string domain.
    if (domain == 'I')
    {
        long a = strtol(lhs, NULL, 10);
        long b = strtol(rhs, NULL, 10);
        if (strcmp(op, "==") == 0)
            return a == b;
        if (strcmp(op, "!=") == 0)
            return a != b;
        if (strcmp(op, ">") == 0)
            return a > b;
        if (strcmp(op, ">=") == 0)
            return a >= b;
        if (strcmp(op, "<") == 0)
            return a < b;
        if (strcmp(op, "<=") == 0)
            return a <= b;
        return 0;
    }

    int c = strcmp(lhs, rhs);
    if (strcmp(op, "==") == 0)
        return c == 0;
    if (strcmp(op, "!=") == 0)
        return c != 0;
    if (strcmp(op, ">") == 0)
        return c > 0;
    if (strcmp(op, ">=") == 0)
        return c >= 0;
    if (strcmp(op, "<") == 0)
        return c < 0;
    if (strcmp(op, "<=") == 0)
        return c <= 0;
    return 0;
}

typedef struct sel_cond
{
    int attr_idx;
    char op[3];
    char value[MAX_TOKEN];
} sel_cond_t;

typedef struct sel_ctx
{
    relation_meta_t *src;
    relation_meta_t *dst;
    int nconds;
    sel_cond_t conds[MAX_ATTR];
} sel_ctx_t;

static int select_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    // Selection callback applies conjunction of all parsed conditions.
    sel_ctx_t *sc = (sel_ctx_t *)ctx;
    char f[MAX_ATTR][MAX_TOKEN];
    (void)rel; // Unused in this callback, but could be used for more complex logic if needed.

    if (parse_fields(tuple_text, f, MAX_ATTR) < sc->src->num_attrs)
    {
        return 1;
    }

    for (int i = 0; i < sc->nconds; i++)
    {
        int idx = sc->conds[i].attr_idx;
        if (!eval_cmp(sc->src->attrs[idx].domain, f[idx], sc->conds[i].op, sc->conds[i].value))
        {
            return 1;
        }
    }

    append_tuple(sc->dst, tuple_text);
    return 1;
}

static int do_select(const char *src_name, const char *dst_name, int n, FILE *in)
{
    // SL command: parse n conditions then filter source relation.
    relation_meta_t *src = find_relation(src_name);
    char line[LINE_BUF];
    sel_ctx_t sc;

    if (src == NULL || find_relation(dst_name) != NULL || is_reserved_relation(dst_name) || n < 0 || n > MAX_ATTR)
    {
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    memset(&sc, 0, sizeof(sc));
    sc.src = src;
    sc.nconds = n;

    // Parse each "attr op value" predicate line.
    for (int i = 0; i < n; i++)
    {
        char att[MAX_NAME], op[3], val[MAX_TOKEN];
        int idx = -1;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            return 0;
        }
        trim_newline(line);
        if (sscanf(line, "%31s %2s %63s", att, op, val) != 3)
        {
            return 0;
        }

        for (int a = 0; a < src->num_attrs; a++)
        {
            if (strcmp(src->attrs[a].name, att) == 0)
            {
                idx = a;
                break;
            }
        }
        if (idx < 0)
        {
            return 0;
        }

        sc.conds[i].attr_idx = idx;
        strncpy(sc.conds[i].op, op, sizeof(sc.conds[i].op) - 1);
        strncpy(sc.conds[i].value, val, sizeof(sc.conds[i].value) - 1);
    }

    if (!create_relation(dst_name, 0, 0, src->attrs, src->num_attrs, src->num_attrs))
    {
        return 0;
    }

    // Run filtered scan and append matching tuples into destination.
    sc.dst = find_relation(dst_name);
    scan_relation(src, select_cb, &sc);
    return 1;
}

static int map_compatible(const relation_meta_t *p, const relation_meta_t *q, int qmap[])
{
    // Compatibility for UN/DF with attribute-name based reordering.
    if (p->num_attrs != q->num_attrs)
    {
        return 0;
    }

    for (int i = 0; i < p->num_attrs; i++)
    {
        int idx = -1;
        for (int j = 0; j < q->num_attrs; j++)
        {
            if (strcmp(p->attrs[i].name, q->attrs[j].name) == 0)
            {
                idx = j;
                break;
            }
        }
        if (idx < 0 || p->attrs[i].domain != q->attrs[idx].domain)
        {
            return 0;
        }
        qmap[i] = idx;
    }
    return 1;
}

typedef struct pass_ctx
{
    relation_meta_t *dst;
} pass_ctx_t;

static int union_seed_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    // Seed result with tuples from first input relation.
    pass_ctx_t *pc = (pass_ctx_t *)ctx;
    (void)rel; // Unused in this callback, but could be used for more complex logic if needed.
    if (!tuple_exists_full_scan(pc->dst, tuple_text))
    {
        append_tuple(pc->dst, tuple_text);
    }
    return 1;
}

typedef struct map_ctx
{
    relation_meta_t *q;
    relation_meta_t *dst;
    int qmap[MAX_ATTR];
} map_ctx_t;

static int union_q_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    // Add second relation tuples after column re-mapping.
    map_ctx_t *mc = (map_ctx_t *)ctx;
    char out[RECSIZE];
    char f[MAX_ATTR][MAX_TOKEN];
    (void)rel; // Unused in this callback, but could be used for more complex logic if needed.

    if (parse_fields(tuple_text, f, MAX_ATTR) < mc->q->num_attrs)
    {
        return 1;
    }

    out[0] = '\0';
    for (int i = 0; i < mc->dst->num_attrs; i++)
    {
        if (i > 0)
        {
            strncat(out, " ", sizeof(out) - strlen(out) - 1);
        }
        strncat(out, f[mc->qmap[i]], sizeof(out) - strlen(out) - 1);
    }

    if (!tuple_exists_full_scan(mc->dst, out))
    {
        append_tuple(mc->dst, out);
    }
    return 1;
}

static int do_union(const char *pname, const char *qname, const char *rname)
{
    // UN command preserves attribute order from first operand p.
    relation_meta_t *p = find_relation(pname);
    relation_meta_t *q = find_relation(qname);
    int qmap[MAX_ATTR];

    if (p == NULL || q == NULL || find_relation(rname) != NULL || is_reserved_relation(rname))
    {
        return 0;
    }
    if (!map_compatible(p, q, qmap))
    {
        return 0;
    }

    if (!create_relation(rname, 0, 0, p->attrs, p->num_attrs, p->num_attrs))
    {
        return 0;
    }

    relation_meta_t *r = find_relation(rname);
    pass_ctx_t pc;
    map_ctx_t mc;

    // Seed result with p tuples first.
    pc.dst = r;
    scan_relation(p, union_seed_cb, &pc);

    memset(&mc, 0, sizeof(mc));
    mc.q = q;
    mc.dst = r;
    for (int i = 0; i < p->num_attrs; i++)
    {
        mc.qmap[i] = qmap[i];
    }
    // Merge q tuples using attribute-name mapping.
    scan_relation(q, union_q_cb, &mc);
    return 1;
}

typedef struct diff_ctx
{
    relation_meta_t *q;
    relation_meta_t *dst;
    int qmap[MAX_ATTR];
} diff_ctx_t;

static int tuple_in_q_mapped(const diff_ctx_t *dc, const char *p_tuple)
{
    // DF helper: check whether mapped-equivalent tuple exists in q.
    char pf[MAX_ATTR][MAX_TOKEN];
    int pg = parse_fields(p_tuple, pf, MAX_ATTR);
    if (pg < dc->dst->num_attrs)
    {
        return 0;
    }

    header_block_t hdr;
    read_header(dc->q, &hdr);

    for (int i = 0; i < BLKMAX; i++)
    {
        if (hdr.data_blocks[i] == 0)
        {
            continue;
        }
        block_t *blk = &data_buffers[0];
        diskread(hdr.data_blocks[i], (unsigned char *)blk->raw);
        for (int s = 0; s < BLKFAC; s++)
        {
            if (blk->slots[s].flag == 1)
            {
                char qf[MAX_ATTR][MAX_TOKEN];
                int qg = parse_fields(blk->slots[s].tuple, qf, MAX_ATTR);
                int same = 1;
                if (qg < dc->q->num_attrs)
                {
                    continue;
                }
                for (int a = 0; a < dc->dst->num_attrs; a++)
                {
                    if (strcmp(pf[a], qf[dc->qmap[a]]) != 0)
                    {
                        same = 0;
                        break;
                    }
                }
                if (same)
                {
                    return 1;
                }
            }
        }
    }

    if (dc->q->is_base)
    {
        for (int i = 0; i < OVBLKMAX; i++)
        {
            if (hdr.overflow_blocks[i] == 0)
            {
                continue;
            }
            block_t *blk = &data_buffers[0];
            diskread(hdr.overflow_blocks[i], (unsigned char *)blk->raw);
            for (int s = 0; s < BLKFAC; s++)
            {
                if (blk->slots[s].flag == 1)
                {
                    char qf[MAX_ATTR][MAX_TOKEN];
                    int qg = parse_fields(blk->slots[s].tuple, qf, MAX_ATTR);
                    int same = 1;
                    if (qg < dc->q->num_attrs)
                    {
                        continue;
                    }
                    for (int a = 0; a < dc->dst->num_attrs; a++)
                    {
                        if (strcmp(pf[a], qf[dc->qmap[a]]) != 0)
                        {
                            same = 0;
                            break;
                        }
                    }
                    if (same)
                    {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

static int diff_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    diff_ctx_t *dc = (diff_ctx_t *)ctx;
    (void)rel; // Unused in this callback, but could be used for more complex logic if needed.
    if (!tuple_in_q_mapped(dc, tuple_text))
    {
        append_tuple(dc->dst, tuple_text);
    }
    return 1;
}

static int do_difference(const char *pname, const char *qname, const char *rname)
{
    // DF command computes p - q using compatibility mapping.
    relation_meta_t *p = find_relation(pname);
    relation_meta_t *q = find_relation(qname);
    int qmap[MAX_ATTR];

    if (p == NULL || q == NULL || find_relation(rname) != NULL || is_reserved_relation(rname))
    {
        return 0;
    }
    if (!map_compatible(p, q, qmap))
    {
        return 0;
    }

    if (!create_relation(rname, 0, 0, p->attrs, p->num_attrs, p->num_attrs))
    {
        return 0;
    }

    diff_ctx_t dc;
    memset(&dc, 0, sizeof(dc));
    dc.q = q;
    dc.dst = find_relation(rname);
    for (int i = 0; i < p->num_attrs; i++)
    {
        dc.qmap[i] = qmap[i];
    }

    // Emit tuples that are present in p but not present in q.
    scan_relation(p, diff_cb, &dc);
    return 1;
}

typedef struct join_ctx
{
    relation_meta_t *p;
    relation_meta_t *q;
    relation_meta_t *r;
    int njoin;
    int pjoin[MAX_ATTR];
    int qjoin[MAX_ATTR];
    int q_is_join[MAX_ATTR];
} join_ctx_t;

static int join_with_p_tuple(const char *p_tuple, join_ctx_t *jc)
{
    // For one tuple in p, scan q and emit all join matches.
    char pf[MAX_ATTR][MAX_TOKEN];
    int pg = parse_fields(p_tuple, pf, MAX_ATTR);
    if (pg < jc->p->num_attrs)
    {
        return 1;
    }

    header_block_t hdr;
    read_header(jc->q, &hdr);

    for (int bi = 0; bi < BLKMAX; bi++)
    {
        if (hdr.data_blocks[bi] == 0)
        {
            continue;
        }
        // Use a dedicated scan buffer so append_tuple() cannot clobber q-block contents mid-loop.
        block_t *blk = &data_buffers[1];
        diskread(hdr.data_blocks[bi], (unsigned char *)blk->raw);
        for (int s = 0; s < BLKFAC; s++)
        {
            if (blk->slots[s].flag != 1)
            {
                continue;
            }

            char qf[MAX_ATTR][MAX_TOKEN];
            int qg = parse_fields(blk->slots[s].tuple, qf, MAX_ATTR);
            int ok = 1;
            char out[RECSIZE];

            if (qg < jc->q->num_attrs)
            {
                continue;
            }

            for (int j = 0; j < jc->njoin; j++)
            {
                if (strcmp(pf[jc->pjoin[j]], qf[jc->qjoin[j]]) != 0)
                {
                    ok = 0;
                    break;
                }
            }
            if (!ok)
            {
                continue;
            }

            out[0] = '\0';
            for (int i = 0; i < jc->p->num_attrs; i++)
            {
                if (i > 0)
                {
                    strncat(out, " ", sizeof(out) - strlen(out) - 1);
                }
                strncat(out, pf[i], sizeof(out) - strlen(out) - 1);
            }
            for (int i = 0; i < jc->q->num_attrs; i++)
            {
                if (!jc->q_is_join[i])
                {
                    strncat(out, " ", sizeof(out) - strlen(out) - 1);
                    strncat(out, qf[i], sizeof(out) - strlen(out) - 1);
                }
            }

            append_tuple(jc->r, out);
        }
    }

    if (jc->q->is_base)
    {
        for (int oi = 0; oi < OVBLKMAX; oi++)
        {
            if (hdr.overflow_blocks[oi] == 0)
            {
                continue;
            }
            // Keep overflow scan on the same dedicated scan buffer.
            block_t *blk = &data_buffers[1];
            diskread(hdr.overflow_blocks[oi], (unsigned char *)blk->raw);
            for (int s = 0; s < BLKFAC; s++)
            {
                if (blk->slots[s].flag != 1)
                {
                    continue;
                }

                char qf[MAX_ATTR][MAX_TOKEN];
                int qg = parse_fields(blk->slots[s].tuple, qf, MAX_ATTR);
                int ok = 1;
                char out[RECSIZE];

                if (qg < jc->q->num_attrs)
                {
                    continue;
                }

                for (int j = 0; j < jc->njoin; j++)
                {
                    if (strcmp(pf[jc->pjoin[j]], qf[jc->qjoin[j]]) != 0)
                    {
                        ok = 0;
                        break;
                    }
                }
                if (!ok)
                {
                    continue;
                }

                out[0] = '\0';
                for (int i = 0; i < jc->p->num_attrs; i++)
                {
                    if (i > 0)
                    {
                        strncat(out, " ", sizeof(out) - strlen(out) - 1);
                    }
                    strncat(out, pf[i], sizeof(out) - strlen(out) - 1);
                }
                for (int i = 0; i < jc->q->num_attrs; i++)
                {
                    if (!jc->q_is_join[i])
                    {
                        strncat(out, " ", sizeof(out) - strlen(out) - 1);
                        strncat(out, qf[i], sizeof(out) - strlen(out) - 1);
                    }
                }

                append_tuple(jc->r, out);
            }
        }
    }

    return 1;
}

static int join_p_cb(const relation_meta_t *rel, const char *tuple_text, void *ctx)
{
    // Callback driver for nested-loop natural join.
    join_ctx_t *jc = (join_ctx_t *)ctx;
    (void)rel; // Unused in this callback, but could be used for more complex logic if needed.
    return join_with_p_tuple(tuple_text, jc);
}

static int do_natural_join(const char *pname, const char *qname, const char *rname, int n, FILE *in)
{
    // NJ command parses join columns and builds joined output schema.
    relation_meta_t *p = find_relation(pname);
    relation_meta_t *q = find_relation(qname);
    char line[LINE_BUF];
    int pjoin[MAX_ATTR], qjoin[MAX_ATTR], q_is_join[MAX_ATTR] = {0};
    attr_info_t out_attrs[MAX_ATTR];
    int out_n = 0;

    if (p == NULL || q == NULL || find_relation(rname) != NULL || is_reserved_relation(rname) || n < 0 || n > MAX_ATTR)
    {
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    // Read and validate each join attribute listed after NJ command.
    for (int i = 0; i < n; i++)
    {
        int pi = -1, qi = -1;
        char aname[MAX_NAME];
        if (fgets(line, sizeof(line), in) == NULL)
        {
            return 0;
        }
        trim_newline(line);
        strncpy(aname, line, sizeof(aname) - 1);
        aname[sizeof(aname) - 1] = '\0';

        for (int a = 0; a < p->num_attrs; a++)
        {
            if (strcmp(p->attrs[a].name, aname) == 0)
            {
                pi = a;
                break;
            }
        }
        for (int a = 0; a < q->num_attrs; a++)
        {
            if (strcmp(q->attrs[a].name, aname) == 0)
            {
                qi = a;
                break;
            }
        }

        if (pi < 0 || qi < 0 || p->attrs[pi].domain != q->attrs[qi].domain)
        {
            return 0;
        }

        pjoin[i] = pi;
        qjoin[i] = qi;
        q_is_join[qi] = 1;
    }

    for (int i = 0; i < p->num_attrs; i++)
    {
        // Include all attributes from p first.
        out_attrs[out_n++] = p->attrs[i];
    }
    for (int i = 0; i < q->num_attrs; i++)
    {
        if (!q_is_join[i])
        {
            // Add non-join attributes from q.
            out_attrs[out_n++] = q->attrs[i];
        }
    }

    if (!create_relation(rname, 0, 0, out_attrs, out_n, out_n))
    {
        return 0;
    }

    join_ctx_t jc;
    memset(&jc, 0, sizeof(jc));
    jc.p = p;
    jc.q = q;
    jc.r = find_relation(rname);
    jc.njoin = n;
    for (int i = 0; i < n; i++)
    {
        jc.pjoin[i] = pjoin[i];
        jc.qjoin[i] = qjoin[i];
    }
    for (int i = 0; i < q->num_attrs; i++)
    {
        jc.q_is_join[i] = q_is_join[i];
    }

    // Nested-loop join over p with full scans of q.
    scan_relation(p, join_p_cb, &jc);
    return 1;
}

/* --- Init --- */
static void init_system(void)
{
    // Reset all simulated disk pages and in-memory control structures.
    memset(virtual_disk, 0, sizeof(virtual_disk));
    memset(bitmapblk, 0, sizeof(bitmapblk));
    memset(catalog_header, 0, sizeof(catalog_header));
    memset(columns_header, 0, sizeof(columns_header));
    memset(data_buffers, 0, sizeof(data_buffers));
    memset(header_buffers, 0, sizeof(header_buffers));
    memset(g_relations, 0, sizeof(g_relations));

    /* Reserve bitmap + dictionary header blocks by spec. */
    bitmapblk[0] = 1;
    bitmapblk[1] = 1;
    bitmapblk[2] = 1;
    // Block 0 stores bitmap; blocks 1 and 2 are dictionary headers.
    diskwrite(0, bitmapblk);

    /* Create dictionary relations as base relations. */
    {
        attr_info_t cat_attrs[6] = {
            {"Relname", 'S'},
            {"Kind", 'I'},
            {"Attsize", 'I'},
            {"Keysize", 'I'},
            {"Relsize", 'I'},
            {"Relptr", 'I'}};

        attr_info_t col_attrs[4] = {
            {"Relname", 'S'},
            {"Attname", 'S'},
            {"Attdomain", 'I'},
            {"Attpos", 'I'}};

        header_block_t hdr;
        memset(&hdr, 0, sizeof(hdr));
        hdr.is_base = 1;
        memcpy(catalog_header, &hdr, sizeof(hdr));
        memcpy(columns_header, &hdr, sizeof(hdr));
        diskwrite(1, catalog_header);
        diskwrite(2, columns_header);

        // Register dictionary metadata in the in-memory relation table.
        create_relation_meta("catalog", 1, 1, cat_attrs, 6, 1, 1);
        create_relation_meta("columns", 1, 1, col_attrs, 4, 2, 2);
    }
}

/* --- Parser --- */
static void process_commands(FILE *in)
{
    // Main command loop reads one command per line from stdin.
    char line[LINE_BUF];

    while (fgets(line, sizeof(line), in) != NULL)
    {
        char verb[3] = {0};
        trim_newline(line);

        if (line[0] == '\0')
        {
            // Skip blank lines.
            continue;
        }
        if (sscanf(line, "%2s", verb) != 1)
        {
            // Skip malformed command lines that do not begin with a verb.
            continue;
        }

        if (strcmp(verb, "CR") == 0)
        {
            // CR command is followed by n schema lines.
            char name[MAX_NAME];
            int n, k;
            if (sscanf(line, "CR %31s %d %d", name, &n, &k) == 3)
            {
                attr_info_t attrs[MAX_ATTR];
                for (int i = 0; i < n; i++)
                {
                    char al[LINE_BUF], an[MAX_NAME], d;
                    if (fgets(al, sizeof(al), in) == NULL)
                    {
                        break;
                    }
                    trim_newline(al);
                    if (sscanf(al, "%31s %c", an, &d) != 2)
                    {
                        continue;
                    }
                    strncpy(attrs[i].name, an, MAX_NAME - 1);
                    attrs[i].name[MAX_NAME - 1] = '\0';
                    attrs[i].domain = (char)toupper((unsigned char)d);
                }

                if (!is_reserved_relation(name) && find_relation(name) == NULL)
                {
                    create_relation(name, 1, 0, attrs, n, k);
                }
            }
        }
        else if (strcmp(verb, "DE") == 0)
        {
            // DE command removes one relation and its physical blocks.
            char name[MAX_NAME];
            if (sscanf(line, "DE %31s", name) == 1)
            {
                delete_relation(name);
            }
        }
        else if (strcmp(verb, "IN") == 0)
        {
            // IN command reads n tuple payload lines.
            char name[MAX_NAME];
            int n;
            if (sscanf(line, "IN %31s %d", name, &n) == 2)
            {
                do_insert(name, n, in);
            }
        }
        else if (strcmp(verb, "RM") == 0)
        {
            // RM command reads n key lines.
            char name[MAX_NAME];
            int n;
            if (sscanf(line, "RM %31s %d", name, &n) == 2)
            {
                do_remove(name, n, in);
            }
        }
        else if (strcmp(verb, "UP") == 0)
        {
            // UP command reads n replacement tuple lines.
            char name[MAX_NAME];
            int n;
            if (sscanf(line, "UP %31s %d", name, &n) == 2)
            {
                do_update(name, n, in);
            }
        }
        else if (strcmp(verb, "PR") == 0)
        {
            // PR command prints relation name, schema, and tuples.
            char name[MAX_NAME];
            if (sscanf(line, "PR %31s", name) == 1)
            {
                do_print(name);
            }
        }
        else if (strcmp(verb, "PJ") == 0)
        {
            char p[MAX_NAME], q[MAX_NAME];
            int n;
            if (sscanf(line, "PJ %31s %31s %d", p, q, &n) == 3)
            {
                do_project(p, q, n, in);
            }
        }
        else if (strcmp(verb, "SL") == 0)
        {
            char p[MAX_NAME], q[MAX_NAME];
            int n;
            if (sscanf(line, "SL %31s %31s %d", p, q, &n) == 3)
            {
                do_select(p, q, n, in);
            }
        }
        else if (strcmp(verb, "UN") == 0)
        {
            // UN command materializes union(p, q) into r.
            char p[MAX_NAME], q[MAX_NAME], r[MAX_NAME];
            if (sscanf(line, "UN %31s %31s %31s", p, q, r) == 3)
            {
                do_union(p, q, r);
            }
        }
        else if (strcmp(verb, "DF") == 0)
        {
            // DF command materializes p - q into r.
            char p[MAX_NAME], q[MAX_NAME], r[MAX_NAME];
            if (sscanf(line, "DF %31s %31s %31s", p, q, r) == 3)
            {
                do_difference(p, q, r);
            }
        }
        else if (strcmp(verb, "NJ") == 0)
        {
            // NJ command joins p and q on named attributes into r.
            char p[MAX_NAME], q[MAX_NAME], r[MAX_NAME];
            int n;
            if (sscanf(line, "NJ %31s %31s %31s %d", p, q, r, &n) == 4)
            {
                do_natural_join(p, q, r, n, in);
            }
        }
    }
}

int main(void)
{
    // Initialize the DB system, then process command stream until EOF.
    init_system();
    process_commands(stdin);
    return 0;
}
