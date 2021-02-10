#ifndef SCSS_EVAL_H
#define SCSS_EVAL_H
#include "AST.h"
scss_AST_T *scss_eval(scss_AST_T *ast);
scss_AST_T *scss_eval_style_rule(scss_AST_T *ast);
scss_AST_T *scss_eval_prop_dec(scss_AST_T *ast);
scss_AST_T *scss_eval_name(scss_AST_T *ast);
scss_AST_T *scss_eval_string(scss_AST_T *ast);
scss_AST_T *scss_eval_int(scss_AST_T *ast);
scss_AST_T *scss_eval_float(scss_AST_T *ast);
scss_AST_T *scss_eval_binop(scss_AST_T *ast);
scss_AST_T *scss_eval_compound(scss_AST_T *ast);
#endif
