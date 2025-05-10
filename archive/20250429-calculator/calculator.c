#include "calculator.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

static inline bool calculator_token_is_alphabetical(const char c) {
  return c >= 'a' && c <= 'z';
}

static bool calculator_validate_previous_operators(const calculator_token_t* const previous_token) {
  return previous_token->type == CALCULATOR_TOKEN_TYPE_OPERAND || previous_token->type == CALCULATOR_TOKEN_TYPE_CLOSING_BRACKET;
}

static bool calculator_process_postfix_token(calculator_state_t* const state, const calculator_token_t* const token) {
  if (token->type == CALCULATOR_TOKEN_TYPE_OPERAND) {
    return calculator_stack_push(&state->results, &token->data);
  }

  calculator_number_t a, b;

  if (token->type != CALCULATOR_TOKEN_TYPE_OPERATOR || !calculator_stack_pop(&state->results, &b) || !calculator_stack_pop(&state->results, &a)) {
    return false;
  }

  calculator_number_t result;

  switch (token->data.character) {
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
    }
  }

  return calculator_stack_push(&state->results, &result);
}

static bool calculator_process_infix_token(calculator_state_t* const state, const calculator_token_t* const token) {
  memcpy(&state->previous_token, token, sizeof(calculator_token_t));
  
  switch (token->type) {
    case CALCULATOR_TOKEN_TYPE_OPERAND: {
      return calculator_process_postfix_token(state, token);
    }

    case CALCULATOR_TOKEN_TYPE_OPERATOR: {
      const calculator_token_precedence_t token_precedence = calculator_token_precedence(token);
      calculator_token_t* operator;

      while ((operator = (calculator_token_t*)calculator_stack_peek(&state->operators)) != NULL && operator->type != CALCULATOR_TOKEN_TYPE_OPENING_BRACKET && token_precedence <= calculator_token_precedence(operator)) {
        if (!calculator_process_postfix_token(state, operator)) {
          return false;
        }

        calculator_stack_pop(&state->operators, NULL);
      }

      return calculator_stack_push(&state->operators, token);
    }

    case CALCULATOR_TOKEN_TYPE_OPENING_BRACKET: {
      return calculator_stack_push(&state->operators, token);
    }

    case CALCULATOR_TOKEN_TYPE_CLOSING_BRACKET: {
      calculator_token_t operator;

      memset(&operator, 0, sizeof(calculator_token_t));

      while (calculator_stack_pop(&state->operators, &operator) && operator.type != CALCULATOR_TOKEN_TYPE_OPENING_BRACKET) {
        if (!calculator_process_postfix_token(state, &operator)) {
          return false;
        }
      }

      // 2 + 5) is not valid
      if (operator.type != CALCULATOR_TOKEN_TYPE_OPENING_BRACKET) {
        return false;
      }

      calculator_number_t* const result = (calculator_number_t*)calculator_stack_peek(&state->results);

      if (result == NULL) {
        return false;
      } else if (operator.data.value.function != NULL) {
        // Evaluate identifier functions
        const calculator_number_t function_result = operator.data.value.function(*result);

        if (isnan(function_result) || function_result == CALCULATOR_INVALID_NUMBER) {
          return false;
        }

        *result = function_result;
      }
      
      // If bracket is negative, make it negative
      if (operator.data.value.negative) {
        *result = -*result;
      }
    }
  }

  return true;
}

static bool calculator_embed_multiplication_operator(calculator_state_t* const state) {
  calculator_token_t token;

  memset(&token, 0, sizeof(calculator_token_t));

  token.type = CALCULATOR_TOKEN_TYPE_OPERATOR;
  token.data.character = '*';

  return calculator_process_infix_token(state, &token);
}

bool calculator_new(calculator_state_t* const state) {
  memset(state, 0, sizeof(calculator_state_t));

  return calculator_stack_new(&state->results, sizeof(calculator_number_t)) && calculator_stack_new(&state->operators, sizeof(calculator_token_t));
}

void calculator_free(calculator_state_t* const state) {
  calculator_stack_free(&state->operators);
  calculator_stack_free(&state->results);
}

