#include "probed_hash_table.h"

#include <stdlib.h>
#include <string.h>

#if defined(_DEBUG) || !defined(NDEBUG)
#define _PROBED_HASH_TABLE_ASSERT(expr)  \
  if (!(expr)) {                         \
    return 0;                            \
  }
#else
#define _PROBED_HASH_TABLE_ASSERT(expr)
#endif

bool probed_hash_table_new(probed_hash_table_t* const table, const probed_hash_table_descriptor_t* const descriptor) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL);
  
  memset(table, 0, sizeof(probed_hash_table_t));

  _PROBED_HASH_TABLE_ASSERT(descriptor != NULL && descriptor->primary_probed_hash_function.function != NULL && descriptor->key_is_equal != NULL && descriptor->size != 0);

  if (!stack_new_with_capacity(&table->stack, descriptor->size)) {
    return false;
  }

  memset(&table->stack.stack, 0, descriptor->size * sizeof(stack_data_t));
  memcpy(&table->descriptor, descriptor, sizeof(probed_hash_table_descriptor_t));

  return true;
}

static size_t probed_hash_table_get_index(const probed_hash_table_t* const table, const probed_hash_function_descriptor_t* const function_descriptor, const probed_hash_table_key_t key) {
  probed_hash_function_additional_argument_t probed_hash_function_additional_argument;

  probed_hash_function_additional_argument.data = function_descriptor->additional_argument;
  probed_hash_function_additional_argument.stack_size = table->descriptor.size;

  return function_descriptor->function(key, &probed_hash_function_additional_argument);
}

static bool probed_hash_table_insert_at(probed_hash_table_t* const table, const size_t table_index, const probed_hash_table_key_t key, const probed_hash_table_value_t value) {
  probed_hash_table_entry_t* const new_entry = malloc(sizeof(probed_hash_table_entry_t));

  if (new_entry == NULL) {
    return false;
  }
  
  new_entry->key = key;
  new_entry->value = value;

  table->stack.stack[table_index] = (stack_data_t*)new_entry;

  if (table_index >= table->stack.length) {
    table->stack.length = table_index + 1;
  }

  return true;
}

static bool probed_hash_table_push(probed_hash_table_t* const table, const probed_hash_table_key_t key, const probed_hash_table_value_t value) {
  probed_hash_table_entry_t* const entry = malloc(sizeof(probed_hash_table_entry_t));

  if (entry == NULL) {
    return false;
  }

  entry->key = key;
  entry->value = value;

  if (!stack_push(&table->stack, (const stack_data_t)entry)) {
    free(entry);
    
    return false;
  }

  return true;
}

bool probed_hash_table_linear_get(const probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* const output) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && key != NULL);

  size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  while (table_index < table->stack.capacity) {
    const probed_hash_table_entry_t* const entry = (const probed_hash_table_entry_t* const)table->stack.stack[table_index];

    if (entry == NULL) {
      break;
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      if (output != NULL) {
        *output = entry->value;
      }

      return true;
    }

    table_index++;
  }

  return false;
}

bool probed_hash_table_linear_set(probed_hash_table_t* const table, const probed_hash_table_key_t key, const probed_hash_table_value_t value) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && key != NULL);

  size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  while (table_index < table->stack.capacity) {
    probed_hash_table_entry_t* const entry = (probed_hash_table_entry_t* const)table->stack.stack[table_index];

    if (entry == NULL) {
      return probed_hash_table_insert_at(table, table_index, key, value);
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      if (table->descriptor.entry_free.value_free != NULL) {
        table->descriptor.entry_free.value_free(entry->value, table->descriptor.entry_free.value_free_argument);
      }

      entry->value = value;

      return true;
    }

    table_index++;
  }

  return probed_hash_table_push(table, key, value);
}

bool probed_hash_table_quadratic_get(const probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* const output) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && key != NULL);

  const size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  size_t new_table_index = table_index, quadratic_iteration = 1;

  while (new_table_index < table->stack.capacity) {
    const probed_hash_table_entry_t* const entry = (const probed_hash_table_entry_t* const)table->stack.stack[new_table_index];

    if (entry == NULL) {
      break;
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      if (output != NULL) {
        *output = entry->value;
      }

      return true;
    }

    quadratic_iteration++;
    new_table_index = table_index + (quadratic_iteration * quadratic_iteration);
  }

  return false;
}

bool probed_hash_table_quadratic_set(probed_hash_table_t* const table, const probed_hash_table_key_t key, const probed_hash_table_value_t value) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && key != NULL);

  const size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  size_t new_table_index = table_index, quadratic_iteration = 1;

  while (new_table_index < table->stack.capacity) {
    probed_hash_table_entry_t* const entry = (probed_hash_table_entry_t* const)table->stack.stack[new_table_index];

    if (entry == NULL) {
      return probed_hash_table_insert_at(table, new_table_index, key, value);
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      if (table->descriptor.entry_free.value_free != NULL) {
        table->descriptor.entry_free.value_free(entry->value, table->descriptor.entry_free.value_free_argument);
      }

      entry->value = value;

      return true;
    }

    quadratic_iteration++;
    new_table_index = table_index + (quadratic_iteration * quadratic_iteration);
  }

  return probed_hash_table_push(table, key, value);
}

bool probed_hash_table_double_get(const probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* const output) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && table->descriptor.secondary_probed_hash_function.function != NULL && key != NULL);

  size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  while (table_index < table->stack.capacity) {
    const probed_hash_table_entry_t* const entry = (const probed_hash_table_entry_t* const)table->stack.stack[table_index];

    if (entry == NULL) {
      break;
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      if (output != NULL) {
        *output = entry->value;
      }

      return true;
    }

    table_index += probed_hash_table_get_index(table, &table->descriptor.secondary_probed_hash_function, key);
  }

  return false;
}

