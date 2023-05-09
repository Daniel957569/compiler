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

Array initArray();
void pushArray(Token *item, Array *arr);
void freeArray(Array *arr);

AstArray *init_ast_array();
void push_ast_array(AstArray *arr, struct AstNode *item);
void free_ast_array(AstArray *arr);

#endif
