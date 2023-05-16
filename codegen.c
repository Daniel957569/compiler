#include "codegen.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

FILE *file;
Table *strings_table;
int result;
int depth;

static void generate_asm(AstNode *program);

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
  fprintf(file, "; global variabless\n");
  for (int i = 0; i < AS_LIST_SIZE(program); i++) {
    if (AS_LIST_ELEMENT(program, i)->type != AST_LET_DECLARATION)
      continue;

    fprintf(file, "%s %s %s\n", AS_LIST_ELEMENT(program, i)->data.variable.name,
            data_byte_size(AS_LIST_ELEMENT(program, i)->data_type), "0");
  }
}

static void generate_strings() {
  fprintf(file, "; Strings\n");
  for (int i = 0; i < strings_table->capacity; i++) {
    if (strings_table->entries[i].key != NULL) {
      char var_name[20];
      Entry entry = strings_table->entries[i];

      sprintf(var_name, "string_%d", entry.hash % strings_table->capacity);
      fprintf(file, "%s db %s, 0\n", var_name, entry.key);

      strings_table->entries++;
    }
  }
  fprintf(file, "\n");
}

static void setup_asm_file(AstNode *program) {
  fprintf(file, "section .data\n");
  fprintf(file, "  equal db 'Equal', 0, 10\n"
                "  len_equal equ $ - equal\n"
                "  not_equal db 'Not Equal', 0, 10\n"
                "  len_not_equal equ $ - not_equal\n");

  generate_strings();
  generate_global_vars(program);

  fprintf(file, "\nsection .text\n"
                "\tglobal _start\n\n");

  fprintf(file, "\nend:\n"
                "   mov rax, 60\n"
                "   xor rdi, rdi\n"
                "   syscall\n");

  fprintf(file, "\nprint_equal: \n"
                "   mov rax, 1\n"
                "   mov rdi, 1\n"
                "   mov rsi, equal\n"
                "   mov rdx, len_equal\n"
                "   syscall\n"
                "   jmp end\n");

  fprintf(file, "\nprint_not_equal: \n"
                "   mov rax, 1\n"
                "   mov rdi, 1\n"
                "   mov rsi, not_equal\n"
                "   mov rdx, len_not_equal\n"
                "   syscall\n"
                "   jmp end\n");

  fprintf(file, "\n_start:\n");
  generate_asm(program);
  fprintf(file,
          "\n   pop r8\n"
          "   cmp r8, %d\n"
          "   je print_equal\n"
          "   jmp print_not_equal\n",
          result);
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
    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      generate_asm(AS_BINARYOP_RIGHT(node));

      fprintf(file, "   pop r9\n"
                    "   pop r10\n"
                    "   add r9, r10\n"
                    "   push r9\n\n");
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type == AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_RIGHT(node));
      fprintf(file,
              "   pop r9\n"
              "   mov r8, r9\n"
              "   add r8, %d\n"
              "   push r8\n\n",
              AS_BINARYOP_LEFT(node)->data.integer_val);
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type == AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      fprintf(file,
              "   pop r9\n"
              "   mov r8, r9\n"
              "   add r8, %d\n"
              "   push r8\n\n",
              AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    fprintf(file,
            "   mov r8, %d\n"
            "   add r8, %d\n"
            "   push r8\n\n",
            AS_BINARYOP_LEFT(node)->data.integer_val,
            AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;
  case AST_SUBTRACT:
    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      generate_asm(AS_BINARYOP_RIGHT(node));

      fprintf(file, "   pop r10\n"
                    "   pop r9\n"
                    "   sub r9, r10\n"
                    "   push r9\n\n");
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type == AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_RIGHT(node));
      fprintf(file,
              "   pop r9\n"
              "   mov r8, r9\n"
              "   sub r8, %d\n"
              "   push r8\n\n",
              AS_BINARYOP_LEFT(node)->data.integer_val);
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type == AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      fprintf(file,
              "   pop r9\n"
              "   mov r8, r9\n"
              "   sub r8, %d\n"
              "   push r8\n\n",
              AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    fprintf(file,
            "   mov r8, %d\n"
            "   sub r8, %d\n"
            "   push r8\n\n",
            AS_BINARYOP_LEFT(node)->data.integer_val,
            AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;
  case AST_MULTIPLY:
    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      generate_asm(AS_BINARYOP_RIGHT(node));

      fprintf(file, "   pop r9\n"
                    "   pop r10\n"
                    "   imul r9, r10\n"
                    "   push r9\n\n");
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type == AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_RIGHT(node));
      fprintf(file,
              "   pop r9\n"
              "   imul r9, %d\n"
              "   push r9\n\n",
              AS_BINARYOP_LEFT(node)->data.integer_val);
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type == AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      fprintf(file,
              "   pop r9\n"
              "   imul r9, %d\n"
              "   push r9\n\n",
              AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    fprintf(file,
            "   mov r9, %d\n"
            "   imul r9, %d\n"
            "   push r9\n\n",
            AS_BINARYOP_LEFT(node)->data.integer_val,
            AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;
  case AST_DIVIDE:
    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      generate_asm(AS_BINARYOP_RIGHT(node));

      fprintf(file, "   pop rax\n"
                    "   pop r9\n"
                    "   idiv r9\n"
                    "   push rax\n\n");
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type == AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      generate_asm(AS_BINARYOP_RIGHT(node));
      fprintf(file,
              "   pop r9\n"
              "   mov rax, %d\n"
              "   idiv r9\n"
              "   push rax\n\n",
              AS_BINARYOP_LEFT(node)->data.integer_val);
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type == AST_INTEGER) {

      generate_asm(AS_BINARYOP_LEFT(node));
      fprintf(file,
              "   pop rax\n"
              "   mov r9, %d\n"
              "   idiv r9\n"
              "   push rax\n\n",
              AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    fprintf(file,
            "   mov rax, %d\n"
            "   mov r9, %d\n"
            "   idiv r9\n"
            "   push rax\n\n",
            AS_BINARYOP_LEFT(node)->data.integer_val,
            AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;

    break;

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
    fprintf(file, "   push 0\n");
    generate_asm(node->data.variable.value);
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
    generate_asm(node->data.block.elements->items[0]);
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

static int evaluate(AstNode *node) {
  double val = 0;

#define evaluateNode(op)                                                       \
  evaluate(node->data.binaryop.left) op evaluate(node->data.binaryop.right)

  switch (node->type) {
  case AST_ADD:
    val += evaluateNode(+);
    break;
  case AST_SUBTRACT:
    val += evaluateNode(-);
    break;
  case AST_MULTIPLY:
    val += evaluateNode(*);
    break;
  case AST_DIVIDE:
    val += evaluateNode(/);
    break;
  case AST_NEGATE:
    val += -1 * test_evaluate(node->data.unaryop.operand);
    break;
  case AST_EQUAL_EQUAL:

  case AST_INTEGER:
    return node->data.integer_val;
    break;
  }
  return val;
}

void codegen(AstNode *program, Table *string_table) {
  file = open_file("./code.asm");
  strings_table = string_table;

  result =
      evaluate(program->data.block.elements->items[0]->data.variable.value);
  setup_asm_file(program);

  printf("%s\n", file->_IO_buf_base);
  fclose(file);

  system("nasm -f elf64 ./code.asm && ld code.o -o code && ./code");
}
