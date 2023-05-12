#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "scanner.h"

AstNode *parse(Token *tokens);
// temparory remove later
/* double parseExpression(const char *source); */

#endif
