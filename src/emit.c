#include "include/emit.h"
#include "include/css/call.css.h"
#include "include/css/prop_dec.css.h"
#include "include/css/style_rule.css.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

#define AST scss_AST_T

#define TEMPLATE(template_name, tmpname, len, ...)                                                 \
  tmpname = calloc(_tmp_##template_name##_css_len + len + 128, sizeof(char));                      \
  sprintf(tmpname, (const char*)_tmp_##template_name##_css, __VA_ARGS__);

char* emit_spaced_list(list_T* list_value)
{
  if (!list_value)
    return strdup("");

  char* str = 0;

  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST* child = (AST*)living->items[i];

    char* child_str = scss_emit(child);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1)
      str = str_append(&str, " ");

    free(child_str);
  }

  return str ? str : strdup("");
}

char* emit_comma_list(list_T* list_value)
{
  if (!list_value)
    return strdup(" ");

  char* str = 0;
  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST* child = (AST*)living->items[i];

    char* child_str = scss_emit(child);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1)
      str = str_append(&str, ",");

    free(child_str);
  }

  return str ? str : strdup("");
}

char* emit_colon_list(list_T* list_value)
{
  if (!list_value)
    return strdup(" ");

  char* str = 0;
  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST* child = (AST*)living->items[i];

    char* child_str = scss_emit(child);

    if (!child_str)
      continue;

    str = str_append(&str, ":");

    str = str_append(&str, child_str);

    free(child_str);
  }

  return str ? str : strdup("");
}

char* scss_emit(scss_AST_T* ast)
{
  char* str = 0;

  if (!ast) {
    printf("Cannot emit nil scss ast.\n");
    exit(1);
  }
  switch (ast->type) {
    case AST_STYLE_RULE: str = scss_emit_style_rule(ast); break;
    case AST_PROP_DEC: str = scss_emit_prop_dec(ast); break;
    case AST_CALL: str = scss_emit_call(ast); break;
    case AST_NAME: str = scss_emit_name(ast); break;
    case AST_STRING: str = scss_emit_string(ast); break;
    case AST_INT: str = scss_emit_int(ast); break;
    case AST_FLOAT: str = scss_emit_float(ast); break;
    case AST_BINOP: str = scss_emit_binop(ast); break;
    case AST_COMPOUND: str = scss_emit_compound(ast); break;
    case AST_NOOP: str = strdup(""); break;
    default: {
      printf("Cannot emit `%d`\n", ast->type);
      exit(1);
    }
  }

  if (ast->options) {
    char* optstr = emit_colon_list(ast->options);

    if (optstr) {
      str = str_append(&str, optstr);
      free(optstr);
    }
  }

  return str ? str : strdup("");
}

char* scss_emit_style_rule(scss_AST_T* ast)
{
  char* str = 0;

  char* leftstr = ast->list_value ? emit_spaced_list(ast->list_value) : strdup(" ");

  char* bodystr = ast->body ? scss_emit(ast->body) : strdup(" ");

  char* footerstr = ast->footer && ast->footer->list_value ? scss_emit(ast->footer) : strdup(" ");

  if (footerstr) {
    char* f = 0;
    f = str_append(&f, "\n");
    f = str_append(&f, footerstr);
    free(footerstr);
    footerstr = f;
  }

  TEMPLATE(style_rule, str, strlen(leftstr) + strlen(bodystr) + strlen(footerstr), leftstr, bodystr,
           footerstr);

  if (bodystr)
    free(bodystr);

  if (footerstr)
    free(footerstr);

  if (leftstr)
    free(leftstr);

  return str ? str : strdup("");
}

char* scss_emit_prop_dec(scss_AST_T* ast)
{
  char* str = 0;
  char* leftstr = ast->list_value ? emit_spaced_list(ast->list_value)
                  : ast->left     ? scss_emit(ast->left)
                                  : strdup("");
  char* valuestr = scss_emit(ast->value);
  TEMPLATE(prop_dec, str, strlen(leftstr) + strlen(valuestr), leftstr, valuestr);
  str = str_append(&str, ";");

  if (valuestr)
    free(valuestr);
  if (leftstr)
    free(leftstr);

  return str ? str : strdup("");
}

char* scss_emit_call(scss_AST_T* ast)
{
  char* str = 0;
  char* name = strdup(ast->name);
  char* argsstr = ast->args ? emit_comma_list(ast->args) : strdup("");

  TEMPLATE(call, str, strlen(name) + strlen(argsstr), name, argsstr);

  if (name)
    free(name);
  if (argsstr)
    free(argsstr);

  return str ? str : strdup("");
}

char* scss_emit_name(scss_AST_T* ast)
{
  return strdup(ast->name);
}

char* scss_emit_string(scss_AST_T* ast)
{
  char* str = 0;
  str = str_append(&str, "\"");
  char* strval = ast->string_value ? strdup(ast->string_value) : strdup("");
  if (strval) {
    str = str_append(&str, strval);
    free(strval);
  }
  str = str_append(&str, "\"");

  return str ? str : strdup("");
}

char* scss_emit_int(scss_AST_T* ast)
{
  return strdup(ast->string_value);
}

char* scss_emit_float(scss_AST_T* ast)
{
  return strdup(ast->string_value);
}

char* scss_emit_binop(scss_AST_T* ast)
{
  char* str = 0;
  char* leftstr = scss_emit(ast->left);
  char* tokstr = strdup(ast->token->value);
  char* rightstr = scss_emit(ast->right);
  str = str_append(&str, leftstr);
  str = str_append(&str, " ");
  str = str_append(&str, tokstr);
  str = str_append(&str, " ");
  str = str_append(&str, rightstr);

  if (leftstr)
    free(leftstr);
  if (rightstr)
    free(rightstr);
  if (tokstr)
    free(tokstr);

  return str ? str : strdup("");
}

char* scss_emit_compound(scss_AST_T* ast)
{
  char* str = 0;

  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST* child = (AST*)ast->list_value->items[i];
      if (!child)
        continue;

      char* childstr = scss_emit(child);

      if (!childstr)
        continue;

      str = str_append(&str, childstr);

      if (childstr)
        free(childstr);
    }
  }

  return str ? str : strdup("");
}
