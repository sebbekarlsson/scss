#include "include/AST.h"


scss_AST_T* init_scss_ast(int type)
{
  scss_AST_T* ast = calloc(1, sizeof(struct SCSS_AST_STRUCT));
  ast->type = type;

  return ast;
}
