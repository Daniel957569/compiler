#ifndef ARRAY_H
#define ARRAY_H

/* #include "ast.h" */
#include "../memory.h"
#include "../scanner.h"

struct AstNode; // forward declaration, lsp throws recursive error, only fix i
struct Identifier;
struct Environment;
struct Table;

// can think of.

typedef struct {
  int size;
  int capacity;
  Token *items;
} TokenArray;

typedef struct {
  int size;
  int capacity;
  struct AstNode **items;
} AstArray;

typedef struct {
  int size;
  int capacity;
  struct Identifier **items;
} IdentifierArray;

typedef struct {
  int size;
  int capacity;
  struct Environment **items;
} EnvironmentArray;

TokenArray init_token_array();
void push_token_array(Token *item, TokenArray *arr);
void free_token_array(TokenArray *arr);

AstArray *init_ast_array();
void push_ast_array(AstArray *arr, struct AstNode *item);
void free_ast_array(AstArray *arr);

IdentifierArray *init_identifier_array();
void push_identifier_array(IdentifierArray *arr, struct Identifier *identifier);
void free_identifier_array(IdentifierArray *arr);

EnvironmentArray *init_environment_array();
void push_environment_array(EnvironmentArray *arr, struct Environment *env);
void free_environment_array(EnvironmentArray *arr);

#endif
