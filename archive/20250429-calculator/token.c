#include "token.h"

#include <string.h>
#include <ctype.h>
#include <math.h>

#define CALCULATOR_TOKEN_CHECK_IDENTIFIER_NO_MATCHES -2
#define CALCULATOR_TOKEN_CHECK_IDENTIFIER_REDUCED_MATCHES -1

static calculator_number_t calculator_abs(const calculator_number_t x) {
  return x < 0 ? -x : x;
}

static calculator_number_t calculator_rad(const calculator_number_t x) {
  return x * 0.0174532925199432;
}

static calculator_number_t calculator_csc(const calculator_number_t x) {
  const calculator_number_t csc_result = sin(x);

  return csc_result == 0 ? CALCULATOR_INVALID_NUMBER : 1.0 / csc_result;
}

static calculator_number_t calculator_sec(const calculator_number_t x) {
  const calculator_number_t sec_result = cos(x);

  return sec_result == 0 ? CALCULATOR_INVALID_NUMBER : 1.0 / sec_result;
}

static calculator_number_t calculator_cot(const calculator_number_t x) {
  const calculator_number_t cot_result = tan(x);

  return cot_result == 0 ? CALCULATOR_INVALID_NUMBER : 1.0 / cot_result;
}

static const char g_calculator_function_names[][5] = {
  { 'a', 'b', 's', '\0', '\0' },
  { 'r', 'a', 'd', '\0', '\0' },
  { 's', 'i', 'n', '\0', '\0' },
  { 'c', 'o', 's', '\0', '\0' },
  { 't', 'a', 'n', '\0', '\0' },
  { 'a', 's', 'i', 'n', '\0' },
  { 'a', 'c', 'o', 's', '\0' },
  { 'a', 't', 'a', 'n', '\0' },
  { 'c', 's', 'c', '\0', '\0' },
  { 's', 'e', 'c', '\0', '\0' },
  { 'c', 'o', 't', '\0', '\0' },
  { 'f', 'l', 'o', 'o', 'r' },
  { 't', 'r', 'u', 'n', 'c' },
  { 'c', 'e', 'i', 'l', '\0' },
  { 'r', 'o', 'u', 'n', 'd' },
  { 'l', 'o', 'g', '2', '\0' },
  { 'l', 'o', 'g', '1', '0' }
};

static const calculator_function_t g_calculator_functions[] = {
  calculator_abs,
  calculator_rad,
  sin,
  cos,
  tan,
  asin,
  acos,
  atan,
  calculator_csc,
  calculator_sec,
  calculator_cot,
  floor,
  trunc,
  ceil,
  round,
  log2,
  log10,
};

static inline bool calculator_token_is_alphabetical(const char c) {
  return c >= 'a' && c <= 'z';
}

static inline bool calculator_token_is_numeric(const char c) {
  return c >= '0' && c <= '9';
}

static inline bool calculator_token_is_decimal_point(const char c) {
  return c == '.' || c == ',';
}

static int8_t calculator_token_check_identifier(calculator_token_t* const token, const char c) {
  if (token->additional_data.identifier.character_search_index == sizeof(g_calculator_function_names[0])) {
    return CALCULATOR_TOKEN_CHECK_IDENTIFIER_NO_MATCHES;
  }

  uint8_t left = sizeof(g_calculator_function_names) / sizeof(g_calculator_function_names[0]);
  uint32_t last_found_index;

  for (uint32_t i = 0; i < sizeof(g_calculator_function_names) / sizeof(g_calculator_function_names[0]); i++) {
    const char function_ident_char = g_calculator_function_names[i][token->additional_data.identifier.character_search_index];

    if (function_ident_char != '\0' && (token->additional_data.identifier.candidate_bits & (1 << i)) != 0) {
      if (function_ident_char == c) {
        last_found_index = i;
        continue;
      } else {
        token->additional_data.identifier.candidate_bits ^= (1 << i);
      }
    }

    left--;
  }

  token->additional_data.identifier.character_search_index++;

  if (left == 0) {
    return CALCULATOR_TOKEN_CHECK_IDENTIFIER_NO_MATCHES;
  } else if (left == 1) {
    return (int8_t)last_found_index;
  }

  return CALCULATOR_TOKEN_CHECK_IDENTIFIER_REDUCED_MATCHES;
}

void calculator_token_new(calculator_token_t* const token) {
  memset(token, 0, sizeof(calculator_token_t));
}

