#include "test_scanner.h"
#include "../scanner.h"
#include "../utils/file.h"
#include <stdio.h>
#include <stdlib.h>

void test_scanner() {
  const char *source = readFile("../tests/test_ex.txt");
  TokenType correctPattern[74] = {
      TOKEN_FUN,          TOKEN_IDENTIFIER,   TOKEN_LEFT_PAREN,
      TOKEN_IDENTIFIER,   TOKEN_COLONS,       TOKEN_INTEGER_TYPE,
      TOKEN_COMMA,        TOKEN_IDENTIFIER,   TOKEN_COLONS,
      TOKEN_STRING_TYPE,  TOKEN_RIGHT_PAREN,  TOKEN_COLONS,
      TOKEN_INTEGER_TYPE, TOKEN_LEFT_BRACE,   TOKEN_LET,
      TOKEN_IDENTIFIER,   TOKEN_COLONS,       TOKEN_INTEGER_TYPE,
      TOKEN_EQUAL,        TOKEN_NUMBER,       TOKEN_SEMICOLON,
      TOKEN_WHILE,        TOKEN_IDENTIFIER,   TOKEN_GREATER,
      TOKEN_IDENTIFIER,   TOKEN_LEFT_BRACE,   TOKEN_IDENTIFIER,
      TOKEN_EQUAL,        TOKEN_IDENTIFIER,   TOKEN_PLUS,
      TOKEN_NUMBER,       TOKEN_SEMICOLON,    TOKEN_PRINT,
      TOKEN_IDENTIFIER,   TOKEN_PLUS,         TOKEN_IDENTIFIER,
      TOKEN_SEMICOLON,    TOKEN_PRINT,        TOKEN_IDENTIFIER,
      TOKEN_SEMICOLON,    TOKEN_RIGHT_BRACE,  TOKEN_RETURN,
      TOKEN_IDENTIFIER,   TOKEN_SEMICOLON,    TOKEN_RIGHT_BRACE,
      TOKEN_FUN,          TOKEN_IDENTIFIER,   TOKEN_LEFT_PAREN,
      TOKEN_RIGHT_PAREN,  TOKEN_COLONS,       TOKEN_INTEGER_TYPE,
      TOKEN_LEFT_BRACE,   TOKEN_LET,          TOKEN_IDENTIFIER,
      TOKEN_COLONS,       TOKEN_INTEGER_TYPE, TOKEN_EQUAL,
      TOKEN_IDENTIFIER,   TOKEN_LEFT_PAREN,   TOKEN_NUMBER,
      TOKEN_COMMA,        TOKEN_STRING,       TOKEN_RIGHT_PAREN,
      TOKEN_SEMICOLON,    TOKEN_PRINT,        TOKEN_STRING,
      TOKEN_SEMICOLON,    TOKEN_PRINT,        TOKEN_IDENTIFIER,
      TOKEN_SEMICOLON,    TOKEN_RETURN,       TOKEN_NUMBER,
      TOKEN_SEMICOLON,    TOKEN_RIGHT_BRACE};

  Token *output = getTokens(source);

  int i = 0;
  while (output->type != TOKEN_EOF) {
    if (output->type != correctPattern[i]) {
      printf("Wrong Token produced. index: %d\n", i);
      exit(64);
    }
    i++;
    output++;
  }

  printf("Tokens were made successfully\n");
}

