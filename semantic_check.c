#include "semantic_check.h"
#include "./utils/table.h"
#include "ast.h"
#include "utils/array.h"
#include <stdio.h>
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
  printf("------ TABLE ------\n");
  for (int i = 0; i < table->capacity; i++) {
    printf("%d: %s %d\n", i, table->entries[i].key, table->entries[i].hash);
  }
}

static void init_environment() {
  // free scopes that dont matter anymore.
  // {
  //  current_scope: 1
  // }
  //
  // {
  // here we dont care or need the last current_scope = 1
  // so need to free that data
  //  current_scope: 1
  // }
  if (env_array->items[current_scope]->is_initialized && current_scope != 0) {
    printf("free scope %d\n", current_scope);
    free_table(&env_array->items[current_scope]->table);
  }

  /* Environment *env = malloc(sizeof(Environment)); */
  env_array->items[current_scope]->is_initialized = true;
  env_array->items[current_scope]->current_scope = current_scope;

  init_table(AS_TABLE(current_scope));
}

static Symbol *init_symbol(AstNode *value, SymbolType type,
                           DataType data_type) {
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
  case AST_INTEGER:
  case AST_FLOAT:
  case AST_STRING:
  case AST_TRUE:
  case AST_FALSE:
    break;

  case AST_ADD: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type != TYPE_INTEGER &&
            node->data.binaryop.right->data_type != TYPE_INTEGER ||
        node->data.binaryop.left->data_type != TYPE_FLOAT &&
            node->data.binaryop.right->data_type != TYPE_FLOAT) {
      errorAt(node->line, "+", "can only add two integers/floats.");
    }
    node->data_type = TYPE_INTEGER;

    break;
  }

  case AST_SUBTRACT: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type != TYPE_INTEGER &&
            node->data.binaryop.right->data_type != TYPE_INTEGER ||
        node->data.binaryop.left->data_type) {
      errorAt(node->line, "-", "Can only subtract two integers/floats.");
    }

    break;
  }

  case AST_MULTIPLY: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type != TYPE_INTEGER &&
            node->data.binaryop.right->data_type != TYPE_INTEGER ||
        node->data.binaryop.left->data_type) {
      errorAt(node->line, "*", "Can only mulitpy two integers/floats.");
    }

    break;
  }

  case AST_DIVIDE: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type != TYPE_INTEGER &&
            node->data.binaryop.right->data_type != TYPE_INTEGER ||
        node->data.binaryop.left->data_type) {
      errorAt(node->line, "/", "Can only divide two integers/floats.");
    }

    break;
  }

  case AST_EQUAL_EQUAL: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line,
              "==", "Can only compare two values of the same type.");
    }

    break;
  }

  case AST_NOT_EQUAL: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line,
              "!=", "Can only compare two values of the same type.");
    }

    break;
  }

  case AST_GREATER: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line, ">", "Can only compare two values of the same type.");
    }

    break;
  }

  case AST_GREATER_EQUAL: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line,
              ">=", "Can only compare two values of the same type.");
    }

    break;
  }

  case AST_LESS: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line, "<", "Can only compare two values of the same type.");
    }

    break;
  }

  case AST_LESS_EQUAL: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line,
              "<=", "Can only compare two values of the same type.");
    }

    break;
  }

  case AST_NEGATE: {
    semantic_check(node->data.unaryop.operand);

    if (node->data.unaryop.operand->data_type != TYPE_INTEGER) {
      errorAt(node->line, "-", "Can only negate intger type.");
    }
    break;
  }

  case AST_BANG: {
    if (node->data.unaryop.operand->data_type != TYPE_BOOLEAN) {
      errorAt(node->line, "!", "Can only logical negate boolean type.");
    }

    semantic_check(node->data.unaryop.operand);
    break;
  }

  case AST_IDENTIFIER_REFRENCE: {
    // check for the variable assigning and change its data type to its
    // correct one if exist

    identifier = init_symbol(NULL, VARIABLE_TYPE, TYPE_VOID);
    bool hasDefinition = false;

    for (int i = current_scope; i >= 0; i--) {
      if (table_get(AS_TABLE(i), AS_VARIABLE_NAME(node), AS_VARIABLE_HASH(node),
                    identifier)) {
        node->data_type = identifier->data_type;
        hasDefinition = true;
        break;
      } else {
        hasDefinition = false;
      }
    }

    if (!hasDefinition) {
      errorAt(node->line + 1, AS_VARIABLE_NAME(node),
              "Use of undeclared identifier.");
    }

    identifier = NULL;
    break;
  }

  case AST_VAR_ASSIGNMENT: {
    // add more in depth error messages later.
    identifier = init_symbol(NULL, VARIABLE_TYPE, TYPE_VOID);

    bool hasDefinition = false;
    for (int i = current_scope; i >= 0; i--) {

      if (table_get(AS_TABLE(i), AS_VARIABLE_NAME(node), AS_VARIABLE_HASH(node),
                    identifier)) {
        node->data_type = identifier->data_type;
        hasDefinition = true;
        break;
      } else {
        hasDefinition = false;
      }
    }

    if (!hasDefinition) {
      errorAt(node->line + 1, AS_VARIABLE_NAME(node),
              "Use of undeclared identifier.");
    }

    semantic_check(AS_VARIABLE_VALUE(node));
    if (node->data_type != AS_VARIABLE_VALUE(node)->data_type) {
      errorAt(AS_VARIABLE_VALUE(node)->line + 1, AS_VARIABLE_NAME(node),
              "Cannot assign a value that is not equal to type definition");
    }

    break;
  }

  case AST_LET_DECLARATION: {
    for (int i = current_scope; i >= 0; i--) {
      if (table_contains(AS_TABLE(i), AS_VARIABLE_NAME(node),
                         AS_VARIABLE_HASH(node))) {
        errorAt(node->line, AS_VARIABLE_NAME(node),
                "Cannot redefine variable.");
        break;
      }
    }
    // add variable name to the current scope table
    table_set(AS_TABLE(current_scope), AS_VARIABLE_NAME(node),
              AS_VARIABLE_HASH(node),
              init_symbol(node, VARIABLE_TYPE, node->data_type));

    semantic_check(AS_VARIABLE_VALUE(node));
    // check if the variable type matches the values assgined
    if (node->data_type != AS_VARIABLE_VALUE(node)->data_type) {
      errorAt(node->line, AS_VARIABLE_NAME(node),
              "Type definition does not match value type");
    }

    break;
  }

  case AST_IF_STATEMNET: {
    // semantic first because first you resolve the identifiers and then check
    // their types
    semantic_check(node->data.if_stmt.condition);
    if (node->data.if_stmt.condition->data_type != TYPE_BOOLEAN) {
      errorAt(node->line, "if", "Condition must be of type boolean");
    }

    // process then block of an if stmt
    semantic_check(node->data.if_stmt.then_body);

    // check if there is else block
    if (node->data.if_stmt.else_body != NULL) {
      semantic_check(node->data.if_stmt.else_body);
    }

    break;
  }

  case AST_FUNCTION: {
    if (!table_set(AS_TABLE(current_scope), AS_FUNCTION_DECL(node).name,
                   AS_FUNCTION_DECL(node).string_hash,
                   init_symbol(node, FUNCTION_TYPE, node->data_type))) {

      errorAt(node->line, node->data.function_decl.name,
              "Cannot redefine function.");
    }

    // start function scope here to fit in the parameters
    current_scope++;
    is_function = true;

    // add parameters to function scope
    init_environment();
    for (int i = 0; i < node->data.function_decl.parameters->size; i++) {
      if (!table_set(AS_TABLE(current_scope), AS_PARAMETER(node, i)->name,
                     AS_PARAMETER(node, i)->hash,
                     init_symbol(NULL, PARAMETER_TYPE,
                                 AS_PARAMETER(node, i)->data_type))) {
        errorAt(node->line, node->data.function_decl.name,
                "Cannot redefine parameters.");
      }
    }

    semantic_check(node->data.function_decl.body);

    break;
  }

  case AST_FUNCTION_CALL: {
#ifndef FUNCTION_CALL
#define FUNCTION_CALL

#define AS_FUNCTION_DECL_SIZE(node)                                            \
  (node)->value->data.function_decl.parameters->size

#define AS_FUNCTION_CALL_SIZE(node) (node)->data.function_call.arguments->size
#define AS_ARGUMENT(node, index)                                               \
  (node)->data.function_call.arguments->items[(index)]

    AstNode *function =
        ast_create_function_declaration(TYPE_VOID, "yo", NULL, NULL, 1);
    Symbol *identifier = init_symbol(function, FUNCTION_TYPE, TYPE_VOID);

    // check if there is definition of the function
    bool has_definition = false;
    for (int i = current_scope; i >= 0; i--) {

      if (table_get_function(AS_TABLE(i), AS_FUNCTION_CALL(node).name,
                             AS_FUNCTION_CALL(node).string_hash, identifier)) {
        // check to see if the identifier is a function or variable
        if (identifier->type != FUNCTION_TYPE) {
          printf("%d\n", identifier->data_type);
          errorAt(node->line, node->data.function_call.name,
                  "Variable is not a function calle.");
          break;
        }

        node->data_type = identifier->data_type;
        has_definition = true;
        break;
      } else {
        has_definition = false;
      }
    }

    if (!has_definition) {
      errorAt(node->line, AS_FUNCTION_CALL(node).name,
              "Undefined function call.");
      break;
    }

    // check if parameters and arguments size is equal
    if (AS_FUNCTION_DECL_SIZE(identifier) != AS_FUNCTION_CALL_SIZE(node)) {
      char error[100];
      sprintf(error, "Function '%s' takes %d arguments but %d were given",
              AS_FUNCTION_CALL(node).name, AS_FUNCTION_DECL_SIZE(identifier),
              AS_FUNCTION_CALL_SIZE(node));
      errorAt(node->line + 1, AS_FUNCTION_CALL(node).name, error);
    }

    for (int i = 0; i < AS_FUNCTION_DECL_SIZE(identifier); i++) {
      // reslove argument first
      semantic_check(AS_ARGUMENT(node, i));

      // compare parameters and argument
      if (AS_PARAMETER(identifier->value, i)->data_type !=
          AS_ARGUMENT(node, i)->data_type) {
        char error[100];
        sprintf(error, "Argument '%s' does not match function type",
                AS_ARGUMENT(node, i)->data.identifier_refrence);
        errorAt(node->line + 1, AS_FUNCTION_CALL(node).name, error);
      }
    }
#endif
    break;
  }

  case AST_BLOCK: {
    // if its a function the block has already made to fit in the
    // parameters in the AST_FUNCTION
    if (!is_function) {
      current_scope++;
      init_environment();
    } else {
      is_function = false;
    }

    for (int i = 0; i < node->data.block.elements->size; i++) {
      semantic_check(node->data.block.elements->items[i]);
    }

    current_scope--;
    break;
  }
  case AST_PROGRAM: {
    init_environment();

    for (int i = 0; i < node->data.block.elements->size; i++) {
      semantic_check(node->data.block.elements->items[i]);
    }
    break;
  }
  default:
    break;
  }
}

void semantic_analysis(AstNode *program) {
  env_array = init_environment_array();

  semantic_check(program);

  free_environment_array(env_array);
}
