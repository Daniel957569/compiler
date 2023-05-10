#ifndef SEMANTIC_CHECK_H
#define SEMANTIC_CHECK_H

#include "./utils/array.h"
#include "./utils/table.h"
#include "ast.h"

typedef enum {
  FUNCTION_TYPE,
  VARIABLE_TYPE,
  PARAMETER_TYPE,

} SymbolType;

typedef struct Symbol {
  AstNode *value;
  int scope;
  SymbolType type;
} Symbol;

typedef struct Environment {
  Table table;
  int current_scope;
} Environment;

void semantic_analysis(AstNode *program);

#endif
