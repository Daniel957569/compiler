#include "ast.h"
#include "codegen.h"
#include "parser.h"
#include "scanner.h"
#include "tests/test_parser.h"
#include "tests/test_scanner.h"
#include "utils/file.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static Token *run_scanner(const char *path) {
  clock_t start_time, end_time;
  double cpu_time_used;

  start_time = clock();

  char *source = readFile(path);
  Token *tokens = getTokens(source);

  end_time = clock();
  cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

  printf("\n---------------------------------------\n"
         "Time taken by Scanner: %f Seconds\n"
         "---------------------------------------\n\n",
         cpu_time_used);

  return tokens;
}

static AstNode *run_parser(Token *tokens) {
  clock_t start_time, end_time;
  double cpu_time_used;

  start_time = clock();

  AstNode *program = parse(tokens);

  end_time = clock();
  cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

  printf("\n---------------------------------------\n"
         "Time taken by Parser: %f Seconds\n"
         "---------------------------------------\n\n",
         cpu_time_used);

  return program;
}

static void run_codegen() {}

static void runFile(const char *path) {
  Token *tokens = run_scanner(path);
  AstNode *program = run_parser(tokens);
  codegen(program);
}

int main(int argc, const char *argv[]) {
  if (strcmp(argv[1], "--test") == 0 && argc == 2) {
    testScanner();
    testParser();
    return 0;
  }

  if (argc == 2) {
    runFile(argv[1]);
  } else {
    exit(64);
  }

  return 0;
}
