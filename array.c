#include "array.h"
#include "ast.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

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

AstArray *init_ast_array() {
  AstArray *array = malloc(sizeof(AstArray));
  array->capacity = 8;
  array->size = 0;
  array->items = ALLOCATE(AstNode *, 8);
  return array;
}

void push_ast_array(AstArray *arr, AstNode *item) {
  if (arr->capacity * GROWTH_FACTOR < arr->size) {
    arr->items =
        GROW_ARRAY(AstNode *, arr->items, arr->capacity, arr->capacity * 1.5);
    arr->capacity *= 1.5;
  }

  arr->items[arr->size] = item;
  arr->size++;
}

void free_ast_array(AstArray *arr) {
  for (int i = 0; i < arr->size; i++) {
    free(arr->items[i]);
  }

  free(arr->items);
}
