#ifndef array_h
#define array_h

#include "memory.h"
#include "scanner.h"

typedef struct {
  int size;
  int capacity;
  void **items;
} Array;

Array initArray();
void pushArray(void *item, Array *arr);
void freeArray(Array *arr);

#endif
