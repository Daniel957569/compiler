#ifndef AST_H
#define AST_H

#include "common.h"
#include <stddef.h>

#define AS_LIST_SIZE(program) (program)->data.declaration_list.size
#define AS_LIST_CAPACITY(program) (program)->data.declaration_list.capacity
#define AS_LIST_ELEMENTS(program) (program)->data.declaration_list.elements

typedef enum {
  AST_LITERAL,

  AST_TRUE,
  AST_FALSE,

  AST_ADD,
  AST_SUBTRACT,
  AST_MULTIPLY,
  AST_DIVIDE,

  AST_EQUAL,
  AST_EQUAL_EQUAL,

  AST_BANG,
  AST_BANG_EQUAL,

  AST_GREATER,
  AST_GREATER_EQUAL,

  AST_LESS,
  AST_LESS_EQUAL,

  AST_NEGATE,

  AST_LET_DECL,
  AST_ASSIGN,

  AST_IF_STMT,

  AST_PROGRAM,
  AST_BLOCK,
} AstNodeType;

typedef struct AstNode {
  AstNodeType type;
  union {
    double value; // AST_LITERAL
    bool boolean; // AST_TRUE AST_FALSE
    struct {      // AST_ADD, AST_SUBTRACT, AST_MULTIPLY, AST_DIVIDE ...
      struct AstNode *left;
      struct AstNode *right;
    } binaryop;
    struct { // AST_NEGATE AST_BANG
      struct AstNode *operand;
    } unaryop;
    struct {
      const char *name;
      struct AstNode *value;
    } var_assign;
    struct {
      const char *name;
      struct AstNode *value;
    } let_decl;
    struct {
      struct AstNode *condition;
      struct AstNode *then_body;
      struct AstNode *else_body;
    } if_stmt;
    struct { // block, program_node
      struct AstNode **elements;
      size_t size;
      size_t capacity;
    } declaration_list;
  } data;
} AstNode;

AstNode *ast_create_literal(int value);
AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right);
AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand);
AstNode *ast_create_boolean(AstNodeType type, bool boolean);
AstNode *ast_create_let_decl(AstNodeType type, const char *name,
                             AstNode *value);
AstNode *ast_create_if_stmt(AstNode *condition, AstNode *then_body,
                            AstNode *else_body);

AstNode *ast_create_var_assign(AstNodeType type, const char *name,
                               AstNode *value);

AstNode *ast_create_declaration_list(AstNodeType type);
void ast_push_list_node(AstNode *program, AstNode *node);

double test_evaluate(AstNode *node);
void free_tree(AstNode *node);
void print_ast(AstNode *node, int depth);

#endif
