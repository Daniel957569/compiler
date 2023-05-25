#include "codegen.h"
#include "ast.h"
#include "memory.h"
#include "semantic_check.h"
#include "utils/array.h"
#include "utils/table.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WRITE_TO_STRING(dest, source, format, ...)                             \
  sprintf(source, format, ##__VA_ARGS__);                                      \
  allocate_to_string(dest, source);

#define GENERATE_LEFT_BINARYOP(node, format, ...)                              \
  generate_arithmetic(AS_BINARYOP_LEFT((node)));                               \
  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, (format),       \
                  ##__VA_ARGS__);

#define GENERATE_RIGHT_BINARYOP(node, format, ...)                             \
  generate_arithmetic(AS_BINARYOP_RIGHT((node)));                              \
  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, (format),       \
                  ##__VA_ARGS__);

#define GENERATE_BOTH_BINARYOP(node, format, ...)                              \
  generate_arithmetic(AS_BINARYOP_LEFT((node)));                               \
  generate_arithmetic(AS_BINARYOP_RIGHT((node)));                              \
  WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, (format),       \
                  ##__VA_ARGS__);

#define GENERATE_COMPARISON_OP(node)                                           \
  if ((node)->data.binaryop.right->type == AST_IDENTIFIER_REFRENCE) {          \
    generate_arithmetic((node)->data.binaryop.right);                          \
    generate_arithmetic((node)->data.binaryop.left);                           \
  } else {                                                                     \
    generate_arithmetic((node)->data.binaryop.left);                           \
    generate_arithmetic((node)->data.binaryop.right);                          \
  }

#define AS_IF_CONTROL_FLOW(node)                                               \
  (node)->data.control_flow.control_flow_statement->data.if_stmt
#define AS_WHILE_CONTROL_FLOW(node)                                            \
  (node)->data.control_flow.control_flow_statement->data.while_stmt

#define STACK_POSITION(pos) codegen_env.function_total_bytes_allocated - pos

#define DECLARATION true
#define SPECEFIZER false

typedef struct {
  FILE *file;
  Table *strings_table;
  StringArray *string_block_array;
  String *string;
  int function_total_bytes_allocated;
  char temp_string[1000];
  int label_sum;
} CodegenEnviroment;

const char *REGISTERS[10] = {"r10", "r11", "r12", "r13", "r14",
                             "r15", "rcx", "rdx", "rsi", "rdi"};
CodegenEnviroment codegen_env;
int result;

static void generate_arithmetic(AstNode *node);

static void init_codegen_enviroment() {
  CodegenEnviroment env;
  codegen_env.string_block_array = NULL;
  codegen_env.file = NULL;
  codegen_env.strings_table = NULL;
  codegen_env.string = NULL;
  codegen_env.label_sum = 0;
  codegen_env.function_total_bytes_allocated = 0;
}

static char *fill_whitespaces(char *str) {
  for (int i = 0; i < strlen(str); i++) {
    if (str[i] == ' ' || str[i] == ':' || str[i] == ';')
      str[i] = '_';
  }
  return str;
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
    // return is_declaring ? "dd" : "DWORD";

  case TYPE_BOOLEAN:
  case TYPE_VOID:
    return is_declaring ? "db" : "BYTE";

  case TYPE_FLOAT:
  case TYPE_INTEGER:
  case TYPE_STRING:
    return is_declaring ? "dq" : "QWORD";

  default:
    printf("unknown at data_byte_size function at codegen.c\n");
    return "-1";
  }
}

static char *data_type_to_register(DataType type) {
  switch (type) {
  case TYPE_FLOAT:
  case TYPE_STRING:
  case TYPE_INTEGER:
    return "rax";

    // return "eax";

  case TYPE_BOOLEAN:
    return "al";

  default:
    printf("unknown at data_type_to_register function at codegen.c\n");
    return "-1";
  }
}

static int align_function_stack(int total_bytes) {
#define ALIGNMENT 16
  int remainder = total_bytes % ALIGNMENT;
  int aligned_value =
      total_bytes + (remainder != 0 ? ALIGNMENT - remainder : 0);
  return aligned_value;
}

