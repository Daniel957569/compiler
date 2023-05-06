#include "parser.h"
#include "ast.h"
#include "common.h"
#include "scanner.h"

typedef struct {
  Token *tokens;
  Token *currentToken;
  AstNode *ast;
  int current;
} Parser;

Parser parser;

static void initParser(Token *tokens) {
  parser.tokens = tokens;
  parser.currentToken = tokens;
  parser.current = 0;
}

static void advance() {
  parser.current++;
  parser.currentToken++;
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
  // add error later
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

  consume(TOKEN_LEFT_PAREN, "Expect left parent at factor.");
  AstNode *node = expr();
  consume(TOKEN_RIGHT_PAREN, "Expect right parent at factor.");

  return node;
}

static AstNode *factor() {
  if (match(TOKEN_MINUS)) {
    AstNode *node = primary();
    node = ast_create_binaryop(AST_NEGATE, node, NULL);
    return node;
  }

  AstNode *node = primary();

  while (match(TOKEN_STAR) || match(TOKEN_SLASH)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = primary();
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