calculator_number_t calculator_compute(calculator_state_t* state, const char* expression) {
  calculator_number_t result = CALCULATOR_INVALID_NUMBER;
  bool dynamic_state;

  if (dynamic_state = state == NULL) {
    if ((state = malloc(sizeof(calculator_state_t))) == NULL || !calculator_new(state)) {
      goto CALCULATOR_COMPUTE_END;
    }
  } else {
    memset(&state->previous_token, 0, sizeof(calculator_token_t));

    calculator_stack_clear(&state->operators);
    calculator_stack_clear(&state->results);
  }

  calculator_token_t token;
  bool unary_negative = false;
  bool unary = true;
  char c = *expression;

  while (true) {
    while (isspace(c = *expression)) {
      expression++;
    }

    memset(&token, 0, sizeof(calculator_token_t));

    switch (c) {
      case '\0': {
        if (calculator_validate_previous_operators(&state->previous_token)) {
          goto CALCULATOR_COMPUTE_RESULT;
        }

        goto CALCULATOR_COMPUTE_END;
      }

      case '-': {
        if (unary) {
          unary_negative = true;
          break;
        }
      }

      case '+':
      case '*':
      case '/':
      case '^': {
        token.type = CALCULATOR_TOKEN_TYPE_OPERATOR;
        token.data.character = c;

        if (!calculator_validate_previous_operators(&state->previous_token) || !calculator_process_infix_token(state, &token)) {
          goto CALCULATOR_COMPUTE_END;
        }

        expression++;
        unary = true;

        continue;
      }

      case '(': {
        if (unary_negative) {
          unary_negative = false;
          token.data.value.negative = true;
        }

        token.type = CALCULATOR_TOKEN_TYPE_OPENING_BRACKET;

        if (((state->previous_token.type == CALCULATOR_TOKEN_TYPE_OPERAND || state->previous_token.type == CALCULATOR_TOKEN_TYPE_CLOSING_BRACKET) && !calculator_embed_multiplication_operator(state)) || !calculator_process_infix_token(state, &token)) {
          goto CALCULATOR_COMPUTE_END;
        }

        expression++;
        unary = true;

        continue;
      }

      case ')': {
        token.type = CALCULATOR_TOKEN_TYPE_CLOSING_BRACKET;

        if (!calculator_validate_previous_operators(&state->previous_token) || !calculator_process_infix_token(state, &token)) {
          goto CALCULATOR_COMPUTE_END;
        }

        expression++;
        unary = false;

        continue;
      }
    }

    calculator_number_t number = calculator_token_parse_number(&expression);

    if (number != CALCULATOR_INVALID_NUMBER) {
      if (unary_negative) {
        unary_negative = false;
        number = -number;
      }

      token.type = CALCULATOR_TOKEN_TYPE_OPERAND;
      token.data.number = number;

      if ((state->previous_token.type == CALCULATOR_TOKEN_TYPE_CLOSING_BRACKET && !calculator_embed_multiplication_operator(state)) || state->previous_token.type == CALCULATOR_TOKEN_TYPE_OPERAND || !calculator_process_infix_token(state, &token)) {
        goto CALCULATOR_COMPUTE_END;
      }

      unary = false;

      continue;
    }

    const calculator_function_t function = calculator_token_parse_function_call(&expression);

    if (function != NULL) {
      if (*(expression++) == '(') {
        if (unary_negative) {
          unary_negative = false;
          token.data.value.negative = true;
        }
        
        token.type = CALCULATOR_TOKEN_TYPE_OPENING_BRACKET;
        token.data.value.function = function;
        
        if (((state->previous_token.type == CALCULATOR_TOKEN_TYPE_OPERAND || state->previous_token.type == CALCULATOR_TOKEN_TYPE_CLOSING_BRACKET) && !calculator_embed_multiplication_operator(state)) || !calculator_process_infix_token(state, &token)) {
          goto CALCULATOR_COMPUTE_END;
        }
        
        unary = true;
        
        continue;
      }

      goto CALCULATOR_COMPUTE_END;
    }

    if (!unary_negative) {
      goto CALCULATOR_COMPUTE_END;
    }
  }

CALCULATOR_COMPUTE_RESULT:
  if (token.type != CALCULATOR_TOKEN_TYPE_NULL && !calculator_process_infix_token(state, &token)) {
    goto CALCULATOR_COMPUTE_END;
  }
  
  calculator_token_t remainder_operator;
  
  while (calculator_stack_pop(&state->operators, &remainder_operator)) {
    if (!calculator_process_postfix_token(state, &remainder_operator)) {
      goto CALCULATOR_COMPUTE_END;
    }
  }
  
  const calculator_number_t* const final_result = (calculator_number_t*)calculator_stack_peek(&state->results);

  if (final_result != NULL) {
    result = *final_result;
  }

CALCULATOR_COMPUTE_END:
  if (dynamic_state && state != NULL) {
    calculator_free(state);
    free(state);
  }

  return result;
}
