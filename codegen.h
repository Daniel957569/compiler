#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "utils/table.h"
void codegen(AstNode *program, Table *string_table);

#endif
