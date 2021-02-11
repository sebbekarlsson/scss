#include "include/AST.h"

scss_AST_T *init_scss_ast(int type) {
  scss_AST_T *ast = calloc(1, sizeof(struct SCSS_AST_STRUCT));
  ast->type = type;

  if (ast->type == AST_STYLE_RULE)
    ast->footer = init_scss_ast(AST_COMPOUND);

  if (ast->type == AST_COMPOUND) {
    ast->list_value = init_list(sizeof(scss_AST_T *));
  }

  ast->options = init_list(sizeof(scss_AST_T *));
  ast->args = init_list(sizeof(scss_AST_T *));

  return ast;
}
