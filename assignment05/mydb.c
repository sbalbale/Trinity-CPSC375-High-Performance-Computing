/*
 * File: mydb.c
 * Purpose: Simple database implementation
 *         This program simulates a basic in-memory database with a disk bitmap and
 *         a catalog of relations. It supports creating/deleting relations, inserting/removing/updating tuples,
 *         and relational algebra operations like projection, selection, union, difference, and natural join.
 * Author: Sean Balbale
 * Date: 3/25/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* --- System Constants (from assignment spec) --- */
#define RECSIZE 63
#define BLKSIZE 256
#define BLKFAC 4
#define DISKSIZE 256
#define BLKMAX 16
#define OVBLKMAX 4

#define MAX_REL 64
#define MAX_ATTR 10
#define MAX_NAME 32
#define MAX_VAL 64 /* value text storage */
#define LINE_BUF 512

typedef struct
{
    /* Attribute identifier and domain code ('S' or 'I'). */
    char name[MAX_NAME];
    char domain; /* 'S' or 'I' */
} attr_info_t;

typedef struct
{
    /* Tuple payload stored as tokenized string values by position. */
    char values[MAX_ATTR][MAX_VAL];
} tuple_t;

typedef struct
{
    /* Slot state in the global relation registry. */
    int in_use;
    int is_base;       /* 1=base, 0=derived */
    int is_dictionary; /* catalog or columns */

    char name[MAX_NAME];
    int num_attrs;
    int keysize;
    attr_info_t attrs[MAX_ATTR];

    tuple_t *rows;
    int row_count;
    int row_cap;

    int relptr; /* simulated header block pointer */
} relation_t;

typedef struct
{
    /* Pre-parsed selection predicate: attribute index, operator, literal value. */
    int attr_idx;
    char op[3];
    char value[MAX_VAL];
} condition_t;

/* --- Simulated disk bitmap --- */
static unsigned char bitmapblk[DISKSIZE];

/* --- In-memory catalog of relations --- */
static relation_t g_relations[MAX_REL];

/* --- Forward declarations --- */
static void init_system(void);
static int allocate_block(void);
static void free_block(int b);

static int find_relation_index(const char *name);
static relation_t *find_relation(const char *name);
static int is_reserved_name(const char *name);
static int relation_exists(const char *name);

static int create_relation_internal(const char *name, int is_base, attr_info_t *attrs, int num_attrs,
                                    int keysize, int is_dictionary, int add_to_dictionary);
static int delete_relation_internal(const char *name, int update_dictionary);

static void ensure_capacity(relation_t *rel, int need);
static int tuple_equal(const relation_t *rel, const tuple_t *a, const tuple_t *b);
static int tuple_exists(const relation_t *rel, const tuple_t *t);

static int parse_fields(const char *line, char out[][MAX_VAL], int max_fields);
static void trim_newline(char *s);

static int tuple_from_line(relation_t *rel, const char *line, tuple_t *out);
static int find_row_by_key(const relation_t *rel, char key_vals[][MAX_VAL], int key_count);

static void dict_add_relation_row(const relation_t *rel);
static void dict_add_columns_rows(const relation_t *rel);
static void dict_remove_relation_rows(const char *relname);
static void dict_remove_columns_rows(const char *relname);
static void dict_update_relsize(const relation_t *rel);

static int do_create(const char *name, int n_attrs, int keysize, attr_info_t *attrs);
static int do_delete(const char *name);
static int do_insert(const char *name, int n, FILE *in);
static int do_remove(const char *name, int n, FILE *in);
static int do_update(const char *name, int n, FILE *in);
static int do_print(const char *name);
static int do_project(const char *src, const char *dst, int n, FILE *in);
static int do_select(const char *src, const char *dst, int n, FILE *in);
static int do_union(const char *p, const char *q, const char *r);
static int do_difference(const char *p, const char *q, const char *r);
static int do_natural_join(const char *p, const char *q, const char *r, int n, FILE *in);

/* --- Disk helpers --- */
/* Allocates one simulated block by setting the bitmap entry to 1. */
static int allocate_block(void)
{
    for (int i = 0; i < DISKSIZE; i++)
    {
        // Scan bitmap for the first free simulated disk block.
        if (bitmapblk[i] == 0)
        {
            bitmapblk[i] = 1;
            return i;
        }
    }
    return -1;
}

/* Frees a simulated block by clearing the bitmap entry. */
static void free_block(int b)
{
    if (b >= 0 && b < DISKSIZE)
    {
        bitmapblk[b] = 0;
    }
}

