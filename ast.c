#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

AstNode *ast_create_literal(int value) {
  AstNode *node = malloc(sizeof(AstNode));
  node->type = AST_LITERAL;
  node->data.value = value;
  return node;
}

AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right) {
  AstNode *node = malloc(sizeof(AstNode));
  node->type = type;
  node->data.binaryop.left = left;
  node->data.binaryop.right = right;
  return node;
}

void printAst(AstNode *node, int depth) {
  if (node == NULL) {
    return;
  }

  for (int i = 0; i < depth; i++) {
    printf("  ");
  }

  switch (node->type) {
  case AST_LITERAL:
    printf("%f\n", node->data.value);
    break;
  case AST_ADD:
    printf("+\n");
    printAst(node->data.binaryop.left, depth + 1);
    printAst(node->data.binaryop.right, depth + 1);
    break;
  case AST_SUBTRACT:
    printf("-\n");
    printAst(node->data.binaryop.left, depth + 1);
    printAst(node->data.binaryop.right, depth + 1);
    break;
  case AST_MULTIPLY:
    printf("*\n");
    printAst(node->data.binaryop.left, depth + 1);
    printAst(node->data.binaryop.right, depth + 1);
    break;
  case AST_DIVIDE:
    printf("/\n");
    printAst(node->data.binaryop.left, depth + 1);
    printAst(node->data.binaryop.right, depth + 1);
    break;
  default:
    printf("Unknown node type\n");
    break;
  }
}