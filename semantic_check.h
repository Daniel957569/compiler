#ifndef SEMANTIC_CHECK_H
#define SEMANTIC_CHECK_H

#include "./utils/array.h"
#include "./utils/table.h"
#include "ast.h"

#define AS_PARAMETER_NAME(node, index)                                         \
  (node)->data.function_decl.parameters->items[(index)]->name

#define AS_PARAMETER_HASH(node, index)                                         \
  (node)->data.function_decl.parameters->items[(index)]->hash

#define AS_PARAMETER_TYPE(node, index)                                         \
  (node)->data.function_decl.parameters->items[(index)]->type

#define AS_VARIABLE_NAME(node) (node)->data.variable.name
#define AS_VARIABLE_HASH(node) (node)->data.variable.string_hash
#define AS_VARIABLE_VALUE(node) (node)->data.variable.value

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
} Environment;

void semantic_analysis(AstNode *program);

#endif
