#include "scanner.h"
#include "tests/test_scanner.h"
#include "utils/file.h"
#include <stdio.h>
#include <string.h>

static void runFile(const char *path) {
  char *source = readFile(path);
  Tokens tokens = getTokens(source);
}

int main(int argc, const char *argv[]) {
  if (strcmp(argv[1], "--test") == 0 && argc == 2) {
    testScanner();
    return 0;
  }

  if (argc == 2) {
    runFile(argv[1]);
  } else {
    exit(64);
  }

  return 0;
}
