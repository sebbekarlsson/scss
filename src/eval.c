#include "include/eval.h"
#include <stdio.h>

#define AST scss_AST_T

eval_T* init_eval()
{
  eval_T* eval = calloc(1, sizeof(struct EVAL_STRUCT));
  eval->stack = init_list(sizeof(AST*));

  return eval;
}

void eval_free(eval_T* eval)
{
  if (eval->stack) {
    list_free_shallow(eval->stack);
  }

  free(eval);
}

scss_AST_T* scss_eval(scss_AST_T* ast, eval_T* eval)
{
  if (!ast) {
    printf("[Eval] cannot evaluate nil ast.\n");
    exit(1);
  }
  switch (ast->type) {
    case SCSS_AST_STYLE_RULE: return scss_eval_style_rule(ast, eval); break;
    case SCSS_AST_PROP_DEC: return scss_eval_prop_dec(ast, eval); break;
    case SCSS_AST_CALL: return scss_eval_call(ast, eval); break;
    case SCSS_AST_VAR:
    case SCSS_AST_NAME: return scss_eval_name(ast, eval); break;
    case SCSS_AST_CLASSNAME: return scss_eval_classname(ast, eval); break;
    case SCSS_AST_HASHNAME: return scss_eval_hashname(ast, eval); break;
    case SCSS_AST_TAGNAME: return scss_eval_tagname(ast, eval); break;
    case SCSS_AST_STRING: return scss_eval_string(ast, eval); break;
    case SCSS_AST_INT: return scss_eval_int(ast, eval); break;
    case SCSS_AST_FLOAT: return scss_eval_float(ast, eval); break;
    case SCSS_AST_BINOP: return scss_eval_binop(ast, eval); break;
    case SCSS_AST_COMPOUND: return scss_eval_compound(ast, eval); break;
    case SCSS_AST_IMPORT:
    case SCSS_AST_MEDIA_QUERY:
    case SCSS_AST_NOOP: return ast; break;
    default: {
      printf("Cannot eval `%d`\n", ast->type);
      exit(1);
    }
  }
}

scss_AST_T* scss_eval_style_rule(scss_AST_T* ast, eval_T* eval)
{
  if (!ast)
    return 0;

  if (eval->stack)
    list_push(eval->stack, ast);

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST* child = (AST*)ast->list_value->items[i];
    if (!child)
      continue;
    ast->list_value->items[i] = scss_eval(child, eval);
  }

  list_T* children = ast_get_children(ast);
  unsigned int len = children->size;

  /**
   * Unwrap every children from the body and make them inherit
   * the css queries from the current evaluated AST.
   */
  for (unsigned int i = 0; i < len; i++) {
    scss_AST_T* child = (scss_AST_T*)children->items[i];

    // inherit css query.
    list_T* merged = list_merge(ast->list_value, child->list_value);
    child->list_value = merged;

    // move child to siblings of the current evaluated AST.
    list_push_safe(ast->siblings, child);

    // remove the child from the body of the current evaluated AST.
    if (ast->body && ast->body->list_value && ast->body->list_value->size)
      list_remove(ast->body->list_value, child, 0);
  }

  // remove last sibling because only one is to be emitted.
  if (ast->parent)
    list_pop(ast->siblings);

  if (ast->siblings && ast->siblings->size) {
    for (unsigned int i = 0; i < ast->siblings->size; i++)
      ast->siblings->items[i] = scss_eval(ast->siblings->items[i], eval);
  }

  ast->body = ast->body ? scss_eval(ast->body, eval) : ast->body;

  return ast;
}

scss_AST_T* scss_eval_prop_dec(scss_AST_T* ast, eval_T* eval)
{

  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST* child = (AST*)ast->list_value->items[i];
      if (!child)
        continue;
      ast->list_value->items[i] = scss_eval(child, eval);
    }
  }

  if (ast->left)
    ast->left = scss_eval(ast->left, eval);
  if (ast->value)
    ast->value = scss_eval(ast->value, eval);

  return ast;
}

scss_AST_T* scss_eval_name(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_classname(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_hashname(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_tagname(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_call(scss_AST_T* ast, eval_T* eval)
{

  if (ast->args) {
    for (unsigned int i = 0; i < ast->args->size; i++) {
      AST* child = (AST*)ast->args->items[i];
      if (!child)
        continue;

      ast->args->items[i] = scss_eval(child, eval);
    }
  }

  return ast;
}

scss_AST_T* scss_eval_string(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_int(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_float(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_binop(scss_AST_T* ast, eval_T* eval)
{
  return ast;
}

scss_AST_T* scss_eval_compound(scss_AST_T* ast, eval_T* eval)
{
  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST* child = (AST*)ast->list_value->items[i];
    if (!child)
      continue;

    if ((child->type == SCSS_AST_STYLE_RULE || child->type == SCSS_AST_MEDIA_QUERY) &&
        !eval->callee) {
      eval->callee = child;
    }
    ast->list_value->items[i] = scss_eval(child, eval);

    eval->callee = 0;

    if (eval->stack) {
      list_free_shallow(eval->stack);
      eval->stack = init_list(sizeof(AST*));
    }
  }

  return ast;
}
