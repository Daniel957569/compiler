#ifndef AST_H
#define AST_H

#include "./utils/array.h"
#include "common.h"
#include <stddef.h>

#define AS_LIST_SIZE(program) (program)->data.block.size
#define AS_LIST_CAPACITY(program) (program)->data.block.capacity
#define AS_LIST_ELEMENTS(program) (program)->data.block.elements

typedef enum {
  AST_LITERAL,
  AST_STRING,

  AST_IDENTIFIER,

  AST_TRUE,
  AST_FALSE,

  AST_ADD,
  AST_SUBTRACT,
  AST_MULTIPLY,
  AST_DIVIDE,

  AST_EQUAL,
  AST_EQUAL_EQUAL,

  AST_BANG,
  AST_NOT_EQUAL,

  AST_GREATER,
  AST_GREATER_EQUAL,

  AST_LESS,
  AST_LESS_EQUAL,

  AST_NEGATE,

  AST_VARIABLE,
  AST_LET_DECLARATION,
  AST_VAR_ASSIGNMENT,

  AST_IF_STATEMNET,
  AST_WHILE_STATEMENT,

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
  union {
    double value;              // AST_LITERAL
    bool boolean;              // AST_TRUE AST_FALSE
    char *str;                 // AST_STRING
    char *identifier_refrence; // AST_IDENTIFIER
    struct { // AST_ADD, AST_SUBTRACT, AST_MULTIPLY, AST_DIVIDE ...
      struct AstNode *left;
      struct AstNode *right;
    } binaryop;
    struct { // AST_NEGATE AST_BANG
      struct AstNode *operand;
    } unaryop;
    struct {
      const char *name;
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
      const char *name;
      StringArray *parameters;
      struct AstNode *body;
    } function_decl;
    struct {
      const char *name;
      AstArray *arguments;
    } function_call;
    struct { // block, program_node
      struct AstNode *parent_block;
      AstArray *elements;
    } block;
  } data;
} AstNode;

AstNode *ast_create_literal(int value);
AstNode *ast_create_binaryop(AstNodeType type, AstNode *left, AstNode *right);
AstNode *ast_create_unaryop(AstNodeType type, AstNode *operand);
AstNode *ast_create_boolean(AstNodeType type, bool boolean);
AstNode *ast_create_string(AstNodeType type, char *str);
AstNode *ast_create_identifier_refrence(char *str);
AstNode *ast_create_variable_stmt(AstNodeType type, Type data_type,
                                  const char *name, AstNode *value);
AstNode *ast_create_if_stmt(AstNode *condition, AstNode *then_body,
                            AstNode *else_body);
AstNode *ast_create_while_stmt(AstNode *condition, AstNode *then_body);
AstNode *ast_create_function_declaration(Type type, const char *function_name,
                                         StringArray *parameters,
                                         AstNode *function_body);
AstNode *ast_create_function_call(const char *function_name,
                                  AstArray *arguments);

AstNode *ast_create_block(AstNodeType type, AstNode *parent_block);

double test_evaluate(AstNode *node);
void free_tree(AstNode *node);
void print_ast(AstNode *node, int depth);

#endif
