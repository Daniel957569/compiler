#ifndef array_h
#define array_h

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
