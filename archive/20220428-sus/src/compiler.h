#ifndef _SUS_COMPILER_H
#define _SUS_COMPILER_H

#include <stdbool.h>
#include "codegen.h"
#include "parser.h"

typedef struct {
  sus_parser_t * parser;
  sus_codegen_t codegen;
} sus_compiler_t;

#define sus_compiler_new(parser) (sus_compiler_t){parser}

bool sus_compiler_compile(sus_compiler_t *);
void sus_compiler_save(sus_compiler_t *, char *, char *);

static inline void sus_compiler_delete(sus_compiler_t * compiler) {
  sus_codegen_delete(&compiler->codegen);
}

#endif