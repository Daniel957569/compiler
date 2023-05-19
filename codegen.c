#include "codegen.h"
#include "ast.h"
#include "memory.h"
#include "utils/array.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define WRITE_TO_STRING(dest, source, format, ...)                             \
  sprintf(source, format, ##__VA_ARGS__);                                      \
  allocate_to_string(dest, source);

#define GENERATE_LEFT_BINARYOP(node, format, ...)                              \
  generate_asm(AS_BINARYOP_LEFT(node));                                        \
  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, format,         \
                  ##__VA_ARGS__);

#define GENERATE_RIGHT_BINARYOP(node, format, ...)                             \
  generate_asm(AS_BINARYOP_RIGHT(node));                                       \
  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, format,         \
                  ##__VA_ARGS__);

#define GENERATE_BOTH_BINARYOP(node, format, ...)                              \
  generate_asm(AS_BINARYOP_LEFT(node));                                        \
  generate_asm(AS_BINARYOP_RIGHT(node));                                       \
  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, format,         \
                  ##__VA_ARGS__);

#define DECLARATION true
#define SPECEFIZER false

typedef struct {
  FILE *file;
  Table *strings_table;
  StringArray *string_block_array;
  String *string;
  char temp_string[1000];
} CodegenEnviroment;

CodegenEnviroment codegen_env;
int result;

static void init_codegen_enviroment() {
  CodegenEnviroment env;
  codegen_env.string_block_array = NULL;
  codegen_env.file = NULL;
  codegen_env.strings_table = NULL;
  codegen_env.string = NULL;
}

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

static char *data_byte_size(DataType type, bool is_declaring) {
  switch (type) {
  case TYPE_INTEGER:
  case TYPE_FLOAT:
    return is_declaring ? "dd" : "DWORD";

  case TYPE_BOOLEAN:
  case TYPE_VOID:
  case TYPE_STRING:
    return is_declaring ? "db" : "BYTE";

  default:
    printf("unknown at data_byte_size function at codegen.c\n");
    return "-1";
  }
}

static void generate_global_vars(AstNode *program) {
  fprintf(codegen_env.file, "; global variabless\n");
  for (int i = 0; i < AS_LIST_SIZE(program); i++) {
    if (AS_LIST_ELEMENT(program, i)->type != AST_LET_DECLARATION)
      continue;

    fprintf(codegen_env.file, "%s %s %s\n",
            AS_LIST_ELEMENT(program, i)->data.variable.name,
            data_byte_size(AS_LIST_ELEMENT(program, i)->data_type, DECLARATION),
            "0");
  }
}

static void assign_global_vars(AstNode *variable) {
  codegen_env.string = init_string();
  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                  "\n; start of %s\n", variable->data.variable.name);

  generate_asm(variable->data.variable.value);

  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                  "   pop r8\n"
                  "   mov [%s], r8\n"
                  "\n; end of %s\n\n",
                  variable->data.variable.name, variable->data.variable.name);
  push_string_array(codegen_env.string_block_array, codegen_env.string);
}

static void generate_strings() {
  fprintf(codegen_env.file, "; Strings\n");
  for (int i = 0; i < codegen_env.strings_table->capacity; i++) {
    if (codegen_env.strings_table->entries[i].key != NULL) {
      char var_name[20];
      Entry entry = codegen_env.strings_table->entries[i];

      sprintf(var_name, "string_%d",
              entry.hash % codegen_env.strings_table->capacity);
      fprintf(codegen_env.file, "%s db %s, 0\n", var_name, entry.key);

      codegen_env.strings_table->entries++;
    }
  }
  fprintf(codegen_env.file, "\n");
}

