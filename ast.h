#ifndef AST_H
#define AST_H

#include "common.h"

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
    struct { // AST_NEGATE
      struct AstNode *operand;
    } unaryop;
  } data;
} AstNode;

AstNode *ast_create_literal(int value);
AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right);
AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand);
AstNode *ast_create_boolean(AstNodeType type, bool boolean);

double test_evaluate(AstNode *node);
void free_tree(AstNode *node);
void print_ast(AstNode *node, int depth);

#endif
