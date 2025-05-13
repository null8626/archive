#ifndef _STACK_H
#define _STACK_H

#include <stdbool.h>
#include <stddef.h>

typedef void* stack_data_t;
typedef void* stack_data_free_argument_t;
typedef void (*stack_data_free_t)(stack_data_t, stack_data_free_argument_t);

typedef struct {
  stack_data_t* stack;
  size_t length;
  size_t capacity;
} stack_t;

bool stack_new(stack_t* const stack);

void stack_clear(stack_t* const stack, const stack_data_free_t data_free, const stack_data_free_argument_t data_free_argument);

inline bool stack_is_empty(const stack_t* const stack) {
  return stack->length == 0;
}

bool stack_push(stack_t* const stack, const stack_data_t data);
bool stack_pop(stack_t* const stack, stack_data_t* const output);
stack_data_t stack_peek(const stack_t* const stack);

void stack_free(stack_t* const stack, const stack_data_free_t data_free, const stack_data_free_argument_t data_free_argument);

#endif