/* --- Utility --- */
/* Removes trailing CR/LF so input lines can be parsed safely. */
static void trim_newline(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
    {
        s[n - 1] = '\0';
        n--;
    }
}

/* Splits a line by whitespace into positional fields. */
static int parse_fields(const char *line, char out[][MAX_VAL], int max_fields)
{
    char tmp[LINE_BUF];
    int count = 0;
    char *tok;

    strncpy(tmp, line, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    // Split on assignment whitespace separators.
    tok = strtok(tmp, " \t\r\n");
    while (tok != NULL && count < max_fields)
    {
        strncpy(out[count], tok, MAX_VAL - 1);
        out[count][MAX_VAL - 1] = '\0';
        count++;
        tok = strtok(NULL, " \t\r\n");
    }

    return count;
}

/* Returns non-zero when s is a valid integer literal. */
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

/* Ensures relation row storage is large enough for need tuples. */
static void ensure_capacity(relation_t *rel, int need)
{
    if (need <= rel->row_cap)
    {
        return;
    }

    int new_cap = (rel->row_cap == 0) ? 16 : rel->row_cap;
    while (new_cap < need)
    {
        // Grow geometrically to keep amortized append cost low.
        new_cap *= 2;
    }

    tuple_t *new_rows = (tuple_t *)realloc(rel->rows, (size_t)new_cap * sizeof(tuple_t));
    if (new_rows == NULL)
    {
        fprintf(stderr, "ERROR: out of memory\n");
        exit(1);
    }

    rel->rows = new_rows;
    rel->row_cap = new_cap;
}

/* Compares two tuples value-by-value using relation arity. */
static int tuple_equal(const relation_t *rel, const tuple_t *a, const tuple_t *b)
{
    for (int i = 0; i < rel->num_attrs; i++)
    {
        if (strcmp(a->values[i], b->values[i]) != 0)
        {
            return 0;
        }
    }
    return 1;
}

/* Checks whether tuple t already exists in relation rel. */
static int tuple_exists(const relation_t *rel, const tuple_t *t)
{
    for (int i = 0; i < rel->row_count; i++)
    {
        if (tuple_equal(rel, &rel->rows[i], t))
        {
            return 1;
        }
    }
    return 0;
}

/* Parses a tuple text line and validates per-attribute domain constraints. */
static int tuple_from_line(relation_t *rel, const char *line, tuple_t *out)
{
    char fields[MAX_ATTR][MAX_VAL];
    int got = parse_fields(line, fields, MAX_ATTR);

    if (got != rel->num_attrs)
    {
        return 0;
    }

    for (int i = 0; i < rel->num_attrs; i++)
    {
        if (rel->attrs[i].domain == 'I' && !str_is_int(fields[i]))
        {
            return 0;
        }
        strncpy(out->values[i], fields[i], MAX_VAL - 1);
        out->values[i][MAX_VAL - 1] = '\0';
    }

    return 1;
}

/* Finds tuple index by matching all key attributes; returns -1 if not found. */
static int find_row_by_key(const relation_t *rel, char key_vals[][MAX_VAL], int key_count)
{
    // Key arity must match the relation's primary-key width.
    if (key_count != rel->keysize)
    {
        return -1;
    }

    for (int i = 0; i < rel->row_count; i++)
    {
        // Compare all key components in order.
        int match = 1;
        for (int k = 0; k < rel->keysize; k++)
        {
            if (strcmp(rel->rows[i].values[k], key_vals[k]) != 0)
            {
                match = 0;
                break;
            }
        }
        if (match)
        {
            return i;
        }
    }

    return -1;
}

/* --- Relation registry --- */
/* Returns registry index for a relation name, or -1 when absent. */
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

/* Returns a pointer to a relation by name, or NULL when absent. */
static relation_t *find_relation(const char *name)
{
    int idx = find_relation_index(name);
    if (idx < 0)
    {
        return NULL;
    }
    return &g_relations[idx];
}

/* Convenience existence check for relation name. */
static int relation_exists(const char *name)
{
    return find_relation_index(name) >= 0;
}

/* Disallows user creation/redefinition of dictionary relation names. */
static int is_reserved_name(const char *name)
{
    return (strcmp(name, "catalog") == 0 || strcmp(name, "columns") == 0);
}

/*
 * Creates and registers a relation object.
 * Optionally writes metadata rows into catalog/columns dictionaries.
 */
static int create_relation_internal(const char *name, int is_base, attr_info_t *attrs, int num_attrs,
                                    int keysize, int is_dictionary, int add_to_dictionary)
{
    if (relation_exists(name))
    {
        return 0;
    }

    // Find an unused slot in the global relation registry.
    int idx = -1;
    for (int i = 0; i < MAX_REL; i++)
    {
        if (!g_relations[i].in_use)
        {
            idx = i;
            break;
        }
    }
    if (idx < 0)
    {
        return 0;
    }

    // Allocate one simulated header block for this relation.
    int ptr = allocate_block();
    if (ptr < 0)
    {
        return 0;
    }

    relation_t *r = &g_relations[idx];
    memset(r, 0, sizeof(*r));

    r->in_use = 1;
    r->is_base = is_base;
    r->is_dictionary = is_dictionary;
    strncpy(r->name, name, MAX_NAME - 1);
    r->name[MAX_NAME - 1] = '\0';
    r->num_attrs = num_attrs;
    r->keysize = keysize;
    r->relptr = ptr;
    for (int i = 0; i < num_attrs; i++)
    {
        r->attrs[i] = attrs[i];
    }

    if (add_to_dictionary)
    {
        dict_add_relation_row(r);
        dict_add_columns_rows(r);
    }

    return 1;
}

/* Deletes one non-dictionary relation and optionally cleans dictionary rows. */
static int delete_relation_internal(const char *name, int update_dictionary)
{
    int idx = find_relation_index(name);
    if (idx < 0)
    {
        return 0;
    }

    relation_t *rel = &g_relations[idx];

    if (rel->is_dictionary)
    {
        return 0;
    }

    if (update_dictionary)
    {
        dict_remove_relation_rows(rel->name);
        dict_remove_columns_rows(rel->name);
    }

    free(rel->rows);
    rel->rows = NULL;
    rel->row_count = 0;
    rel->row_cap = 0;

    free_block(rel->relptr);
    memset(rel, 0, sizeof(*rel));

    return 1;
}

/* --- Data dictionary maintenance --- */
/* Adds one tuple to catalog describing a relation's metadata. */
static void dict_add_relation_row(const relation_t *rel)
{
    relation_t *catalog = find_relation("catalog");
    if (catalog == NULL)
    {
        return;
    }

    tuple_t row;
    // catalog schema order: Relname Kind Attsize Keysize Relsize Relptr.
    snprintf(row.values[0], MAX_VAL, "%s", rel->name);
    snprintf(row.values[1], MAX_VAL, "%d", rel->is_base ? 0 : 1);
    snprintf(row.values[2], MAX_VAL, "%d", rel->num_attrs);
    snprintf(row.values[3], MAX_VAL, "%d", rel->keysize);
    snprintf(row.values[4], MAX_VAL, "%d", rel->row_count);
    snprintf(row.values[5], MAX_VAL, "%d", rel->relptr);

    ensure_capacity(catalog, catalog->row_count + 1);
    catalog->rows[catalog->row_count++] = row;
}

/* Adds one tuple per attribute into columns dictionary. */
static void dict_add_columns_rows(const relation_t *rel)
{
    relation_t *columns = find_relation("columns");
    if (columns == NULL)
    {
        return;
    }

    for (int i = 0; i < rel->num_attrs; i++)
    {
        tuple_t row;
        // columns schema order: Relname Attname Attdomain Attposition.
        snprintf(row.values[0], MAX_VAL, "%s", rel->name);
        snprintf(row.values[1], MAX_VAL, "%s", rel->attrs[i].name);
        snprintf(row.values[2], MAX_VAL, "%d", rel->attrs[i].domain == 'S' ? 0 : 1);
        snprintf(row.values[3], MAX_VAL, "%d", i + 1);

        ensure_capacity(columns, columns->row_count + 1);
        columns->rows[columns->row_count++] = row;
    }
}

/* Removes all catalog tuples for a relation name. */
static void dict_remove_relation_rows(const char *relname)
{
    relation_t *catalog = find_relation("catalog");
    if (catalog == NULL)
    {
        return;
    }

    int w = 0;
    // Compact rows in place while skipping matching relation name.
    for (int i = 0; i < catalog->row_count; i++)
    {
        if (strcmp(catalog->rows[i].values[0], relname) != 0)
        {
            if (w != i)
            {
                catalog->rows[w] = catalog->rows[i];
            }
            w++;
        }
    }
    catalog->row_count = w;
}

/* Removes all columns tuples for a relation name. */
static void dict_remove_columns_rows(const char *relname)
{
    relation_t *columns = find_relation("columns");
    if (columns == NULL)
    {
        return;
    }

    int w = 0;
    // Compact rows in place while skipping matching relation name.
    for (int i = 0; i < columns->row_count; i++)
    {
        if (strcmp(columns->rows[i].values[0], relname) != 0)
        {
            if (w != i)
            {
                columns->rows[w] = columns->rows[i];
            }
            w++;
        }
    }
    columns->row_count = w;
}

/* Updates catalog.Relsize for a relation after tuple-count changes. */
static void dict_update_relsize(const relation_t *rel)
{
    relation_t *catalog = find_relation("catalog");
    if (catalog == NULL)
    {
        return;
    }

    for (int i = 0; i < catalog->row_count; i++)
    {
        if (strcmp(catalog->rows[i].values[0], rel->name) == 0)
        {
            snprintf(catalog->rows[i].values[4], MAX_VAL, "%d", rel->row_count);
            return;
        }
    }
}

/* --- Command implementations --- */
/* Implements CR: create a new base relation and dictionary entries. */
static int do_create(const char *name, int n_attrs, int keysize, attr_info_t *attrs)
{
    // Reject reserved names and duplicate relation names.
    if (is_reserved_name(name) || relation_exists(name))
    {
        return 0;
    }

    // Validate schema and key constraints.
    if (n_attrs <= 0 || n_attrs > MAX_ATTR || keysize <= 0 || keysize > n_attrs)
    {
        return 0;
    }

    return create_relation_internal(name, 1, attrs, n_attrs, keysize, 0, 1);
}

/* Implements DE: delete an existing non-dictionary relation. */
static int do_delete(const char *name)
{
    // Delete relation and prune dictionary entries.
    return delete_relation_internal(name, 1);
}

/*
 * Implements IN: insert n tuples into a base relation.
 * Duplicate primary keys are ignored to enforce key uniqueness.
 */
static int do_insert(const char *name, int n, FILE *in)
{
    relation_t *rel = find_relation(name);
    char line[LINE_BUF];

    if (rel == NULL || !rel->is_base || rel->is_dictionary)
    {
        // Consume tuple payload lines even when command cannot execute.
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
        tuple_t t;
        int duplicate = 0;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            break;
        }
        trim_newline(line);

        // Skip malformed tuples or domain mismatches.
        if (!tuple_from_line(rel, line, &t))
        {
            continue;
        }

        // Enforce primary-key uniqueness using leading key attributes.
        for (int r = 0; r < rel->row_count && !duplicate; r++)
        {
            int same_key = 1;
            for (int k = 0; k < rel->keysize; k++)
            {
                if (strcmp(rel->rows[r].values[k], t.values[k]) != 0)
                {
                    same_key = 0;
                    break;
                }
            }
            if (same_key)
            {
                duplicate = 1;
            }
        }

        if (duplicate)
        {
            continue;
        }

        ensure_capacity(rel, rel->row_count + 1);
        rel->rows[rel->row_count++] = t;
    }

    dict_update_relsize(rel);
    return 1;
}

