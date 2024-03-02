#ifndef _SUS_PARSER_H
#define _SUS_PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint32_t num;
  uint32_t col;
} sus_line_t;

typedef struct {
  sus_line_t line;
  char * file_name;
  char * contents;
  size_t index;
  FILE * file;
} sus_parser_t;

static inline void sus_parser_new(sus_parser_t * parser, char * file_name) {
  parser->line.num = 1;
  parser->line.col = 1;
  parser->index = 0;
  parser->file_name = file_name;
  parser->file = fopen(file_name, "r");

  if (parser->file != NULL) {
    fseek(parser->file, 0L, SEEK_END);
    parser->contents = (char *)malloc((size_t)ftell(parser->file));
    fseek(parser->file, 0L, SEEK_SET);
  }
}

int sus_parser_advance(sus_parser_t *);
void sus_line_advance(sus_line_t *, const int);

static inline void sus_parser_delete(sus_parser_t * parser) {
  fclose(parser->file);
  
  // ownership of parser->contents is passed to codegen,
  // we don't need to free this.

  // free(parser->contents);
}

#endif