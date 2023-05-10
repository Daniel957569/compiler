#include "parser.h"
#include "./utils/array.h"
#include "ast.h"
#include "common.h"
#include "scanner.h"
#include "semantic_check.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  Token *tokens;
  Token *currentToken;
  AstNode *current_block;
  int current;
  bool inPanicMode;
  bool hadError;
} Parser;

Parser parser;

static void initParser(Token *tokens, AstNode *block) {
  parser.tokens = tokens;
  parser.current_block = block;
  parser.currentToken = tokens;
  parser.current = 0;
  parser.hadError = false;
  parser.inPanicMode = false;
}

static void errorAt(Token *token, const char *message) {
  if (parser.inPanicMode)
    return;
  parser.inPanicMode = true;
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }
  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}

static void error(const char *message) {
  errorAt(parser.currentToken, message);
}

static void errorAtCurrent(const char *message) {
  errorAt(parser.currentToken, message);
}

static void advance() {
  parser.current++;
  parser.currentToken++;

  if (parser.currentToken->type == TOKEN_ERROR) {
    errorAtCurrent(parser.currentToken->start);
  }
}

static Token *previous() { return parser.currentToken - 1; }

static Token *current() { return parser.currentToken; }

static bool match(TokenType type) {
  if (type == parser.currentToken->type) {
    advance();
    return true;
  }

  return false;
}

static void consume(TokenType type, const char *errorMessage) {
  if (match(type)) {
    return;
  }

  errorAtCurrent(errorMessage);
}

static bool check(TokenType type) { return type == parser.currentToken->type; }

static bool checkNext(TokenType type) {
  return type == (parser.currentToken + 1)->type;
}

static TokenType peek() { return parser.currentToken->type; }

// fix in the future. unnecessary
static AstNodeType toNodeType(TokenType type) {
  switch (type) {
  case TOKEN_SLASH:
    return AST_DIVIDE;
  case TOKEN_STAR:
    return AST_MULTIPLY;
  case TOKEN_PLUS:
    return AST_ADD;
  case TOKEN_MINUS:
    return AST_SUBTRACT;

  case TOKEN_GREATER:
    return AST_GREATER;
  case TOKEN_GREATER_EQUAL:
    return AST_GREATER_EQUAL;

  case TOKEN_LESS:
    return AST_LESS;
  case TOKEN_LESS_EQUAL:
    return AST_LESS_EQUAL;

  case TOKEN_EQUAL_EQUAL:
    return AST_EQUAL_EQUAL;

  case TOKEN_BANG_EQUAL:
    return AST_NOT_EQUAL;
  case TOKEN_BANG:
    return AST_BANG;

  default:
    return AST_INTEGER;
    // error:
  }
}

static void synchronize() {
  parser.inPanicMode = false;

  while (parser.currentToken->type != TOKEN_EOF) {
    if (previous()->type == TOKEN_SEMICOLON)
      return;
    switch (parser.currentToken->type) {
    case TOKEN_FUN:
    case TOKEN_LET:
    case TOKEN_FOR:
    case TOKEN_IF:
    case TOKEN_WHILE:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
      return;

    default:; // Do nothing.
    }

    advance();
  }
}

static char *parseName(Token *token) {
  char *result = malloc(sizeof(char) * token->length);
  strncpy(result, token->start, token->length);
  result[token->length] = '\0';
  return result;
}

static DataType variable_type() {
  TokenType type = peek();
  advance();
  switch (type) {
  case TOKEN_BOOLEAN_TYPE:
    return TYPE_BOOLEAN;
  case TOKEN_FLOAT_TYPE:
    return TYPE_FLOAT;
  case TOKEN_STRING_TYPE:
    return TYPE_STRING;
  case TOKEN_INTEGER_TYPE:
    return TYPE_INTEGER;
  case TOKEN_VOID_TYPE:
    return TYPE_VOID;
  default:
    parser.inPanicMode = true;
    errorAtCurrent("Expected valid variable type.");
    return TYPE_VOID;
  }
}

static AstNode *expr();
static AstNode *comparison();
static AstNode *declaration();
static AstNode *equality();
static AstNode *statement();

static AstNode *number() {
  double num = strtod(parser.currentToken->start, NULL);
  return ast_create_number(num, current()->line);
}

static AstNode *float_num() {
  float num = strtod(parser.currentToken->start, NULL);
  return ast_create_float(num, current()->line);
}

