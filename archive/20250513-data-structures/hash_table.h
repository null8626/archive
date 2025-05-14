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

#ifdef HASH_TABLE_ITERATOR
typedef struct {
  size_t outer_index;
  size_t inner_index;
} hash_table_iterator_t;
#endif

/**
 * @brief Creates a brand new hash table.
 * 
 * @param table The hash table to create.
 * @param size Maximum capacity for the hash table used.
 * @param hash_function The hash function to be used by the hash tables.
 * @param hash_function_additional_argument_data_t The data to passed as an extra argument to hash function. Optional.
 * @param key_is_equal The function to check whether two key pairs are equal.
 * @param entry_free A pointer to a struct for instructing on freeing the key and/or value pairs. Optional.
 * @see hash_table_get
 * @see hash_table_set
 * @see hash_table_pop
 * @see hash_table_has
 * @see hash_table_iterator_begin
 * @see hash_table_free
 * @return Whether the hash table creation was successful.
 */
bool hash_table_new(hash_table_t* const table, const size_t size, const hash_function_t hash_function, const hash_function_additional_argument_data_t hash_function_additional_argument, const hash_table_key_is_equal_t key_is_equal, const hash_table_entry_free_t* const entry_free);

/**
 * @brief Retrieves a value from a key.
 * 
 * @param table The hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see hash_table_has
 * @see hash_table_set
 * @see hash_table_pop
 * @see hash_table_iterator_begin
 * @return Whether the retrieval was successful.
 */
bool hash_table_get(const hash_table_t* const table, const hash_table_key_t key, hash_table_value_t* const output);

/**
 * @brief Inserts a key-value pair.
 * 
 * @param table The hash table to use.
 * @param key The key to be set.
 * @param value The value to be set.
 * @see hash_table_get
 * @see hash_table_pop
 * @return Whether the operation was successful.
 * @note This will override a value with an existing key. Such existing value will be freed prior to reassignment.
 */
bool hash_table_set(hash_table_t* const table, const hash_table_key_t key, const hash_table_value_t value);

/**
 * @brief Removes and returns a value from a key.
 * 
 * @param table The hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see hash_table_get
 * @see hash_table_set
 * @see hash_table_free
 * @return Whether the operation was successful.
 * @note If output is NULL, the value will be freed after being removed from the hash table.
 */
bool hash_table_pop(hash_table_t* const table, const hash_table_key_t key, hash_table_value_t* output);

/**
 * @brief Frees every key-value pair from a hash table before also freeing the hash table itself.
 * 
 * @param table The hash table to be freed.
 */
void hash_table_free(hash_table_t* const table);

/**
 * @brief Checks if a key already exists in the hash table.
 * 
 * @param table The hash table to use.
 * @param key The key to check.
 * @see hash_table_get
 * @see hash_table_set
 * @see hash_table_iterator_begin
 * @return Whether the key exists.
 */
inline bool hash_table_has(const hash_table_t* const table, const hash_table_key_t key) {
  return hash_table_get(table, key, NULL);
}

#ifdef HASH_TABLE_ITERATOR
/**
 * @brief Initiates a hash table key-value iterator.
 * 
 * @param iterator The iterator to be initiated.
 * @see hash_table_iterator_next
 * @note An existing iterator instance can be reused safely with this function.
 */
inline void hash_table_iterator_begin(hash_table_iterator_t* const iterator) {
  iterator->inner_index = 0;
  iterator->outer_index = 0;
}

/**
 * @brief Retrieves the next key-value pair from a hash table.
 * 
 * @param table The hash table to use.
 * @param iterator The iterator to use.
 * @param entry A pointer to a key-value entry.
 * @return Whether there is a new key-value pair.
 */
bool hash_table_iterator_next(const hash_table_t* const table, hash_table_iterator_t* const iterator, hash_table_entry_t* const entry);
#endif
#endif