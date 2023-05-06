#ifndef AST_H
#define AST_H

typedef enum {
  AST_LITERAL,
  AST_ADD,
  AST_SUBTRACT,
  AST_MULTIPLY,
  AST_DIVIDE,
  AST_NEGATE,
} AstNodeType;

typedef struct AstNode {
  AstNodeType type;
  union {
    double value; // AST_LITERAL
    struct {      // AST_ADD, AST_SUBTRACT, AST_MULTIPLY, AST_DIVIDE
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
double evaluate(AstNode *node);
void free_tree(AstNode *node);
void print_ast(AstNode *node, int depth);

#endif
