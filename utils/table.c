#include <stdlib.h>
#include <string.h>

#include "table.h"

#define TABLE_MAX_LOAD 0.75

static void print_table(Table *table) {
  for (int i = 0; i < table->capacity; i++) {
    printf("%d: %s\n", i, table->entries[i].key);
  }
}

void init_table(Table *table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

static Entry *find_entry(Entry *entries, int capacity, const char *key,
                         u_int32_t hash) {
  uint32_t index = hash % capacity;
  Entry *tombstone = NULL;

  for (;;) {
    Entry *entry = &entries[index];
    if (entry->key == NULL) {
      if (!entry->is_tombstone) {
        // empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // we found a tombstone.
        if (tombstone == NULL)
          tombstone = entry;
      }
    } else if (strcmp(entry->key, key) == 0) {
      // we found the key.
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

static void adjust_capacity(Table *table, int capacity, const char *key,
                            u_int32_t hash) {
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NULL;
  }

  // recalculate table size since tombstones aren't copied
  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL)
      continue;

    Entry *dest = find_entry(entries, capacity, key, hash);
    dest->key = entry->key;
    dest->value = entry->value;
    dest->hash = entry->hash;
    dest->is_tombstone = entry->is_tombstone;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}

bool table_get(Table *table, const char *key, u_int32_t hash,
               struct Symbol *value) {
  if (table->count == 0)
    return false;

  Entry *entry = find_entry(table->entries, table->capacity, key, hash);
  if (entry->key == NULL)
    return false;

  *value = *entry->value;
  return true;
}

bool table_set(Table *table, const char *key, uint32_t hash,
               struct Symbol *value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjust_capacity(table, capacity, key, hash);
  }

  Entry *entry = find_entry(table->entries, table->capacity, key, hash);
  bool isNewKey = entry->key == NULL;

  // return false if already exist
  if (!isNewKey) {
    return false;
  }

  if (isNewKey && !entry->is_tombstone)
    table->count++;

  entry->key = key;
  entry->hash = hash;
  entry->value = value;
  entry->is_tombstone = false;
  return isNewKey;
}

bool table_delete(Table *table, const char *key, uint32_t hash) {
  if (table->count == 0)
    return false;

  // find the entry.
  Entry *entry = find_entry(table->entries, table->capacity, key, hash);
  if (entry->key == NULL)
    return false;

  // place a tombstone in the entry.
  entry->key = NULL;
  entry->hash = 0;
  entry->value = NULL;
  entry->is_tombstone = true;
  return true;
}

bool table_contains(Table *table, const char *key, u_int32_t hash) {
  if (table->count == 0)
    return false;

  Entry *entry = find_entry(table->entries, table->capacity, key, hash);
  if (entry->key == NULL) {
    return false;
  }

  if (strcmp(entry->key, key) == 0) {
    return true;
  }

  return false;
}

void table_add_all(Table *from, Table *to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry *entry = &from->entries[i];
    if (entry->key != NULL) {
      table_set(to, entry->key, entry->hash, entry->value);
    }
  }
}

void free_table(Table *table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  init_table(table);
}
