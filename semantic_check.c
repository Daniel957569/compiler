#include "semantic_check.h"
#include "./utils/table.h"
#include "ast.h"
#include "utils/array.h"
#include <stdlib.h>
#include <string.h>

EnvironmentArray *env_array;
int current_scope = 0;

// for some odd reason i cant make one in AST_IDENTIFIER_REFRENCE, cant be
// bothered
Symbol *identifier;

bool is_function = false;

static void errorAt(int line, const char *token, const char *message) {
  fprintf(stderr, "[line %d] Error", line - 1);
  fprintf(stderr, " at '%s'", token);
  fprintf(stderr, ": %s\n", message);
}

static void print_table(Table *table) {
  for (int i = 0; i < table->capacity; i++) {
    printf("%d: %s\n", i, table->entries[i].key);
  }
}

static Environment *init_environment() {
  Environment *env = malloc(sizeof(Environment));
  env->current_scope = current_scope;
  init_table(&env->table);
  return env;
}

static Symbol *init_symbol(AstNode *value, SymbolType type, Type data_type) {
  Symbol *symbol = malloc(sizeof(Symbol));
  symbol->scope = current_scope;
  symbol->value = value;
  symbol->type = type;
  symbol->data_type = data_type;
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

  case AST_IDENTIFIER_REFRENCE:
    // check for the variable assigning and change its data type to its
    // correct one if exist

    identifier = init_symbol(NULL, VARIABLE_TYPE, TYPE_VOID);
    if (table_get(&env_array->items[current_scope]->table,
                  node->data.variable.name, node->data.variable.string_hash,
                  identifier)) {
      node->data_type = identifier->data_type;
    } else {
      errorAt(node->line, node->data.variable.name,
              "Use of undeclared identifier.");
    }

    identifier = NULL;
    break;

  case AST_VAR_ASSIGNMENT:
    identifier = init_symbol(NULL, VARIABLE_TYPE, TYPE_VOID);
    if (table_get(&env_array->items[current_scope]->table,
                  node->data.variable.name, node->data.variable.string_hash,
                  identifier)) {
      node->data_type = identifier->data_type;
    } else {
      errorAt(node->line, node->data.variable.name,
              "Use of undeclared identifier.");
    }

    semantic_check(node->data.variable.value);
    if (node->data_type != node->data.variable.value->data_type) {
      errorAt(node->data.variable.value->line, node->data.variable.name,
              "Cannot assign a value that is not equal to type definition");
    }

    break;

  case AST_LET_DECLARATION:
    // add variable name to the current scope table
    if (!table_set(&env_array->items[current_scope]->table,
                   node->data.variable.name, node->data.variable.string_hash,
                   init_symbol(node, VARIABLE_TYPE, node->data_type))) {
      // if already has a variable with the same name
      errorAt(node->line, node->data.variable.name,
              "Cannot redefine variable.");
    }

    semantic_check(node->data.variable.value);
    // check if the variable type matches the values assgined
    if (node->data_type != node->data.variable.value->data_type) {
      errorAt(node->line, node->data.variable.name,
              "Type definition does not match value type");
    }

    break;

  case AST_FUNCTION:
    if (!table_set(&env_array->items[current_scope]->table,
                   node->data.function_decl.name,
                   node->data.function_decl.string_hash,
                   init_symbol(NULL, FUNCTION_TYPE, node->data_type))) {
      errorAt(node->line, node->data.function_decl.name,
              "Cannot redefine function.");
    }

    // start function scope here to fit in the parameters
    current_scope++;
    is_function = true;

    // add parameters to function scope
    env_array->items[current_scope] = init_environment();
    for (int i = 0; i < node->data.function_decl.parameters->size; i++) {
      if (!table_set(
              &env_array->items[current_scope]->table,
              node->data.function_decl.parameters->items[i]->name,
              node->data.function_decl.parameters->items[i]->hash,
              init_symbol(
                  NULL, PARAMETER_TYPE,
                  node->data.function_decl.parameters->items[i]->type))) {
        errorAt(node->line, node->data.function_decl.name,
                "Cannot redefine parameters.");
      }
    }

    semantic_check(node->data.function_decl.body);

    break;
  case AST_BLOCK:
    // if its a function the block has already made to fit in the
    // parameters in the AST_FUNCTION
    if (!is_function) {
      current_scope++;

      env_array->items[current_scope] = init_environment();
    }
    for (int i = 0; i < node->data.block.elements->size; i++) {
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