/* switch (output.tokens[i].type) { */
/* case TOKEN_LEFT_PAREN: */
/*   printf("LEFT_PAREN\n"); */
/*   break; */
/* case TOKEN_RIGHT_PAREN: */
/*   printf("RIGHT_PAREN\n"); */
/*   break; */
/* case TOKEN_LEFT_BRACE: */
/*   printf("LEFT_BRACE\n"); */
/*   break; */
/* case TOKEN_RIGHT_BRACE: */
/*   printf("RIGHT_BRACE\n"); */
/*   break; */
/* case TOKEN_LEFT_BRACKET: */
/*   printf("LEFT_BRACKET\n"); */
/*   break; */
/* case TOKEN_RIGHT_BRACKET: */
/*   printf("RIGHT_BRACKET\n"); */
/*   break; */
/* case TOKEN_COMMA: */
/*   printf("COMMA\n"); */
/*   break; */
/* case TOKEN_DOT: */
/*   printf("DOT\n"); */
/*   break; */
/* case TOKEN_MINUS: */
/*   printf("MINUS\n"); */
/*   break; */
/* case TOKEN_PLUS: */
/*   printf("PLUS\n"); */
/*   break; */
/* case TOKEN_COLONS: */
/*   printf("COLONS\n"); */
/*   break; */
/* case TOKEN_SEMICOLON: */
/*   printf("SEMICOLON\n"); */
/*   break; */
/* case TOKEN_SLASH: */
/*   printf("SLASH\n"); */
/*   break; */
/* case TOKEN_STAR: */
/*   printf("STAR\n"); */
/*   break; */
/* case TOKEN_BANG: */
/*   printf("BANG\n"); */
/*   break; */
/* case TOKEN_BANG_EQUAL: */
/*   printf("BANG_EQUAL\n"); */
/*   break; */
/* case TOKEN_EQUAL: */
/*   printf("EQUAL\n"); */
/*   break; */
/* case TOKEN_EQUAL_EQUAL: */
/*   printf("EQUAL_EQUAL\n"); */
/*   break; */
/* case TOKEN_GREATER: */
/*   printf("GREATER\n"); */
/*   break; */
/* case TOKEN_GREATER_EQUAL: */
/*   printf("GREATER_EQUAL\n"); */
/*   break; */
/* case TOKEN_LESS: */
/*   printf("LESS\n"); */
/*   break; */
/* case TOKEN_LESS_EQUAL: */
/*   printf("LESS_EQUAL\n"); */
/*   break; */
/* case TOKEN_STAR_EQUAL: */
/*   printf("STAR_EQUAL\n"); */
/*   break; */
/* case TOKEN_MINUS_EQUAL: */
/*   printf("MINUS_EQUAL\n"); */
/*   break; */
/* case TOKEN_SLASH_EQUAL: */
/*   printf("SLASH_EQUAL\n"); */
/*   break; */
/* case TOKEN_PLUS_EQUAL: */
/*   printf("PLUS_EQUAL\n"); */
/*   break; */
/* case TOKEN_PLUS_PLUS: */
/*   printf("PLUS_PLUS\n"); */
/*   break; */
/* case TOKEN_IDENTIFIER: */
/*   printf("IDENTIFIER\n"); */
/*   break; */
/* case TOKEN_STRING: */
/*   printf("STRING\n"); */
/*   break; */
/* case TOKEN_NUMBER: */
/*   printf("NUMBER\n"); */
/*   break; */
/* case TOKEN_STRUCT: */
/*   printf("STRUCT\n"); */
/*   break; */
/* case TOKEN_ELSE: */
/*   printf("ELSE\n"); */
/*   break; */
/* case TOKEN_FALSE: */
/*   printf("FALSE\n"); */
/*   break; */
/* case TOKEN_FOR: */
/*   printf("FOR\n"); */
/*   break; */
/* case TOKEN_FUN: */
/*   printf("FUN\n"); */
/*   break; */
/* case TOKEN_SWITCH: */
/*   printf("SWITCH\n"); */
/*   break; */
/* case TOKEN_DEFAULT: */
/*   printf("DEFAULT\n"); */
/*   break; */
/* case TOKEN_CASE: */
/*   printf("CASE\n"); */
/*   break; */
/* case TOKEN_IF: */
/*   printf("IF\n"); */
/*   break; */
/* case TOKEN_NIL: */
/*   printf("NIL\n"); */
/*   break; */
/* case TOKEN_OR: */
/*   printf("OR\n"); */
/*   break; */
/* case TOKEN_PRINT: */
/*   printf("PRINT\n"); */
/*   break; */
/* case TOKEN_RETURN: */
/*   printf("RETURN\n"); */
/*   break; */
/* case TOKEN_BREAK: */
/*   printf("BREAK\n"); */
/*   break; */
/* case TOKEN_CONTINUE: */
/*   printf("CONTINUE\n"); */
/*   break; */
/* case TOKEN_SUPER: */
/*   printf("SUPER\n"); */
/*   break; */
/* case TOKEN_TRUE: */
/*   printf("TRUE\n"); */
/*   break; */
/* case TOKEN_LET: */
/*   printf("LET\n"); */
/*   break; */
/* case TOKEN_CONST: */
/*   printf("CONST\n"); */
/*   break; */
/* case TOKEN_WHILE: */
/*   printf("WHILE\n"); */
/*   break; */
/* case TOKEN_ERROR: */
/*   printf("ERROR\n"); */
/*   break; */
/* case TOKEN_EOF: */
/*   printf("EOF\n"); */
/*   break; */
/* } */
