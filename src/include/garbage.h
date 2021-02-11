#ifndef SCSS_GARBAGE_H
#define SCSS_GARBAGE_H
#include "AST.h"
#include "list.h"
typedef struct SCSS_GARBAGE_STRUCT
{
  list_T* asts;
  list_T* tokens;
} scss_garbage_T;

scss_garbage_T* init_scss_garbage();

void scss_garbage_free(scss_garbage_T* garbage);

void scss_garbage_mark_ast(scss_garbage_T* garbage, scss_AST_T* ast);

void scss_garbage_mark_token(scss_garbage_T* garbage, scss_token_T* token);

void scss_garbage_sweep(scss_garbage_T* garbage);
#endif
