#ifndef __CALCULATOR_TOKEN_H
#define __CALCULATOR_TOKEN_H

#include "calculator.h"

#include <stdbool.h>
#include <float.h>

#define CALCULATOR_TOKEN_TYPE_NULL 0
#define CALCULATOR_TOKEN_TYPE_OPERAND 1
#define CALCULATOR_TOKEN_TYPE_OPERATOR 2
#define CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND 3

#define CALCULATOR_TOKEN_FEED_FATAL_ERROR 0
#define CALCULATOR_TOKEN_FEED_SUCCESSFUL 1
#define CALCULATOR_TOKEN_FEED_ANOTHER_TOKEN 2
#define CALCULATOR_TOKEN_FEED_GOT_WHITESPACE 3

typedef calculator_number_t calculator_token_data_t;
typedef uint8_t calculator_token_decimal_length_t;
typedef int8_t calculator_token_precedence_t;
typedef uint8_t calculator_token_feed_status_t;

typedef union {
  bool unary;
  bool negative;
  calculator_token_decimal_length_t decimal_length;
} calculator_token_additional_data_t;

typedef struct {
  calculator_token_data_t data;
  calculator_token_additional_data_t additional_data;
  uint8_t type;
} calculator_token_t;

void calculator_token_new(calculator_token_t* const token);

inline bool calculator_token_is_operand(const calculator_token_t* const token) {
  return token->type == CALCULATOR_TOKEN_TYPE_OPERAND || token->type == CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND;
}

inline bool calculator_token_is_opening_bracket(const calculator_token_t* const token) {
  return token->type == CALCULATOR_TOKEN_TYPE_OPERATOR && (char)token->data == '(';
}

inline bool calculator_token_is_closing_bracket(const calculator_token_t* const token) {
  return token->type == CALCULATOR_TOKEN_TYPE_OPERATOR && (char)token->data == ')';
}

calculator_token_feed_status_t calculator_token_feed(calculator_token_t* const token, const char c);
calculator_token_precedence_t calculator_token_precedence(const calculator_token_t* const token);

#endif