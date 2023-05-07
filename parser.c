#include "parser.h"
#include "ast.h"
#include "common.h"
#include "scanner.h"

typedef struct {
  Token *tokens;
  Token *currentToken;
  AstNode *ast;
  int current;
  bool inPanicMode;
  bool hadError;
} Parser;

Parser parser;

static void initParser(Token *tokens) {
  parser.tokens = tokens;
  parser.currentToken = tokens;
  parser.current = 0;
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
  return type == parser.currentToken++->type;
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

  case TOKEN_EQUAL:
    return AST_EQUAL;
  case TOKEN_EQUAL_EQUAL:
    return AST_EQUAL_EQUAL;

  case TOKEN_BANG_EQUAL:
    return AST_BANG_EQUAL;
  case TOKEN_BANG:
    return AST_BANG;

  default:
    return AST_LITERAL;
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

static AstNode *expr();
static AstNode *comparison();
static AstNode *equality();

static AstNode *number() {
  double num = strtod(parser.currentToken->start, NULL);
  return ast_create_literal(num);
}

static AstNode *primary() {
  if (check(TOKEN_NUMBER)) {
    AstNode *node = number();
    advance();
    return node;
  }
  if (match(TOKEN_TRUE)) {
    AstNode *node = ast_create_boolean(AST_TRUE, true);
    advance();
    return node;
  }
  if (match(TOKEN_FALSE)) {
    AstNode *node = ast_create_boolean(AST_FALSE, false);
    advance();
    return node;
  }

  consume(TOKEN_LEFT_PAREN, "Expected ( at Expression.");
  AstNode *node = expr();
  consume(TOKEN_RIGHT_PAREN, "Expected ) after Expression.");

  return node;
}

static AstNode *unary() {
  if (match(TOKEN_MINUS) || match(TOKEN_BANG)) {
    AstNodeType unaryop = toNodeType(previous()->type);
    AstNode *node = primary();
    node = ast_create_binaryop(unaryop, node, NULL);
    return node;
  }

  return primary();
}

static AstNode *factor() {
  AstNode *node = unary();

  while (match(TOKEN_STAR) || match(TOKEN_SLASH)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = unary();
    node = ast_create_binaryop(operator, node, right);
  }
  return node;
}

static AstNode *term() {
  AstNode *node = factor();

  while (match(TOKEN_MINUS) || match(TOKEN_PLUS)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = factor();
    node = ast_create_binaryop(operator, node, right);
  }

  return node;
}

static AstNode *comparison() {
  AstNode *node = term();

  while (match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) ||
         match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = term();
    node = ast_create_binaryop(operator, node, right);
  }

  return node;
}

static AstNode *equality() {
  AstNode *node = comparison();

  while (match(TOKEN_BANG) || match(TOKEN_EQUAL_EQUAL)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = comparison();
    node = ast_create_binaryop(operator, node, right);
  }

  return node;
}

static AstNode *expr() { return equality(); }

double parseExpression(const char *source) {
  Token *tokens = getTokens(source);
  initParser(tokens);
  AstNode *root = expr();
  return test_evaluate(root);
}

void parse(const char *source) {
  Token *tokens = getTokens(source);
  initParser(tokens);
  AstNode *root = expr();
  print_ast(root, 0);
  /* printf("value: %f\n", test_evaluate(root)); */
}
