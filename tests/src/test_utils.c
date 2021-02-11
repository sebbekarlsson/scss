#include "include/test_utils.h"
#include <scss/eval.h>
#include <scss/io.h>
#include <scss/lexer.h>
#include <scss/parser.h>
#include <scss/scss.h>
#include <scss/string_utils.h>
#include <stdio.h>
#include <string.h>

#define OK() printf(GREEN "\t[OK]\n" RESET)

#define MSG() printf("%s\n", msg)

extern scss_garbage_T *GARBAGE;

void ASSERT(unsigned int condition, const char *msg) {
  MSG();

  if (!condition) {
    printf(RED "\t[FAIL]: Assert failed\n" RESET);
    exit(1);
  }

  OK();
}

scss_AST_T *run_get_ast(const char *filepath) {
  init_scss();
  char *source = scss_read_file(filepath);

  lexer_T *lexer = init_lexer(source, filepath);
  scss_parser_T *parser = init_scss_parser(lexer);
  scss_AST_T *root = scss_parser_parse(parser);

  eval_T *eval = init_eval();

  root = scss_eval(root, eval);

  lexer_free(lexer);
  parser_free(parser);
  eval_free(eval);

  return root;
}
