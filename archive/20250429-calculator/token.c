#include "token.h"

#include <math.h>

typedef uint32_t calculator_token_identifier_candidate_bits_t;

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
  { 's', 'q', 'r', 't', '\0' },
  { 'c', 'b', 'r', 't', '\0' },
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
  sqrt,
  cbrt,
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

calculator_function_t calculator_token_parse_function_call(const char** expression) {
  static const uint8_t calculator_function_count = sizeof(g_calculator_function_names) / sizeof(g_calculator_function_names[0]);

  calculator_token_identifier_candidate_bits_t candidate_bits = (calculator_token_identifier_candidate_bits_t)(1 << (sizeof(g_calculator_function_names) / sizeof(g_calculator_function_names[0]))) - 1;
  uint8_t character_search_index = 0;

  while (true) {
    const char c = *((*expression)++);
    uint8_t left = calculator_function_count;

    for (uint8_t i = 0; i < calculator_function_count; i++) {
      const char function_ident_char = g_calculator_function_names[i][character_search_index];
  
      if (function_ident_char != '\0' && (candidate_bits & (1 << i)) != 0) {
        if (function_ident_char == c) {
          if (character_search_index == (sizeof(g_calculator_function_names[0]) - 1) || g_calculator_function_names[i][character_search_index + 1] == '\0') {
            return g_calculator_functions[i];
          }

          continue;
        } else {
          candidate_bits ^= (1 << i);
        }
      }
  
      left--;
    }

    if (left == 0) {
      return NULL;
    }

    character_search_index++;
  }
}

calculator_number_t calculator_token_parse_number(const char** expression) {
  calculator_number_t result = 0;
  uint8_t decimal_length = 0;
  bool got_something = false;
  bool got_decimal = false;
  bool negative = false;

  for (char c = **expression;; (*expression)++) {
    switch (c) {
      case '.':
      case ',': {
        if (got_decimal) {
          return CALCULATOR_INVALID_NUMBER;
        }
        
        got_decimal = true;
        continue;
      }

      case '%': {
        if (got_something) {
          result /= 100.0;
          continue;
        }
        
        return CALCULATOR_INVALID_NUMBER;
      }

      default: {
        if (c >= '0' && c <= '9') {
          got_something = true;

          if (got_decimal) {
            if (++decimal_length > 16) {
              return CALCULATOR_INVALID_NUMBER;
            }

            result += (calculator_number_t)(c - '0') / (calculator_number_t)pow(10.0, (double)decimal_length);
          } else {
            result = (result * 10.0) + (calculator_number_t)(c - '0');
          }

          continue;
        }
      }
    }

    if (got_something) {
      if (negative) {
        result = -result;
      }

      return result;
    }

    return CALCULATOR_INVALID_NUMBER;
  }
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