static AstArray *arguments() {
  // arguments      → expression ( "," expression )* ;
  AstArray *arguments_list = init_ast_array();

  if (!check(TOKEN_RIGHT_PAREN)) {
    push_ast_array(arguments_list, expr());

    while (match(TOKEN_COMMA)) {
      push_ast_array(arguments_list, expr());
    }
  }

  consume(TOKEN_RIGHT_PAREN, "Expected ')' after function arguments");

  return arguments_list;
}

static IdentifierArray *parameters() {
  // parameters     → IDENTIFIER: type ( "," IDENTIFIER: type )* ;
  IdentifierArray *parameters_array = init_identifier_array();
  if (check(TOKEN_IDENTIFIER)) {
    do {
      char *parameter = parseName(current());
      advance();
      consume(TOKEN_COLONS, "Expected ':' after parameter to define type");
      DataType parameter_type = variable_type();
      push_identifier_array(parameters_array,
                            create_identifier(parameter, parameter_type));
    } while (match(TOKEN_COMMA));
  }

  return parameters_array;
}

static AstNode *primary() {
  if (check(TOKEN_NUMBER)) {
    AstNode *node = number();
    advance();
    return node;
  }
  if (check(TOKEN_FLOAT)) {
    AstNode *node = float_num();
    advance();
    return node;
  }
  if (match(TOKEN_TRUE)) {
    AstNode *node = ast_create_boolean(AST_TRUE, true, current()->line);
    return node;
  }
  if (match(TOKEN_FALSE)) {
    AstNode *node = ast_create_boolean(AST_FALSE, false, current()->line);
    return node;
  }
  if (match(TOKEN_STRING)) {
    AstNode *node = ast_create_string(parseName(previous()), current()->line);
    return node;
  }
  if (match(TOKEN_IDENTIFIER)) {
    return ast_create_identifier_refrence(parseName(previous()),
                                          current()->line);
  }

  consume(TOKEN_LEFT_PAREN, "Expected '(' before Expression.");
  AstNode *node = expr();
  consume(TOKEN_RIGHT_PAREN, "Expected ')' after Expression.");
  return node;
}

static AstNode *call() {
  if (check(TOKEN_IDENTIFIER)) {
    char *calle = parseName(current());

    // assignment
    if (checkNext(TOKEN_EQUAL)) {
      // change later, twice to get up to the right of =
      advance();
      advance();

      AstNode *node = expr();
      return ast_create_variable_stmt(AST_VAR_ASSIGNMENT, node->data_type,
                                      calle, current()->line, node);
    }

    // function callling
    if (checkNext(TOKEN_LEFT_PAREN)) {
      // change later
      advance();
      advance();

      AstArray *args = arguments();

      return ast_create_function_call(calle, args, current()->line);
    }

    // add struct props
    if (checkNext(TOKEN_DOT)) {
      printf("TODO: access struct props\n");
    }
  }

  // add for struct later:  if (match(TOKEN_DOT))
  return primary();
}

static AstNode *unary() {
  if (match(TOKEN_MINUS) || match(TOKEN_BANG)) {
    AstNodeType unaryop = toNodeType(previous()->type);
    AstNode *node = call();
    node = ast_create_binaryop(unaryop, node, NULL, current()->line);
    return node;
  }

  return call();
}

static AstNode *factor() {
  AstNode *node = unary();

  while (match(TOKEN_STAR) || match(TOKEN_SLASH)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = unary();
    node = ast_create_binaryop(operator, node, right, current()->line);
  }
  return node;
}

static AstNode *term() {
  AstNode *node = factor();

  while (match(TOKEN_MINUS) || match(TOKEN_PLUS)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = factor();
    node = ast_create_binaryop(operator, node, right, current()->line);
  }

  return node;
}

static AstNode *comparison() {
  AstNode *node = term();

  while (match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) ||
         match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = term();
    node = ast_create_binaryop(operator, node, right, current()->line);
  }

  return node;
}

static AstNode *equality() {
  AstNode *node = comparison();

  while (match(TOKEN_BANG) || match(TOKEN_EQUAL_EQUAL)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = comparison();
    node = ast_create_binaryop(operator, node, right, current()->line);
  }

  return node;
}

static AstNode *expr() { return equality(); }

static AstNode *expr_statment() {
  AstNode *node = expr();
  consume(TOKEN_SEMICOLON, "Expected ';' after Expression.");
  return node;
}

/* static AstNode *assignment() { */
/*   if (match(TOKEN_LEFT_PAREN)) { */
/*     char *calle = parseName(previous()); */

