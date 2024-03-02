#include "../deps/tcc/libtcc.h"
#include "compiler.h"
#include "array.h"
#include "token.h"
#include "error.h"

#include <string.h>
#include <stdint.h>

static void sus_compiler_tokenize(sus_compiler_t * compiler, sus_array_t * tokens) {
  sus_array_new(tokens, sizeof(sus_token_data_t));
  
  sus_token_data_t token_data;
  sus_token_t raw_token;
  int code;

  do {
    code = sus_token_next(compiler->parser, &raw_token);
  
    if (code != 0) {
      token_data.token = sus_token_translate(raw_token.token, (bool *)&token_data.packed_data);
  
      if (token_data.token != TOKEN_INVALID) {
        token_data.line.num    = compiler->parser->line.num;
        token_data.line.col    = compiler->parser->line.col - raw_token.len;
        token_data.index       = compiler->parser->index - raw_token.len - 1;
        token_data.packed_data = (token_data.packed_data << 4) | raw_token.len;

        sus_array_append(tokens, (sus_array_member_t)&token_data);
      }

      sus_line_advance(&compiler->parser->line, code);
    }
  } while (!raw_token.eof);
}

// stfu gcc <3
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

static inline int64_t sus_abs(const int64_t n) {
  return n > -1 ? n : -n;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop "-Wmaybe-uninitialized"
#endif

bool sus_compiler_compile(sus_compiler_t * compiler) {
  sus_array_t tokens;
  sus_compiler_tokenize(compiler, &tokens);

  if (tokens.len == 0) {
    sus_error_simple_literal("Empty file. That's kinda sus.");
    goto sus_compiler_end;
  }
  
  compiler->parser->contents = realloc(compiler->parser->contents, compiler->parser->index);

  // validate SUS statements

  uint32_t i = 0;

  sus_token_data_t * tok;
  sus_token_data_t * invalid_tok;
  
  sus_error_ctx_t err_ctx;
  sus_error_ctx_new(&err_ctx, compiler->parser);

  int64_t who_depth = 0;

sus_compiler_check_loop:
  tok = (sus_token_data_t *)sus_array_get(&tokens, i);

  // out of bounds access!
  if (tok == NULL) {
    goto sus_compiler_post_err_check;
  }

  // SUS usage validation
  if (tok->token == TOKEN_SUS) {
    if (i == 0) {
      sus_error_token_literal(
        &err_ctx, compiler->parser, tok,
        "What's so sussy? Stop being a weirdo!"
      );
      
      sus_array_pop(&tokens, i);
      goto sus_compiler_check_loop;
    } else if (!sus_token_data_is_crewmate(invalid_tok = (sus_token_data_t *)sus_array_get_unchecked(&tokens, i - 1)) && invalid_tok->token != TOKEN_SUS) {
      sus_error_token_format(
        &err_ctx, compiler->parser, invalid_tok, 
        "'%s' is NOT a crewmate! sigh",
        sus_token_to_string(invalid_tok->token)
      );

      sus_array_pop(&tokens, i - 1);
      sus_array_pop(&tokens, i - 1);
      i--;
      goto sus_compiler_check_loop;
    }
  }

  // usage of WHERE before WHO and vice versa
  else if (tok->token == TOKEN_WHO) {
    who_depth++;
  } else if (tok->token == TOKEN_WHERE) {
    who_depth--;

    if (who_depth == -1) {
      sus_error_token_literal(
        &err_ctx, compiler->parser, tok,
        "Don't misuse WHERE! this will fuck up other WHO and WHEREs in this code >:)"
      );
      sus_array_pop(&tokens, i);
      goto sus_compiler_check_loop;
    } else if (who_depth < -1) {
      sus_error_token_literal(
        &err_ctx, compiler->parser, tok,
        "[fucked up WHERE from previous WHERE error]"
      );
      sus_array_pop(&tokens, i);
      goto sus_compiler_check_loop;
    }
  }

  i++;
  goto sus_compiler_check_loop;

sus_compiler_post_err_check:

  // scenario: WHO ... WHO ... WHERE
  if (who_depth != 0) {
    i = 0;
    who_depth = sus_abs(who_depth);

sus_who_depth_check:
    tok = (sus_token_data_t *)sus_array_get(&tokens, i);
  
    if (tok != NULL && tok->token == TOKEN_WHO) {
      sus_error_token_literal(
        &err_ctx, compiler->parser, tok,
        "This WHO is missing it's partner: WHERE ;'("
      );
      who_depth--;
    }

    if (who_depth != 0 && tok != NULL) {
      i++;
      goto sus_who_depth_check;
    }
  }

  // quit on error
  if (err_ctx.errored) {
    goto sus_compiler_end;
  }

  sus_codegen_new(&compiler->codegen, compiler->parser->contents, compiler->parser->index);
  
  for (i = 0; i < tokens.len; i++) {
    tok = (sus_token_data_t *)sus_array_get_unchecked(&tokens, i);

    sus_codegen_add_instruction(&compiler->codegen, tok);
  }
  
  if (compiler->codegen.len == 0) {
    sus_error_simple_literal("This executable does not do anything. Executable ejected from ship.");
    sus_codegen_delete(&compiler->codegen);
    err_ctx.errored = true;
    goto sus_compiler_end;
  }

  sus_codegen_finalize(&compiler->codegen);

sus_compiler_end:
  sus_array_delete(&tokens);
  sus_parser_delete(compiler->parser);
  return !err_ctx.errored;
}

static void sus_tcc_callback(void * opaque, const char * message) {
  if (strstr(message, "warning: ") == NULL) {
    sus_error_simple("%s", message);
  }
}

void sus_compiler_save(sus_compiler_t * compiler, char * file, char * exe_path) {
  TCCState * tcc = tcc_new();

  tcc_set_error_func(tcc, NULL, sus_tcc_callback);
  tcc_set_output_type(tcc, TCC_OUTPUT_EXE);
  tcc_add_library_path(tcc, exe_path);
  tcc_compile_string(tcc, compiler->codegen.code);
  tcc_output_file(tcc, file);

  tcc_delete(tcc);
}