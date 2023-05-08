#include "ast.h"
#include "memory.h"
#include <stdio.h>

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

AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand) {
  AstNode *node = malloc(sizeof(AstNode));
  node->type = type;
  node->data.unaryop.operand = operand;
  return node;
}

AstNode *ast_create_boolean(AstNodeType type, bool boolean) {
  AstNode *node = malloc(sizeof(AstNode));
  node->type = type;
  node->data.boolean = boolean;
  return node;
}

AstNode *ast_create_let_decl(AstNodeType type, const char *name,
                             AstNode *value) {
  AstNode *node = malloc(sizeof(AstNode));
  node->type = type;
  node->data.let_decl.value = value;
  node->data.let_decl.name = name;
  return node;
}

AstNode *ast_create_var_assign(AstNodeType type, const char *name,
                               AstNode *value) {
  AstNode *node = malloc(sizeof(AstNode));
  node->type = type;
  node->data.let_decl.value = value;
  node->data.let_decl.name = name;
  return node;
}

AstNode *ast_create_program_node() {
  AstNode *node = malloc(sizeof(AstNode));
  node->data.program_node.capacity = 8;
  node->data.program_node.size = 0;
  node->data.program_node.elements = ALLOCATE(AstNode *, 8);
  return node;
}

void ast_push_program_node(AstNode *program, AstNode *node) {
  if (AS_PROGRAM_CAPACITY(program) * 0.75 < AS_PROGRAM_SIZE(program)) {
    program->data.program_node.elements = GROW_ARRAY(
        AstNode *, AS_PROGRAM_ELEMENTS(program), AS_PROGRAM_CAPACITY(program),
        AS_PROGRAM_CAPACITY(program) * 2);
    program->data.program_node.capacity *= 2;
  }
  program->data.program_node.elements[AS_PROGRAM_SIZE(program)] = node;
  program->data.program_node.size++;
}

double test_evaluate(AstNode *node) {
  double val = 0;

#define evaluateNode(op)                                                       \
  test_evaluate(node->data.binaryop.left) op test_evaluate(                    \
      node->data.binaryop.right)

  switch (node->type) {
  case AST_ADD:
    val += evaluateNode(+);
    break;
  case AST_SUBTRACT:
    val += evaluateNode(-);
    break;
  case AST_MULTIPLY:
    val += evaluateNode(*);
    break;
  case AST_DIVIDE:
    val += evaluateNode(/);
    break;
  case AST_NEGATE:
    val += -1 * test_evaluate(node->data.unaryop.operand);
    break;
  case AST_EQUAL_EQUAL:

  case AST_LITERAL:
    return node->data.value;
    break;
  }
  return val;
}

void freeTree(AstNode *node) {
  if (node == NULL) {
    return;
  }

  freeTree(node->data.binaryop.left);
  freeTree(node->data.binaryop.right);
  free(node);
}

void print_ast(AstNode *node, int depth) {
  if (node == NULL) {
    return;
  }

  for (int i = 0; i < depth; i++) {
    printf("  ");
  }

  switch (node->type) {
  case AST_TRUE:
    printf("True\n");
    break;
  case AST_FALSE:
    printf("False\n");
    break;
  case AST_LITERAL:
    printf("%f\n", node->data.value);
    break;
  case AST_ADD:
    printf("+\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_SUBTRACT:
    printf("-\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_MULTIPLY:
    printf("*\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_DIVIDE:
    printf("/\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_EQUAL:
    printf("=\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_EQUAL_EQUAL:
    printf("==\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_BANG_EQUAL:
    printf("!=\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_GREATER:
    printf(">\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_GREATER_EQUAL:
    printf(">=\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_LESS:
    printf("<\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_LESS_EQUAL:
    printf("<=\n");
    print_ast(node->data.binaryop.left, depth + 1);
    print_ast(node->data.binaryop.right, depth + 1);
    break;
  case AST_NEGATE:
    printf("--\n");
    print_ast(node->data.unaryop.operand, depth + 1);
    break;
  case AST_BANG:
    printf("!\n");
    print_ast(node->data.unaryop.operand, depth + 1);
    break;
  case AST_LET_DECL:
    printf("= : declaration\n");
    printf("  %s\n", node->data.let_decl.name);
    print_ast(node->data.let_decl.value, depth + 1);
    break;
  case AST_ASSIGN:
    printf("= : assign\n");
    printf("  %s\n", node->data.let_decl.name);
    print_ast(node->data.let_decl.value, depth + 1);
    break;
  default:
    printf("Unknown node type\n");
    break;
  }
}
