#include "semantic_check.h"
#include "./utils/table.h"
#include "ast.h"
#include "memory.h"
#include "utils/array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EIGHT_BYTES 8

// add clearer error messages, such as "return type 'integer' doesn't match
// function return type 'String'"

#define AS_TABLE(index) &env.env_array->items[(index)]->table
#define INIT_EMPTY_SYMBOL() init_symbol(NULL, VARIABLE_TYPE, TYPE_VOID, 0);

typedef struct {
  EnvironmentArray *env_array;
  Table *string_table;
  int current_scope;
  AstNode *current_function;
  bool has_error;

} SemanticEnviroment;

SemanticEnviroment env;

static void errorAt(int line, const char *token, const char *message) {
  env.has_error = true;
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

static void init_semantic_enviroment() {
  env.current_scope = 0;
  env.string_table = ALLOCATE(Table, 1);
  init_table(env.string_table);
  env.env_array = init_environment_array();
  env.current_function = NULL;
  env.has_error = false;
}

static void init_environment() {
  // free scopes that dont matter anymore.
  // {
  //  current_scope: 1
  // }
  // {
  //  current_scope: 1
  // {
  // here we dont care or need the last current_scope = 1
  // so need to free that data
  if (env.env_array->items[env.current_scope]->is_initialized &&
      env.current_scope != 0) {
    free_table(&env.env_array->items[env.current_scope]->table);
  }

  /* Environment *env = malloc(sizeof(Environment)); */
  env.env_array->items[env.current_scope]->is_initialized = true;
  env.env_array->items[env.current_scope]->current_scope = env.current_scope;

  init_table(AS_TABLE(env.current_scope));
}

static bool has_main_function(AstNode *program) {
  for (int i = 0; i < program->data.block.elements->size; i++) {
    if (program->data.block.elements->items[i]->type != AST_FUNCTION) {
      continue;
    }

    if (strcmp(AS_FUNCTION_DECL(program->data.block.elements->items[i]).name,
               "main") == 0) {
      return true;
    }
  }
  return false;
}

static Symbol *init_symbol(AstNode *value, SymbolType type, DataType data_type,
                           int stack_pos) {
  Symbol *symbol = malloc(sizeof(Symbol));
  symbol->scope = env.current_scope;
  symbol->stack_pos = stack_pos;
  symbol->value = value;
  symbol->type = type;
  symbol->data_type = data_type;
  return symbol;
}

static bool check_data_type(AstNode *node) {
  if (AS_BINARYOP(node).left->data_type == TYPE_INTEGER &&
      AS_BINARYOP(node).right->data_type == TYPE_INTEGER) {
    node->data_type = TYPE_INTEGER;
    return true;
  }

  if (AS_BINARYOP(node).left->data_type == TYPE_FLOAT &&
      AS_BINARYOP(node).right->data_type == TYPE_FLOAT) {
    node->data_type = TYPE_FLOAT;
    return true;
  }

  return false;
}

static void removeQuotes(char *str) {
  int len = strlen(str);
  int j = 0;

  for (int i = 0; i < len; i++) {
    if (str[i] != '"') {
      str[j] = str[i];
      j++;
    }
  }
  str[j] = '\0';
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

  case AST_TRUE:
  case AST_FALSE:
    break;

  case AST_STRING: {
    // save strings for code generation to declare them.
    removeQuotes(node->data.str);
    char *string = node->data.str;
    table_set(env.string_table, string, hash_string(string, strlen(string)),
              NULL);

    break;
  }

  case AST_ADD: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (!check_data_type(node)) {
      errorAt(node->line, "+", "can only add two integers/floats.");
    }

    printf("%d\n", node->data_type);
    break;
  }

  case AST_SUBTRACT: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (!check_data_type(node)) {
      errorAt(node->line, "-", "Can only subtract two integers/floats.");
    }

    break;
  }

  case AST_MULTIPLY: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (!check_data_type(node)) {
      errorAt(node->line, "*", "Can only mulitpy two integers/floats.");
    }

    break;
  }

  case AST_DIVIDE: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (!check_data_type(node)) {
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
    node->data_type = TYPE_BOOLEAN;

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
    node->data_type = TYPE_BOOLEAN;

    break;
  }

  case AST_GREATER: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line, ">", "Can only compare two values of the same type.");
    }
    node->data_type = TYPE_BOOLEAN;

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
    node->data_type = TYPE_BOOLEAN;

    break;
  }

  case AST_LESS: {
    semantic_check(node->data.binaryop.left);
    semantic_check(node->data.binaryop.right);

    if (node->data.binaryop.left->data_type !=
        node->data.binaryop.right->data_type) {
      errorAt(node->line, "<", "Can only compare two values of the same type.");
    }
    node->data_type = TYPE_BOOLEAN;

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

    node->data_type = TYPE_BOOLEAN;

    break;
  }

  case AST_NEGATE: {
    semantic_check(node->data.unaryop.operand);
    if (node->data.unaryop.operand->data_type != TYPE_INTEGER) {
      errorAt(node->line, "-", "Can only negate intger type.");
    }
    node->data_type = TYPE_INTEGER;

    break;
  }

  case AST_BANG: {
    semantic_check(node->data.unaryop.operand);
    if (node->data.unaryop.operand->data_type != TYPE_BOOLEAN) {
      errorAt(node->line, "!", "Can only logical negate boolean type.");
    }

    node->data_type = TYPE_BOOLEAN;

    break;
  }

  case AST_RETURN_STATEMENT: {
    // global scope
    if (env.current_function == NULL) {
      errorAt(node->line, "return", "Cannot return from global scope");
    }

    // incase return has void value, such as return;
    if (node->data.return_stmt.value != NULL) {
      semantic_check(node->data.return_stmt.value);
    }

    if (env.current_function->data_type != node->data_type) {
      errorAt(node->line, "return",
              "return type does not match function return type");
    }

    break;
  }

  case AST_PRINT_STATEMENT:
    semantic_check(node->data.print_stmt.value);
    node->data_type = node->data.print_stmt.value->data_type;

    break;

  case AST_IDENTIFIER_REFRENCE: {
    // check for the variable assigning and change its data type to its
    // correct one if exist

    Symbol *identifier = INIT_EMPTY_SYMBOL();
    bool hasDefinition = false;

    for (int i = env.current_scope; i >= 0; i--) {
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

    if (node->data_type == TYPE_STRING) {
      printf("%d\n", identifier->stack_pos);
    }

    node->data.variable.is_global = identifier->scope == 0 ? true : false;
    node->data.variable.stack_pos = identifier->stack_pos;

    free(identifier);
    break;
  }

  case AST_VAR_ASSIGNMENT: {
    // add more in depth error messages later.
    Symbol *identifier = INIT_EMPTY_SYMBOL();

    bool hasDefinition = false;
    for (int i = env.current_scope; i >= 0; i--) {

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
              "Cannot assign a value that is not equal to type "
              "definition");
    }
    node->data.variable.stack_pos = identifier->stack_pos;

    free(identifier);
    break;
  }

  case AST_LET_DECLARATION: {
    if (env.current_scope > 0) {
      // so if allocated bytes is 4 its location on the stack is just rsp
      node->data.variable.stack_pos =
          env.current_function->data.function_decl.byte_allocated;

      env.current_function->data.function_decl.byte_allocated += EIGHT_BYTES;
    }

    for (int i = env.current_scope; i >= 0; i--) {
      if (table_contains(AS_TABLE(i), AS_VARIABLE_NAME(node),
                         AS_VARIABLE_HASH(node))) {
        errorAt(node->line, AS_VARIABLE_NAME(node),
                "Cannot redefine variable.");
        break;
      }
    }

    // add variable name to the current scope table
    table_set(AS_TABLE(env.current_scope), AS_VARIABLE_NAME(node),
              AS_VARIABLE_HASH(node),
              init_symbol(node, VARIABLE_TYPE, node->data_type,
                          node->data.variable.stack_pos));

    semantic_check(AS_VARIABLE_VALUE(node));
    // check if the variable type matches the values assgined
    if (node->data_type != AS_VARIABLE_VALUE(node)->data_type) {
      errorAt(node->line, AS_VARIABLE_NAME(node),
              "Type definition does not match value type");
    }

    break;
  }

  case AST_IF_STATEMNET: {
    if (env.current_scope == 0) {
      errorAt(node->line, "if", "Cannot declare if statement in global scope");
    }
    // semantic first because first you resolve the identifiers and
    // then check their types
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

  case AST_WHILE_STATEMENT: {
    if (env.current_scope == 0) {
      errorAt(node->line, "while",
              "Cannot declare while statement in global scope");
    }
    // semantic first because first you resolve the identifiers and
    // then check their types
    semantic_check(node->data.while_stmt.condition);
    if (node->data.while_stmt.condition->data_type != TYPE_BOOLEAN) {
      errorAt(node->line, "while", "Condition must be of type boolean");
    }

    // process then block of an while stmt
    semantic_check(node->data.while_stmt.then_body);
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
    Symbol *identifier = init_symbol(function, FUNCTION_TYPE, TYPE_VOID, 0);

    // check if there is definition of the function
    bool has_definition = false;
    for (int i = env.current_scope; i >= 0; i--) {

      if (table_get_function(AS_TABLE(i), AS_FUNCTION_CALL(node).name,
                             AS_FUNCTION_CALL(node).string_hash, identifier)) {
        // check to see if the identifier is a function or variable
        if (identifier->type != FUNCTION_TYPE) {
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

  case AST_FUNCTION: {
    // add function declaration inside a function if possible later on
    if (env.current_scope > 0) {
      errorAt(node->line, node->data.function_decl.name,
              "Cannot define a function inside of a function");
    }

    if (node->data.function_decl.parameters->size > 10) {
      // TODO: pass arguments through the stack? or stack + registers
      errorAt(node->line, node->data.function_decl.name,
              "A Function cannot have more than 10 parameters");
    }

    if (!table_set(AS_TABLE(env.current_scope), AS_FUNCTION_DECL(node).name,
                   AS_FUNCTION_DECL(node).string_hash,
                   init_symbol(node, FUNCTION_TYPE, node->data_type, 0))) {

      errorAt(node->line, node->data.function_decl.name,
              "Cannot redefine function.");
    }

    // start function scope here to fit in the parameters
    env.current_scope++;
    env.current_function = node;

    // add parameters to function scope
    init_environment();
    for (int i = 0; i < node->data.function_decl.parameters->size; i++) {
      if (!table_set(AS_TABLE(env.current_scope), AS_PARAMETER(node, i)->name,
                     AS_PARAMETER(node, i)->hash,
                     init_symbol(NULL, PARAMETER_TYPE,
                                 AS_PARAMETER(node, i)->data_type,
                                 node->data.function_decl.byte_allocated))) {
        errorAt(node->line, node->data.function_decl.name,
                "Cannot redefine parameters.");
      }

      AS_PARAMETER(node, i)->stack_pos =
          node->data.function_decl.byte_allocated;

      node->data.function_decl.byte_allocated += EIGHT_BYTES;
    }

    semantic_check(node->data.function_decl.body);
    env.current_function = NULL;
    env.current_scope--;

    break;
  }

  case AST_BLOCK: {
    // if its a function the block has already made to fit in the
    // parameters in the AST_FUNCTION

    if (env.current_function > 0) {
      env.current_scope++;
      init_environment();
    }

    for (int i = 0; i < node->data.block.elements->size; i++) {
      semantic_check(node->data.block.elements->items[i]);
    }

    env.current_scope--;
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

Table *semantic_analysis(AstNode *program) {
  init_semantic_enviroment();
  semantic_check(program);

  if (!has_main_function(program)) {
    printf("[Program] Error entry point: Program does not have main "
           "function\n");
    exit(64);
  }

  if (env.has_error) {
    printf("Cannot compile due to errors.\n");
    exit(64);
  }

  free_environment_array(env.env_array);
  return env.string_table;
}
