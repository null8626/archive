#ifndef _SUS_CODEGEN_H
#define _SUS_CODEGEN_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "token.h"

#ifndef SUS_CODEGEN_CHUNK_SIZE
#define SUS_CODEGEN_CHUNK_SIZE 50
#endif

typedef struct {
  char var[3];
  int64_t value;
  bool modify_value;
} sus_accumulator_queue_t;

#define SUS_CODEGEN_STOP_COMPILE_TIME   1
#define SUS_CODEGEN_MALLOC_USED         2
#define SUS_CODEGEN_CYAN_USED           4
#define SUS_CODEGEN_BLACK_USED          8

typedef struct {
  char * code;
  size_t len;
  size_t cap;

  sus_translated_token_t impostor;

  uint8_t accumulator1;
  uint8_t accumulator2;
  
  sus_accumulator_queue_t accumulator1_queue;
  sus_accumulator_queue_t accumulator2_queue;
  
  uint8_t flags;
} sus_codegen_t;

void sus_codegen_add_instruction(sus_codegen_t *, sus_token_data_t *);
void sus_codegen_finalize(sus_codegen_t *);

static inline void sus_accumulator_queue_new(sus_accumulator_queue_t * queue, char * var) {
  strcpy(queue->var, var);
  queue->value = 0;
  queue->modify_value = false;
}

static inline void sus_codegen_new(sus_codegen_t * codegen, char * previous_ptr, const size_t size) {
  codegen->code = previous_ptr;
  codegen->len = 0;
  codegen->cap = size;
  codegen->accumulator1 = 0;
  codegen->accumulator2 = 0;
  codegen->flags = 0;

  sus_accumulator_queue_new(&codegen->accumulator1_queue, "a1");
  sus_accumulator_queue_new(&codegen->accumulator2_queue, "a2");
}

static inline void sus_codegen_delete(sus_codegen_t * codegen) {
  free(codegen->code);
}

#endif