#include "test_parser.h"

bool testParser() {
  bool isValid;

  const char *source = "10 + 10";
  double result = parseExpression(source);
  if (result != 20) {
    printf("first test in testParser has failed, 10 + 10 failed.\n");
    bool isValid = false;
  }

  source = "10 * 10";
  result = parseExpression(source);
  if (result != 100) {
    printf("second test in testParser has failed, 10 * 10 failed.\n");
    bool isValid = false;
  }

  source = "10 / 10";
  result = parseExpression(source);
  if (result != 1) {
    printf("third test in testParser has failed, 10 / 10 failed.\n");
    bool isValid = false;
  }

  source = "10 - 10";
  result = parseExpression(source);
  if (result != 0) {
    printf("fourth test in testParser has failed, 10 - 10 failed.\n");
    bool isValid = false;
  }

  source = "10 + 10 * 10";
  result = parseExpression(source);
  if (result != 110) {
    printf("fifth test in testParser has failed, 10 + 10 * 10 failed.\n");
    bool isValid = false;
  }

  source = "10 + 10 / 10";
  result = parseExpression(source);
  if (result != 11) {
    printf("sixth test in testParser has failed, 10 + 10 / 10 failed.\n");
    bool isValid = false;
  }

  source = "(10 + 10) * 10";
  result = parseExpression(source);
  if (result != 200) {
    printf("seventh test in testParser has failed, (10 + 10) * 10 failed.\n");
    bool isValid = false;
  }

  source = "(10 + 10) * 10 / 5";
  result = parseExpression(source);
  if (result != 40) {
    printf(
        "eighth test in testParser has failed, (10 + 10) * 10 / 5 failed.\n");
    bool isValid = false;
  }
  return isValid;
}
