#include "parser.h"
#include "tests/test_parser.h"
#include "tests/test_scanner.h"
#include "utils/file.h"
#include <stdio.h>
#include <string.h>

static void runFile(const char *path) {
  char *source = readFile(path);
  parse(source);
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
