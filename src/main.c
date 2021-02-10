#include "include/emit.h"
#include "include/eval.h"
#include "include/io.h"
#include "include/lexer.h"
#include "include/parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  char *contents = scss_read_file(argv[1]);

  lexer_T *lexer = init_lexer(contents, argv[1]);
  scss_parser_T *parser = init_scss_parser(lexer);
  scss_AST_T *root = scss_parser_parse(parser);
  root = scss_eval(root);

  char *result = scss_emit(root);

  if (result)
    printf("%s\n", result);

  return 0;
}
