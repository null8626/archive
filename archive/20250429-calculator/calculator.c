#include "calculator.h"

#include <math.h>

typedef uint8_t calculator_tokenization_flag_t;

#define CALCULATOR_TOKENIZATION_FLAG_NORMAL 0
#define CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND 1
#define CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND_AND_WHITESPACE 2

bool calculator_new(calculator_t* const calculator) {
  return calculator_stack_new(&calculator->results, sizeof(calculator_number_t)) && calculator_stack_new(&calculator->operators, sizeof(calculator_token_t));
}

static bool calculator_compute_postfix_token(calculator_t* const calculator, const calculator_token_t* const token) {
  if (calculator_token_is_operand(token)) {
    if (token->data == CALCULATOR_INVALID_NUMBER) {
      return false;
    }

    return calculator_stack_push(&calculator->results, &token->data);
  }

  calculator_number_t a, b;

  if (!calculator_stack_pop(&calculator->results, &b) || !calculator_stack_pop(&calculator->results, &a)) {
    return false;
  }

  calculator_number_t result;

  switch ((char)token->data) {
    case '+': {
      result = a + b;
      break;
    }

    case '-': {
      result = a - b;
      break;
    }

    case '*': {
      result = a * b;
      break;
    }

    case '/': {
      if (b == 0) {
        return false;
      }

      result = a / b;
      break;
    }

    case '^': {
      result = (calculator_number_t)pow((double)a, (double)b);
      break;
    }

    default: {
      return false;
    }
  }

  return calculator_stack_push(&calculator->results, &result);
}

static bool calculator_compute_infix_token(calculator_t* const calculator, calculator_token_t* const token) {
  const bool next_is_negative = calculator->state == CALCULATOR_STATE_NEXT_NEGATIVE_NUMBER;
  
  if (calculator_token_is_operand(token)) {
    // Incomplete decimal number, e.g: .
    if (token->data == CALCULATOR_INVALID_NUMBER) {
      return false;
    } else if (next_is_negative) {
      // Negative number, e.g: -5
      token->data = -token->data;
    }

    calculator->state = CALCULATOR_STATE_NEXT_OP_BINARY;

    return calculator_compute_postfix_token(calculator, token);
  }

  switch ((char)token->data) {
    case '(': {
      // -(2 + 5), mark this entire bracket's computation result as negative
      if (next_is_negative) {
        token->type = CALCULATOR_TOKEN_TYPE_NEGATIVE_BRACKET;
      }

      calculator->state = CALCULATOR_STATE_NEXT_OP_UNARY;

      return calculator_stack_push(&calculator->operators, token);
    }

    case ')': {
      // (2 + 5 -) is not valid
      if (next_is_negative) {
        return false;
      }

      calculator_token_t operator;

      calculator_token_new(&operator);

      while (calculator_stack_pop(&calculator->operators, &operator) && (char)operator.data != '(') {
        if (!calculator_compute_postfix_token(calculator, &operator)) {
          return false;
        }
      }

      // 2 + 5) is not valid
      if ((char)operator.data != '(') {
        return false;
      } else if (operator.type == CALCULATOR_TOKEN_TYPE_NEGATIVE_BRACKET) {
        calculator_number_t* const result = (calculator_number_t*)calculator_stack_peek(&calculator->results);

        if (result == NULL) {
          return false;
        }

        *result = -*result;
      }

      calculator->state = CALCULATOR_STATE_NEXT_OP_BINARY;

      break;
    }

    case '-': {
      if (calculator->state == CALCULATOR_STATE_NEXT_OP_UNARY) {
        calculator->state = CALCULATOR_STATE_NEXT_NEGATIVE_NUMBER;

        break;
      }

      // Skip the check below
      goto CALCULATOR_HANDLE_INFIX_OPERATOR_TOKEN;
    }

    default: {
      // -+ is not valid
      if (next_is_negative) {
        return false;
      }

CALCULATOR_HANDLE_INFIX_OPERATOR_TOKEN:
      calculator->state = CALCULATOR_STATE_NEXT_OP_UNARY;

      const calculator_token_precedence_t token_precedence = calculator_token_precedence(token);
      calculator_token_t* operator;

      while ((operator = (calculator_token_t*)calculator_stack_peek(&calculator->operators)) != NULL && (char)operator->data != '(' && token_precedence <= calculator_token_precedence(operator)) {
        if (!calculator_compute_postfix_token(calculator, operator)) {
          return false;
        }

        calculator_stack_pop(&calculator->operators, NULL);
      }

      return calculator_stack_push(&calculator->operators, token);
    }
  }

  return true;
}

