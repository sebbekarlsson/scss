#ifndef SCSS_EVAL_H
#define SCSS_EVAL_H
#include "AST.h"
#include "list.h"

typedef struct EVAL_STRUCT {
  list_T *stack;
  scss_AST_T *callee;
} eval_T;

eval_T *init_eval();

void eval_free(eval_T *eval);

scss_AST_T *scss_eval(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_style_rule(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_prop_dec(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_call(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_name(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_string(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_int(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_float(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_binop(scss_AST_T *ast, eval_T *eval);
scss_AST_T *scss_eval_compound(scss_AST_T *ast, eval_T *eval);
#endif
