#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

FILE *file;

static FILE *open_file(const char *path) {
  FILE *fp;
  fp = fopen("code.asm", "w");

  if (fp == NULL) {
    printf("Failed to open file at codegen\n");
    exit(64);
  }
  return fp;
}

static void generate_asm(AstNode *node) {
  switch (node->type) {
  case AST_INTEGER:
    // Handle integer node
    break;
  case AST_FLOAT:
    // Handle float node
    break;
  case AST_STRING:
    // Handle string node
    break;
  case AST_IDENTIFIER_REFRENCE:
    // Handle identifier reference node
    break;
  case AST_TRUE:
    // Handle true node
    break;
  case AST_FALSE:
    // Handle false node
    break;
  case AST_ADD:
    // Handle add node
    break;
  case AST_SUBTRACT:
    // Handle subtract node
    break;
  case AST_MULTIPLY:
    // Handle multiply node
    break;
  case AST_DIVIDE:
    // Handle divide node
    break;
  case AST_EQUAL_EQUAL:
    // Handle equal equal node
    break;
  case AST_BANG:
    // Handle bang node
    break;
  case AST_NOT_EQUAL:
    // Handle not equal node
    break;
  case AST_GREATER:
    // Handle greater node
    break;
  case AST_GREATER_EQUAL:
    // Handle greater equal node
    break;
  case AST_LESS:
    // Handle less node
    break;
  case AST_LESS_EQUAL:
    // Handle less equal node
    break;
  case AST_NEGATE:
    // Handle negate node
    break;
  case AST_LET_DECLARATION:
    // Handle let declaration node
    break;
  case AST_VAR_ASSIGNMENT:
    // Handle variable assignment node
    break;
  case AST_IF_STATEMNET:
    // Handle if statement node
    break;
  case AST_WHILE_STATEMENT:
    // Handle while statement node
    break;
  case AST_RETURN_STATEMENT:
    // Handle return statement node
    break;
  case AST_CONTINUE_STATEMENT:
    // Handle continue statement node
    break;
  case AST_FUNCTION:
    // Handle function node
    break;
  case AST_FUNCTION_CALL:
    // Handle function call node
    break;
  case AST_PROGRAM:
    // Handle program node
    break;
  case AST_BLOCK:
    // Handle block node
    break;
  default:
    // Handle unknown node type
    break;
  }
}

void codegen(AstNode *program) {
  file = open_file("code.asm");
  generate_asm(program);
}