static void declare_global_vars(AstNode *program) {
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

static void generate_global_variables(AstNode *node) {
  codegen_env.string = init_string();

  switch (node->data.variable.value->type) {
  case AST_STRING:
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "\n; start of %s\n"
                    "   mov rdi, %s_string\n"
                    "   mov [%s], rdi\n"
                    "; end of %s\n\n",
                    node->data.variable.name,
                    fill_whitespaces(node->data.variable.value->data.str),
                    node->data.variable.name, node->data.variable.name);

    break;
  case AST_INTEGER:
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "\n; start of %s\n"
                    "   mov QWORD [%s], %d\n"
                    "; end of %s\n\n",
                    node->data.variable.name, node->data.variable.name,
                    node->data.variable.value->data.integer_val,
                    node->data.variable.name);
    break;

  case AST_TRUE:
  case AST_FALSE:
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "\n; start of %s\n"
                    "   mov BYTE [%s], %d\n"
                    "; end of %s\n\n",
                    node->data.variable.name, node->data.variable.name,
                    node->data.variable.value->data.boolean,
                    node->data.variable.name);
    break;

  default:
    generate_arithmetic(node->data.variable.value);
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   pop r8\n"
                    "   mov QWORD [%s], r8\n",
                    node->data.variable.name);

    break;
  }

  push_string_array(codegen_env.string_block_array, codegen_env.string);
}

static void generate_strings() {
  fprintf(codegen_env.file, "\n; Strings\n");
  for (int i = 0; i < codegen_env.strings_table->capacity; i++) {
    if (codegen_env.strings_table->entries[i].key != NULL) {
      Entry entry = codegen_env.strings_table->entries[i];

      // 7 for "string_"
      // surely there is a better way, but cant be bothered. sorry, mby later.
      char var_name[strlen(entry.key) + 100];
      strcpy(var_name, entry.key);
      sprintf(var_name, "%s_string", fill_whitespaces(var_name));

      fprintf(codegen_env.file, "%s db \"%s\", 0\n", var_name, entry.key);
      entry.key = var_name;
    }
  }
  fprintf(codegen_env.file, "\n");
}

static void setup_asm_file(AstNode *program) {
  fprintf(codegen_env.file, "section .data\n");
  fprintf(codegen_env.file, "  equal db 'Equal', 0, 10\n"
                            "  len_equal equ $ - equal\n"
                            "  not_equal db 'Not Equal', 0, 10\n"
                            "  len_not_equal equ $ - not_equal\n"
                            "  format_number db \"%%lld\", 10, 0\n"
                            "  format_string db \"%%s\", 10, 0\n"
                            "  format_true db \"true\", 10, 0\n"
                            "  format_false db \"false\", 10, 0\n");

  generate_strings();
  declare_global_vars(program);

  fprintf(codegen_env.file, "\nsection .text\n"
                            "\textern printf\n"
                            "\tglobal asm_start\n\n");

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
                            "   ret\n"

                            "\nprint_not_equal: \n"
                            "   mov rax, 1\n"
                            "   mov rdi, 1\n"
                            "   mov rsi, not_equal\n"
                            "   mov rdx, len_not_equal\n"
                            "   syscall\n"
                            "   ret\n");

  generate_asm(program);
  fprintf(codegen_env.file, "\nasm_start:\n"
                            "   push rbp\n"
                            "   mov rbp, rsp\n");
  for (int i = 0; i < codegen_env.string_block_array->size; i++) {
    fprintf(codegen_env.file, "%s",
            codegen_env.string_block_array->items[i]->content);
  }
  fprintf(codegen_env.file, "\n   call function_main\n"
                            "   call end\n");
}

static char *comparison_op_to_jump_instruction(AstNodeType type) {
  // everthing is the opposite because, just becuase.
  switch (type) {
  case AST_EQUAL_EQUAL:
    return "jne";

  case AST_NOT_EQUAL:
    return "je";

  case AST_LESS_EQUAL:
    return "jg";

  case AST_LESS:
    return "jge";

  case AST_GREATER_EQUAL:
    return "jl";

  case AST_GREATER:
    return "jle";
  default:
    printf("Undefined behevoir in comparison_op_to_jump_instruction\n");
    return "yo";
  }
}

static void generate_condition(AstNode *node) {
  codegen_env.string = init_string();

  generate_arithmetic(node);

  fprintf(codegen_env.file,
          "%s\n"
          "   pop r8\n"
          "   pop r9\n"
          "   cmp r8, r9\n"
          "   %s .L%d\n",
          codegen_env.string->content,
          comparison_op_to_jump_instruction(node->type), codegen_env.label_sum);

  free_string(codegen_env.string);
}

