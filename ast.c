#include "ast.h"
#include "./utils/table.h"
#include "memory.h"
#include "utils/array.h"
#include <stdio.h>
#include <string.h>

AstNode *ast_create_number(double value, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_INTEGER;
  node->type = AST_INTEGER;
  node->data.integer_val = (int)value;
  return node;
}

AstNode *ast_create_float(float value, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_FLOAT;
  node->type = AST_FLOAT;
  node->data.float_val = value;
  return node;
}

AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right,
                             int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_VOID;
  node->type = type;
  node->data.binaryop.left = left;
  node->data.binaryop.right = right;
  return node;
}

AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = type == AST_NEGATE ? TYPE_INTEGER : TYPE_BOOLEAN;
  node->type = type;
  node->data.unaryop.operand = operand;

  return node;
}

AstNode *ast_create_boolean(AstNodeType type, bool boolean, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_BOOLEAN;
  node->type = type;
  node->data.boolean = boolean;
  return node;
}

AstNode *ast_create_string(char *str, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_STRING;
  node->type = AST_STRING;
  node->data.str = str;
  return node;
}

AstNode *ast_create_identifier_refrence(const char *str, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_VOID;
  node->type = AST_IDENTIFIER_REFRENCE;
  node->data.variable.name = str;
  node->data.variable.string_hash = hash_string(str, strlen(str));
  node->data.variable.is_global = false;
  node->data.variable.value = NULL;
  return node;
}

AstNode *ast_create_variable_stmt(AstNodeType type, DataType data_type,
                                  const char *name, int line, AstNode *value) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = data_type;
  node->type = type;
  node->data.variable.value = value;
  node->data.variable.stack_pos = 0;
  node->data.variable.string_hash = hash_string(name, strlen(name));
  node->data.variable.is_global = false;
  node->data.variable.name = name;
  return node;
}

AstNode *ast_create_if_statement(AstNode *condition, AstNode *then_body,
                                 AstNode *else_body, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_VOID;
  node->type = AST_IF_STATEMNET;
  node->data.if_stmt.condition = condition;
  node->data.if_stmt.then_body = then_body;
  node->data.if_stmt.else_body = else_body;
  node->data.if_stmt.end_label = 0;
  return node;
}

AstNode *ast_create_while_statement(AstNode *condition, AstNode *then_body,
                                    int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_VOID;
  node->type = AST_WHILE_STATEMENT;
  node->data.while_stmt.condition = condition;
  node->data.while_stmt.then_body = then_body;
  node->data.while_stmt.end_label = 0;
  node->data.while_stmt.start_label = 0;
  return node;
}

AstNode *ast_create_return_statement(DataType type, AstNode *value, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = type;
  node->type = AST_RETURN_STATEMENT;
  node->data.return_stmt.value = value;
  return node;
}

AstNode *ast_create_control_flow_statement(AstNodeType type, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->type = type;
  node->data_type = TYPE_VOID;
  node->data.control_flow.control_flow_statement = NULL;
  return node;
}

AstNode *ast_create_print_statement(DataType type, AstNode *value, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = type;
  node->type = AST_PRINT_STATEMENT;
  node->data.return_stmt.value = value;
  return node;
}

AstNode *ast_create_function_declaration(DataType type,
                                         const char *function_name,
                                         IdentifierArray *parameters,
                                         AstNode *function_body, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = type;
  node->type = AST_FUNCTION;
  node->data.function_decl.parameters = parameters;
  node->data.function_decl.name = function_name;
  node->data.function_decl.byte_allocated = 8;
  node->data.function_decl.body = function_body;
  node->data.function_decl.string_hash =
      hash_string(function_name, strlen(function_name));
  return node;
}

AstNode *ast_create_function_call(const char *function_name,
                                  AstArray *arguments, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_VOID;
  node->type = AST_FUNCTION_CALL;
  node->data.function_call.arguments = arguments;
  node->data.function_call.name = function_name;
  node->data.function_call.string_hash =
      hash_string(function_name, strlen(function_name));
  return node;
}

AstNode *ast_create_block(AstNodeType type, int line) {
  AstNode *node = ALLOCATE(AstNode, 1);
  node->line = line;
  node->data_type = TYPE_VOID;
  node->type = type;
  node->data.block.elements = init_ast_array();
  return node;
}

Identifier *create_identifier(char *name, DataType type) {
  Identifier *identifier = ALLOCATE(Identifier, 1);
  identifier->name = name;
  identifier->stack_pos = 8;
  identifier->data_type = type;
  identifier->hash = hash_string(name, strlen(name));
  return identifier;
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

  case AST_INTEGER:
    return node->data.integer_val;
    break;
  }
  return val;
}

