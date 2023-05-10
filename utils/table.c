#include <stdlib.h>
#include <string.h>

#include "../memory.h"
#include "table.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table *table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

static Entry *findEntry(Entry *entries, int capacity, char *key,
                        u_int32_t hash) {
  uint32_t index = hash % capacity;
  Entry *tombstone = NULL;

  for (;;) {
    Entry *entry = &entries[index];
    if (entry->key == NULL) {
      if (entry->is_tombstone) {
        // empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // we found a tombstone.
        if (tombstone == NULL)
          tombstone = entry;
      }
    } else if (entry->key == key) {
      // we found the key.
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

static void adjustCapacity(Table *table, int capacity, char *key,
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

    Entry *dest = findEntry(entries, capacity, key, hash);
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

bool tableSet(Table *table, char *key, u_int32_t hash, AstNode *value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity, key, hash);
  }

  Entry *entry = findEntry(table->entries, table->capacity, key, hash);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && entry->is_tombstone)
    table->count++;

  entry->key = key;
  entry->hash = hash;
  entry->value = value;
  entry->is_tombstone = false;
  return isNewKey;
}

bool tableDelete(Table *table, char *key, u_int32_t hash) {
  if (table->count == 0)
    return false;

  // find the entry.
  Entry *entry = findEntry(table->entries, table->capacity, key, hash);
  if (entry->key == NULL)
    return false;

  // place a tombstone in the entry.
  entry->key = NULL;
  entry->hash = 0;
  entry->value = NULL;
  entry->is_tombstone = true;
  return true;
}

void tableAddAll(Table *from, Table *to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry *entry = &from->entries[i];
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->hash, entry->value);
    }
  }
}

void freeTable(Table *table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}
