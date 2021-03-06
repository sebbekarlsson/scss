#ifndef SCSS_AST_H
#define SCSS_AST_H
#include "lexer.h"
#include <list/list.h>
#include "token.h"
typedef struct SCSS_AST_STRUCT
{
  enum
  {
    SCSS_AST_NAME,
    SCSS_AST_CLASSNAME,
    SCSS_AST_HASHNAME,
    SCSS_AST_TAGNAME,
    SCSS_AST_VAR,
    SCSS_AST_STRING,
    SCSS_AST_FLOAT,
    SCSS_AST_INT,
    SCSS_AST_IMPORT,
    SCSS_AST_COMPOUND,
    SCSS_AST_STYLE_RULE,
    SCSS_AST_RULE,
    SCSS_AST_MEDIA_QUERY,
    SCSS_AST_PROP_DEC,
    SCSS_AST_BINOP,
    SCSS_AST_CALL,
    SCSS_AST_NOOP,
  } type;

  struct SCSS_AST_STRUCT* left;
  struct SCSS_AST_STRUCT* right;
  struct SCSS_AST_STRUCT* body;
  struct SCSS_AST_STRUCT* value;
  struct SCSS_AST_STRUCT* footer; // compound
  struct SCSS_AST_STRUCT* typedata;
  struct SCSS_AST_STRUCT* parent;

  scss_token_T* token;

  char* string_value;
  char* name;
  float float_value;
  int int_value;
  unsigned int capsulated;
  unsigned int dead;

  list_T* list_value;
  list_T* args;
  list_T* options;
  list_T* copies;
  list_T* flags;
  list_T* siblings;

} scss_AST_T;

scss_AST_T* init_scss_ast(int type);

void scss_ast_free(scss_AST_T* ast);

scss_AST_T* init_style_rule(list_T* selectors, scss_AST_T* body);

list_T* ast_get_children(scss_AST_T* ast);

unsigned int ast_has_properties(scss_AST_T* ast);
#endif
