#ifndef TABLE_H
#define TABLE_H

#include "../ast.h"
#include "../common.h"
#include <sys/types.h>

struct Symbol; // forward declaration from semantic_check.h

typedef struct {
  const char *key;
  u_int32_t hash;
  struct Symbol *value;
  bool is_tombstone;
} Entry;

typedef struct Table {
  int count;
  int capacity;
  Entry *entries;
} Table;

void init_table(Table *table);
bool table_set(Table *table, const char *key, u_int32_t hash,
               struct Symbol *value);
bool table_get(Table *table, const char *key, u_int32_t hash,
               struct Symbol *value);
bool table_delete(Table *table, const char *key, u_int32_t hash);
void table_add_all(Table *from, Table *to);
void free_table(Table *table);

#endif
