#include "stack.h"

#include <stdlib.h>
#include <string.h>

bool calculator_stack_new(calculator_stack_t* const stack, const size_t member_size) {
  stack->length = 0;
  stack->capacity = 2;
  stack->member_size = member_size;

  return (stack->stack = malloc(stack->capacity * stack->member_size)) != NULL;
}

static bool calculator_stack_check_capacity(calculator_stack_t* const stack) {
  if (stack->length == stack->capacity) {
    stack->capacity *= 2;

    uint8_t* const new_stack = realloc(stack->stack, stack->capacity * stack->member_size);

    if (new_stack == NULL) {
      return false;
    }

    stack->stack = new_stack;
  }

  return true;
}

void* calculator_stack_alloc(calculator_stack_t* const stack) {
  return calculator_stack_check_capacity(stack) ? stack->stack + (stack->length++ * stack->member_size) : NULL;
}

bool calculator_stack_push(calculator_stack_t* const stack, const void* const data) {
  void* const allocated = calculator_stack_alloc(stack);
  
  if (allocated == NULL) {
    return false;
  }

  memcpy(allocated, data, stack->member_size);

  return true;
}

bool calculator_stack_pop(calculator_stack_t* const stack, void* const output) {
  if (stack->length == 0) {
    return false;
  }

  stack->length--;

  if (output != NULL) {
    memcpy(output, stack->stack + (stack->length * stack->member_size), stack->member_size);
  }

  return true;
}

void* calculator_stack_peek(const calculator_stack_t* const stack) {
  return stack->length == 0 ? NULL : stack->stack + ((stack->length - 1) * stack->member_size);
}

void calculator_stack_free(calculator_stack_t* const stack) {
  if (stack->stack != NULL) {
    free(stack->stack);

    stack->stack = NULL;
  }
}