calculator_function_t calculator_token_get_function(const uint8_t identifier_index) {
  return g_calculator_functions[identifier_index];
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
          token->data.character = c;
          token->type = CALCULATOR_TOKEN_TYPE_OPERATOR;
          token->additional_data.operator.identifier_index = CALCULATOR_INVALID_IDENTIFIER_INDEX;

          return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
        }
  
        case '.':
        case ',': {
          // Decimal points without numbers are not valid.
          token->data.number = CALCULATOR_INVALID_NUMBER;
          token->type = CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND;
  
          return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
        }
  
        default: {
          if (calculator_token_is_numeric(c)) {
            token->data.number = (calculator_number_t)(c - '0');
            token->type = CALCULATOR_TOKEN_TYPE_OPERAND;

            return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
          } else if (calculator_token_is_alphabetical(c)) {
            token->data.identifier_index = CALCULATOR_INVALID_IDENTIFIER_INDEX;
            token->type = CALCULATOR_TOKEN_TYPE_IDENTIFIER;
            token->additional_data.identifier.candidate_bits = (uint32_t)(1 << (sizeof(g_calculator_function_names) / sizeof(g_calculator_function_names[0]))) - 1;

            return calculator_token_check_identifier(token, c) == CALCULATOR_TOKEN_CHECK_IDENTIFIER_NO_MATCHES ? CALCULATOR_TOKEN_FEED_FATAL_ERROR : CALCULATOR_TOKEN_FEED_SUCCESSFUL;
          }
        }
      }

      break;
    }

    case CALCULATOR_TOKEN_TYPE_OPERAND: {
      if (calculator_token_is_numeric(c)) {
        token->data.number = (token->data.number * 10.0) + (calculator_number_t)(c - '0');

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      } else if (calculator_token_is_decimal_point(c)) {
        // 5. is 5.0
        token->type = CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND;

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      } else if (c == '%') {
        token->data.number = token->data.number / 100.0;

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      }

      break;
    }

    case CALCULATOR_TOKEN_TYPE_DECIMAL_OPERAND: {
      if (calculator_token_is_numeric(c)) {
        // Past C's double limit
        if (++token->additional_data.decimal_length > 16) {
          return CALCULATOR_TOKEN_FEED_FATAL_ERROR;
        } else if (token->data.number == CALCULATOR_INVALID_NUMBER) {
          // .5 is 0.5
          token->data.number = 0;
        }
  
        token->data.number += (calculator_number_t)(c - '0') / (calculator_number_t)pow(10.0, (double)token->additional_data.decimal_length);
  
        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      } else if (calculator_token_is_decimal_point(c)) {
        // .. is not valid
        return CALCULATOR_TOKEN_FEED_FATAL_ERROR;
      } else if (c == '%') {
        token->data.number = token->data.number / 100.0;

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      }

      break;
    }

    case CALCULATOR_TOKEN_TYPE_OPERATOR: {
      if (token->data.character == c && c == '-') {
        if (token->additional_data.operator.subtype == CALCULATOR_TOKEN_OPERATOR_SUBTYPE_UNARY_NEGATIVE) {
          // Treat -- (unary) as error
          return CALCULATOR_TOKEN_FEED_FATAL_ERROR;
        } else {
          // Treat -- (binary) as +
          token->data.character = '+';

          return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
        }
      }

      break;
    }

    case CALCULATOR_TOKEN_TYPE_IDENTIFIER: {
      const calculator_token_identifier_index_t identifier_index = token->data.identifier_index;

      // Start of sin(), cos(), etc
      if (identifier_index != CALCULATOR_INVALID_IDENTIFIER_INDEX && c == '(') {
        // Change token to bracket with an identifier index
        calculator_token_new(token);

        token->data.character = c;
        token->type = CALCULATOR_TOKEN_TYPE_OPERATOR;
        token->additional_data.operator.identifier_index = identifier_index;

        return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
      }

      const int8_t identifier_found_index = calculator_token_check_identifier(token, c);

      // Identifier does not exist
      if (identifier_found_index == CALCULATOR_TOKEN_CHECK_IDENTIFIER_NO_MATCHES) {
        return CALCULATOR_TOKEN_FEED_FATAL_ERROR;
      } else if (identifier_found_index != CALCULATOR_TOKEN_CHECK_IDENTIFIER_REDUCED_MATCHES) {
        token->data.identifier_index = (calculator_token_identifier_index_t)identifier_found_index;
      }

      return CALCULATOR_TOKEN_FEED_SUCCESSFUL;
    }
  }

  return isspace(c) ? CALCULATOR_TOKEN_FEED_GOT_WHITESPACE : CALCULATOR_TOKEN_FEED_TRY_AGAIN;
}

calculator_token_precedence_t calculator_token_precedence(const calculator_token_t* const token) {
  if (token->type == CALCULATOR_TOKEN_TYPE_OPERATOR) {
    switch (token->data.character) {
      case '+':
      case '-': return 1;
      case '*':
      case '/': return 2;
      case '^': return 3;
    }
  }
  
  return -1;
}
