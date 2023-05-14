#include "codegen.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

FILE *file;
Table *strings_table;

static FILE *open_file(const char *path) {

  FILE *fp;
  fp = fopen(path, "w");

  if (fp == NULL) {
    printf("Failed to open file at codegen\n");
    exit(64);
  }
  return fp;
}

static char *data_byte_size(DataType type) {
  switch (type) {
  case TYPE_INTEGER:
  case TYPE_FLOAT:
    return "dd";

  case TYPE_BOOLEAN:
    return "db";

    // probabaly extract it and make it more personalize in the future
  case TYPE_STRING:
    return "db";

  default:
    printf("unknown at data_byte_size function at codegen.c\n");
    return "-1";
  }
}

static void generate_global_vars(AstNode *program) {
  for (int i = 0; i < AS_LIST_SIZE(program); i++) {
    if (AS_LIST_ELEMENT(program, i)->type != AST_LET_DECLARATION)
      continue;

    fprintf(file, "%s %s %s\n", AS_LIST_ELEMENT(program, i)->data.variable.name,
            data_byte_size(AS_LIST_ELEMENT(program, i)->data_type), "0");
  }
}

static void generate_strings() {
  for (int i = 0; i < strings_table->capacity; i++) {
    if (strings_table->entries[i].key != NULL) {
      char var_name[20];
      Entry entry = strings_table->entries[i];

      sprintf(var_name, "string_%d", entry.hash % strings_table->capacity);
      fprintf(file, "%s db %s, 0\n", var_name, entry.key);

      strings_table->entries++;
    }
  }
}

static void setup_asm_file(AstNode *program) {
  fprintf(file, "section .data\n");

  generate_strings();
  generate_global_vars(program);

  fprintf(file, "\nsection .text\n"
                "\tglobal _start\n\n"
                "_start: ");
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

void codegen(AstNode *program, Table *string_table) {
  file = open_file("../code.asm");
  strings_table = string_table;

  setup_asm_file(program);
  printf("%s\n", file->_IO_buf_base);
  fclose(file);

  generate_asm(program);
}
