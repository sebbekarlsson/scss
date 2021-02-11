#include "include/scss.h"
#include "include/emit.h"
#include "include/eval.h"
#include "include/garbage.h"
#include "include/lexer.h"
#include "include/parser.h"
#include <stdio.h>

extern scss_garbage_T* GARBAGE;

char* scss(char* source, const char* filepath)
{
  lexer_T* lexer = init_lexer(source, filepath);
  scss_parser_T* parser = init_scss_parser(lexer);
  scss_AST_T* root = scss_parser_parse(parser);

  eval_T* eval = init_eval();

  root = scss_eval(root, eval);

  char* out = scss_emit(root);

  lexer_free(lexer);
  parser_free(parser);
  eval_free(eval);

  scss_garbage_sweep(GARBAGE);
  scss_garbage_free(GARBAGE);

  return out;
}

void init_scss()
{
  if (!GARBAGE)
    GARBAGE = init_scss_garbage();
}
