#include "token.h"

const char * sus_token_to_string(const sus_translated_token_t token) {
  // we don't need the commented ones.
  
  switch (token) {
    // case 0U:
    //   return "SUS";
    case 1U:
      return "VENTED";
    case 2U:
      return "SUSSY";
    case 3U:
      return "ELECTRICAL";
    case 4U:
      return "WHO";
    case 5U:
      return "WHERE";
    // case 6U:
    //   return "RED";
    // case 7U:
    //   return "BLUE";
    // case 8U:
    //   return "PURPLE";
    // case 9U:
    //   return "GREEN";
    // case 10U:
    //   return "YELLOW";
    // case 11U:
    //   return "CYAN";
    // case 12U:
    //   return "BLACK";
    // case 13U:
    //   return "WHITE";
    // case 14U:
    //   return "BROWN";
    // case 15U:
    //   return "LIME";
    // case 16U:
    //   return "PINK";
    // case 17U:
    //   return "ORANGE";
    default:
      return NULL; // unreachable
  }
}

sus_translated_token_t sus_token_translate(sus_raw_token_t raw, bool * is_crewmate) {
  *is_crewmate = false;
  
  switch (raw) {
    case 0x4A92ULL:
      return 0U;
    case 0x2A46CC83ULL:
      return 1U;
    case 0x12A4A58ULL:
      return 2U;
    case 0x28B20A714080BULL:
      return 3U;
    case 0x58EEULL:
      return 4U;
    case 0x1639224ULL:
      return 5U;
  }

  *is_crewmate = true;

  switch (raw) {
    case 0x4483ULL:
      return 6U;
    case 0x8AE84ULL:
      return 7U;
    case 0x3F48BD64ULL:
      return 8U;
    case 0x168908DULL:
      return 9U;
    case 0x3045ADD6ULL:
      return 10U;
    case 0x9600DULL:
      return 11U;
    case 0x115804AULL:
      return 12U;
    case 0x163A264ULL:
      return 13U;
    case 0x118BACDULL:
      return 14U;
    case 0xDA184ULL:
      return 15U;
    case 0xFA1AAULL:
      return 16U;
    case 0x3D1034C4ULL:
      return 17U;
    default:
      return 0xFFU;
  }
}

int sus_token_next(sus_parser_t * parser, sus_token_t * token) {
  token->token = 1;
  token->len = 0;
  token->eof = false;
  int c;
  
token_advance_char:
  c = sus_parser_advance(parser);

  if (token->len == 0) {
    if (c == EOF) {
      token->eof = true;
      return 0;
    } else if (c < 'A' || c > 'Y') {
      sus_line_advance(&parser->line, c);
      goto token_advance_char;
    }
  }
  
  token->len++;
  
  if (token->len <= 11) {
    if (c >= 'A' && c <= 'Y') {
      token->token = (token->token << (token->token == 1 ? 4 : 5)) | (c - 'A');
      sus_line_advance(&parser->line, c);
      goto token_advance_char;
    } else if ((token->eof = (c == EOF)) || c == '\n' || c == ' ' || c == '?') {
      token->len--;
      return c;
    }
  }
  
  return 0;
}