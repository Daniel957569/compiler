#ifndef scanner_h
#define scanner_h

typedef enum {
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_LEFT_BRACKET,
  TOKEN_RIGHT_BRACKET,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_COLONS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,
  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_STAR_EQUAL,
  TOKEN_MINUS_EQUAL,
  TOKEN_SLASH_EQUAL,
  TOKEN_PLUS_EQUAL,
  TOKEN_PLUS_PLUS,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,
  TOKEN_STRUCT,
  TOKEN_ELSE,
  TOKEN_FALSE,
  TOKEN_FOR,
  TOKEN_FUN,
  TOKEN_SWITCH,
  TOKEN_DEFAULT,
  TOKEN_CASE,
  TOKEN_IF,
  TOKEN_NIL,
  TOKEN_OR,
  TOKEN_PRINT,
  TOKEN_RETURN,
  TOKEN_BREAK,
  TOKEN_CONTINUE,
  TOKEN_SUPER,
  TOKEN_TRUE,
  TOKEN_LET,
  TOKEN_CONST,
  TOKEN_WHILE,
  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  const char *start;
  int length;
  int line;
} Token;

// im a noobie so im using cheap solution cant use Array because of recursive,
// fix later future me.

typedef struct {
  Token *tokens;
  int size;
} Tokens;

Tokens getTokens(const char *source);

#endif
