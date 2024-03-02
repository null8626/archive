#include "codegen.h"

#ifndef SUS_STACK_CHUNK_SIZE
#define SUS_STACK_CHUNK_SIZE "15"
#endif

#if UINTPTR_MAX == 0xffffffffffffffff
#define SUS_SIZE_T_STR "unsigned long long"
#else
#define SUS_SIZE_T_STR "unsigned long"
#endif

#define SUS_CODEGEN_ADD_TO_STACK        \
  "static void as(unsigned char d){"    \
  "if(stl>=stc){"                       \
  "stc+=" SUS_STACK_CHUNK_SIZE ";"      \
  "st=realloc(st,stc);"                 \
  "}"                                   \
  "st[stl]=d;"                          \
  "stl++;"                              \
  "}"                                   
                                        
#define SUS_CODEGEN_ADD_TO_STACK_SIZE   (sizeof(SUS_CODEGEN_ADD_TO_STACK) - 1)
                                        
#define SUS_CODEGEN_CYAN_FUNCTION       \
  "static void cy(){"                   \
  "srand(time(0));"                     \
  "unsigned short i=rand()%%(a1+1);"    \
  "for(unsigned short j=0;j<i;j++)"     \
  "if(stl>0){stl--;}"                   \
  "}"                                   
                                        
#define SUS_CODEGEN_CYAN_FUNCTION_SIZE  (sizeof(SUS_CODEGEN_CYAN_FUNCTION) - 2)
                                        
#define SUS_CODEGEN_BLACK_FUNCTION      \
  "static void b(){"                    \
  "char bs[4];"                         \
  "sprintf(bs,\"%%d\",st[stl-1]);"      \
  "char*bsc=&bsc[0];"                   \
  "while(*bsc!=0){"                     \
  "putchar(*bsc);"                      \
  "bsc++;"                              \
  "}"                                   \
  "}"

#define SUS_CODEGEN_BLACK_FUNCTION_SIZE (sizeof(SUS_CODEGEN_BLACK_FUNCTION) - 2)

static const char codegen_initial_code_fmt[] = 
  "static unsigned char a1=%d;"
  "static unsigned char a2=%d;"
  "static unsigned char*pl;"
  "static unsigned char*st=0;"
  "static " SUS_SIZE_T_STR " stl=0;"
  "static " SUS_SIZE_T_STR " stc=" SUS_STACK_CHUNK_SIZE ";"
  SUS_CODEGEN_ADD_TO_STACK
  SUS_CODEGEN_CYAN_FUNCTION
  SUS_CODEGEN_BLACK_FUNCTION
  "int main(){";

static void sus_codegen_write_init(sus_codegen_t * codegen) {
  size_t size = snprintf(NULL, 0, codegen_initial_code_fmt, codegen->accumulator1, codegen->accumulator2);

  if (size > codegen->cap) {
    codegen->cap = size + SUS_CODEGEN_CHUNK_SIZE;
    codegen->code = (char *)realloc(codegen->code, codegen->cap);
  }

  snprintf(codegen->code, codegen->cap, codegen_initial_code_fmt, codegen->accumulator1, codegen->accumulator2);
  codegen->len = size;
}

#define sus_codegen_append_code(cg, cstr) _sus_codegen_append_code(cg, cstr, sizeof(cstr) - 1)

static void _sus_codegen_append_code(sus_codegen_t * codegen, const char * code, const uint8_t code_size) {
  if (codegen->len == 0) {
    sus_codegen_write_init(codegen);
  }
  
  codegen->len += code_size;

  if (codegen->len > codegen->cap) {
    codegen->cap = codegen->len + SUS_CODEGEN_CHUNK_SIZE;
    codegen->code = (char *)realloc(codegen->code, codegen->cap);
  }

  memcpy(codegen->code + (codegen->len - code_size), code, code_size);
}

static inline int64_t sus_abs(const int64_t n) {
  return n > -1 ? n : -n;
}

static void sus_codegen_consume_queue(sus_codegen_t * codegen, sus_accumulator_queue_t * queue) {
  if (queue->value == 0) {
    return;
  }

  char s[26];
  char fmt[10] = "%s+=%llu;";

  if (queue->value < 0) {
    fmt[2] = '-';
    queue->value = sus_abs(queue->value);
  }

  if (queue->modify_value) {
    fmt[2] = '=';

    if (queue->value < 0) {
      fmt[3] = '-';
    } else {
      memcpy(fmt + 3, fmt + 4, 6);
    }
  }

  const uint8_t len = sprintf(s, fmt, queue->var, queue->value);

  _sus_codegen_append_code(codegen, s, len);

  queue->modify_value = false;
  queue->value = 0;
}

static void sus_codegen_use_malloc(sus_codegen_t * codegen) {
  if (codegen->flags & SUS_CODEGEN_MALLOC_USED) {
    return;
  }
  
  sus_codegen_append_code(codegen, "st=malloc(" SUS_STACK_CHUNK_SIZE ");");
  codegen->flags |= SUS_CODEGEN_MALLOC_USED;
}

