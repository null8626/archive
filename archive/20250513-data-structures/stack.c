#include "stack.h"

#include <stdlib.h>

#if defined(_DEBUG) || !defined(NDEBUG)
#define _STACK_ASSERT(expr)  \
  if (!(expr)) {             \
    return 0;                \
  }
#else
#define _STACK_ASSERT(expr)
#endif

bool stack_new(stack_t* const stack) {
  _STACK_ASSERT(stack != NULL);

  stack->length = 0;
  stack->capacity = 2;

  return (stack->stack = malloc(stack->capacity * sizeof(stack_data_t))) != NULL;
}

static bool stack_check_capacity(stack_t* const stack) {
  _STACK_ASSERT(stack != NULL);

  if (stack->length == stack->capacity) {
    stack->capacity *= 2;

    stack_data_t* new_stack = realloc(stack->stack, stack->capacity * sizeof(stack_data_t));

    if (new_stack == NULL) {
      return false;
    }

    stack->stack = new_stack;
  }

  return true;
}

void stack_clear(stack_t* const stack, const stack_data_free_t data_free, const stack_data_free_argument_t data_free_argument) {
  if (data_free != NULL) {
    for (size_t i = 0; i < stack->length; i++) {
      data_free(stack->stack[i], data_free_argument);
    }
  }

  stack->length = 0;
}

bool stack_push(stack_t* const stack, const stack_data_t data) {
  _STACK_ASSERT(stack != NULL);

  if (!stack_check_capacity(stack)) {
    return false;
  }

  stack->stack[stack->length++] = data;

  return true;
}

bool stack_pop(stack_t* const stack, stack_data_t* const output) {
  _STACK_ASSERT(stack != NULL);

  if (output != NULL) {
    *output = NULL;
  }

  if (stack->length == 0) {
    return false;
  }

  stack->length--;

  if (output != NULL) {
    *output = stack->stack[stack->length];
  }

  return true;
}

stack_data_t stack_peek(const stack_t* const stack) {
  _STACK_ASSERT(stack != NULL);
  
  return stack->length == 0 ? NULL : stack->stack[stack->length - 1];
}

void stack_free(stack_t* const stack, const stack_data_free_t data_free, const stack_data_free_argument_t data_free_argument) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (stack == NULL) {
    return;
  }
#endif
  
  if (stack->stack != NULL) {
    if (data_free != NULL) {
      for (size_t i = 0; i < stack->length; i++) {
        data_free(stack->stack[i], data_free_argument);
      }
    }
    
    free(stack->stack);

    stack->stack = NULL;
  }
}