#ifndef __CALCULATOR_TOKEN_H
#define __CALCULATOR_TOKEN_H

#include <stdbool.h>
#include <stdint.h>
#include <float.h>

#define CALCULATOR_INVALID_NUMBER DBL_MAX

#define CALCULATOR_TOKEN_TYPE_NULL 0
#define CALCULATOR_TOKEN_TYPE_OPERAND 1
#define CALCULATOR_TOKEN_TYPE_OPERATOR 2
#define CALCULATOR_TOKEN_TYPE_OPENING_BRACKET 3
#define CALCULATOR_TOKEN_TYPE_CLOSING_BRACKET 4

typedef double calculator_number_t;
typedef int8_t calculator_token_precedence_t;

typedef calculator_number_t (*calculator_function_t)(const calculator_number_t);

typedef union {
  calculator_number_t number;
  char character;
  struct {
    calculator_function_t function;
    bool negative;
  } value;
} calculator_token_data_t;

typedef struct {
  calculator_token_data_t data;
  uint8_t type;
} calculator_token_t;

calculator_function_t calculator_token_parse_function_call(const char** expression);
calculator_number_t calculator_token_parse_number(const char** expression);
calculator_token_precedence_t calculator_token_precedence(const calculator_token_t* const token);

#endif