static bool calculator_compute_infix_multiplication(calculator_t* const calculator) {
  calculator_token_t token;

  calculator_token_new(&token);

  token.data = (calculator_token_data_t)('*');
  token.type = CALCULATOR_TOKEN_TYPE_OPERATOR;

  return calculator_compute_infix_token(calculator, &token);
}

calculator_number_t calculator_compute(calculator_t* const calculator, const char* expression) {
  calculator->state = CALCULATOR_STATE_NEXT_OP_UNARY;

  calculator_stack_clear(&calculator->results);
  calculator_stack_clear(&calculator->operators);

  calculator_tokenization_flag_t flag = CALCULATOR_TOKENIZATION_FLAG_NORMAL;
  calculator_token_t token, replacement_token;
  char c;

  calculator_token_new(&token);

  while ((c = *(expression++)) != '\0') {
    switch (calculator_token_feed(&token, c)) {
      case CALCULATOR_TOKEN_FEED_SUCCESSFUL: {
        if (calculator_token_is_operand(&token)) {
          // 5 2 is not valid
          if (flag == CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND_AND_WHITESPACE) {
            return CALCULATOR_INVALID_NUMBER;
          }

          flag = CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND;
        } else {
          flag = CALCULATOR_TOKENIZATION_FLAG_NORMAL;
        }

        break;
      }

      case CALCULATOR_TOKEN_FEED_FATAL_ERROR: {
        return CALCULATOR_INVALID_NUMBER;
      }

      case CALCULATOR_TOKEN_FEED_GOT_WHITESPACE: {
        // Check for impending 5 2 which is invalid
        if (flag == CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND) {
          flag = CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND_AND_WHITESPACE;
        }

        break;
      }

      // -- cancels out
      case CALCULATOR_TOKEN_FEED_CANCELS_OUT: {
        // Rebuild brand new token
        calculator_token_new(&token);
        
        break;
      }

      case CALCULATOR_TOKEN_FEED_ANOTHER_TOKEN: {
        calculator_token_new(&replacement_token);

        // Same thing, most possibly an invalid character/syntax
        if (calculator_token_feed(&replacement_token, c) == CALCULATOR_TOKEN_FEED_ANOTHER_TOKEN) {
          return CALCULATOR_INVALID_NUMBER;
        }
        
        const bool replacement_token_is_operand = calculator_token_is_operand(&replacement_token);
        
        // Treat 5(2 + 1) as 5 * (2 + 1)
        // Treat (1 + 6)(2 + 1) as (1 + 6) * (2 + 1)
        // Treat (2 + 1)7 as (2 + 1) * 7
        
        // By the way I think I just wrote my biggest if statement to date lol

        if (token.type != CALCULATOR_TOKEN_TYPE_NULL && (!calculator_compute_infix_token(calculator, &token) || (
          (calculator_token_is_opening_bracket(&replacement_token) && (
            calculator_token_is_operand(&token) ||
            calculator_token_is_closing_bracket(&token))
          ) || (replacement_token_is_operand && calculator_token_is_closing_bracket(&token))
        ) && !calculator_compute_infix_multiplication(calculator))) {
          return CALCULATOR_INVALID_NUMBER;
        }
        
        if (replacement_token_is_operand) {
          // 5 2 is not valid
          if (flag == CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND_AND_WHITESPACE) {
            return CALCULATOR_INVALID_NUMBER;
          }
  
          flag = CALCULATOR_TOKENIZATION_FLAG_GOT_OPERAND;
        } else {
          flag = CALCULATOR_TOKENIZATION_FLAG_NORMAL;
        }
        
        memcpy(&token, &replacement_token, sizeof(calculator_token_t));
      }
    }
  }

  if (token.type != CALCULATOR_TOKEN_TYPE_NULL && !calculator_compute_infix_token(calculator, &token)) {
    return CALCULATOR_INVALID_NUMBER;
  }

  calculator_token_t remainder_operator;
  
  while (calculator_stack_pop(&calculator->operators, &remainder_operator)) {
    if (!calculator_compute_postfix_token(calculator, &remainder_operator)) {
      return CALCULATOR_INVALID_NUMBER;
    }
  }

  const calculator_number_t* const result = calculator_stack_peek(&calculator->results);

  return result == NULL ? CALCULATOR_INVALID_NUMBER : *result;
}

void calculator_free(calculator_t* const calculator) {
  calculator_stack_free(&calculator->operators);
  calculator_stack_free(&calculator->results);
}