bool probed_hash_table_double_set(probed_hash_table_t* const table, const probed_hash_table_key_t key, const probed_hash_table_value_t value) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && table->descriptor.secondary_probed_hash_function.function != NULL && key != NULL);

  size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  while (table_index < table->stack.capacity) {
    probed_hash_table_entry_t* const entry = (probed_hash_table_entry_t* const)table->stack.stack[table_index];

    if (entry == NULL) {
      return probed_hash_table_insert_at(table, table_index, key, value);
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      if (table->descriptor.entry_free.value_free != NULL) {
        table->descriptor.entry_free.value_free(entry->value, table->descriptor.entry_free.value_free_argument);
      }

      entry->value = value;

      return true;
    }

    table_index += probed_hash_table_get_index(table, &table->descriptor.secondary_probed_hash_function, key);
  }

  return probed_hash_table_push(table, key, value);
}

static void probed_hash_table_entry_free(probed_hash_table_entry_t* const entry, const probed_hash_table_entry_free_t* const entry_free) {
  if (entry != NULL && entry_free != NULL) {
    if (entry_free->key_free != NULL) {
      entry_free->key_free(entry->key, entry_free->key_free_argument);
    }

    if (entry_free->value_free != NULL) {
      entry_free->value_free(entry->value, entry_free->value_free_argument);
    }

    free(entry);
  }
}

static void probed_hash_table_pop(probed_hash_table_t* const table, const size_t table_index, probed_hash_table_value_t* output) {
  probed_hash_table_entry_t* const entry = (probed_hash_table_entry_t* const)table->stack.stack[table_index];
  probed_hash_table_value_free_t old_value_free = NULL;

  if (output != NULL) {
    old_value_free = table->descriptor.entry_free.value_free;

    *output = entry->value;
  }

  probed_hash_table_entry_free(entry, &table->descriptor.entry_free);
  table->stack.stack[table_index] = NULL;

  if (old_value_free != NULL) {
    table->descriptor.entry_free.value_free = old_value_free;
  }
}

bool probed_hash_table_linear_pop(probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* output) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && key != NULL);

  size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  while (table_index < table->stack.capacity) {
    const probed_hash_table_entry_t* const entry = (const probed_hash_table_entry_t* const)table->stack.stack[table_index];

    if (entry == NULL) {
      break;
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      probed_hash_table_pop(table, table_index, output);
      break;
    }

    table_index++;
  }

  return true;
}

bool probed_hash_table_quadratic_pop(probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* output) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && key != NULL);

  const size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  size_t new_table_index = table_index, quadratic_iteration = 1;

  while (table_index < table->stack.capacity) {
    const probed_hash_table_entry_t* const entry = (const probed_hash_table_entry_t* const)table->stack.stack[table_index];

    if (entry == NULL) {
      break;
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      probed_hash_table_pop(table, table_index, output);
      break;
    }

    quadratic_iteration++;
    new_table_index = table_index + (quadratic_iteration * quadratic_iteration);
  }

  return true;
}

bool probed_hash_table_double_pop(probed_hash_table_t* const table, const probed_hash_table_key_t key, probed_hash_table_value_t* output) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && table->descriptor.secondary_probed_hash_function.function != NULL && key != NULL);

  size_t table_index = probed_hash_table_get_index(table, &table->descriptor.primary_probed_hash_function, key);
  
  if (table_index >= table->stack.capacity) {
    return false;
  }

  while (table_index < table->stack.capacity) {
    const probed_hash_table_entry_t* const entry = (const probed_hash_table_entry_t* const)table->stack.stack[table_index];

    if (entry == NULL) {
      break;
    } else if (table->descriptor.key_is_equal((const probed_hash_table_key_t)entry->key, key)) {
      probed_hash_table_pop_ref(table, &table->stack.stack[table_index], output);
      break;
    }

    table_index += probed_hash_table_get_index(table, &table->descriptor.secondary_probed_hash_function, key);
  }

  return true;
}

#ifdef PROBED_HASH_TABLE_ITERATOR
static probed_hash_table_entry_t* probed_hash_table_iterator_next_inner(const probed_hash_table_t* const table, probed_hash_table_iterator_t* iterator) {
  probed_hash_table_entry_t* current = NULL;

  while (*iterator < table->stack.length && (current = (probed_hash_table_entry_t*)table->stack.stack[*iterator]) == NULL) {
    *iterator++;
  }

  return current;
}

bool probed_hash_table_iterator_next(const probed_hash_table_t* const table, probed_hash_table_iterator_t* iterator, probed_hash_table_entry_t* const entry) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && iterator != NULL && entry != NULL);

  probed_hash_table_entry_t* current = probed_hash_table_iterator_next_inner(table, iterator);

  if (current == NULL) {
    return false;
  }

  memcpy(entry, current, sizeof(probed_hash_table_entry_t));
  return true;
}

bool probed_hash_table_iterator_next_ref(const probed_hash_table_t* const table, probed_hash_table_iterator_t* iterator, probed_hash_table_entry_t** entry) {
  _PROBED_HASH_TABLE_ASSERT(table != NULL && table->stack.stack != NULL && iterator != NULL && entry != NULL);

  return *iterator != table->stack.length && (*entry = probed_hash_table_iterator_next_inner(table, iterator)) != NULL;
}
#endif

void probed_hash_table_free(probed_hash_table_t* const table) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (table == NULL) {
    return;
  }
#endif

  if (table->stack.stack != NULL) {
    stack_free(&table->stack, probed_hash_table_entry_free, (const stack_data_free_argument_t)&table->descriptor.entry_free);
  }
}