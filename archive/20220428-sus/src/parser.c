#include "parser.h"

static const uint16_t whitespace[24] = { 
  9,    11,   12,   13,   32,   133,  160,
  5760, 8192, 8193, 8194, 8195, 8196, 8197, 8198,
  8199, 8200, 8201, 8202, 8232, 8233, 8239, 8287,
  12288
};

void sus_line_advance(sus_line_t * line, const int c) {
  if (c == '\n') {
    line->num++;
    line->col = 1;
  } else {
    line->col++;
  }
}

int sus_parser_advance(sus_parser_t * parser) {
  const int res = fgetc(parser->file);

  if (res != EOF) {
    parser->contents[parser->index] = res;
  }

  parser->index++;

  for (uint8_t i = 0; i < 24; i++) {
    if (whitespace[i] == res) {
      return ' ';
    }
  }
  
  return res;
}