#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "utils/table.h"

#define AS_BINARYOP(node) (node)->data.binaryop

#define AS_BINARYOP_RIGHT(node) AS_BINARYOP(node).right
#define AS_BINARYOP_LEFT(node) AS_BINARYOP(node).left
#define AS_INT_vAL (node)(node)->data.integer_Val

void codegen(AstNode *program, Table *string_table);

#endif
