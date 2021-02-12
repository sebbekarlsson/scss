#include "include/AST.h"
#include "include/garbage.h"

extern scss_garbage_T* GARBAGE;

scss_AST_T* init_scss_ast(int type)
{
  scss_AST_T* ast = calloc(1, sizeof(struct SCSS_AST_STRUCT));
  ast->type = type;

  if (ast->type == SCSS_AST_STYLE_RULE)
    ast->footer = init_scss_ast(SCSS_AST_COMPOUND);

  if (ast->type == SCSS_AST_COMPOUND || ast->type == SCSS_AST_STYLE_RULE ||
      ast->type == SCSS_AST_PROP_DEC) {
    ast->list_value = init_list(sizeof(scss_AST_T*));
  }

  ast->options = init_list(sizeof(scss_AST_T*));
  ast->args = init_list(sizeof(scss_AST_T*));

  if (ast->type == SCSS_AST_STYLE_RULE)
    ast->copies = init_list(sizeof(scss_AST_T*));

  scss_garbage_mark_ast(GARBAGE, ast);

  return ast;
}

void scss_ast_free(scss_AST_T* ast)
{
  if (ast->string_value)
    free(ast->string_value);
  if (ast->name)
    free(ast->name);
  if (ast->list_value)
    list_free_shallow(ast->list_value);
  if (ast->copies)
    list_free_shallow(ast->copies);
  if (ast->options)
    list_free_shallow(ast->options);
  if (ast->args)
    list_free_shallow(ast->args);
  free(ast);
}

scss_AST_T* init_style_rule(list_T* selectors, scss_AST_T* body)
{
  scss_AST_T* ast = init_scss_ast(SCSS_AST_STYLE_RULE);
  if (ast->list_value)
    list_free_shallow(ast->list_value);

  ast->list_value = list_copy(selectors);
  ast->body = body;

  return ast;
}
