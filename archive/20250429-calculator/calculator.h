#ifndef __CALCULATOR_H
#define __CALCULATOR_H

#include "token.h"
#include "stack.h"

typedef struct {
  calculator_token_t previous_token;
  calculator_stack_t results;
  calculator_stack_t operators;
} calculator_state_t;

bool calculator_new(calculator_state_t* state);
calculator_number_t calculator_compute(calculator_state_t* state, const char* expression);
void calculator_free(calculator_state_t* const state);

#endif