static void setup_asm_file(AstNode *program) {
  fprintf(codegen_env.file, "section .data\n");
  fprintf(codegen_env.file, "  equal db 'Equal', 0, 10\n"
                            "  len_equal equ $ - equal\n"
                            "  not_equal db 'Not Equal', 0, 10\n"
                            "  len_not_equal equ $ - not_equal\n");

  generate_strings();
  generate_global_vars(program);

  fprintf(codegen_env.file, "\nsection .text\n"
                            "\tglobal _start\n\n");

  fprintf(codegen_env.file, "\nend:\n"
                            "   mov rax, 60\n"
                            "   xor rdi, rdi\n"
                            "   syscall\n"

                            "\nprint_equal: \n"
                            "   mov rax, 1\n"
                            "   mov rdi, 1\n"
                            "   mov rsi, equal\n"
                            "   mov rdx, len_equal\n"
                            "   syscall\n"
                            "   jmp end\n"

                            "\nprint_not_equal: \n"
                            "   mov rax, 1\n"
                            "   mov rdi, 1\n"
                            "   mov rsi, not_equal\n"
                            "   mov rdx, len_not_equal\n"
                            "   syscall\n"
                            "   jmp end\n");

  generate_asm(program);
  fprintf(codegen_env.file, "\n_start:\n");
  for (int i = 0; i < codegen_env.string_block_array->size; i++) {
    fprintf(codegen_env.file, "%s",
            codegen_env.string_block_array->items[i]->content);
  }
  fprintf(codegen_env.file, "\n   call main\n"
                            "   call end\n");
}

static void generate_binary_operations() {}

static void generate_condition(AstNode *node) {
  codegen_env.string = init_string();

  generate_asm(node);

  fprintf(codegen_env.file,
          "%s\n"
          "   pop r8\n"
          "   pop r9\n"
          "   cmp r8, r9\n"
          "   je print_equal\n"
          "   jmp print_not_equal\n",
          codegen_env.string->content);

  free_string(codegen_env.string);
}

