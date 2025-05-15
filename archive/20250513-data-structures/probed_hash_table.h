#ifndef _PROBED_HASH_TABLE_H
#define _PROBED_HASH_TABLE_H

#include "stack.h"

typedef stack_data_t probed_hash_table_data_t;
typedef probed_hash_table_data_t probed_hash_table_key_t;
typedef probed_hash_table_data_t probed_hash_table_value_t;
typedef stack_data_free_t probed_hash_table_key_free_t;
typedef stack_data_free_t probed_hash_table_value_free_t;
typedef stack_data_free_argument_t probed_hash_table_key_free_argument_t;
typedef stack_data_free_argument_t probed_hash_table_value_free_argument_t;
typedef bool (*probed_hash_table_key_is_equal_t)(const probed_hash_table_key_t, const probed_hash_table_key_t);

typedef struct {
  probed_hash_table_key_t key;
  probed_hash_table_value_t value;
} probed_hash_table_entry_t;

typedef void* probed_hash_function_additional_argument_data_t;

typedef struct {
  size_t stack_size;
  probed_hash_function_additional_argument_data_t data;
} probed_hash_function_additional_argument_t;

typedef size_t (*probed_hash_function_t)(const probed_hash_table_key_t, const probed_hash_function_additional_argument_t* const);

typedef struct {
  probed_hash_table_key_free_t key_free;
  probed_hash_table_value_free_t value_free;
  probed_hash_table_key_free_argument_t key_free_argument;
  probed_hash_table_value_free_argument_t value_free_argument;
} probed_hash_table_entry_free_t;

typedef struct {
  probed_hash_function_t function;
  probed_hash_function_additional_argument_data_t additional_argument;
} probed_hash_function_descriptor_t;

typedef struct {
  size_t size;
  probed_hash_function_descriptor_t primary_probed_hash_function;
  probed_hash_function_descriptor_t secondary_probed_hash_function;
  probed_hash_table_key_is_equal_t key_is_equal;
  probed_hash_table_entry_free_t entry_free;
} probed_hash_table_descriptor_t;

typedef struct {
  stack_t stack;
  probed_hash_table_descriptor_t descriptor;
} probed_hash_table_t;

#ifdef PROBED_HASH_TABLE_ITERATOR
typedef size_t probed_hash_table_iterator_t;
#endif

/**
 * @brief Creates a brand new probed hash table.
 * 
 * @param table The probed hash table to create.
 * @param descriptor A pointer to a descriptor that describes the new probed hash table's behavior.
 * @see probed_hash_table_linear_get
 * @see probed_hash_table_linear_set
 * @see probed_hash_table_linear_pop
 * @see probed_hash_table_linear_has
 * @see probed_hash_table_quadratic_get
 * @see probed_hash_table_quadratic_set
 * @see probed_hash_table_quadratic_pop
 * @see probed_hash_table_quadratic_has
 * @see probed_hash_table_double_get
 * @see probed_hash_table_double_set
 * @see probed_hash_table_double_pop
 * @see probed_hash_table_double_has
 * @see probed_hash_table_iterator_begin
 * @see probed_hash_table_free
 * @return Whether the probed hash table creation was successful.
 */
bool probed_hash_table_new(probed_hash_table_t* const table, const probed_hash_table_descriptor_t* const descriptor);

/**
 * @brief Retrieves a value from a key using linear probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see probed_hash_table_quadratic_get
 * @see probed_hash_table_double_get
 * @see probed_hash_table_linear_has
 * @see probed_hash_table_linear_set
 * @see probed_hash_table_linear_pop
 * @see probed_hash_table_iterator_begin
 * @return Whether the retrieval was successful.
 */
bool probed_hash_table_linear_get(const probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* const output);

/**
 * @brief Inserts a key-value pair using linear probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to be set.
 * @param value The value to be set.
 * @see probed_hash_table_quadratic_set
 * @see probed_hash_table_double_set
 * @see probed_hash_table_linear_get
 * @see probed_hash_table_linear_pop
 * @return Whether the operation was successful.
 * @note This will override a value with an existing key. Such existing value will be freed prior to reassignment.
 */
bool probed_hash_table_linear_set(probed_hash_table_t* const table, const probed_hash_table_key_t key, const probed_hash_table_value_t value);

/**
 * @brief Removes and returns a value from a key using linear probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see probed_hash_table_quadratic_pop
 * @see probed_hash_table_double_pop
 * @see probed_hash_table_linear_get
 * @see probed_hash_table_linear_set
 * @see probed_hash_table_free
 * @return Whether the operation was successful.
 * @note If output is NULL, the value will be freed after being removed from the probed hash table.
 */
bool probed_hash_table_linear_pop(probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* output);

/**
 * @brief Checks if a key already exists in the probed hash table using linear probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to check.
 * @see probed_hash_table_quadratic_has
 * @see probed_hash_table_double_has
 * @see probed_hash_table_linear_get
 * @see probed_hash_table_linear_set
 * @see probed_hash_table_iterator_begin
 * @return Whether the key exists.
 */
inline bool probed_hash_table_linear_has(const probed_hash_table_t* const table, const probed_hash_table_key_t key) {
  return probed_hash_table_linear_get(table, key, NULL);
}

/**
 * @brief Retrieves a value from a key using quadratic probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see probed_hash_table_linear_get
 * @see probed_hash_table_double_get
 * @see probed_hash_table_quadratic_has
 * @see probed_hash_table_quadratic_set
 * @see probed_hash_table_quadratic_pop
 * @see probed_hash_table_iterator_begin
 * @return Whether the retrieval was successful.
 */
