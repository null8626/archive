#include "args.h"

static inline bool _sus_args_is_flag(char * ptr) {
  return ptr[0] == '-' && ptr[1] != '\0';
}

static bool sus_parse_flag(sus_args_t * args, char * ptr) {
  if (ptr[0] == '-' && ptr[1] != '\0') {
    if (ptr[1] == '-' && ptr[2] != '\0') {
      args->name = ptr + 2;
      args->name_short = 0;
    } else {
      args->name_short = ptr[1];
      args->name = NULL;
    }

    args->found_flags = true;
    return true;
  }

  return false;
}

bool sus_args(sus_args_t * args) {
  bool is_flag;

sus_args_ignore:
  args->index++;

  // off-limits
  if (args->index >= args->argc) {
    return false;
  }

  is_flag = sus_parse_flag(args, args->argv[args->index]);

  // [...] <flags>
  if (!args->found_flags) {
    args->value = args->argv[args->index];
    return true;
  } else if (!is_flag) {
    // <flag> something somethingagain
    goto sus_args_ignore;
  }

  args->index++;

  // --args <end> || <arg> <nextarg>
  if (args->index >= args->argc || _sus_args_is_flag(args->argv[args->index])) {
    args->value = NULL;
    args->index--;
  } else {
    args->value = args->argv[args->index];
  }

  return true;
}