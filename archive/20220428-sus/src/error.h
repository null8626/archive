#ifndef _SUS_ERROR_H
#define _SUS_ERROR_H

#include "parser.h"
#include "token.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  char line_number[14];
  bool errored;
} sus_error_ctx_t;

void sus_error_ctx_new(sus_error_ctx_t *, sus_parser_t *);
void sus_error_token(
  sus_error_ctx_t * ctx,
  sus_parser_t * parser,
  sus_token_data_t * token,
  const char * fmt,
  ...
);

#ifdef _WIN32
#define sus_error_simple_literal(msg) sus_error_simple_size(msg, sizeof(msg) - 1)
void sus_error_simple_size(const char *, const uint8_t);
#else
#define sus_error_simple_literal      sus_error_simple_nofmt
void sus_error_simple_nofmt(const char *);
#endif

void sus_error_simple(const char *, ...);
void _sus_error_token_before(sus_parser_t *, sus_token_data_t *);
void _sus_error_token_after(sus_error_ctx_t *, sus_parser_t *, sus_token_data_t *);

#define sus_error_token_format(ctx, parser, token, fmt, ...) \
  _sus_error_token_before(parser, token);                            \
  fprintf(stderr, fmt "\n", __VA_ARGS__);                            \
  _sus_error_token_after(ctx, parser, token)

#define sus_error_token_literal(ctx, parser, token, msg)             \
  _sus_error_token_before(parser, token);                            \
  sus_error_simple_literal(msg);                                     \
  _sus_error_token_after(ctx, parser, token)

#endif