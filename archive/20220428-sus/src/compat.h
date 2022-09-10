#ifndef _SUS_COMPAT_H
#define _SUS_COMPAT_H

#define _SUS_MACRO_STRINGIFY(x)         #x
#define SUS_MACRO_STRINGIFY(macro)      _SUS_MACRO_STRINGIFY(macro)

#ifndef SUS_TCC_LIB_FILE
#error "Missing required macros found."
#endif

#ifdef _WIN32

#include <windows.h>
#include <string.h>
#include <stdlib.h>

typedef char sus_tcc_lib_dir_t[350];

#define SUS_PATH_SEP                    '\\'
#define SUS_OUTPUT_EXE                  _output_file
#define sus_tcc_lib_dir_init()          { 0 }
#define sus_tcc_lib_dir_new(buf)        GetModuleFileNameA(NULL, buf, sizeof(buf))

static void sus_replace_filename(char * str) {
  char * ptr_to_slash;
  
  while (*str != 0) {
    if (*str == SUS_PATH_SEP) {
      ptr_to_slash = str;
    }

    str++;
  }
  
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

  strcpy(ptr_to_slash + 1, "*.a");

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop "-Wmaybe-uninitialized"
#endif
}

typedef WIN32_FIND_DATAA sus_dir_t;

#define sus_dir_init()                  { 0 }
#define sus_dir_from(name, path)        sus_replace_filename(path); \
                                        HANDLE __handle_##name = FindFirstFileA(path, &name)
#define sus_dir_defer(path)             memcpy(strstr(path, "\\*.a"), "", 1)
#define sus_dir_failed_init(name)       (__handle_##name == INVALID_HANDLE_VALUE)
#define sus_dir_iter(name, iter_block)  do iter_block while (FindNextFileA(__handle_##name, &name) != 0)
#define sus_dir_file_is(name, second)   (strcmp(name.cFileName, second) == 0)
#define sus_dir_delete(name)            FindClose(__handle_##name)

#else

#if !defined(SUS_TCC_RUNTIME_PATH) || !defined(SUS_TCC_RUNTIME_PATH_LEN)
#error "Missing required macros found."
#endif

#include <dirent.h>

typedef char sus_tcc_lib_dir_t[SUS_TCC_RUNTIME_PATH_LEN + 1];

#define SUS_PATH_SEP                    '/'
#define SUS_OUTPUT_EXE                  output_file
#define sus_tcc_lib_dir_init()          SUS_MACRO_STRINGIFY(SUS_TCC_RUNTIME_PATH)
#define sus_tcc_lib_dir_new(_)

typedef DIR * sus_dir_t;

#define sus_dir_init()                  NULL
#define sus_dir_from(name, path)        struct dirent * __file_##name; \
                                        name = opendir(path)
#define sus_dir_failed_init(name)       (name == NULL)
#define sus_dir_iter(name, iter_block)  while ((__file_##name = readdir(name)) != NULL) iter_block
#define sus_dir_file_is(name, second)   (strcmp(__file_##name->d_name, second) == 0)
#define sus_dir_delete                  closedir
#define sus_dir_defer(_)

#endif

#endif
