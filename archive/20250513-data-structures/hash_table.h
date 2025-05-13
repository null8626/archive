#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#include "stack.h"

typedef stack_data_t hash_table_data_t;
typedef hash_table_data_t hash_table_key_t;
typedef hash_table_data_t hash_table_value_t;
typedef stack_data_free_t hash_table_key_free_t;
typedef stack_data_free_t hash_table_value_free_t;
typedef stack_data_free_argument_t hash_table_key_free_argument_t;
typedef stack_data_free_argument_t hash_table_value_free_argument_t;
typedef bool (*hash_table_key_is_equal_t)(const hash_table_key_t, const hash_table_key_t);

typedef struct {
  hash_table_key_t key;
  hash_table_value_t value;
} hash_table_entry_t;

typedef void* hash_function_additional_argument_data_t;

typedef struct {
  size_t table_size;
  hash_function_additional_argument_data_t data;
} hash_function_additional_argument_t;

typedef size_t (*hash_function_t)(const hash_table_key_t, const hash_function_additional_argument_t* const);

typedef struct {
  hash_table_key_free_t key_free;
  hash_table_value_free_t value_free;
  hash_table_key_free_argument_t key_free_argument;
  hash_table_value_free_argument_t value_free_argument;
} hash_table_entry_free_t;

typedef struct {
  stack_t* table;
  size_t size;
  hash_function_t hash_function;
  hash_function_additional_argument_data_t hash_function_additional_argument;
  hash_table_key_is_equal_t key_is_equal;
  const hash_table_entry_free_t* entry_free;
} hash_table_t;

typedef struct {
  size_t outer_index;
  size_t inner_index;
} hash_table_iterator_t;

inline void hash_table_iterator_begin(hash_table_iterator_t* const iterator) {
  iterator->inner_index = 0;
  iterator->outer_index = 0;
}

bool hash_table_iterator_next(const hash_table_t* const table, hash_table_iterator_t* const iterator, hash_table_entry_t* const entry);

bool hash_table_new(hash_table_t* const table, const size_t size, const hash_function_t hash_function, const hash_function_additional_argument_data_t hash_function_additional_argument, const hash_table_key_is_equal_t key_is_equal, const hash_table_entry_free_t* const entry_free);
bool hash_table_get(const hash_table_t* const table, const hash_table_key_t key, hash_table_value_t* const output);
bool hash_table_set(hash_table_t* const table, const hash_table_key_t key, const hash_table_value_t value);
bool hash_table_pop(hash_table_t* const table, const hash_table_key_t key, hash_table_value_t* output);
void hash_table_free(hash_table_t* const table);

inline bool hash_table_has(const hash_table_t* const table, const hash_table_key_t key) {
  return hash_table_get(table, key, NULL);
}

#endif