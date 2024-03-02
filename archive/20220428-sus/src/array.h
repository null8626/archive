#ifndef _SUS_ARRAY_H
#define _SUS_ARRAY_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef SUS_ARRAY_CHUNK_SIZE
#define SUS_ARRAY_CHUNK_SIZE 5
#endif

typedef uint8_t * sus_array_member_t;

typedef struct {
  sus_array_member_t array;
  uint32_t len;
  uint32_t cap;
  uint8_t memb;
} sus_array_t;

void sus_array_append(sus_array_t *, sus_array_member_t);
sus_array_member_t sus_array_get(const sus_array_t *, const uint32_t);

static inline void sus_array_new(sus_array_t * array, const uint8_t member_size) {
  array->array = (sus_array_member_t)malloc(member_size * SUS_ARRAY_CHUNK_SIZE);
  array->len   = 0;
  array->cap   = SUS_ARRAY_CHUNK_SIZE;
  array->memb  = member_size;
}

static inline sus_array_member_t sus_array_get_unchecked(const sus_array_t * array, const uint32_t index) {
  return array->array + (array->memb * index);
}

static inline void sus_array_pop(sus_array_t * array, const uint32_t index) {
  memcpy(array->array + (index * array->memb), array->array + ((index + 1) * array->memb), (array->len - index) * array->memb);
  array->len--;
}

static inline uint32_t sus_array_index_of(const sus_array_t * array, const sus_array_member_t member) {
  if (array->array == member) {
    return 0UL;
  } else {
    return (uint32_t)((float)((size_t)member - (size_t)array->array) / (float)array->memb);
  }
}

static inline void sus_array_delete(sus_array_t * array) {
  free(array->array);
}

#endif