#ifndef ARRAY_H
#define ARRAY_H

#include "memory.h"
#include "scanner.h"

typedef struct {
  int size;
  int capacity;
  Token *items;
} Array;

Array initArray();
void pushArray(Token *item, Array *arr);
void freeArray(Array *arr);

#endif
