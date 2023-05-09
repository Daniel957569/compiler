#include "array.h"
#include "ast.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GROWTH_FACTOR 0.75
#define INITIAL_CAPACITY 10

Array initArray() {
  Array array;
  array.capacity = INITIAL_CAPACITY;
  array.size = 0;
  array.items = ALLOCATE(Token, INITIAL_CAPACITY);
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
  array->capacity = INITIAL_CAPACITY;
  array->size = 0;
  array->items = ALLOCATE(AstNode *, INITIAL_CAPACITY);
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

StringArray *init_string_array() {
  StringArray *array = malloc(sizeof(StringArray));
  array->items = ALLOCATE(char *, 8);
  array->size = 0;
  array->capacity = 8;
  return array;
}

void push_string_array(StringArray *arr, char *str) {
  if (arr->capacity * GROWTH_FACTOR < arr->size) {
    arr->capacity *= 2;
    arr->items = realloc(arr->items, arr->capacity * sizeof(char *));
  }

  arr->items[arr->size] = malloc((strlen(str) + 1) * sizeof(char));
  strcpy(arr->items[arr->size], str);
  arr->size++;
}

void free_string_array(StringArray *arr) {
  for (int i = 0; i < arr->size; i++) {
    free(arr->items[i]);
  }

  free(arr->items);
}
