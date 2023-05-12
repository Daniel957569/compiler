#include "array.h"
#include "../ast.h"
#include "../semantic_check.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GROWTH_FACTOR 0.75
#define INITIAL_CAPACITY 10

TokenArray init_token_array() {
  TokenArray array;
  array.capacity = INITIAL_CAPACITY;
  array.size = 0;
  array.items = ALLOCATE(Token, INITIAL_CAPACITY);
  return array;
}

void push_token_array(Token *item, TokenArray *arr) {
  if (arr->capacity * GROWTH_FACTOR < arr->size) {
    arr->items =
        GROW_ARRAY(Token, arr->items, arr->capacity, arr->capacity * 2);
    arr->capacity *= 2;
  }
  arr->items[arr->size] = *item;
  arr->size++;
}

void free_token_array(TokenArray *arr) {
  free(arr->items);
  free(arr);
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
    free_tree(arr->items[i]);
  }

  free(arr->items);
  free(arr);
}

IdentifierArray *init_identifier_array() {
  IdentifierArray *array = malloc(sizeof(IdentifierArray));
  array->items = ALLOCATE(Identifier *, 8);
  array->size = 0;
  array->capacity = 8;
  return array;
}

void push_identifier_array(IdentifierArray *arr, Identifier *identifier) {
  if (arr->capacity * GROWTH_FACTOR < arr->size) {
    arr->capacity *= 2;
    arr->items = realloc(arr->items, arr->capacity * sizeof(char *));
  }

  arr->items[arr->size] = identifier;
  arr->size++;
}

void free_identifier_array(IdentifierArray *arr) {
  for (int i = 0; i < arr->size; i++) {
    free(arr->items[i]);
  }

  free(arr->items);
  free(arr);
}

EnvironmentArray *init_environment_array() {
  EnvironmentArray *array = malloc(sizeof(EnvironmentArray));
  array->items = ALLOCATE(Environment *, 8);
  /* array->items = NULL; */
  array->size = 0;
  array->capacity = 8;

  for (int i = 0; i < 8; i++) {
    array->items[i] = malloc(sizeof(Environment));
    array->items[i]->is_initialized = false;
    init_table(&array->items[i]->table);
  }

  return array;
}
void push_environment_array(EnvironmentArray *arr, struct Environment *env) {
  if (arr->capacity * GROWTH_FACTOR < arr->size) {
    arr->items = GROW_ARRAY(struct Environment *, arr->items, arr->capacity,
                            arr->capacity * 1.5);

    for (int i = arr->capacity; i < arr->capacity * 1.5; i++) {
      arr->items[i]->is_initialized = false;
    }
    arr->capacity *= 1.5;
  }

  arr->items[arr->size] = env;
  arr->size++;
}

void free_environment_array(EnvironmentArray *arr) {
  for (int i = 0; i < arr->size; i++) {
    free_table(&arr->items[i]->table);
    free(arr->items[i]);
  }

  free(arr->items);
  free(arr);
}
