#ifndef __CALCULATOR_H
#define __CALCULATOR_H

#include <stdbool.h>
#include <stdint.h>
#include <float.h>

#define CALCULATOR_INVALID_NUMBER DBL_MAX

#define CALCULATOR_STATE_NEXT_OP_BINARY 0
#define CALCULATOR_STATE_NEXT_OP_UNARY 1
#define CALCULATOR_STATE_NEXT_DECIMAL_POINT 2
#define CALCULATOR_STATE_NEXT_NEGATIVE_NUMBER 3

typedef uint8_t calculator_state_t;
typedef double calculator_number_t;

#include "token.h"
#include "stack.h"

typedef struct {
  calculator_state_t state;
  calculator_stack_t results;
  calculator_stack_t operators;
} calculator_t;

bool calculator_new(calculator_t* const calculator);
calculator_number_t calculator_compute(calculator_t* const calculator, const char* expression);
void calculator_free(calculator_t* const calculator);

#endif