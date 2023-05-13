#ifndef SEMANTIC_CHECK_H
#define SEMANTIC_CHECK_H

#include "./utils/array.h"
#include "./utils/table.h"
#include "ast.h"

#define AS_PARAMETER(node, index)                                              \
  (node)->data.function_decl.parameters->items[(index)]

#define AS_FUNCTION_DECL(node) (node)->data.function_decl
#define AS_FUNCTION_CALL(node) (node)->data.function_call

#define AS_VARIABLE(node) (node)->data.variable
#define AS_VARIABLE_NAME(node) AS_VARIABLE((node)).name
#define AS_VARIABLE_HASH(node) AS_VARIABLE((node)).string_hash
#define AS_VARIABLE_VALUE(node) AS_VARIABLE((node)).value

#define AS_BINARYOP(node) (node)->data.binaryop

#define AS_TABLE(index) &env_array->items[(index)]->table

/* typedef struct Symbol { */
/*   AstNode *value; */
/*   int scope; */
/*   SymbolType type; */
/*   Type data_type; */
/* } Symbol; */

typedef struct Environment {
  Table table;
  int current_scope;
  bool is_initialized;
} Environment;

void semantic_analysis(AstNode *program);

#endif