static void generate_local_variable(AstNode *node) {
  // both declaration and assignment share the smae functionallty
  switch (node->data.variable.value->type) {
  case AST_STRING:
    fprintf(codegen_env.file,
            "   mov rdi, %s_string\n"
            "   mov [rbp - %d], rdi\n\n",
            fill_whitespaces(node->data.variable.value->data.str),
            STACK_POSITION(node->data.variable.stack_pos));
    return;

  case AST_INTEGER:
    fprintf(codegen_env.file, "   mov QWORD [rbp - %d], %d\n\n",
            STACK_POSITION(node->data.variable.stack_pos),
            node->data.variable.value->data.integer_val);

    return;

  case AST_TRUE:
  case AST_FALSE:
    fprintf(codegen_env.file, "   mov BYTE [rbp - %d], %d\n\n",
            STACK_POSITION(node->data.variable.stack_pos),
            node->data.variable.value->data.boolean);
    return;

  case AST_FUNCTION_CALL:

    generate_asm(node->data.variable.value);

    fprintf(codegen_env.file, "   mov [rbp - %d], rdx\n",
            STACK_POSITION(node->data.variable.stack_pos));
    break;

  default:
    codegen_env.string = init_string();

    generate_arithmetic(node->data.variable.value);

    fprintf(codegen_env.file,
            "%s"
            "   pop r8\n"
            "   mov [rbp - %d], r8\n\n",
            codegen_env.string->content,
            STACK_POSITION(node->data.variable.stack_pos));

    free_string(codegen_env.string);
    return;
  }
}

static void initialize_function_argumnets(AstNode *node) {
  for (int i = 0; i < node->data.function_call.arguments->size; i++) {

    generate_arithmetic(node->data.function_call.arguments->items[i]);

    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, "   pop %s\n",
                    REGISTERS[i]);
  }
}

static void initialize_function_parameters(AstNode *node) {
  for (int i = 0; i < node->data.function_decl.parameters->size; i++) {
    fprintf(codegen_env.file, "   mov [rbp - %d], %s\n",
            STACK_POSITION(AS_PARAMETER(node, i)->stack_pos), REGISTERS[i]);
  }
}

static void generate_print_statement(AstNode *node) {
  switch (node->data_type) {
  case TYPE_STRING:
    fprintf(codegen_env.file, "\n   pop rax\n"
                              "   mov rsi, rax\n"
                              "   mov rdi, format_string\n"
                              "   xor rax, rax\n"
                              "   call printf\n");
    return;

  case TYPE_INTEGER:
    fprintf(codegen_env.file, "\n   pop rax\n"
                              "   mov rsi, rax\n"
                              "   mov rdi, format_number\n"
                              "   xor rax, rax\n"
                              "   call printf\n");
    return;

  case TYPE_BOOLEAN:
    fprintf(codegen_env.file,
            "   xor rsi, rsi\n"
            "   mov rdi, %s\n"
            "   xor rax, rax\n"
            "   call printf\n",
            node->data.boolean ? "format_true" : "format_false");

    break;

  case TYPE_FLOAT:
    // todo
  default:
    return;
  }
}

static void generate_arithmetic(AstNode *node) {
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
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   push %s_string\n", fill_whitespaces(node->data.str));
    // Handle string node
    break;
  case AST_IDENTIFIER_REFRENCE:
    if (node->data.variable.is_global) {

      WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                      "   mov %s, %s [%s]\n"
                      "   push rax\n",
                      data_type_to_register(node->data_type),
                      data_byte_size(node->data_type, SPECEFIZER),
                      node->data.variable.name);
    } else {
      WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                      "   mov %s, %s [rbp - %d]\n"
                      "   push rax\n",
                      data_type_to_register(node->data_type),
                      data_byte_size(node->data_type, SPECEFIZER),
                      STACK_POSITION(node->data.variable.stack_pos))
    }

    break;
  case AST_TRUE:
  case AST_FALSE:
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string, "   push %d\n",
                    node->data.boolean);

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
  case AST_DIVIDE: {
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
  }

  case AST_EQUAL_EQUAL:
  case AST_BANG:
  case AST_NOT_EQUAL:
  case AST_GREATER:
  case AST_GREATER_EQUAL:
  case AST_LESS:
  case AST_LESS_EQUAL:
    GENERATE_COMPARISON_OP(node);
    break;

  case AST_NEGATE:
    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   mov r9, %d\n"
                    "   neg r9\n"
                    "   push r9\n\n",
                    node->data.unaryop.operand->data.integer_val);
    break;
  case AST_FUNCTION_CALL:
    if (node->data.function_decl.parameters->size > 0) {
      initialize_function_argumnets(node);
    }

    WRITE_TO_STRING(codegen_env.string, codegen_env.temp_string,
                    "   call function_%s\n"
                    "   %s\n",
                    node->data.function_call.name,
                    node->type == TYPE_VOID ? "" : "push rdx\n");
    break;
  }
}