bool probed_hash_table_quadratic_get(const probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* const output);

/**
 * @brief Inserts a key-value pair using quadratic probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to be set.
 * @param value The value to be set.
 * @see probed_hash_table_linear_set
 * @see probed_hash_table_double_set
 * @see probed_hash_table_quadratic_get
 * @see probed_hash_table_quadratic_pop
 * @return Whether the operation was successful.
 * @note This will override a value with an existing key. Such existing value will be freed prior to reassignment.
 */
bool probed_hash_table_quadratic_set(probed_hash_table_t* const table, const probed_hash_table_key_t key, const probed_hash_table_value_t value);

/**
 * @brief Removes and returns a value from a key using quadratic probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see probed_hash_table_linear_pop
 * @see probed_hash_table_double_pop
 * @see probed_hash_table_quadratic_get
 * @see probed_hash_table_quadratic_set
 * @see probed_hash_table_free
 * @return Whether the operation was successful.
 * @note If output is NULL, the value will be freed after being removed from the probed hash table.
 */
bool probed_hash_table_quadratic_pop(probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* output);

/**
 * @brief Checks if a key already exists in the probed hash table using quadratic probing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to check.
 * @see probed_hash_table_linear_has
 * @see probed_hash_table_double_has
 * @see probed_hash_table_quadratic_get
 * @see probed_hash_table_quadratic_set
 * @see probed_hash_table_iterator_begin
 * @return Whether the key exists.
 */
inline bool probed_hash_table_quadratic_has(const probed_hash_table_t* const table, const probed_hash_table_key_t key) {
  return probed_hash_table_quadratic_get(table, key, NULL);
}

/**
 * @brief Retrieves a value from a key using double hashing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see probed_hash_table_linear_get
 * @see probed_hash_table_quadratic_get
 * @see probed_hash_table_double_has
 * @see probed_hash_table_double_set
 * @see probed_hash_table_double_pop
 * @see probed_hash_table_iterator_begin
 * @return Whether the retrieval was successful.
 */
bool probed_hash_table_double_get(const probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* const output);

/**
 * @brief Inserts a key-value pair using double hashing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to be set.
 * @param value The value to be set.
 * @see probed_hash_table_linear_set
 * @see probed_hash_table_quadratic_set
 * @see probed_hash_table_double_get
 * @see probed_hash_table_double_pop
 * @return Whether the operation was successful.
 * @note This will override a value with an existing key. Such existing value will be freed prior to reassignment.
 */
bool probed_hash_table_double_set(probed_hash_table_t* const table, const probed_hash_table_key_t key, const probed_hash_table_value_t value);

/**
 * @brief Removes and returns a value from a key using double hashing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to use.
 * @param output A pointer to the output value. Optional.
 * @see probed_hash_table_linear_pop
 * @see probed_hash_table_quadratic_pop
 * @see probed_hash_table_double_get
 * @see probed_hash_table_double_set
 * @see probed_hash_table_free
 * @return Whether the operation was successful.
 * @note If output is NULL, the value will be freed after being removed from the probed hash table.
 */
bool probed_hash_table_double_pop(probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* output);

/**
 * @brief Checks if a key already exists in the probed hash table using double hashing.
 * 
 * @param table The probed hash table to use.
 * @param key The key to check.
 * @see probed_hash_table_linear_has
 * @see probed_hash_table_quadratic_has
 * @see probed_hash_table_double_get
 * @see probed_hash_table_double_set
 * @see probed_hash_table_iterator_begin
 * @return Whether the key exists.
 */
inline bool probed_hash_table_double_has(const probed_hash_table_t* const table, const probed_hash_table_key_t key) {
  return probed_hash_table_double_get(table, key, NULL);
}

/**
 * @brief Frees every key-value pair from a probed hash table before also freeing the probed hash table itself.
 * 
 * @param table The probed hash table to be freed.
 */
void probed_hash_table_free(probed_hash_table_t* const table);

#ifdef PROBED_HASH_TABLE_ITERATOR
/**
 * @brief Initiates a probed hash table key-value iterator.
 * 
 * @param iterator The iterator to be initiated.
 * @see probed_hash_table_iterator_next
 * @see probed_hash_table_iterator_next_ref
 * @note An existing iterator instance can be reused safely with this function.
 */
inline void probed_hash_table_iterator_begin(probed_hash_table_iterator_t* const iterator) {
  *iterator = 0;
}

/**
 * @brief Retrieves the next key-value pair from a probed hash table with copying.
 * 
 * @param table The probed hash table to use.
 * @param iterator The iterator to use.
 * @param entry A pointer to a key-value entry.
 * @see probed_hash_table_iterator_next_ref
 * @return Whether there is a new key-value pair.
 */
bool probed_hash_table_iterator_next(const probed_hash_table_t* const table, probed_hash_table_iterator_t* iterator, probed_hash_table_entry_t* const entry);

/**
 * @brief Retrieves the next key-value pair from a probed hash table without copying.
 * 
 * @param table The probed hash table to use.
 * @param iterator The iterator to use.
 * @param entry A pointer to a key-value entry reference.
 * @see probed_hash_table_iterator_next
 * @return Whether there is a new key-value pair.
 */
bool probed_hash_table_iterator_next_ref(const probed_hash_table_t* const table, probed_hash_table_iterator_t* iterator, probed_hash_table_entry_t** entry);
#endif
#endif