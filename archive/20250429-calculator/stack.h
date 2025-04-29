#ifndef __CALCULATOR_STACK_H
#define __CALCULATOR_STACK_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t* stack;
  size_t length;
  size_t capacity;
  size_t member_size;
} calculator_stack_t;

bool calculator_stack_new(calculator_stack_t* const stack, const size_t member_size);

inline void calculator_stack_clear(calculator_stack_t* const stack) {
  stack->length = 0;
}

void* calculator_stack_alloc(calculator_stack_t* const stack);
bool calculator_stack_push(calculator_stack_t* const stack, const void* const data);
bool calculator_stack_pop(calculator_stack_t* const stack, void* const output);
void* calculator_stack_peek(const calculator_stack_t* const stack);

void calculator_stack_free(calculator_stack_t* const stack);

#endif