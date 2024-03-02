#include "array.h"

#include <stdlib.h>
#include <string.h>

sus_array_member_t sus_array_get(const sus_array_t * array, const uint32_t index) {
  if (index >= array->len) {
    return NULL;
  } else {
    return sus_array_get_unchecked(array, index);
  }
}

void sus_array_append(sus_array_t * array, sus_array_member_t data) {
  if (array->len >= array->cap) {
    array->cap += SUS_ARRAY_CHUNK_SIZE;
    array->array = (sus_array_member_t)realloc(array->array, array->cap * array->memb);
  }
  
  memcpy(array->array + (array->len * array->memb), data, array->memb);
  array->len++;
}