/* Implements RM: remove n tuples addressed by primary key values. */
static int do_remove(const char *name, int n, FILE *in)
{
    relation_t *rel = find_relation(name);
    char line[LINE_BUF];

    if (rel == NULL || !rel->is_base || rel->is_dictionary)
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
        char keys[MAX_ATTR][MAX_VAL];
        int kcount;
        int pos;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            break;
        }
        trim_newline(line);

        kcount = parse_fields(line, keys, MAX_ATTR);
        // Only remove if the provided key exists.
        pos = find_row_by_key(rel, keys, kcount);
        if (pos < 0)
        {
            continue;
        }

        // Shift subsequent tuples left to fill deleted slot.
        for (int j = pos + 1; j < rel->row_count; j++)
        {
            rel->rows[j - 1] = rel->rows[j];
        }
        rel->row_count--;
    }

    dict_update_relsize(rel);
    return 1;
}

/* Implements UP: replace existing tuples identified by primary key. */
static int do_update(const char *name, int n, FILE *in)
{
    relation_t *rel = find_relation(name);
    char line[LINE_BUF];

    if (rel == NULL || !rel->is_base || rel->is_dictionary)
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
        tuple_t t;
        char keys[MAX_ATTR][MAX_VAL];
        int pos;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            break;
        }
        trim_newline(line);

        if (!tuple_from_line(rel, line, &t))
        {
            continue;
        }

        // Extract key values from incoming tuple for lookup.
        for (int k = 0; k < rel->keysize; k++)
        {
            strncpy(keys[k], t.values[k], MAX_VAL - 1);
            keys[k][MAX_VAL - 1] = '\0';
        }

        pos = find_row_by_key(rel, keys, rel->keysize);
        if (pos < 0)
        {
            continue;
        }

        // Replace the matched tuple in place.
        rel->rows[pos] = t;
    }

    dict_update_relsize(rel);
    return 1;
}

