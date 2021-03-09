#include "include/AST.h"
#include "include/garbage.h"

extern scss_garbage_T* GARBAGE;

scss_AST_T* init_scss_ast(int type)
{
  scss_AST_T* ast = calloc(1, sizeof(struct SCSS_AST_STRUCT));
  ast->type = type;

  if (ast->type == SCSS_AST_STYLE_RULE) {
    ast->footer = init_scss_ast(SCSS_AST_COMPOUND);
    ast->children = init_list(sizeof(scss_AST_T*));
  }

  if (ast->type == SCSS_AST_COMPOUND || ast->type == SCSS_AST_STYLE_RULE ||
      ast->type == SCSS_AST_PROP_DEC || ast->type == SCSS_AST_MEDIA_QUERY) {
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

unsigned int ast_has_properties(scss_AST_T* ast)
{
  if (!ast)
    return 0;
  if (!ast->body)
    return 0;
  if (!ast->body->list_value)
    return 0;
  if (!ast->body->list_value->size)
    return 0;

  for (unsigned int i = 0; i < ast->body->list_value->size; i++) {
    scss_AST_T* child = (scss_AST_T*)ast->body->list_value->items[i];
    if (!child)
      continue;
    if (child->type == SCSS_AST_PROP_DEC)
      return 1;
  }

  return 0;
}

list_T* ast_get_parents(scss_AST_T* ast)
{
  list_T* list = init_list(sizeof(scss_AST_T*));

  if (!ast)
    return list;

  scss_AST_T* parent = ast->parent;

  while (parent) {
    list_push(list, parent);
    parent = parent->parent;
  }

  return list;
}

list_T* ast_get_children(scss_AST_T* ast)
{
  list_T* list = init_list(sizeof(scss_AST_T*));

  if (!ast)
    return list;
  if (!ast->body)
    return list;
  if (!ast->body->list_value)
    return list;
  list_T* children = ast->body->list_value;

  for (unsigned int i = 0; i < children->size; i++) {
    scss_AST_T* child = (scss_AST_T*)children->items[i];

    if (child->type != SCSS_AST_STYLE_RULE)
      continue;

    list_push(list, child);

    list_T* x = ast_get_children(child);

    list = list_merge(list, x);
  }

  return list;
}
