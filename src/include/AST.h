#ifndef SCSS_AST_H
#define SCSS_AST_H
#include "lexer.h"
#include "list.h"
#include "token.h"
typedef struct SCSS_AST_STRUCT {
  enum {
    AST_NAME,
    AST_STRING,
    AST_FLOAT,
    AST_INT,
    AST_COMPOUND,
    AST_STYLE_RULE,
    AST_PROP_DEC,
    AST_BINOP,
    AST_CALL,
    AST_NOOP,
  } type;

  struct SCSS_AST_STRUCT *left;
  struct SCSS_AST_STRUCT *right;
  struct SCSS_AST_STRUCT *body;
  struct SCSS_AST_STRUCT *value;
  struct SCSS_AST_STRUCT *footer; // compound

  scss_token_T *token;

  char *string_value;
  char *name;
  float float_value;
  int int_value;

  list_T *list_value;
  list_T *args;
  list_T *options;
  list_T *copies;

} scss_AST_T;

scss_AST_T *init_scss_ast(int type);

void scss_ast_free(scss_AST_T *ast);

scss_AST_T *init_style_rule(list_T *selectors, scss_AST_T *body);
#endif
