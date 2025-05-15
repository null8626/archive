#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

#if defined(_DEBUG) || !defined(NDEBUG)
#define _HASH_TABLE_ASSERT(expr)  \
  if (!(expr)) {                  \
    return 0;                     \
  }
#else
#define _HASH_TABLE_ASSERT(expr)
#endif

static bool hash_table_standard_is_equal(const hash_table_key_t first, const hash_table_key_t second) {
  return first == second;
}

bool hash_table_new(hash_table_t* const table, const hash_table_descriptor_t* const descriptor) {
  _HASH_TABLE_ASSERT(table != NULL);
  
  memset(table, 0, sizeof(hash_table_t));

  _HASH_TABLE_ASSERT(descriptor != NULL && descriptor->hash_function.function != NULL && descriptor->size != 0);

  if ((table->table = malloc(descriptor->size * sizeof(stack_t))) == NULL) {
    return false;
  }

  memcpy(&table->descriptor, descriptor, sizeof(hash_table_descriptor_t));
  memset(table->table, 0, descriptor->size * sizeof(stack_t));

  if (table->descriptor.key_is_equal == NULL) {
    table->descriptor.key_is_equal = hash_table_standard_is_equal;
  }

  for (size_t i = 0; i < descriptor->size; i++) {
    if (!stack_new(&table->table[i])) {
      return false;
    }
  }

  return true;
}

static size_t hash_table_get_index(const hash_table_descriptor_t* const table_descriptor, const hash_table_key_t key) {
  hash_function_additional_argument_t hash_function_additional_argument;

  hash_function_additional_argument.data = table_descriptor->hash_function.additional_argument;
  hash_function_additional_argument.table_size = table_descriptor->size;

  return table_descriptor->hash_function.function(key, &hash_function_additional_argument);
}

bool hash_table_get(const hash_table_t* const table, const hash_table_key_t key, hash_table_value_t* const output) {
  _HASH_TABLE_ASSERT(table != NULL && table->table != NULL && key != NULL);

  const size_t table_index = hash_table_get_index(&table->descriptor, key);

  if (table_index >= table->descriptor.size) {
    return false;
  }

  const stack_t* const stack = &table->table[table_index];

  for (size_t i = 0; i < stack->length; i++) {
    const hash_table_entry_t* const entry = (const hash_table_entry_t* const)stack->stack[i];

    if (table->descriptor.key_is_equal((const hash_table_key_t)entry->key, key)) {
      if (output != NULL) {
        *output = entry->value;
      }

      return true;
    }
  }

  return false;
}

bool hash_table_set(hash_table_t* const table, const hash_table_key_t key, const hash_table_value_t value) {
  _HASH_TABLE_ASSERT(table != NULL && table->table != NULL && key != NULL);

  const size_t table_index = hash_table_get_index(&table->descriptor, key);

  if (table_index >= table->descriptor.size) {
    return false;
  }

  stack_t* const stack = &table->table[table_index];

  for (size_t i = 0; i < stack->length; i++) {
    hash_table_entry_t* const entry = (hash_table_entry_t* const)stack->stack[i];

    if (table->descriptor.key_is_equal((const hash_table_key_t)entry->key, key)) {
      if (table->descriptor.entry_free.value_free != NULL) {
        table->descriptor.entry_free.value_free(entry->value, table->descriptor.entry_free.value_free_argument);
      }

      entry->value = value;
        
      return true;
    }
  }

  hash_table_entry_t* const entry = malloc(sizeof(hash_table_entry_t));

  if (entry == NULL) {
    return false;
  }

  entry->key = key;
  entry->value = value;

  if (!stack_push(stack, (const stack_data_t)entry)) {
    free(entry);
    
    return false;
  }

  return true;
}

static void hash_table_entry_free(hash_table_entry_t* const entry, const hash_table_entry_free_t* const entry_free) {
  if (entry_free != NULL) {
    if (entry_free->key_free != NULL) {
      entry_free->key_free(entry->key, entry_free->key_free_argument);
    }

    if (entry_free->value_free != NULL) {
      entry_free->value_free(entry->value, entry_free->value_free_argument);
    }

    free(entry);
  }
}

bool hash_table_pop(hash_table_t* const table, const hash_table_key_t key, hash_table_value_t* output) {
  _HASH_TABLE_ASSERT(table != NULL && table->table != NULL && key != NULL);

  const size_t table_index = hash_table_get_index(&table->descriptor, key);

  if (table_index >= table->descriptor.size) {
    return false;
  }

  stack_t* const stack = &table->table[table_index];

  for (size_t i = 0; i < stack->length; i++) {
    hash_table_entry_t* const entry = (hash_table_entry_t* const)stack->stack[i];

    if (table->descriptor.key_is_equal((const hash_table_key_t)entry->key, key)) {
      hash_table_value_free_t old_value_free = NULL;

      if (output != NULL) {
        old_value_free = table->descriptor.entry_free.value_free;

        *output = entry->value;
      }

      hash_table_entry_free(entry, &table->descriptor.entry_free);
      memcpy(entry, entry + 1, stack->length - i);

      if (old_value_free != NULL) {
        table->descriptor.entry_free.value_free = old_value_free;
      }

      stack->length--;

      break;
    }
  }

  return true;
}

#ifdef HASH_TABLE_ITERATOR
static stack_t* hash_table_iterator_next_stack(const hash_table_t* const table, hash_table_iterator_t* iterator) {
  if (iterator->outer_index == table->descriptor.size) {
    return NULL;
  }

  stack_t* stack = &table->table[iterator->outer_index];

  if (iterator->inner_index == stack->length) {
    iterator->outer_index++;
    iterator->inner_index = 0;

    return hash_table_iterator_next_stack(table, iterator);
  }

  return stack;
}

bool hash_table_iterator_next(const hash_table_t* const table, hash_table_iterator_t* const iterator, hash_table_entry_t* const entry) {
  _HASH_TABLE_ASSERT(table != NULL && table->table != NULL && iterator != NULL && entry != NULL);

  stack_t* stack = hash_table_iterator_next_stack(table, iterator);

  if (stack == NULL) {
    return false;
  }

  memcpy(entry, &stack->stack[iterator->inner_index++], sizeof(hash_table_entry_t));

  return true;
}

bool hash_table_iterator_next_ref(const hash_table_t* const table, hash_table_iterator_t* const iterator, hash_table_entry_t** entry) {
  _HASH_TABLE_ASSERT(table != NULL && table->table != NULL && iterator != NULL && entry != NULL);

  stack_t* stack = hash_table_iterator_next_stack(table, iterator);

  if (stack == NULL) {
    return false;
  }

  *entry = (hash_table_entry_t*)stack->stack[iterator->inner_index++];

  return true;
}
#endif

void hash_table_free(hash_table_t* const table) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (table == NULL) {
    return;
  }
#endif

  if (table->table != NULL) {
    for (size_t i = 0; i < table->descriptor.size; i++) {
      stack_free(&table->table[i], hash_table_entry_free, (const stack_data_free_argument_t)&table->descriptor.entry_free);
    }

    free(table->table);
    table->table = NULL;
  }
}