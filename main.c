#include "ast.h"
#include "codegen.h"
#include "memory.h"
#include "parser.h"
#include "scanner.h"
#include "semantic_check.h"
#include "tests/test_codegen.h"
#include "tests/test_parser.h"
#include "tests/test_scanner.h"
#include "utils/array.h"
#include "utils/file.h"
#include "utils/table.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

double scanner_time;
double parser_time;
double codegen_time;

static Token *run_scanner(const char *path) {
  clock_t start_time, end_time;
  double cpu_time_used;

  start_time = clock();

  char *source = readFile(path);
  Token *tokens = getTokens(source);

  end_time = clock();
  scanner_time = cpu_time_used =
      ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

  printf("\n-------------------------------------------\n"
         "| Time taken by Scanner: %f Seconds |\n"
         "-------------------------------------------\n",
         cpu_time_used);

  return tokens;
}

static AstNode *run_parser(Token *tokens, Table *string_table) {
  clock_t start_time, end_time;
  double cpu_time_used;

  start_time = clock();

  AstNode *program = parse(tokens);
  *string_table = *semantic_analysis(program);

  free(tokens);

  end_time = clock();
  parser_time = cpu_time_used =
      ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

  printf("\n------------------------------------------\n"
         "| Time taken by Parser: %f Seconds |\n"
         "------------------------------------------\n",
         cpu_time_used);

  return program;
}

static void run_codegen(AstNode *program, Table *string_table) {
  clock_t start_time, end_time;
  double cpu_time_used;

  start_time = clock();

  codegen(program, string_table);

  end_time = clock();
  codegen_time = cpu_time_used =
      ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

  printf("\n-------------------------------------------\n"
         "| Time taken by Codegen: %f Seconds |\n"
         "-------------------------------------------\n",
         cpu_time_used);
}

static void runFile(const char *path) {
  Table string_table;
  Token *tokens = run_scanner(path);
  AstNode *program = run_parser(tokens, &string_table);
  run_codegen(program, &string_table);

  printf("\n-------------------------------------------\n"
         "| Time taken to Compile: %f Seconds |\n"
         "-------------------------------------------\n",
         scanner_time + parser_time + codegen_time);
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    printf("Please enter a path to a file as an argument.\n");
    return 0;
  }

  if (strcmp(argv[1], "--test") == 0 && argc == 2) {
    test_scanner();
    test_parser();
    test_codegen();
    return 0;
  }

  if (argc == 2) {
    runFile(argv[1]);
  } else {
    exit(64);
  }

  return 0;
}
