#ifndef AST_H
#define AST_H

typedef enum {
  AST_LITERAL,
  AST_ADD,
  AST_SUBTRACT,
  AST_MULTIPLY,
  AST_DIVIDE
} AstNodeType;

typedef struct AstNode {
  AstNodeType type;
  union {
    double value; // AST_LITERAL
    struct {      // AST_ADD, AST_SUBTRACT, AST_MULTIPLY, AST_DIVIDE
      struct AstNode *left;
      struct AstNode *right;
    } binaryop;
  } data;
} AstNode;

AstNode *ast_create_literal(int value);
AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right);
double evaluate(AstNode *node);
void freeTree(AstNode *node);
void printAst(AstNode *node, int depth);
#endif
