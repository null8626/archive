#ifndef _SUS_TOKEN_H
#define _SUS_TOKEN_H

#include "parser.h"
#include <stdbool.h>
#include <stdint.h>

#define TOKEN_SUS              0U
#define TOKEN_VENTED           1U
#define TOKEN_SUSSY            2U
#define TOKEN_ELECTRICAL       3U
#define TOKEN_WHO              4U
#define TOKEN_WHERE            5U
#define TOKEN_RED              6U
#define TOKEN_BLUE             7U
#define TOKEN_PURPLE           8U
#define TOKEN_GREEN            9U
#define TOKEN_YELLOW           10U
#define TOKEN_CYAN             11U
#define TOKEN_BLACK            12U
#define TOKEN_WHITE            13U
#define TOKEN_BROWN            14U
#define TOKEN_LIME             15U
#define TOKEN_PINK             16U
#define TOKEN_ORANGE           17U
#define TOKEN_INVALID          0xFFU

typedef uint8_t  sus_translated_token_t;
typedef uint64_t sus_raw_token_t;

typedef struct {
  sus_line_t line;
  uint32_t index;
  sus_translated_token_t token;
  uint8_t packed_data;
} sus_token_data_t;

static inline bool sus_token_data_is_crewmate(sus_token_data_t * token) {
  return (bool)(token->packed_data >> 4);
}

static inline uint8_t sus_token_data_len(sus_token_data_t * token) {
  return token->packed_data & 15;
}

typedef struct {
  bool eof;
  sus_raw_token_t token;
  uint8_t len; // ranges 0-10
} sus_token_t;

sus_translated_token_t sus_token_translate(sus_raw_token_t, bool *);
int sus_token_next(sus_parser_t *, sus_token_t *);
const char * sus_token_to_string(const sus_translated_token_t);

#endif