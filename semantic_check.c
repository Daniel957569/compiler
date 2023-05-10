#include "semantic_check.h"
#include "./utils/table.h"
#include "ast.h"
#include "utils/array.h"
#include <stdlib.h>

EnvironmentArray *env_array;
int current_scope = 0;

static void errorAt(int line, const char *token, const char *message) {
  fprintf(stderr, "[line %d] Error", line);
  fprintf(stderr, " at '%s'", token);
  fprintf(stderr, ": %s\n", message);
}

static Environment *init_environment() {
  Environment *env = malloc(sizeof(Environment));
  env->current_scope = current_scope;
  init_table(&env->table);
  return env;
}

static Symbol *init_symbol(AstNode *value, SymbolType type) {
  Symbol *symbol = malloc(sizeof(Symbol));
  symbol->scope = current_scope;
  symbol->value = value;
  symbol->type = type;
  return symbol;
}

static void semantic_check(AstNode *node) {
  if (node == NULL) {
    printf("NULL node\n");
    exit(64);
  }

  switch (node->type) {
    // skip literals
  case AST_LITERAL:
  case AST_STRING:
  case AST_TRUE:
  case AST_FALSE:
    break;

  case AST_ADD:
    if (node->data.binaryop.left->data_type != TYPE_INTEGER ||
        node->data.binaryop.right->data_type != TYPE_INTEGER) {
      errorAt(node->line, "+", "can only add two integers.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;
  case AST_SUBTRACT:

    if (node->data.binaryop.left->data_type != TYPE_INTEGER ||
        node->data.binaryop.right->data_type != TYPE_INTEGER) {

      errorAt(node->line, "-", "Can only subtract two integers.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_MULTIPLY:
    if (node->data.binaryop.left->data_type != TYPE_INTEGER ||
        node->data.binaryop.right->data_type != TYPE_INTEGER) {

      errorAt(node->line, "*", "Can only mulitpy two integers.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_DIVIDE:
    if (node->data.binaryop.left->data_type != TYPE_INTEGER ||
        node->data.binaryop.right->data_type != TYPE_INTEGER) {

      errorAt(node->line, "/", "Can only divide two integers.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_EQUAL_EQUAL:
    if (node->data.binaryop.left->data_type ==
        node->data.binaryop.right->data_type) {

      errorAt(node->line,
              "==", "Can only compare two values of the same type.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_NOT_EQUAL:
    if (node->data.binaryop.left->data_type ==
        node->data.binaryop.right->data_type) {

      errorAt(node->line,
              "!=", "Can only compare two values of the same type.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_GREATER:
    if (node->data.binaryop.left->data_type ==
        node->data.binaryop.right->data_type) {

      errorAt(node->line, ">", "Can only compare two values of the same type.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_GREATER_EQUAL:
    if (node->data.binaryop.left->data_type ==
        node->data.binaryop.right->data_type) {

      errorAt(node->line,
              ">=", "Can only compare two values of the same type.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_LESS:
    if (node->data.binaryop.left->data_type ==
        node->data.binaryop.right->data_type) {

      errorAt(node->line,
              "<=", "Can only compare two values of the same type.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_LESS_EQUAL:
    if (node->data.binaryop.left->data_type ==
        node->data.binaryop.right->data_type) {

      errorAt(node->line,
              "<=", "Can only compare two values of the same type.");
    }

    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);
    break;

  case AST_NEGATE:
    if (node->data.unaryop.operand->data_type != TYPE_INTEGER) {

      errorAt(node->line, "-", "Can only negate intger type.");
    }

    semantic_check(node->data.unaryop.operand);
    break;

  case AST_BANG:
    if (node->data.unaryop.operand->data_type != TYPE_BOOLEAN) {

      errorAt(node->line, "!", "Can only logical negate boolean type.");
    }

    semantic_check(node->data.unaryop.operand);
    break;

  case AST_LET_DECLARATION:
    // add variable name to the current scope table
    if (!table_set(&env_array->items[current_scope]->table,
                   node->data.variable.name, node->data.variable.string_hash,
                   init_symbol(node, VARIABLE_TYPE))) {
      // if already has a variable with the same name
      errorAt(node->line, node->data.variable.name,
              "Cannot redefine variable.");
    }

    // check if the variable type matches the values assgined
    if (node->data_type != node->data.variable.value->data_type) {
      errorAt(node->line, node->data.variable.name,
              "Type definition does not match value type");
    }

    semantic_check(node->data.variable.value);
    break;

  case AST_FUNCTION:
    semantic_check(node->data.function_decl.body);

    break;
  case AST_BLOCK:
    current_scope++;
    env_array->items[current_scope] = init_environment();
    for (int i = 0; i < node->data.block.elements->size; i++) {
      printf("%d\n", node->data.block.elements->items[i]->type);
      semantic_check(node->data.block.elements->items[i]);
    }

    current_scope--;
    break;
  case AST_PROGRAM:
    env_array->items[0] = init_environment();
    for (int i = 0; i < node->data.block.elements->size; i++) {
      semantic_check(node->data.block.elements->items[i]);
    }
    break;
  default:
    break;
  }
}

void semantic_analysis(AstNode *program) {
  env_array = init_environment_array();

  semantic_check(program);

  free_environment_array(env_array);
}