/* Implements PR: print relation name, heading row, then tuple rows. */
static int do_print(const char *name)
{
    relation_t *rel = find_relation(name);
    if (rel == NULL)
    {
        return 0;
    }

    // Print relation name first.
    printf("%s\n", rel->name);
    // Print header row with attribute names.
    for (int i = 0; i < rel->num_attrs; i++)
    {
        if (i > 0)
        {
            printf(" ");
        }
        printf("%s", rel->attrs[i].name);
    }
    printf("\n");

    // Print each tuple on its own line.
    for (int r = 0; r < rel->row_count; r++)
    {
        for (int c = 0; c < rel->num_attrs; c++)
        {
            if (c > 0)
            {
                printf(" ");
            }
            printf("%s", rel->rows[r].values[c]);
        }
        printf("\n");
    }
    printf("\n");

    return 1;
}

/* Resolves attribute name to positional index in a relation schema. */
static int find_attr_index(const relation_t *rel, const char *name)
{
    for (int i = 0; i < rel->num_attrs; i++)
    {
        if (strcmp(rel->attrs[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

/* Implements PJ: project selected attributes and suppress duplicates. */
static int do_project(const char *src, const char *dst, int n, FILE *in)
{
    relation_t *p = find_relation(src);
    char line[LINE_BUF];
    char proj_names[MAX_ATTR][MAX_NAME];
    int proj_idx[MAX_ATTR];
    attr_info_t out_attrs[MAX_ATTR];

    if (p == NULL || relation_exists(dst) || is_reserved_name(dst) || n <= 0 || n > p->num_attrs)
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

    // Read the requested projection attributes.
    for (int i = 0; i < n; i++)
    {
        if (fgets(line, sizeof(line), in) == NULL)
        {
            return 0;
        }
        trim_newline(line);
        strncpy(proj_names[i], line, MAX_NAME - 1);
        proj_names[i][MAX_NAME - 1] = '\0';
    }

    // Resolve each projection attribute to its source column index.
    for (int i = 0; i < n; i++)
    {
        proj_idx[i] = find_attr_index(p, proj_names[i]);
        if (proj_idx[i] < 0)
        {
            return 0;
        }
        out_attrs[i] = p->attrs[proj_idx[i]];
    }

    if (!create_relation_internal(dst, 0, out_attrs, n, n, 0, 1))
    {
        return 0;
    }

    relation_t *q = find_relation(dst);
    if (q == NULL)
    {
        return 0;
    }

    for (int i = 0; i < p->row_count; i++)
    {
        tuple_t t;
        for (int c = 0; c < n; c++)
        {
            strncpy(t.values[c], p->rows[i].values[proj_idx[c]], MAX_VAL - 1);
            t.values[c][MAX_VAL - 1] = '\0';
        }
        // Projection result is a set, so suppress duplicates.
        if (!tuple_exists(q, &t))
        {
            ensure_capacity(q, q->row_count + 1);
            q->rows[q->row_count++] = t;
        }
    }

    dict_update_relsize(q);
    return 1;
}

/* Evaluates one comparison operation for either integer or string domain. */
static int compare_value(char domain, const char *lhs, const char *op, const char *rhs)
{
    if (domain == 'I')
    {
        // Integer comparisons should use numeric values, not lexical order.
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

    // String comparisons use lexical order for inequalities.
    int cmp = strcmp(lhs, rhs);
    if (strcmp(op, "==") == 0)
        return cmp == 0;
    if (strcmp(op, "!=") == 0)
        return cmp != 0;
    if (strcmp(op, ">") == 0)
        return cmp > 0;
    if (strcmp(op, ">=") == 0)
        return cmp >= 0;
    if (strcmp(op, "<") == 0)
        return cmp < 0;
    if (strcmp(op, "<=") == 0)
        return cmp <= 0;
    return 0;
}

/* Implements SL: selection by conjunction of n predicates. */
static int do_select(const char *src, const char *dst, int n, FILE *in)
{
    relation_t *p = find_relation(src);
    char line[LINE_BUF];
    condition_t conds[MAX_ATTR];

    if (p == NULL || relation_exists(dst) || is_reserved_name(dst) || n < 0 || n > MAX_ATTR)
    {
        // Keep stream aligned by consuming all predicate lines.
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    // Parse all predicates before scanning tuples.
    for (int i = 0; i < n; i++)
    {
        char att[MAX_NAME], op[3], val[MAX_VAL];
        if (fgets(line, sizeof(line), in) == NULL)
        {
            return 0;
        }
        trim_newline(line);
        if (sscanf(line, "%31s %2s %63s", att, op, val) != 3)
        {
            return 0;
        }
        conds[i].attr_idx = find_attr_index(p, att);
        if (conds[i].attr_idx < 0)
        {
            return 0;
        }
        strncpy(conds[i].op, op, sizeof(conds[i].op) - 1);
        conds[i].op[sizeof(conds[i].op) - 1] = '\0';
        strncpy(conds[i].value, val, MAX_VAL - 1);
        conds[i].value[MAX_VAL - 1] = '\0';
    }

    if (!create_relation_internal(dst, 0, p->attrs, p->num_attrs, p->num_attrs, 0, 1))
    {
        return 0;
    }

    relation_t *q = find_relation(dst);
    if (q == NULL)
    {
        return 0;
    }

    // Keep tuple only when all predicates in the conjunction are true.
    for (int i = 0; i < p->row_count; i++)
    {
        int ok = 1;
        for (int c = 0; c < n; c++)
        {
            int idx = conds[c].attr_idx;
            char domain = p->attrs[idx].domain;
            if (!compare_value(domain, p->rows[i].values[idx], conds[c].op, conds[c].value))
            {
                ok = 0;
                break;
            }
        }
        if (ok)
        {
            ensure_capacity(q, q->row_count + 1);
            q->rows[q->row_count++] = p->rows[i];
        }
    }

    dict_update_relsize(q);
    return 1;
}

/*
 * Checks union/difference compatibility and builds q->p attribute mapping.
 * Mapping enables same-name attributes in different column orders.
 */
static int compatible_relations(const relation_t *p, const relation_t *q, int map_q_to_p[])
{
    // Relations must have identical number of attributes.
    if (p->num_attrs != q->num_attrs)
    {
        return 0;
    }

    for (int i = 0; i < p->num_attrs; i++)
    {
        // Match by attribute name so different orders are allowed.
        int idx = find_attr_index(q, p->attrs[i].name);
        if (idx < 0)
        {
            return 0;
        }
        if (q->attrs[idx].domain != p->attrs[i].domain)
        {
            return 0;
        }
        map_q_to_p[i] = idx;
    }

    return 1;
}

/* Implements UN: set union of compatible relations with duplicate suppression. */
static int do_union(const char *pname, const char *qname, const char *rname)
{
    relation_t *p = find_relation(pname);
    relation_t *q = find_relation(qname);
    int qmap[MAX_ATTR];

    if (p == NULL || q == NULL || relation_exists(rname) || is_reserved_name(rname))
    {
        return 0;
    }
    if (!compatible_relations(p, q, qmap))
    {
        return 0;
    }

    if (!create_relation_internal(rname, 0, p->attrs, p->num_attrs, p->num_attrs, 0, 1))
    {
        return 0;
    }

    relation_t *r = find_relation(rname);
    if (r == NULL)
    {
        return 0;
    }

    // Seed result with all tuples from p.
    for (int i = 0; i < p->row_count; i++)
    {
        if (!tuple_exists(r, &p->rows[i]))
        {
            ensure_capacity(r, r->row_count + 1);
            r->rows[r->row_count++] = p->rows[i];
        }
    }

    // Reorder q into p's schema order before duplicate checks.
    for (int i = 0; i < q->row_count; i++)
    {
        tuple_t t;
        for (int c = 0; c < p->num_attrs; c++)
        {
            strncpy(t.values[c], q->rows[i].values[qmap[c]], MAX_VAL - 1);
            t.values[c][MAX_VAL - 1] = '\0';
        }
        if (!tuple_exists(r, &t))
        {
            ensure_capacity(r, r->row_count + 1);
            r->rows[r->row_count++] = t;
        }
    }

    dict_update_relsize(r);
    return 1;
}

/* Implements DF: set difference p - q for compatible relations. */
static int do_difference(const char *pname, const char *qname, const char *rname)
{
    relation_t *p = find_relation(pname);
    relation_t *q = find_relation(qname);
    int qmap[MAX_ATTR];

    if (p == NULL || q == NULL || relation_exists(rname) || is_reserved_name(rname))
    {
        return 0;
    }
    if (!compatible_relations(p, q, qmap))
    {
        return 0;
    }

    if (!create_relation_internal(rname, 0, p->attrs, p->num_attrs, p->num_attrs, 0, 1))
    {
        return 0;
    }

    relation_t *r = find_relation(rname);
    if (r == NULL)
    {
        return 0;
    }

    // Keep only tuples present in p but absent in q.
    for (int i = 0; i < p->row_count; i++)
    {
        int found = 0;
        for (int j = 0; j < q->row_count && !found; j++)
        {
            int same = 1;
            for (int c = 0; c < p->num_attrs; c++)
            {
                if (strcmp(p->rows[i].values[c], q->rows[j].values[qmap[c]]) != 0)
                {
                    same = 0;
                    break;
                }
            }
            if (same)
            {
                found = 1;
            }
        }
        if (!found)
        {
            ensure_capacity(r, r->row_count + 1);
            r->rows[r->row_count++] = p->rows[i];
        }
    }

    dict_update_relsize(r);
    return 1;
}

/*
 * Implements NJ: natural join using caller-provided common attribute names.
 * Result keeps all attributes of p first, then non-join attributes of q.
 */
static int do_natural_join(const char *pname, const char *qname, const char *rname, int n, FILE *in)
{
    relation_t *p = find_relation(pname);
    relation_t *q = find_relation(qname);
    char line[LINE_BUF];
    int p_join_idx[MAX_ATTR];
    int q_join_idx[MAX_ATTR];
    int q_use[MAX_ATTR] = {0};
    attr_info_t out_attrs[MAX_ATTR];
    int out_n = 0;

    if (p == NULL || q == NULL || relation_exists(rname) || is_reserved_name(rname) || n < 0 || n > MAX_ATTR)
    {
        // Keep stream aligned by consuming join-attribute lines.
        for (int i = 0; i < n; i++)
        {
            if (fgets(line, sizeof(line), in) == NULL)
            {
                break;
            }
        }
        return 0;
    }

    // Resolve join attributes in both schemas and validate compatibility.
    for (int i = 0; i < n; i++)
    {
        char aname[MAX_NAME];
        int pi, qi;

        if (fgets(line, sizeof(line), in) == NULL)
        {
            return 0;
        }
        trim_newline(line);
        strncpy(aname, line, MAX_NAME - 1);
        aname[MAX_NAME - 1] = '\0';

        pi = find_attr_index(p, aname);
        qi = find_attr_index(q, aname);
        if (pi < 0 || qi < 0)
        {
            return 0;
        }
        if (p->attrs[pi].domain != q->attrs[qi].domain)
        {
            return 0;
        }

        p_join_idx[i] = pi;
        q_join_idx[i] = qi;
        q_use[qi] = 1;
    }

    // Output layout starts with every attribute from p.
    for (int i = 0; i < p->num_attrs; i++)
    {
        out_attrs[out_n++] = p->attrs[i];
    }
    // Then append only non-join attributes from q.
    for (int i = 0; i < q->num_attrs; i++)
    {
        if (!q_use[i])
        {
            out_attrs[out_n++] = q->attrs[i];
        }
    }

    if (!create_relation_internal(rname, 0, out_attrs, out_n, out_n, 0, 1))
    {
        return 0;
    }

    relation_t *r = find_relation(rname);
    if (r == NULL)
    {
        return 0;
    }

    // Nested-loop join across tuples from p and q.
    for (int i = 0; i < p->row_count; i++)
    {
        for (int j = 0; j < q->row_count; j++)
        {
            int ok = 1;
            tuple_t t;
            int c = 0;

            // Join succeeds only when all named join attributes match.
            for (int k = 0; k < n; k++)
            {
                if (strcmp(p->rows[i].values[p_join_idx[k]], q->rows[j].values[q_join_idx[k]]) != 0)
                {
                    ok = 0;
                    break;
                }
            }
            if (!ok)
            {
                // This tuple pair does not satisfy the join condition.
                continue;
            }

            for (int a = 0; a < p->num_attrs; a++)
            {
                strncpy(t.values[c], p->rows[i].values[a], MAX_VAL - 1);
                t.values[c][MAX_VAL - 1] = '\0';
                c++;
            }
            for (int a = 0; a < q->num_attrs; a++)
            {
                if (!q_use[a])
                {
                    strncpy(t.values[c], q->rows[j].values[a], MAX_VAL - 1);
                    t.values[c][MAX_VAL - 1] = '\0';
                    c++;
                }
            }

            ensure_capacity(r, r->row_count + 1);
            r->rows[r->row_count++] = t;
        }
    }

    dict_update_relsize(r);
    return 1;
}

/* --- Initialization --- */
/* Bootstraps bitmap and initializes dictionary relations with self-descriptions. */
static void init_system(void)
{
    // Initialize bitmap and relation registry to all-zero state.
    memset(bitmapblk, 0, sizeof(bitmapblk));
    memset(g_relations, 0, sizeof(g_relations));

    /* Reserve first three blocks to follow assignment layout. */
    bitmapblk[0] = 1;
    bitmapblk[1] = 1;
    bitmapblk[2] = 1;

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

    create_relation_internal("catalog", 1, cat_attrs, 6, 1, 1, 0);
    create_relation_internal("columns", 1, col_attrs, 4, 2, 1, 0);

    relation_t *catalog = find_relation("catalog");
    relation_t *columns = find_relation("columns");

    // Add dictionary self-descriptions into catalog and columns.
    if (catalog != NULL)
    {
        dict_add_relation_row(catalog);
    }
    if (columns != NULL)
    {
        dict_add_relation_row(columns);
    }
    if (catalog != NULL)
    {
        dict_add_columns_rows(catalog);
    }
    if (columns != NULL)
    {
        dict_add_columns_rows(columns);
    }

    if (catalog != NULL)
    {
        dict_update_relsize(catalog);
    }
    if (columns != NULL)
    {
        dict_update_relsize(columns);
    }
}

/* --- Command parser --- */
/* Main command loop: parses each verb line and dispatches to handlers. */
static void process_commands(FILE *in)
{
    char line[LINE_BUF];

    // Process command stream line-by-line until EOF.
    while (fgets(line, sizeof(line), in) != NULL)
    {
        char verb[3] = {0};
        trim_newline(line);

        if (line[0] == '\0')
        {
            // Ignore empty lines.
            continue;
        }

        if (sscanf(line, "%2s", verb) != 1)
        {
            // Ignore malformed command lines.
            continue;
        }

        // Dispatch to the command handler by two-letter verb.
        if (strcmp(verb, "CR") == 0)
        {
            char name[MAX_NAME];
            int n, k;
            if (sscanf(line, "CR %31s %d %d", name, &n, &k) == 3)
            {
                attr_info_t attrs[MAX_ATTR];
                // CR is followed by n schema lines: <name> <domain>.
                for (int i = 0; i < n; i++)
                {
                    char a_line[LINE_BUF];
                    char aname[MAX_NAME], d;
                    if (fgets(a_line, sizeof(a_line), in) == NULL)
                    {
                        break;
                    }
                    trim_newline(a_line);
                    if (sscanf(a_line, "%31s %c", aname, &d) != 2)
                    {
                        continue;
                    }
                    attrs[i].domain = (char)toupper((unsigned char)d);
                    strncpy(attrs[i].name, aname, MAX_NAME - 1);
                    attrs[i].name[MAX_NAME - 1] = '\0';
                }
                do_create(name, n, k, attrs);
            }
        }
        else if (strcmp(verb, "DE") == 0)
        {
            char name[MAX_NAME];
            if (sscanf(line, "DE %31s", name) == 1)
            {
                do_delete(name);
            }
        }
        else if (strcmp(verb, "IN") == 0)
        {
            char name[MAX_NAME];
            int n;
            if (sscanf(line, "IN %31s %d", name, &n) == 2)
            {
                do_insert(name, n, in);
            }
        }
        else if (strcmp(verb, "RM") == 0)
        {
            char name[MAX_NAME];
            int n;
            if (sscanf(line, "RM %31s %d", name, &n) == 2)
            {
                do_remove(name, n, in);
            }
        }
        else if (strcmp(verb, "UP") == 0)
        {
            char name[MAX_NAME];
            int n;
            if (sscanf(line, "UP %31s %d", name, &n) == 2)
            {
                do_update(name, n, in);
            }
        }
        else if (strcmp(verb, "PR") == 0)
        {
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
            char p[MAX_NAME], q[MAX_NAME], r[MAX_NAME];
            if (sscanf(line, "UN %31s %31s %31s", p, q, r) == 3)
            {
                do_union(p, q, r);
            }
        }
        else if (strcmp(verb, "DF") == 0)
        {
            char p[MAX_NAME], q[MAX_NAME], r[MAX_NAME];
            if (sscanf(line, "DF %31s %31s %31s", p, q, r) == 3)
            {
                do_difference(p, q, r);
            }
        }
        else if (strcmp(verb, "NJ") == 0)
        {
            char p[MAX_NAME], q[MAX_NAME], r[MAX_NAME];
            int n;
            if (sscanf(line, "NJ %31s %31s %31s %d", p, q, r, &n) == 4)
            {
                do_natural_join(p, q, r, n, in);
            }
        }
    }
}

/* Program entry point: initialize system, process stdin commands, release memory. */
int main(void)
{
    init_system();
    process_commands(stdin);

    for (int i = 0; i < MAX_REL; i++)
    {
        if (g_relations[i].in_use)
        {
            free(g_relations[i].rows);
            g_relations[i].rows = NULL;
        }
    }

    return 0;
}
