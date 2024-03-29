#include "scanner.h"
#include "./utils/array.h"
#include "memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *start;
  const char *current;
  int line;
} Scanner;

Scanner scanner;

void initScanner(const char *source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static bool isAtEnd() { return *scanner.current == '\0'; }

static char advance() {
  scanner.current++;
  return scanner.current[-1];
}

static char peek() { return *scanner.current; }

static char peekNext() {
  if (isAtEnd())
    return '\0';
  return scanner.current[1];
}

static bool match(char expected) {
  if (isAtEnd())
    return false;
  if (*scanner.current != expected)
    return false;
  scanner.current++;
  return true;
}

static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}

static Token errorToken(const char *message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.line;
  return token;
}

static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      scanner.line++;
      advance();
      break;

    case '/':
      if (peekNext() == '/') {
        // A comment goes until the end of the line.
        while (peek() != '\n' && !isAtEnd())
          advance();
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}

static TokenType checkKeyword(int start, int length, const char *rest,
                              TokenType type) {
  if (scanner.current - scanner.start == start + length &&
      memcmp(scanner.start + start, rest, length) == 0) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
  switch (scanner.start[0]) {
  case 'b':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'r':
        return checkKeyword(2, 3, "eak", TOKEN_BREAK);
      case 'o':
        return checkKeyword(2, 5, "olean", TOKEN_BOOLEAN_TYPE);
      }
    }
  case 'c':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'o':
        if (scanner.current - scanner.start > 2) {
          switch (scanner.start[2]) {
          case 'n':
            if (scanner.current - scanner.start > 3) {
              switch (scanner.start[3]) {
              case 's':
                return checkKeyword(4, 1, "t", TOKEN_CONST);
              case 't':
                return checkKeyword(4, 4, "inue", TOKEN_CONTINUE);
              }
            }
          }
        }
        return checkKeyword(2, 3, "nst", TOKEN_CONST);
      case 'a':
        return checkKeyword(2, 2, "se", TOKEN_CASE);
      }
    }
  case 'd':
    return checkKeyword(1, 6, "efault", TOKEN_DEFAULT);
  case 'e':
    return checkKeyword(1, 3, "lse", TOKEN_ELSE);
  case 'i':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'f':
        return checkKeyword(1, 1, "f", TOKEN_IF);
      case 'n':
        return checkKeyword(2, 1, "t", TOKEN_INTEGER_TYPE);
      }
    }
  case 'l':
    return checkKeyword(1, 2, "et", TOKEN_LET);
  case 'n':
    return checkKeyword(1, 2, "il", TOKEN_NIL);
  case 'o':
    return checkKeyword(1, 1, "r", TOKEN_OR);
  case 'p':
    return checkKeyword(1, 4, "rint", TOKEN_PRINT);
  case 'r':
    return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
  case 's':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'w':
        return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
      case 't':
        if (scanner.current - scanner.start > 2) {
          switch (scanner.start[2]) {
          case 'r':
            if (scanner.current - scanner.start > 3) {
              switch (scanner.start[3]) {
              case 'u':
                return checkKeyword(4, 2, "ct", TOKEN_STRUCT);
              case 'i':
                return checkKeyword(4, 2, "ng", TOKEN_STRING_TYPE);
              }
            }
          }
        }
      }
    }
  case 'w':
    return checkKeyword(1, 4, "hile", TOKEN_WHILE);
  case 'f':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'a':
        return checkKeyword(2, 3, "lse", TOKEN_FALSE);
      case 'o':
        return checkKeyword(2, 1, "r", TOKEN_FOR);
      case 'u':
        return checkKeyword(2, 1, "n", TOKEN_FUN);
      case 'l':
        return checkKeyword(2, 3, "oat", TOKEN_FLOAT_TYPE);
      }
    }
    break;
  case 't':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'r':
        return checkKeyword(2, 2, "ue", TOKEN_TRUE);
      }
    }
    break;
  case 'v':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'o':
        return checkKeyword(2, 2, "id", TOKEN_VOID_TYPE);
      }
    }
  }

  return TOKEN_IDENTIFIER;
}

static Token identifier() {
  while (isAlpha(peek()) || isDigit(peek()))
    advance();
  return makeToken(identifierType());
}

static Token number() {
  while (isDigit(peek()))
    advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek())) {
      advance();
    }
    return makeToken(TOKEN_FLOAT);
  }

  return makeToken(TOKEN_NUMBER);
}

static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      scanner.line++;
    advance();
  }

  if (isAtEnd())
    return errorToken("Unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TOKEN_STRING);
}

static Token scanToken() {
  skipWhitespace();

  scanner.start = scanner.current;

  if (isAtEnd())
    return makeToken(TOKEN_EOF);

  char c = advance();
  if (isDigit(c))
    return number();
  if (isAlpha(c))
    return identifier();

  switch (c) {
  case '(':
    return makeToken(TOKEN_LEFT_PAREN);
  case ')':
    return makeToken(TOKEN_RIGHT_PAREN);
  case '{':
    return makeToken(TOKEN_LEFT_BRACE);
  case '}':
    return makeToken(TOKEN_RIGHT_BRACE);
  case '[':
    return makeToken(TOKEN_LEFT_BRACKET);
  case ']':
    return makeToken(TOKEN_RIGHT_BRACKET);
  case ';':
    return makeToken(TOKEN_SEMICOLON);
  case ':':
    return makeToken(TOKEN_COLONS);
  case ',':
    return makeToken(TOKEN_COMMA);
  case '.':
    return makeToken(TOKEN_DOT);
  case '-':
    return makeToken(match('=') ? TOKEN_MINUS_EQUAL : TOKEN_MINUS);
  case '+':
    return makeToken(match('=')   ? TOKEN_PLUS_EQUAL
                     : match('+') ? TOKEN_PLUS_PLUS
                                  : TOKEN_PLUS);
  case '/':
    return makeToken(match('=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
  case '*':
    return makeToken(match('=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
  case '!':
    return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<':
    return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>':
    return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  case '"':
    return string();
  }

  return errorToken("Unexpected character.");
}

Token *getTokens(const char *source) {
  initScanner(source);
  TokenArray array = init_token_array();

  for (;;) {
    Token token = scanToken();
    if (token.type == TOKEN_EOF) {
      break;
    }
    if (token.type == TOKEN_ERROR) {
      printf("%s\n", token.start);
      break;
    }
    push_token_array(&token, &array);
  }
  Token eof = makeToken(TOKEN_EOF);

  push_token_array(&eof, &array);

  return array.items;
}
