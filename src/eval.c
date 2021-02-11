#include "include/eval.h"
#include <stdio.h>

#define AST scss_AST_T

eval_T *init_eval() {
  eval_T *eval = calloc(1, sizeof(struct EVAL_STRUCT));
  eval->stack = init_list(sizeof(AST *));

  return eval;
}

void eval_free(eval_T *eval) {
  if (eval->stack) {
    list_free_shallow(eval->stack);
  }

  free(eval);
}

scss_AST_T *scss_eval(scss_AST_T *ast, eval_T *eval) {
  if (!ast) {
    printf("[Eval] cannot evaluate nil ast.\n");
    exit(1);
  }
  switch (ast->type) {
  case AST_STYLE_RULE:
    return scss_eval_style_rule(ast, eval);
    break;
  case AST_PROP_DEC:
    return scss_eval_prop_dec(ast, eval);
    break;
  case AST_CALL:
    return scss_eval_call(ast, eval);
    ;
  case AST_NAME:
    return scss_eval_name(ast, eval);
    break;
  case AST_STRING:
    return scss_eval_string(ast, eval);
    break;
  case AST_INT:
    return scss_eval_int(ast, eval);
    break;
  case AST_FLOAT:
    return scss_eval_float(ast, eval);
    break;
  case AST_BINOP:
    return scss_eval_binop(ast, eval);
    break;
  case AST_COMPOUND:
    return scss_eval_compound(ast, eval);
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

scss_AST_T *scss_eval_style_rule(scss_AST_T *ast, eval_T *eval) {
  list_T *tmp = init_list(sizeof(AST *));

  if (ast->list_value && eval->stack->size) {
    AST *last = (AST *)eval->stack->items[eval->stack->size - 1];

    if (last && last->list_value && last->list_value->size) {
      unsigned int len = ast->list_value->size;

      for (unsigned int i = 0; i < len; i++) {
        AST *selector = (AST *)last->list_value->items[i];
        if (selector->type == AST_BINOP &&
            selector->token->type == TOKEN_COMMA) {
          list_push(tmp, selector->left);
          list_push(tmp, selector->right);
          continue;
        }

        list_prefix(ast->list_value, selector);
      }
    }
  }

  list_push(eval->stack, ast);

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST *child = (AST *)ast->list_value->items[i];
    if (!child)
      continue;
    ast->list_value->items[i] = scss_eval(child, eval);
  }

  ast->body = ast->body ? scss_eval(ast->body, eval) : ast->body;

  if (eval->callee && eval->callee != ast && eval->stack->size) {
    list_push(eval->callee->footer->list_value, ast);

    for (unsigned int i = 0; i < tmp->size; i++) {
      list_T *rules = init_list(sizeof(AST *));
      list_push(rules, tmp->items[i]);
      list_T *merged = list_merge(rules, ast->list_value);
      list_free_shallow(rules);
      AST *style_rule = init_style_rule(merged, ast->body);
      list_free_shallow(merged);
      list_push(eval->callee->footer->list_value, style_rule);
      ast->type = AST_NOOP;
    }

    list_free_shallow(tmp);

    return init_scss_ast(AST_NOOP);
  }

  list_free_shallow(tmp);

  return ast;
}

scss_AST_T *scss_eval_prop_dec(scss_AST_T *ast, eval_T *eval) {

  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST *child = (AST *)ast->list_value->items[i];
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

scss_AST_T *scss_eval_name(scss_AST_T *ast, eval_T *eval) { return ast; }

scss_AST_T *scss_eval_call(scss_AST_T *ast, eval_T *eval) {

  if (ast->args) {
    for (unsigned int i = 0; i < ast->args->size; i++) {
      AST *child = (AST *)ast->args->items[i];
      if (!child)
        continue;

      ast->args->items[i] = scss_eval(child, eval);
    }
  }

  return ast;
}

scss_AST_T *scss_eval_string(scss_AST_T *ast, eval_T *eval) { return ast; }

scss_AST_T *scss_eval_int(scss_AST_T *ast, eval_T *eval) { return ast; }

scss_AST_T *scss_eval_float(scss_AST_T *ast, eval_T *eval) { return ast; }

scss_AST_T *scss_eval_binop(scss_AST_T *ast, eval_T *eval) { return ast; }

scss_AST_T *scss_eval_compound(scss_AST_T *ast, eval_T *eval) {
  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST *child = (AST *)ast->list_value->items[i];
    if (!child)
      continue;

    if (child->type == AST_STYLE_RULE && !eval->callee) {
      eval->callee = child;
    }
    ast->list_value->items[i] = scss_eval(child, eval);
  }

  return ast;
}
