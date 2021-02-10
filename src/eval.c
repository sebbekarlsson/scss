#include "include/eval.h"
#include <stdio.h>

#define AST scss_AST_T

scss_AST_T *scss_eval(scss_AST_T *ast) {
  switch (ast->type) {
  case AST_STYLE_RULE:
    return scss_eval_style_rule(ast);
    break;
  case AST_PROP_DEC:
    return scss_eval_prop_dec(ast);
    break;
  case AST_NAME:
    return scss_eval_name(ast);
    break;
  case AST_STRING:
    return scss_eval_string(ast);
    break;
  case AST_INT:
    return scss_eval_int(ast);
    break;
  case AST_FLOAT:
    return scss_eval_float(ast);
    break;
  case AST_BINOP:
    return scss_eval_binop(ast);
    break;
  case AST_COMPOUND:
    return scss_eval_compound(ast);
    break;
  case AST_NOOP:
    return ast;
    break;
  default: {
    printf("Cannot eval `%d`\n", ast->type);
    exit(1);
  }
  }
}

scss_AST_T *scss_eval_style_rule(scss_AST_T *ast) {
  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST *child = (AST *)ast->list_value->items[i];
    if (!child)
      continue;
    ast->list_value->items[i] = scss_eval(child);
  }

  return ast;
}

scss_AST_T *scss_eval_prop_dec(scss_AST_T *ast) {
  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST *child = (AST *)ast->list_value->items[i];
    if (!child)
      continue;
    ast->list_value->items[i] = scss_eval(child);
  }

  return ast;
}

scss_AST_T *scss_eval_name(scss_AST_T *ast) { return ast; }

scss_AST_T *scss_eval_string(scss_AST_T *ast) { return ast; }

scss_AST_T *scss_eval_int(scss_AST_T *ast) { return ast; }

scss_AST_T *scss_eval_float(scss_AST_T *ast) { return ast; }

scss_AST_T *scss_eval_binop(scss_AST_T *ast) { return ast; }

scss_AST_T *scss_eval_compound(scss_AST_T *ast) {
  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST *child = (AST *)ast->list_value->items[i];
    if (!child)
      continue;
    ast->list_value->items[i] = scss_eval(child);
  }

  return ast;
}
