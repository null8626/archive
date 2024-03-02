#ifndef _SUS_ARGS_H
#define _SUS_ARGS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  const uint32_t argc;
  char ** argv;
  uint32_t index;

  bool found_flags;

  char name_short;
  char * name;
  char * value;
} sus_args_t;

#define sus_args_new(argc, argv) (sus_args_t){argc,argv,0,false,0,NULL}

bool sus_args(sus_args_t *);

static inline bool sus_args_is_flag(sus_args_t * args) {
  return args->name_short != 0 || args->name != NULL;
}

static inline bool sus_args_check(sus_args_t * args, const char a, const char * b) {
  return args->name_short == a || (args->name != NULL && !strcmp(args->name, b));
}

#endif