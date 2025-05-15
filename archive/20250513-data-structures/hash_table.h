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
  hash_function_t function;
  hash_function_additional_argument_data_t additional_argument;
} hash_function_descriptor_t;

typedef struct {
  size_t size;
  hash_function_descriptor_t hash_function;
  hash_table_key_is_equal_t key_is_equal;
  hash_table_entry_free_t entry_free;
} hash_table_descriptor_t;

typedef struct {
  stack_t* table;
  hash_table_descriptor_t descriptor;
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
 * @param descriptor A pointer to a descriptor that describes the new hash table's behavior.
 * @see hash_table_get
 * @see hash_table_set
 * @see hash_table_pop
 * @see hash_table_has
 * @see hash_table_iterator_begin
 * @see hash_table_free
 * @return Whether the hash table creation was successful.
 */
bool hash_table_new(hash_table_t* const table, const hash_table_descriptor_t* const descriptor);

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

/**
 * @brief Frees every key-value pair from a hash table before also freeing the hash table itself.
 * 
 * @param table The hash table to be freed.
 */
void hash_table_free(hash_table_t* const table);

#ifdef HASH_TABLE_ITERATOR
/**
 * @brief Initiates a hash table key-value iterator.
 * 
 * @param iterator The iterator to be initiated.
 * @see hash_table_iterator_next
 * @see hash_table_iterator_next_ref
 * @note An existing iterator instance can be reused safely with this function.
 */
inline void hash_table_iterator_begin(hash_table_iterator_t* const iterator) {
  iterator->inner_index = 0;
  iterator->outer_index = 0;
}

/**
 * @brief Retrieves the next key-value pair from a hash table with copying.
 * 
 * @param table The hash table to use.
 * @param iterator The iterator to use.
 * @param entry A pointer to a key-value entry.
 * @see hash_table_iterator_next_ref
 * @return Whether there is a new key-value pair.
 */
bool hash_table_iterator_next(const hash_table_t* const table, hash_table_iterator_t* const iterator, hash_table_entry_t* const entry);

/**
 * @brief Retrieves the next key-value pair from a hash table without copying.
 * 
 * @param table The hash table to use.
 * @param iterator The iterator to use.
 * @param entry A pointer to a key-value entry reference.
 * @see hash_table_iterator_next
 * @return Whether there is a new key-value pair.
 */
bool hash_table_iterator_next_ref(const hash_table_t* const table, hash_table_iterator_t* const iterator, hash_table_entry_t** entry);
#endif
#endif