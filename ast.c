#include "ast.h"
#include "array.h"
#include "memory.h"
#include <stdio.h>

AstNode *ast_create_literal(int value) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = TYPE_INTEGER;
  node->type = AST_LITERAL;
  node->data.value = value;
  return node;
}

AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = TYPE_BOOLEAN;
  node->type = type;
  node->data.binaryop.left = left;
  node->data.binaryop.right = right;
  return node;
}

AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = type == AST_NEGATE ? TYPE_INTEGER : TYPE_BOOLEAN;
  node->type = type;
  node->data.unaryop.operand = operand;

  return node;
}

AstNode *ast_create_boolean(AstNodeType type, bool boolean) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = TYPE_BOOLEAN;
  node->type = type;
  node->data.boolean = boolean;
  return node;
}

AstNode *ast_create_variable_stmt(AstNodeType type, Type data_type,
                                  const char *name, AstNode *value) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = data_type;
  node->type = type;
  node->data.let_decl.value = value;
  node->data.let_decl.name = name;
  return node;
}

AstNode *ast_create_if_stmt(AstNode *condition, AstNode *then_body,
                            AstNode *else_body) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = TYPE_VOID;
  node->type = AST_IF_STATEMNET;
  node->data.if_stmt.condition = condition;
  node->data.if_stmt.then_body = then_body;
  node->data.if_stmt.else_body = else_body;
  return node;
}

AstNode *ast_create_while_stmt(AstNode *condition, AstNode *then_body) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = TYPE_VOID;
  node->type = AST_WHILE_STATEMENT;
  node->data.if_stmt.condition = condition;
  node->data.if_stmt.then_body = then_body;
  return node;
}

// add arguments
AstNode *ast_create_function_declaration(Type type, const char *function_name,
                                         AstNode *function_body) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = type;
  node->type = AST_FUNCTION;
  node->data.function_decl.name = function_name;
  node->data.function_decl.body = function_body;
  return node;
}

AstNode *ast_create_block(AstNodeType type, AstNode *parent_block) {
  AstNode *node = malloc(sizeof(AstNode));
  node->data_type = TYPE_VOID;
  node->data.block.parent_block = parent_block;
  node->type = type;
  node->data.block.elements = init_ast_array();
  return node;
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

static char *type_to_string(Type type) {
  switch (type) {
  case TYPE_BOOLEAN:
    return "boolean";
  case TYPE_FLOAT:
    return "float";
  case TYPE_STRING:
    return "string";
  case TYPE_INTEGER:
    return "integer";
  case TYPE_VOID:
    return "void";
  }
}

static void print(const char *string, int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
  printf("%s\n", string);
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

  case AST_NOT_EQUAL:
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

  case AST_LET_DECLARATION:
    printf("Declaration\n");
    char str1[100];
    sprintf(str1, "%s : %s", node->data.let_decl.name,
            type_to_string(node->data_type));

    print(str1, depth + 1);
    print_ast(node->data.let_decl.value, depth + 2);
    break;

  case AST_VAR_ASSIGNMENT:
    printf("Assign\n");
    char str2[100];
    sprintf(str2, "%s : %s", node->data.let_decl.name,
            type_to_string(node->data_type));
    print(str2, depth + 1);
    print_ast(node->data.let_decl.value, depth + 2);
    break;

  case AST_IF_STATEMNET:
    printf("IF_STATEMENT\n");

    print("Condition", depth + 1);
    print_ast(node->data.if_stmt.condition, depth + 2);

    print("then", depth + 1);
    print_ast(node->data.if_stmt.then_body, depth + 2);

    if (node->data.if_stmt.else_body) {
      print("Else", depth + 1);
      print_ast(node->data.if_stmt.else_body, depth + 2);
    }
    break;

  case AST_WHILE_STATEMENT:
    printf("While_Stmt\n");

    print("Condition", depth + 1);
    print_ast(node->data.if_stmt.condition, depth + 2);

    print("then", depth + 1);
    print_ast(node->data.if_stmt.then_body, depth + 2);

    break;

  case AST_BLOCK:
    printf("Block\n");
    for (int i = 0; i < node->data.block.elements->size; i++) {
      print_ast(node->data.block.elements->items[i], depth + 1);
    }
    break;

  case AST_PROGRAM:
    printf("Program\n");
    for (int i = 0; i < node->data.block.elements->size; i++) {
      print_ast(node->data.block.elements->items[i], depth + 1);
    }
    break;

  default:
    printf("Unknown node type\n");
    break;
  }
}