void sus_codegen_add_instruction(sus_codegen_t * codegen, sus_token_data_t * token) {
  if (sus_token_data_is_crewmate(token)) {
    codegen->impostor = token->token;
    return;
  }
  
  // tokens that are able to be evaluated at compile-time
  if ((codegen->flags & SUS_CODEGEN_STOP_COMPILE_TIME) == 0) {
    switch (token->token) {
      case TOKEN_VENTED: {
        codegen->accumulator2 += 10;
        return;
      }

      case TOKEN_SUSSY: {
        codegen->accumulator2--;
        return;
      }

      case TOKEN_ELECTRICAL: {
        codegen->accumulator2 = 0;
        return;
      }

      case TOKEN_SUS: {
        switch (codegen->impostor) {
          case TOKEN_ORANGE: {
            codegen->accumulator1 += 10;
            return;
          }

          case TOKEN_PINK: {
            codegen->accumulator1 = 0;
            return;
          }

          case TOKEN_WHITE: {
            codegen->accumulator1--;
            return;
          }

          case TOKEN_RED: {
            codegen->accumulator1++;
            return;
          }
        }
      }
    }

    // involves stack or rand()
    // turn off compile-time pre_runtime_exec after this.
    codegen->flags |= SUS_CODEGEN_STOP_COMPILE_TIME;
  }

  switch (token->token) {
    case TOKEN_VENTED: {
      codegen->accumulator2_queue.value += 10;
      return;
    }

    case TOKEN_SUSSY: {
      codegen->accumulator2_queue.value--;
      return;
    }

    case TOKEN_ELECTRICAL: {
      codegen->accumulator2_queue.modify_value = true;
      codegen->accumulator2_queue.value = 0;
      return;
    }

    case TOKEN_WHO: {
      sus_codegen_consume_queue(codegen, &codegen->accumulator2_queue);
      sus_codegen_append_code(codegen, "while(stl&&st[stl-1]!=a2){");
      return;
    }

    case TOKEN_WHERE: {
      // accumulator 2 can still be modified in this loop
      sus_codegen_consume_queue(codegen, &codegen->accumulator2_queue);
      sus_codegen_append_code(codegen, "}");
      return;
    }

    case TOKEN_SUS: {
      switch (codegen->impostor) {
        case TOKEN_ORANGE: {
          codegen->accumulator1_queue.value += 10;
          return;
        }

        case TOKEN_PINK: {
          codegen->accumulator1_queue.modify_value = true;
          codegen->accumulator1_queue.value = 0;
          return;
        }

        case TOKEN_WHITE: {
          codegen->accumulator1_queue.value--;
          return;
        }

        case TOKEN_RED: {
          codegen->accumulator1_queue.value++;
          return;
        }

        case TOKEN_BLUE: {
          sus_codegen_consume_queue(codegen, &codegen->accumulator1_queue);
          sus_codegen_use_malloc(codegen);
          sus_codegen_append_code(codegen, "as(a1);");
          return;
        }

        case TOKEN_PURPLE: {
          if (codegen->flags & SUS_CODEGEN_MALLOC_USED) {
            sus_codegen_append_code(codegen, "if(stl>0){stl--;}");
          }
          
          return;
        }

        case TOKEN_GREEN: {
          if (codegen->flags & SUS_CODEGEN_MALLOC_USED) {
            sus_codegen_append_code(codegen, "if(stl>0){putchar(st[stl-1]);}");
          }
          
          return;
        }

        case TOKEN_YELLOW: {
          sus_codegen_use_malloc(codegen);
          sus_codegen_append_code(codegen, "as(getch());");
          return;
        }

        case TOKEN_CYAN: {
          codegen->flags |= SUS_CODEGEN_CYAN_USED;
          sus_codegen_consume_queue(codegen, &codegen->accumulator1_queue);
          sus_codegen_append_code(codegen, "cy();");
          return;
        }

        case TOKEN_BLACK: {
          if (codegen->flags & SUS_CODEGEN_MALLOC_USED) {
            codegen->flags |= SUS_CODEGEN_BLACK_USED;
            sus_codegen_append_code(codegen, "if(stl>0){b();}");
          }
          
          return;
        }

        case TOKEN_BROWN: {
          // something bs idk
          codegen->accumulator1_queue.modify_value = false;
          codegen->accumulator1_queue.value = 0;
          sus_codegen_append_code(codegen, "if(stl>0){a1=st[stl-1];}");
          return;
        }

        case TOKEN_LIME: {
          if (codegen->flags & SUS_CODEGEN_MALLOC_USED) {
            sus_codegen_append_code(codegen, "if(stl>0){pl=&st[stl-1];(*pl)*=2;}");
          }
        }
      }
    }
  }
}

void sus_codegen_finalize(sus_codegen_t * codegen) {
  char * ptr;
  
  if (codegen->flags & SUS_CODEGEN_MALLOC_USED) {
    sus_codegen_append_code(codegen, "free(st);");
  } else {
    ptr = strstr(codegen->code, "static void as(");
    memcpy(ptr, ptr + SUS_CODEGEN_ADD_TO_STACK_SIZE, codegen->len - SUS_CODEGEN_ADD_TO_STACK_SIZE);
    codegen->len -= SUS_CODEGEN_ADD_TO_STACK_SIZE;
  }
  
  if ((codegen->flags & SUS_CODEGEN_CYAN_USED) == 0) {
    ptr = strstr(codegen->code, "static void cy(");
    memcpy(ptr, ptr + SUS_CODEGEN_CYAN_FUNCTION_SIZE, codegen->len - SUS_CODEGEN_CYAN_FUNCTION_SIZE);
    codegen->len -= SUS_CODEGEN_CYAN_FUNCTION_SIZE;
  }
  
  // that's racist
  if ((codegen->flags & SUS_CODEGEN_BLACK_USED) == 0) {
    ptr = strstr(codegen->code, "static void b(");
    memcpy(ptr, ptr + SUS_CODEGEN_BLACK_FUNCTION_SIZE, codegen->len - SUS_CODEGEN_BLACK_FUNCTION_SIZE);
    codegen->len -= SUS_CODEGEN_BLACK_FUNCTION_SIZE;
  }
  
  _sus_codegen_append_code(codegen, "return 0;}", sizeof("return 0;}"));
  codegen->len--; // just kidding let's just lie about it :)
}