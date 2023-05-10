#ifndef AST_H
#define AST_H

#include "./utils/array.h"
#include "common.h"
#include <stddef.h>
#include <sys/types.h>

#define AS_LIST_SIZE(program) (program)->data.block.size
#define AS_LIST_CAPACITY(program) (program)->data.block.capacity
#define AS_LIST_ELEMENTS(program) (program)->data.block.elements

typedef enum {
  AST_LITERAL,
  AST_STRING,

  AST_IDENTIFIER_REFRENCE,

  AST_TRUE,
  AST_FALSE,

  AST_ADD,
  AST_SUBTRACT,
  AST_MULTIPLY,
  AST_DIVIDE,

  AST_EQUAL_EQUAL,

  AST_BANG,
  AST_NOT_EQUAL,

  AST_GREATER,
  AST_GREATER_EQUAL,

  AST_LESS,
  AST_LESS_EQUAL,

  AST_NEGATE,

  AST_LET_DECLARATION,
  AST_VAR_ASSIGNMENT,

  AST_IF_STATEMNET,
  AST_WHILE_STATEMENT,

  AST_RETURN_STATEMENT,
  AST_CONTINUE_STATEMENT,

  AST_FUNCTION,
  AST_FUNCTION_CALL,

  AST_PROGRAM,
  AST_BLOCK,
} AstNodeType;

// add struct later on
typedef enum {
  TYPE_VOID,
  TYPE_BOOLEAN,
  TYPE_INTEGER,
  TYPE_FLOAT,
  TYPE_STRING
} Type;

typedef struct AstNode {
  AstNodeType type;
  Type data_type;
  int line;
  union {
    double value;                    // AST_LITERAL
    bool boolean;                    // AST_TRUE AST_FALSE
    const char *str;                 // AST_STRING
    const char *identifier_refrence; // AST_IDENTIFIER
    struct { // AST_ADD, AST_SUBTRACT, AST_MULTIPLY, AST_DIVIDE ...
      struct AstNode *left;
      struct AstNode *right;
    } binaryop;
    struct { // AST_NEGATE AST_BANG
      struct AstNode *operand;
    } unaryop;
    struct {
      const char *name;
      u_int32_t string_hash;
      struct AstNode *value;
    } variable;
    struct {
      struct AstNode *condition;
      struct AstNode *then_body;
      struct AstNode *else_body;
    } if_stmt;
    struct {
      struct AstNode *condition;
      struct AstNode *then_body;
    } while_stmt;
    struct {
      struct AstNode *value;
    } return_stmt;
    struct {
      const char *name;
      u_int32_t string_hash;
      StringArray *parameters;
      struct AstNode *body;
    } function_decl;
    struct {
      const char *name;
      u_int32_t string_hash;
      AstArray *arguments;
    } function_call;
    struct { // block, program_node
      AstArray *elements;
    } block;
  } data;
} AstNode;

AstNode *ast_create_literal(int value, int line);
AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right,
                             int line);
AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand, int line);
AstNode *ast_create_boolean(AstNodeType type, bool boolean, int line);
AstNode *ast_create_string(const char *str, int line);
AstNode *ast_create_identifier_refrence(const char *str, int line);
AstNode *ast_create_variable_stmt(AstNodeType type, Type data_type,
                                  const char *name, int line, AstNode *value);
AstNode *ast_create_if_statement(AstNode *condition, AstNode *then_body,
                                 AstNode *else_body, int line);
AstNode *ast_create_while_statement(AstNode *condition, AstNode *then_body,
                                    int line);
AstNode *ast_create_return_statement(Type type, AstNode *value, int line);
AstNode *ast_create_continue_statement(int line);
AstNode *ast_create_function_declaration(Type type, const char *function_name,
                                         StringArray *parameters,
                                         AstNode *function_body, int line);
AstNode *ast_create_function_call(const char *function_name,
                                  AstArray *arguments, int line);

AstNode *ast_create_block(AstNodeType type, int line);

double test_evaluate(AstNode *node);
void free_tree(AstNode *node);
void print_ast(AstNode *node, int depth);

#endif