/*     AstArray *args = arguments(); */
/*     consume(TOKEN_SEMICOLON, "Expected ';' after calling a function"); */
/*     return ast_create_function_call(calle, args); */
/*   } */
/*   const char *varName = parseName(previous()); */
/*   consume(TOKEN_EQUAL, "Expected '=' after IDENTIFIER"); */
/*   AstNode *node = expr(); */
/*   consume(TOKEN_SEMICOLON, "Expected ';' after Expression"); */

/*   return ast_create_variable_stmt(AST_VAR_ASSIGNMENT, TYPE_VOID, varName,
 * node); */
/* } */

static AstNode *block() {
  AstNode *block = ast_create_block(AST_BLOCK, current()->line);
  parser.current_block = block;

  while (!match(TOKEN_RIGHT_BRACE)) {
    if (peek() == TOKEN_EOF) {
      // exiting because it has eaten the entire program tokens.
      errorAt(current(), "block was not closed with '}'.");
      exit(64);
    }
    push_ast_array(block->data.block.elements, declaration());
  }

  return block;
}

static AstNode *if_statement() {
  AstNode *condition = equality();

  consume(TOKEN_LEFT_BRACE, "Expect '{' after if condition");
  AstNode *then_body = block();

  AstNode *else_body = NULL;
  if (match(TOKEN_ELSE)) {
    consume(TOKEN_LEFT_BRACE, "Expected '{' after condition");
    else_body = block();
  }

  return ast_create_if_statement(condition, then_body, else_body,
                                 current()->line);
}

static AstNode *while_statement() {
  AstNode *condition = equality();

  consume(TOKEN_LEFT_BRACE, "Expect '{' after if condition");
  AstNode *then_body = block();

  return ast_create_while_statement(condition, then_body, current()->line);
}

static AstNode *return_statement() {
  AstNode *value = expr();
  consume(TOKEN_SEMICOLON, "Expected ';' after return value.");
  return ast_create_return_statement(value->data_type, value, current()->line);
}

static AstNode *continue_statement() {
  consume(TOKEN_SEMICOLON, "Expected ';' after continue statement");
  return ast_create_continue_statement(current()->line);
}

static AstNode *statement() {
  if (match(TOKEN_IF)) {
    return if_statement();
  } else if (match(TOKEN_WHILE)) {
    return while_statement();
  } else if (match(TOKEN_RETURN)) {
    return return_statement();
  } else if (match(TOKEN_CONTINUE)) {
    return continue_statement();
  }

  return expr_statment();
}

static AstNode *let_declaration() {
  const char *varName = parseName(current());
  consume(TOKEN_IDENTIFIER, "Expected IDENTIFIER after 'let'");

  consume(TOKEN_COLONS, "Expected ':' after after variable declaration.");
  // add struct later on
  DataType type = variable_type();

  AstNode *node;
  if (match(TOKEN_EQUAL)) {
    node = expr();
  } else {
    node = NULL;
  }
  consume(TOKEN_SEMICOLON, "Expected ';' after Expression");

  return ast_create_variable_stmt(AST_LET_DECLARATION, type, varName,
                                  current()->line, node);
}

static AstNode *function_delclaration() {
  // fun foo(): integer {}
  const char *varName = parseName(current());
  consume(TOKEN_IDENTIFIER, "Expected IDENTIFIER after 'fun'");

  consume(TOKEN_LEFT_PAREN, "Expected '(' before function arguments");
  IdentifierArray *parameters_array = parameters();
  consume(TOKEN_RIGHT_PAREN, "Expected ')' after function arguments");

  consume(TOKEN_COLONS, "Expected ':' after Parent for type declaration");
  DataType type = variable_type();

  consume(TOKEN_LEFT_BRACE, "Expect '{' after if condition");
  AstNode *fun_block = block();

  return ast_create_function_declaration(type, varName, parameters_array,
                                         fun_block, current()->line);
}

static AstNode *declaration() {
  if (parser.inPanicMode)
    synchronize();

  if (match(TOKEN_LET)) {
    return let_declaration();
  } else if (match(TOKEN_FUN)) {
    return function_delclaration();
  }
  return statement();
}

double parseExpression(const char *source) { return 0.0; }

void parse(const char *source) {
  Token *tokens = getTokens(source);
  AstNode *program = ast_create_block(AST_PROGRAM, 0);
  initParser(tokens, program);

  while (peek() != TOKEN_EOF) {
    push_ast_array(program->data.block.elements, declaration());
  }

  semantic_analysis(program);
  print_ast(program, 0);
  free_tree(program);
}
