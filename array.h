#ifndef ARRAY_H
#define ARRAY_H

/* #include "ast.h" */
#include "memory.h"
#include "scanner.h"

struct AstNode; // forward declaration, lsp throws recursive error, only fix i
// can think of.

typedef struct {
  int size;
  int capacity;
  Token *items;
} Array;

typedef struct {
  int size;
  int capacity;
  struct AstNode **items;
} AstArray;

typedef struct {
  int size;
  int capacity;
  char **items;
} StringArray;

Array initArray();
void pushArray(Token *item, Array *arr);
void freeArray(Array *arr);

AstArray *init_ast_array();
void push_ast_array(AstArray *arr, struct AstNode *item);
void free_ast_array(AstArray *arr);

StringArray *init_string_array();
void push_string_array(StringArray *arr, char *str);
void free_string_array(StringArray *arr);

#endif
