#include "test_codegen.h"
#include "../codegen.h"
#include "../parser.h"
#include "../scanner.h"
#include "../semantic_check.h"
#include "../utils/file.h"

void test_codegen() {
  char *file = readFile("../tests/test_ex.txt");
  Token *tokens = getTokens(file);
  AstNode *program = parse(tokens);
  Table string_table;
  string_table = *semantic_analysis(program);
  codegen(program, &string_table);
}