static void generate_asm(AstNode *node) {
  switch (node->type) {
  case AST_INTEGER:
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, "   push %d\n",
                    node->data.integer_val);
    // Handle integer node
    break;
  case AST_FLOAT:
    // Handle float node
    break;
  case AST_STRING:
    // Handle string node
    break;
  case AST_IDENTIFIER_REFRENCE:
    if (node->data.variable.is_global) {
      WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                      "   mov eax, %s [%s]\n"
                      "   push rax\n",
                      data_byte_size(node->data_type, SPECEFIZER),
                      node->data.variable.name);
    } else {
      WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                      "   mov eax, %s [rsp - %d]\n"
                      "   push rax\n",
                      data_byte_size(node->data_type, SPECEFIZER),
                      node->data.variable.stack_pos);
    }

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
      GENERATE_BOTH_BINARYOP(node, "   pop r9\n"
                                   "   pop r10\n"
                                   "   add r9, r10\n"
                                   "   push r9\n\n");

      break;
    }

    if (AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {
      GENERATE_RIGHT_BINARYOP(node,
                              "   pop r9\n"
                              "   mov r8, r9\n"
                              "   add r8, %d\n"
                              "   push r8\n\n",
                              AS_BINARYOP_LEFT(node)->data.integer_val);
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER) {
      GENERATE_LEFT_BINARYOP(node,
                             "   pop r9\n"
                             "   mov r8, r9\n"
                             "   add r8, %d\n"
                             "   push r8\n\n",
                             AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   mov r8, %d\n"
                    "   add r8, %d\n"
                    "   push r8\n\n",
                    AS_BINARYOP_LEFT(node)->data.integer_val,
                    AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;
  case AST_SUBTRACT:
    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {
      GENERATE_BOTH_BINARYOP(node, "   pop r9\n"
                                   "   pop r10\n"
                                   "   add r9, r10\n"
                                   "   push r9\n\n");

      break;
    }

    if (AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {
      GENERATE_RIGHT_BINARYOP(node,
                              "   pop r9\n"
                              "   mov r8, r9\n"
                              "   sub r8, %d\n"
                              "   push r8\n\n",
                              AS_BINARYOP_LEFT(node)->data.integer_val);
      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER) {
      GENERATE_LEFT_BINARYOP(node,
                             "   pop r9\n"
                             "   mov r8, r9\n"
                             "   sub r8, %d\n"
                             "   push r8\n\n",
                             AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   mov r8, %d\n"
                    "   sub r8, %d\n"
                    "   push r8\n\n",
                    AS_BINARYOP_LEFT(node)->data.integer_val,
                    AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;
  case AST_MULTIPLY:
    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {
      GENERATE_BOTH_BINARYOP(node, "   pop r9\n"
                                   "   pop r10\n"
                                   "   imul r9, r10\n"
                                   "   push r9\n\n");

      break;
    }

    if (AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {
      GENERATE_RIGHT_BINARYOP(node,
                              "   pop r9\n"
                              "   imul r9, %d\n"
                              "   push r9\n\n",
                              AS_BINARYOP_LEFT(node)->data.integer_val);

      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER) {
      GENERATE_LEFT_BINARYOP(node,
                             "   pop r9\n"
                             "   imul r9, %d\n"
                             "   push r9\n\n",
                             AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   mov r9, %d\n"
                    "   imul r9, %d\n"
                    "   push r9\n\n",
                    AS_BINARYOP_LEFT(node)->data.integer_val,
                    AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;
  case AST_DIVIDE:
    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {

      GENERATE_BOTH_BINARYOP(node, "   pop rax\n"
                                   "   pop r9\n"
                                   "   idiv r9\n"
                                   "   push rax\n\n");

      break;
    }

    if (AS_BINARYOP_LEFT(node)->type == AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type != AST_INTEGER) {
      GENERATE_RIGHT_BINARYOP(node,
                              "   pop r9\n"
                              "   mov rax, %d\n"
                              "   idiv r9\n"
                              "   push rax\n\n",
                              AS_BINARYOP_LEFT(node)->data.integer_val);

      break;
    }

    if (AS_BINARYOP_LEFT(node)->type != AST_INTEGER &&
        AS_BINARYOP_RIGHT(node)->type == AST_INTEGER) {
      GENERATE_LEFT_BINARYOP(node,
                             "   pop rax\n"
                             "   mov r9, %d\n"
                             "   idiv r9\n"
                             "   push rax\n\n",
                             AS_BINARYOP_RIGHT(node)->data.integer_val);
      break;
    }

    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   mov rax, %d\n"
                    "   mov r9, %d\n"
                    "   idiv r9\n"
                    "   push rax\n\n",
                    AS_BINARYOP_LEFT(node)->data.integer_val,
                    AS_BINARYOP_RIGHT(node)->data.integer_val);

    break;

  case AST_EQUAL_EQUAL:

    generate_asm(node->data.binaryop.left);
    generate_asm(node->data.binaryop.right);

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
    fprintf(codegen_env.file,
            "   mov r9, %d\n"
            "   neg r9\n"
            "   push r9\n\n",
            node->data.unaryop.operand->data.integer_val);
    break;

  case AST_LET_DECLARATION:

    codegen_env.string = init_string();

    // incase it is the first in the function so it
    // doesn't pop arbitary number
    fprintf(codegen_env.file, "   push 0\n");

    generate_asm(node->data.variable.value);

    fprintf(codegen_env.file, "%s\n", codegen_env.string->content);
    fprintf(codegen_env.file,
            "   pop r8\n"
            "   mov [rsp], r8\n",
            node->data.variable.stack_pos);

    free_string(codegen_env.string);
    break;

  case AST_VAR_ASSIGNMENT:
    break;

  case AST_IF_STATEMNET:
    generate_condition(node->data.if_stmt.condition);

    break;

  case AST_WHILE_STATEMENT:
    break;

  case AST_RETURN_STATEMENT:
    break;

  case AST_CONTINUE_STATEMENT:

    break;

  case AST_FUNCTION:
    fprintf(codegen_env.file,
            "\n%s:\n"
            "   push rbp\n"
            "   mov rbp, rsp\n"
            "   sub rsp, %d\n\n",
            node->data.function_decl.name,
            node->data.function_decl.byte_allocated);

    generate_asm(node->data.function_decl.body);

    fprintf(codegen_env.file, "   leave\n"
                              "   ret\n");
    break;

  case AST_FUNCTION_CALL:
    break;

  case AST_PROGRAM:
    for (int i = 0; i < node->data.block.elements->size; i++) {
      // global variables
      if (node->data.block.elements->items[i]->type == AST_LET_DECLARATION) {
        assign_global_vars(node->data.block.elements->items[i]);
        continue;
      }

      generate_asm(node->data.block.elements->items[i]);
    }
    // Handle program node
    break;
  case AST_BLOCK:
    for (int i = 0; i < node->data.block.elements->size; i++) {
      generate_asm(node->data.block.elements->items[i]);
    }
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
  init_codegen_enviroment();
  codegen_env.file = open_file("./code.asm");
  codegen_env.strings_table = string_table;
  codegen_env.string_block_array = init_string_array();

  result =
      evaluate(program->data.block.elements->items[0]->data.variable.value);
  setup_asm_file(program);

  printf("%s\n", codegen_env.file->_IO_buf_base);
  fclose(codegen_env.file);

  system("nasm -f elf64 ./code.asm && ld code.o -o code && "
         "./code");
}
