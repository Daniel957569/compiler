#include "array.h"
#include "memory.h"
#include <stdio.h>

#define GROWTH_FACTOR 0.75

Array initArray() {
  Array array;
  array.capacity = 8;
  array.size = 0;
  array.items = ALLOCATE(Token, 8);
  return array;
}

void pushArray(Token *item, Array *arr) {
  if (arr->capacity * GROWTH_FACTOR < arr->size) {
    arr->items =
        GROW_ARRAY(Token, arr->items, arr->capacity, arr->capacity * 2);
    arr->capacity *= 2;
  }
  arr->items[arr->size] = *item;
  arr->size++;
}
