#include "include/token.h"
#include "include/garbage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern scss_garbage_T *GARBAGE;

scss_token_T *init_token(char *value, int type) {
  scss_token_T *token = calloc(1, sizeof(struct SCSS_TOKEN_STRUCT));
  token->value = value ? value : strdup("");
  token->type = type;
  scss_garbage_mark_token(GARBAGE, token);

  return token;
}

void token_free(scss_token_T *token) {
  if (token->value)
    free(token->value);

  free(token);
}

scss_token_T *token_clone(scss_token_T *token) {
  if (!token)
    return 0;

  char *value = 0;

  if (token->value)
    value = strdup(token->value);
  else
    value = strdup("");

  scss_token_T *new_token = init_token(value, token->type);
  new_token->c = token->c;
  return new_token;
}
