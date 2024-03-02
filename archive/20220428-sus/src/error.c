#include "error.h"

#include <string.h>
#include <stdio.h>

#ifdef _WIN32

#include <windows.h>

static HANDLE hConsole = NULL;
static WORD wDefaultAttributes;

static void _sus_error_init(void) {
  hConsole = GetStdHandle(STD_ERROR_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  
  wDefaultAttributes = csbi.wAttributes;
}

#define sus_error_init()   \
  if (hConsole == NULL) {  \
    _sus_error_init();     \
  }

// #if defined(__GNUC__) && !defined(__clang__)
// #pragma GCC diagnostic ignored "-Wunused-function"
// #endif
// 
// // strlen but returns a 32-bit unsigned integer
// // instead of 64-bit in the std library
// static DWORD _sus_strlen(char * s) {
//   DWORD dwLen = 0;
//   
//   while (*s != 0) {
//     dwLen++;
//     s++;
//   }
//   
//   return dwLen;
// }
// 
// #if defined(__GNUC__) && !defined(__clang__)
// #pragma GCC diagnostic pop "-Wunused-function"
// #endif

static const char _sus_newline = '\n';

#define sus_error_red()                   SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY)
#define sus_error_reset()                 SetConsoleTextAttribute(hConsole, wDefaultAttributes)
#define sus_error_out_literal(lit)        WriteConsoleA(hConsole, lit, sizeof(lit) - 1, NULL, NULL)
#define sus_error_out_size(string, size)  WriteConsoleA(hConsole, string, size, NULL, NULL)
// #define sus_error_out(string)             WriteConsoleA(hConsole, string, _sus_strlen(string), NULL, NULL)
#define sus_error_newline()               WriteConsoleA(hConsole, &_sus_newline, 1, NULL, NULL)

void sus_error_simple_size(const char * message, const uint8_t size) {
  sus_error_init();
  
  sus_error_red();
  sus_error_out_literal("error: ");
  sus_error_reset();
  
  sus_error_out_size(message, size);
  sus_error_newline();
}

#else

static void sus_error_out_size(char * s, const uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    putchar(s[i]);
  }
}

#define sus_error_init()
#define sus_error_red()                   fprintf(stderr, "\x1B[1;31m")
#define sus_error_reset()                 fprintf(stderr, "\x1B[0m")
#define sus_error_out(string)             fprintf(stderr, "%s", string)
#define sus_error_out_literal             sus_error_out
#define sus_error_newline()               fputc('\n', stderr)

void sus_error_simple_nofmt(const char * message) {
  sus_error_init();
  
  sus_error_red();
  sus_error_out_literal("error: ");
  sus_error_reset();
  
  sus_error_out(message);
  sus_error_newline();
}

#endif

static inline uint64_t sus_safe_sub(const uint64_t a, const uint64_t b) {
  return a > b ? a - b : 0ULL;
}

static uint8_t sus_tiny_strlen(char * s) {
  uint8_t res = 0;

  while (*s != 0) {
    s++;
    res++;
  }

  return res;
}

#ifndef SUS_ERROR_CODE_SIZE
#define SUS_ERROR_CODE_SIZE 50
#endif

#ifndef SUS_ERROR_CODE_PREV_SIZE
#define SUS_ERROR_CODE_PREV_SIZE 30
#endif

void sus_error_ctx_new(sus_error_ctx_t * ctx, sus_parser_t * parser) {
  ctx->line_number[0] = ' ';
  ctx->line_number[11] = ' ';
  ctx->line_number[12] = '|';
  ctx->line_number[13] = ' ';
  ctx->errored = false;
}

static inline void sus_itoa(char * str, const uint32_t num) {
  sprintf(str, "%d", num);
}

static void sus_error_ctx_line(sus_error_ctx_t * ctx, const uint32_t line) {
  char num[11];
  sus_itoa(num, line);

  const uint8_t len = sus_tiny_strlen(num);

  memcpy(ctx->line_number + 1 + (10 - len), num, len);
  memset(ctx->line_number + 1, ' ', 10 - len);
}

void _sus_error_token_before(sus_parser_t * parser, sus_token_data_t * token) {
  sus_error_init();

  fprintf(stderr, "%s:%d:%d: ", parser->file_name, token->line.num, token->line.col);
}

static inline size_t sus_min(const size_t a, const size_t b) {
  return a > b ? b : a;
}

void _sus_error_token_after(sus_error_ctx_t * ctx, sus_parser_t * parser, sus_token_data_t * token) {
  ctx->errored = true;
  
  sus_error_ctx_line(ctx, token->line.num);
  sus_error_out_size(ctx->line_number, 14);

  size_t start = sus_safe_sub(token->index, SUS_ERROR_CODE_PREV_SIZE);
  size_t end   = sus_min(start + SUS_ERROR_CODE_SIZE, parser->index);

  size_t i;

  for (i = 0; i < token->index; i++) {
    if (parser->contents[i] == '\n') {
      start = i + 1;
    }
  }

  for (i = token->index + sus_token_data_len(token); i < end; i++) {
    if (parser->contents[i] == '\n') {
      end = i;
      break;
    }
  }

  for (i = start; i < token->index; i++) {
    fputc(parser->contents[i], stderr);
  }

  sus_error_red();

  size_t iter_end = token->index + sus_token_data_len(token);

  for (i = token->index; i < iter_end; i++) {
    fputc(parser->contents[i], stderr);
  }

  sus_error_reset();

  // i know the var name is inconsistent stfu
  for (; iter_end < end; iter_end++) {
    fputc(parser->contents[iter_end], stderr);
  }

  sus_error_newline();

  memset(ctx->line_number + 1, ' ', 10);
  sus_error_out_size(ctx->line_number, 14);

  for (i = start; i < token->index; i++) {
    fputc(' ', stderr);
  }

  sus_error_red();
  iter_end = token->index + sus_token_data_len(token);
  
  for (; i < iter_end; i++) {
    fputc('~', stderr);
  }

  sus_error_reset();
  sus_error_out_literal("\n\n");
}

void sus_error_simple(const char * fmt, ...) {
  sus_error_init();
  
  sus_error_red();
  sus_error_out_literal("error: ");
  sus_error_reset();
  
  va_list vl;
  va_start(vl, fmt);

  vfprintf(stderr, fmt, vl);

  va_end(vl);
  sus_error_newline();
}