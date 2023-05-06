#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

static TokenType peek() { return parser.currentToken->type; }

// fix in the future. unnecessery
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
  default:
    return AST_LITERAL;
    // error:
  }
}

static AstNode *expr();

static AstNode *number() {
  double num = strtod(parser.currentToken->start, NULL);
  return ast_create_literal(num);
}

static void binaryOp() {
  switch (peek()) {
  case TOKEN_STAR:
    break;
  case TOKEN_SLASH:
    break;
  case TOKEN_PLUS:
    break;
  case TOKEN_MINUS:
    break;
  default:
    break;
  }
}

static AstNode *factor() {
  if (check(TOKEN_NUMBER)) {
    AstNode *node = number();
    advance();
    return node;
  }

  consume(TOKEN_LEFT_PAREN, "Expect left parent at factor.");
  AstNode *node = expr();
  consume(TOKEN_RIGHT_PAREN, "Expect right parent at factor.");

  return node;
}

static AstNode *term() {
  AstNode *node = factor();

  while (match(TOKEN_STAR) || match(TOKEN_SLASH)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = factor();
    node = ast_create_binaryop(operator, node, right);
  }

  return node;
}

static AstNode *expr() {
  AstNode *node = term();

  while (match(TOKEN_MINUS) || match(TOKEN_PLUS)) {
    AstNodeType operator= toNodeType(previous()->type);
    AstNode *right = term();
    node = ast_create_binaryop(operator, node, right);
  }

  return node;
}

void parse(const char *source) {
  Token *tokens = getTokens(source);
  initParser(tokens);
  AstNode *root = expr();
  printAst(root, 0);
}
