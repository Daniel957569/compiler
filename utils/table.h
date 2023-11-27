#ifndef TABLE_H
#define TABLE_H

#include "../ast.h"
#include "../common.h"
#include <sys/types.h>

// struct Symbol; // forward declaration from semantic_check.h
// idk how forward declaration works, it wouldn't let me access symbol data so
// its here now.

typedef enum {
  FUNCTION_TYPE,
  VARIABLE_TYPE,
  PARAMETER_TYPE,
  STRUCT_TYPE,
} SymbolType;

typedef struct Symbol {
  AstNode *value;
  int scope;
  int stack_pos;
  SymbolType type;
  DataType data_type;
} Symbol;

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
bool table_get_function(Table *table, const char *key, u_int32_t hash,
                        struct Symbol *value);
bool table_delete(Table *table, const char *key, u_int32_t hash);
bool table_contains(Table *table, const char *key, u_int32_t hash);
void table_add_all(Table *from, Table *to);
void free_table(Table *table);
uint32_t hash_string(const char *key, int length);

#endif
