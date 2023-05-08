#ifndef AST_H
#define AST_H

#include "common.h"
#include <stddef.h>

#define AS_PROGRAM_SIZE(program) (program)->data.program_node.size
#define AS_PROGRAM_CAPACITY(program) (program)->data.program_node.capacity
#define AS_PROGRAM_ELEMENTS(program) (program)->data.program_node.elements

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

  AST_PROGRAM,
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
      struct AstNode **elements;
      size_t size;
      size_t capacity;
    } program_node;
  } data;
} AstNode;

AstNode *ast_create_literal(int value);
AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right);
AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand);
AstNode *ast_create_boolean(AstNodeType type, bool boolean);
AstNode *ast_create_let_decl(AstNodeType type, const char *name,
                             AstNode *value);

AstNode *ast_create_var_assign(AstNodeType type, const char *name,
                               AstNode *value);

AstNode *ast_create_program_node();
void ast_push_program_node(AstNode *program, AstNode *node);

double test_evaluate(AstNode *node);
void free_tree(AstNode *node);
void print_ast(AstNode *node, int depth);

#endif