static void generate_asm(AstNode *node) {
  switch (node->type) {
  case AST_LET_DECLARATION:
    generate_local_variable(node);
    break;

  case AST_VAR_ASSIGNMENT:
    generate_local_variable(node);

    break;

  case AST_IF_STATEMNET: {
    bool has_else = node->data.if_stmt.else_body != NULL;
    int end_label = codegen_env.label_sum;
    node->data.if_stmt.end_label = end_label;

    generate_condition(node->data.if_stmt.condition);

    codegen_env.label_sum++;

    generate_asm(node->data.if_stmt.then_body);

    fprintf(codegen_env.file, "\n.L%d:\n", end_label);

    if (has_else) {
      generate_asm(node->data.if_stmt.else_body);
    }

    break;
  }

  case AST_WHILE_STATEMENT: {
    int start_label = codegen_env.label_sum;
    int end_label = codegen_env.label_sum + 1;
    node->data.while_stmt.end_label = end_label;
    node->data.while_stmt.start_label = start_label;

    codegen_env.label_sum++;

    fprintf(codegen_env.file, ".L%d:\n", start_label);
    generate_condition(node->data.while_stmt.condition);

    generate_asm(node->data.while_stmt.then_body);

    fprintf(codegen_env.file,
            "\n   jmp .L%d\n"
            "\n.L%d:\n",
            start_label, end_label);

    break;
  }

  case AST_RETURN_STATEMENT:
    codegen_env.string = init_string();

    generate_arithmetic(node->data.return_stmt.value);

    fprintf(codegen_env.file,
            "\n%s"
            "   pop rdx\n",
            codegen_env.string->content);
    free_string(codegen_env.string);
    break;

  case AST_BREAK_STATEMENT: {
    switch (node->data.control_flow.control_flow_statement->type) {
    case AST_IF_STATEMNET:
      fprintf(codegen_env.file, "   jmp .L%d\n",
              AS_IF_CONTROL_FLOW(node).end_label);

    case AST_WHILE_STATEMENT:
      fprintf(codegen_env.file, "   jmp .L%d\n",
              AS_WHILE_CONTROL_FLOW(node).end_label);

    default:
      printf("yo, error at break stmt codegen generate_asm. gl");
    }

    break;
  }

  case AST_CONTINUE_STATEMENT:
    switch (node->data.control_flow.control_flow_statement->type) {
    case AST_IF_STATEMNET:
      fprintf(codegen_env.file, "   jmp .L%d\n",
              AS_IF_CONTROL_FLOW(node).end_label);

    case AST_WHILE_STATEMENT:
      fprintf(codegen_env.file, "   jmp .L%d\n",
              AS_WHILE_CONTROL_FLOW(node).start_label);

    default:
      printf("yo, error at CONTINURE stmt codegen generate_asm. gl");
    }
    break;

  case AST_PRINT_STATEMENT:
    codegen_env.string = init_string();

    generate_arithmetic(node->data.print_stmt.value);

    fprintf(codegen_env.file, "%s", codegen_env.string->content);
    free_string(codegen_env.string);

    generate_print_statement(node->data.print_stmt.value);
    break;

  case AST_FUNCTION:
    node->data.function_decl.byte_allocated =
        codegen_env.function_total_bytes_allocated =
            align_function_stack(node->data.function_decl.byte_allocated);

    fprintf(codegen_env.file,
            "\nfunction_%s:\n"
            "   push rbp\n"
            "   mov rbp, rsp\n"
            "   sub rsp, %d\n\n",
            node->data.function_decl.name,

            node->data.function_decl.byte_allocated);

    if (node->data.function_decl.parameters->size > 0) {
      initialize_function_parameters(node);
    }

    generate_asm(node->data.function_decl.body);

    fprintf(codegen_env.file, "\n   leave\n"
                              "   ret\n");
    break;

  case AST_FUNCTION_CALL:
    codegen_env.string = init_string();
    if (node->data.function_decl.parameters->size > 0) {
      initialize_function_argumnets(node);
    }

    fprintf(codegen_env.file,
            "%s"
            "   call function_%s\n",
            codegen_env.string->content, node->data.function_call.name);

    free_string(codegen_env.string);

    break;

  case AST_PROGRAM:
    for (int i = 0; i < node->data.block.elements->size; i++) {
      // global variables
      if (node->data.block.elements->items[i]->type == AST_LET_DECLARATION ||
          node->data.block.elements->items[i]->type == AST_VAR_ASSIGNMENT) {
        generate_global_variables(node->data.block.elements->items[i]);
        continue;
      }

      generate_asm(node->data.block.elements->items[i]);
    }
    break;
  case AST_BLOCK:
    for (int i = 0; i < node->data.block.elements->size; i++) {
      generate_asm(node->data.block.elements->items[i]);
    }
    break;
  default:
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

  setup_asm_file(program);

  printf("%s\n", codegen_env.file->_IO_buf_base);
  fclose(codegen_env.file);

  system("nasm -f elf64 code.asm && gcc -no-pie -o code code.o asm_link.c && "
         "./code");

  free_tree(program);
  free_table(string_table);
}