void free_tree(AstNode *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case AST_INTEGER:
  case AST_FLOAT:
  case AST_STRING:
  case AST_IDENTIFIER_REFRENCE:
  case AST_TRUE:
  case AST_FALSE:
    free(node);
    break;

  case AST_ADD:
  case AST_SUBTRACT:
  case AST_MULTIPLY:
  case AST_DIVIDE:
  case AST_EQUAL_EQUAL:
  case AST_NOT_EQUAL:
  case AST_GREATER:
  case AST_GREATER_EQUAL:
  case AST_LESS:
  case AST_LESS_EQUAL:
    free_tree(node->data.binaryop.left);
    free_tree(node->data.binaryop.right);
    free(node);
    break;

  case AST_BANG:
  case AST_NEGATE:
    free_tree(node->data.unaryop.operand);
    free(node);
    break;

  case AST_LET_DECLARATION:
  case AST_VAR_ASSIGNMENT:
    free_tree(node->data.variable.value);
    free(node);
    break;

  case AST_IF_STATEMNET:
    free_tree(node->data.if_stmt.condition);
    free_tree(node->data.if_stmt.then_body);
    free_tree(node->data.if_stmt.else_body);
    free(node);
    break;

  case AST_WHILE_STATEMENT:
    free_tree(node->data.while_stmt.condition);
    free_tree(node->data.while_stmt.then_body);
    free(node);
    break;

  case AST_RETURN_STATEMENT:
    free_tree(node->data.return_stmt.value);
    free(node);
    break;

  case AST_CONTINUE_STATEMENT:
    free(node);
    break;

  case AST_FUNCTION:
    free_tree(node->data.function_decl.body);
    free_identifier_array(node->data.function_decl.parameters);
    free(node);
    break;

  case AST_FUNCTION_CALL:
    free_ast_array(node->data.function_call.arguments);
    free(node);
    break;

  case AST_PROGRAM:
  case AST_BLOCK:
    free_ast_array(node->data.block.elements);
    free(node);
    break;
  }
}

static char *type_to_string(DataType type) {
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

static void print_parameters(IdentifierArray *array, int depth) {
  printf("(");
  for (int i = 0; i < array->size; i++) {
    printf("%s, ", array->items[i]->name);
  }
  printf(")");
}

static void print_arguments(AstArray *array, int depth) {
  for (int i = 0; i < array->size; i++) {
    print_ast(array->items[i], depth + 1);
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

  case AST_INTEGER:
    printf("%d\n", node->data.integer_val);
    break;
  case AST_FLOAT:
    printf("%f\n", node->data.float_val);
    break;

  case AST_STRING:
    printf("%s\n", node->data.str);
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

  case AST_IDENTIFIER_REFRENCE:
    printf("%s\n", node->data.variable.name);
    break;

  case AST_LET_DECLARATION:
    printf("Declaration\n");
    char str1[100];
    sprintf(str1, "%s: %s", node->data.variable.name,
            type_to_string(node->data_type));

    print(str1, depth + 1);
    print_ast(node->data.variable.value, depth + 2);
    break;

  case AST_VAR_ASSIGNMENT:
    printf("Assign\n");
    char str2[100];
    sprintf(str2, "%s: %s", node->data.variable.name,
            type_to_string(node->data_type));
    print(str2, depth + 1);
    print_ast(node->data.variable.value, depth + 2);
    break;

  case AST_IF_STATEMNET:
    printf("IF_Statement\n");

    print("Condition", depth + 1);
    print_ast(node->data.if_stmt.condition, depth + 2);

    print("then", depth + 1);
    print_ast(node->data.if_stmt.then_body, depth + 2);

    if (node->data.if_stmt.else_body) {
      print("else", depth + 1);
      print_ast(node->data.if_stmt.else_body, depth + 2);
    }
    break;

  case AST_WHILE_STATEMENT:
    printf("While_Stmt: \n");

    print("Condition", depth + 1);
    print_ast(node->data.if_stmt.condition, depth + 2);

    print("then", depth + 1);
    print_ast(node->data.if_stmt.then_body, depth + 2);

    break;

  case AST_PRINT_STATEMENT:
    printf("Print: ");
    printf("%s\n", type_to_string(node->data_type));
    print_ast(node->data.return_stmt.value, depth + 1);
    break;

  case AST_RETURN_STATEMENT:
    printf("Return: ");
    printf("%s\n", type_to_string(node->data_type));
    print_ast(node->data.return_stmt.value, depth + 1);
    break;

  case AST_CONTINUE_STATEMENT:
    printf("Continue \n");
    break;

  case AST_FUNCTION:
    printf("Function %s: ", node->data.function_decl.name);
    print_parameters(node->data.function_decl.parameters, depth);
    printf(": %s\n", type_to_string(node->data_type));
    print_ast(node->data.function_decl.body, depth + 1);
    break;

  case AST_FUNCTION_CALL:
    printf("Call Function: ");
    printf("%s\n", node->data.function_call.name);
    print("Arguments: ", depth + 1);
    print_arguments(node->data.function_call.arguments, depth + 2);
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
    break;
  }
}
