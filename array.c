#include "array.h"
#include "memory.h"
#include <stdio.h>

#define GROWTH_FACTOR 0.75

Array initArray() {
  Array array;
  array.capacity = 8;
  array.size = 0;
  array.items = ALLOCATE(void *, 8);
  return array;
}

void pushArray(void *item, Array *arr) {
  if (arr->capacity * GROWTH_FACTOR < arr->size) {
    reallocate(arr->items, arr->size, arr->capacity);
    arr->capacity *= 1.5;
  }
  arr->items[arr->size] = (Token *)item;
  arr->size++;
}
