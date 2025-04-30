#include "token.h"

#include <string.h>
#include <ctype.h>
#include <math.h>

static inline bool calculator_token_is_numeric(const char c) {
  return c >= '0' && c <= '9';
}

static inline bool calculator_token_is_decimal_point(const char c) {
  return c == '.' || c == ',';
}

void calculator_token_new(calculator_token_t* const token) {
  memset(token, 0, sizeof(calculator_token_t));
}

calculator_token_feed_status_t calculator_token_feed(calculator_token_t* const token, const char c) {
  switch (token->type) {
    case CALCULATOR_TOKEN_TYPE_NULL: {
      switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case '(':
        case ')': {
          token->data = (calculator_token_data_t)c;
          token->type = CALCULATOR_TOKEN_TYPE_OPERATOR;
  
          return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
        }
  
        case '.':
        case ',': {
          // Decimal points without numbers are not valid.
          token->data = CALCULATOR_INVALID_NUMBER;
          token->type = CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND;
  
          return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
        }
  
        default: {
          if (calculator_token_is_numeric(c)) {
            token->data = (calculator_token_data_t)(c - '0');
            token->type = CALCULATOR_TOKEN_TYPE_OPERAND;

            return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
          }
        }
      }

      break;
    }

    case CALCULATOR_TOKEN_TYPE_OPERAND: {
      if (calculator_token_is_numeric(c)) {
        token->data = (token->data * 10) + (calculator_token_data_t)(c - '0');

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      } else if (calculator_token_is_decimal_point(c)) {
        // 5. is 5.0
        token->type = CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND;

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      } else if (c == '%') {
        token->data = token->data / 100.0;

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      }

      break;
    }

    case CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND: {
      if (calculator_token_is_numeric(c)) {
        // Past C's double limit
        if (++token->additional_data.decimal_length > 16) {
          return CALCULATOR_TOKEN_FEED_FATAL_ERROR;
        } else if (token->data == CALCULATOR_INVALID_NUMBER) {
          // .5 is 0.5
          token->data = 0;
        }
  
        token->data += (calculator_token_data_t)(c - '0') / (calculator_token_data_t)pow(10.0, (double)token->additional_data.decimal_length);
  
        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      } else if (calculator_token_is_decimal_point(c)) {
        // .. is not valid
        return CALCULATOR_TOKEN_FEED_FATAL_ERROR;
      } else if (c == '%') {
        token->data = token->data / 100.0;

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      }

      break;
    }

    case CALCULATOR_TOKEN_TYPE_OPERATOR: {
      if ((char)token->data == c && c == '-') {
        if (token->additional_data.unary) {
          // Treat -- as error
          return CALCULATOR_TOKEN_FEED_FATAL_ERROR;
        } else {
          // Treat -- as +
          token->data = (calculator_token_data_t)('+');

          return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
        }
      }
    }
  }

  return isspace(c) ? CALCULATOR_TOKEN_FEED_GOT_WHITESPACE : CALCULATOR_TOKEN_FEED_ANOTHER_TOKEN;
}

calculator_token_precedence_t calculator_token_precedence(const calculator_token_t* const token) {
  if (token->type == CALCULATOR_TOKEN_TYPE_OPERATOR) {
    switch ((char)token->data) {
      case '+':
      case '-': return 1;
      case '*':
      case '/': return 2;
      case '^': return 3;
    }
  }
  
  return -1;
}