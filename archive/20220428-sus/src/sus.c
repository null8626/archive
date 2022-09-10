#include "compiler.h"
#include "compat.h"
#include "parser.h"
#include "error.h"
#include "args.h"

#include <stdbool.h>
#include <string.h>

static const char sussy_help_message[] = 
  "usage: sus <file.sus> [--help|-h] [-o|--out <c-file|exe>]\n\n"
  "  help      Sends this sussy help message.\n"
  "  out       Changes the sussy output as a C file or\n"
  "            as an executable with a different name.";

#define sus_ends_with(a, b) _sus_ends_with(a, b, sizeof(b) - 1)

static bool _sus_ends_with(char * str, char * with, const uint8_t with_len) {
  if (str == NULL) {
    return false; // for safety reasons
  }
  
  char * last_chars = NULL;
  uint16_t length = 0;
  
  while (*str != 0) {
    length++;
    str++;
    
    if (length == with_len) {
      last_chars = str - with_len;
    } else if (length > with_len) {
      last_chars++;
    }
  }
  
  return last_chars != NULL && length != with_len && !strcmp(last_chars, with);
}

static void sus_replace_sus_with_exe(char * str) {
  char * ptr;
  uint16_t length = 0;

  while (*str != 0) {
    length++;
    str++;
    
    if (length == 3) {
      ptr = str - 3;
    } else if (length > 3) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
      ptr++;
    }
  }

#ifdef _WIN32
  memcpy(ptr, "exe", 3);
#else
  ptr--;
  *ptr = 0;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop "-Wmaybe-uninitialized"
#endif
#endif
}

static bool sus_validate_library_file(char * rt_dir) {
  sus_dir_t dir = sus_dir_init();
  sus_dir_from(dir, rt_dir);

  if (sus_dir_failed_init(dir)) {
    sus_dir_defer(rt_dir);
    return false;
  }
  
  bool res = false;
  
  sus_dir_iter(dir, {
    if (sus_dir_file_is(dir, SUS_MACRO_STRINGIFY(SUS_TCC_LIB_FILE))) {
      res = true;
      break;
    }
  });
  
  sus_dir_delete(dir);
  sus_dir_defer(rt_dir);
  return res;
}

int main(int argc, char ** argv) {
  sus_tcc_lib_dir_t tcclibdir = sus_tcc_lib_dir_init();
  sus_tcc_lib_dir_new(tcclibdir);

  if (!sus_validate_library_file(tcclibdir)) {
    sus_error_simple("Missing required runtime file '%s' expected to be located in '%s'.", SUS_MACRO_STRINGIFY(SUS_TCC_LIB_FILE), tcclibdir);
    return 1;
  }

  char * input_file = NULL;
  char * output_file = NULL;

  sus_args_t args = sus_args_new(argc, argv);

  while (sus_args(&args)) {
    if (sus_args_is_flag(&args)) {
      if (sus_args_check(&args, 'o', "out")) {
        output_file = args.value;
        continue;
      } else if (sus_args_check(&args, 'h', "help")) {
        input_file = NULL; // discard input file
        break;
      }
    } else if (sus_ends_with(args.value, ".sus")) {
      input_file = args.value;
      output_file = args.value;
    }
  }

  if (input_file == NULL) {
    puts(sussy_help_message);
    return 0;
  }

  const bool output_c_file = sus_ends_with(output_file, ".c");

  sus_parser_t parser;
  sus_parser_new(&parser, argv[1]);
  
  if (parser.file == NULL) {
    sus_error_simple("file '%s' does not exist. it's probably already ejected.", parser.file_name);
    return 1;
  }
  
  sus_compiler_t compiler = sus_compiler_new(&parser);
  if (!sus_compiler_compile(&compiler)) {
    return 1;
  }

  if (output_c_file) {
    FILE * cf = fopen(output_file, "w");
    fputs("#include <stdlib.h>\n"
          "#include <stdio.h>\n"
          "#include <time.h>\n\n", cf);
    
    fputs(compiler.codegen.code, cf);
    fclose(cf);   
  } else {
#ifdef _WIN32
    char * _output_file = output_file;
#endif

    if (input_file == output_file) {
      sus_replace_sus_with_exe(output_file);
    }

#ifdef _WIN32
    else if (!sus_ends_with(_output_file, ".exe")) {
      const uint16_t output_file_len = strlen(output_file);
      
      _output_file = (char *)malloc(output_file_len + 5);
      strcpy(_output_file, output_file);
      memcpy(_output_file + output_file_len, ".exe", 4);
    }
#endif

    sus_compiler_save(&compiler, SUS_OUTPUT_EXE, tcclibdir);

#ifdef _WIN32
    // different address - must be from a malloc()
    if (input_file != _output_file) {
      free(_output_file);
    }
#endif
  }

  sus_compiler_delete(&compiler);
  return 0;
}
