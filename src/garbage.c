#include "include/garbage.h"
#include "include/AST.h"
#include <stdio.h>

scss_garbage_T* GARBAGE;

scss_garbage_T* init_scss_garbage()
{
  scss_garbage_T* garbage = calloc(1, sizeof(struct SCSS_GARBAGE_STRUCT));
  garbage->asts = init_list(sizeof(scss_AST_T*));
  garbage->tokens = init_list(sizeof(scss_token_T*));

  return garbage;
}

void scss_garbage_free(scss_garbage_T* garbage)
{
  if (garbage->asts) {
    list_free_shallow(garbage->asts);
  }

  if (garbage->tokens) {
    list_free_shallow(garbage->tokens);
  }

  free(garbage);
}

void scss_garbage_mark_ast(scss_garbage_T* garbage, scss_AST_T* ast)
{
  list_push_safe(garbage->asts, ast);
}

void scss_garbage_mark_token(scss_garbage_T* garbage, scss_token_T* token)
{
  list_push_safe(garbage->tokens, token);
}

void scss_garbage_sweep(scss_garbage_T* garbage)
{
  unsigned int len = garbage->asts->size;

  for (unsigned int i = 0; i < len; i++) {
    scss_AST_T* ast = (scss_AST_T*)garbage->asts->items[i];
    if (!ast)
      continue;

    scss_ast_free(ast);
  }

  len = garbage->tokens->size;

  for (unsigned int i = 0; i < len; i++) {
    scss_token_T* token = (scss_token_T*)garbage->tokens->items[i];
    if (!token)
      continue;

    token_free(token);
  }

  list_free_shallow(garbage->asts);
  list_free_shallow(garbage->tokens);
  garbage->asts = init_list(sizeof(scss_AST_T*));
  garbage->tokens = init_list(sizeof(scss_token_T*));
}
