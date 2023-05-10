#ifndef TABLE_H
#define TABLE_H

#include "../ast.h"
#include "../common.h"
#include <sys/types.h>

typedef struct {
  char *key;
  u_int32_t hash;
  AstNode *value;
  bool is_tombstone;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry *entries;
} Table;

void init_table(Table *table);
bool tableSet(Table *table, char *key, u_int32_t hash, AstNode *value);
bool tableGet(Table *table, char *key, u_int32_t hash, AstNode *value);
bool tableDelete(Table *table, char *key, u_int32_t hash);
void tableAddAll(Table *from, Table *to);
void freeTable(Table *table);

#endif
