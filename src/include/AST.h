#ifndef SCSS_AST_H
#define SCSS_AST_H
#include "lexer.h"
#include "list.h"
#include "token.h"
typedef struct SCSS_AST_STRUCT
{
  enum {
    AST_NAME,
    AST_STRING,
    AST_FLOAT,
    AST_INT,
    AST_COMPOUND,
    AST_STYLE_RULE,
    AST_PROP_DEC,
    AST_BINOP,
    AST_NOOP,
  } type;

  struct SCSS_AST_STRUCT* left;
  struct SCSS_AST_STRUCT* right;
  struct SCSS_AST_STRUCT* body;
  struct SCSS_AST_STRUCT* value;

  scss_token_T* token;

  char* string_value;
  char* name;
  float float_value;
  int int_value;

  list_T* list_value;
} scss_AST_T;

scss_AST_T* init_scss_ast(int type);
#endif
