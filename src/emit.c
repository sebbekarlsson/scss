#include "include/emit.h"
#include "include/css/prop_dec.css.h"
#include "include/css/style_rule.css.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

#define AST scss_AST_T

#define TEMPLATE(template_name, tmpname, len, ...)                             \
  tmpname = calloc(_tmp_##template_name##_css_len + len + 128, sizeof(char));  \
  sprintf(tmpname, (const char *)_tmp_##template_name##_css, __VA_ARGS__);

char *emit_spaced_list(list_T *list_value) {
  if (!list_value)
    return 0;

  char *str = 0;
  list_T *living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST *child = (AST *)living->items[i];

    char *child_str = scss_emit(child);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1)
      str = str_append(&str, " ");

    free(child_str);
  }

  return str;
}

char *scss_emit(scss_AST_T *ast) {
  if (!ast) {
    printf("Cannot emit nil scss ast.\n");
    exit(1);
  }
  switch (ast->type) {
  case AST_STYLE_RULE:
    return scss_emit_style_rule(ast);
    break;
  case AST_PROP_DEC:
    return scss_emit_prop_dec(ast);
    break;
  case AST_NAME:
    return scss_emit_name(ast);
    break;
  case AST_STRING:
    return scss_emit_string(ast);
    break;
  case AST_INT:
    return scss_emit_int(ast);
    break;
  case AST_FLOAT:
    return scss_emit_float(ast);
    break;
  case AST_BINOP:
    return scss_emit_binop(ast);
    break;
  case AST_COMPOUND:
    return scss_emit_compound(ast);
    break;
  case AST_NOOP:
    return strdup("");
    break;
  default: {
    printf("Cannot emit `%d`\n", ast->type);
    exit(1);
  }
  }

  return 0;
}

char *scss_emit_style_rule(scss_AST_T *ast) {
  char *str = 0;

  char *leftstr = emit_spaced_list(ast->list_value);
  char *bodystr = scss_emit(ast->body);

  TEMPLATE(style_rule, str, strlen(leftstr) + strlen(bodystr), leftstr,
           bodystr);
  return str;
}

char *scss_emit_prop_dec(scss_AST_T *ast) {
  char *str = 0;
  char *leftstr = emit_spaced_list(ast->list_value);
  char *valuestr = scss_emit(ast->value);
  TEMPLATE(prop_dec, str, strlen(leftstr) + strlen(valuestr), leftstr,
           valuestr);
  str = str_append(&str, ";");
  return str;
}

char *scss_emit_name(scss_AST_T *ast) { return strdup(ast->name); }

char *scss_emit_string(scss_AST_T *ast) {
  return ast->string_value ? strdup(ast->string_value) : strdup("");
}

char *scss_emit_int(scss_AST_T *ast) { return strdup(ast->string_value); }

char *scss_emit_float(scss_AST_T *ast) { return strdup(ast->string_value); }

char *scss_emit_binop(scss_AST_T *ast) {
  char *str = 0;
  char *leftstr = scss_emit(ast->left);
  char *tokstr = strdup(ast->token->value);
  char *rightstr = scss_emit(ast->right);
  str = str_append(&str, leftstr);
  str = str_append(&str, " ");
  str = str_append(&str, tokstr);
  str = str_append(&str, " ");
  str = str_append(&str, rightstr);

  return str;
}

char *scss_emit_compound(scss_AST_T *ast) {
  char *str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST *child = (AST *)ast->list_value->items[i];
    if (!child)
      continue;

    char *childstr = scss_emit(child);

    if (!childstr)
      continue;

    str = str_append(&str, childstr);
  }

  return str;
}
