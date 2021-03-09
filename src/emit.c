#include "include/emit.h"
#include "include/css/call.css.h"
#include "include/css/import.css.h"
#include "include/css/media_query.css.h"
#include "include/css/prop_dec.css.h"
#include "include/css/style_rule.css.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

#define AST scss_AST_T

#define TEMPLATE(template_name, tmpname, len, ...)                                                 \
  tmpname = calloc(_tmp_##template_name##_css_len + len + 128, sizeof(char));                      \
  sprintf(tmpname, (const char*)_tmp_##template_name##_css, __VA_ARGS__);

static char* emit_spaced_list(list_T* list_value)
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

static char* emit_spaced_list2(list_T* list_value)
{
  if (!list_value)
    return strdup("");

  char* str = 0;

  list_T* living = list_value;

  if (living->size == 1)
    str = str_append(&str, " ");

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

static char* emit_comma_list(list_T* list_value)
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

static char* emit_colon_list(list_T* list_value)
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

  if (ast->capsulated)
    str = str_append(&str, "(");

  switch (ast->type) {
    case SCSS_AST_MEDIA_QUERY: str = str_append(&str, scss_emit_media_query(ast)); break;
    case SCSS_AST_STYLE_RULE: str = str_append(&str, scss_emit_style_rule(ast)); break;
    case SCSS_AST_PROP_DEC: str = str_append(&str, scss_emit_prop_dec(ast)); break;
    case SCSS_AST_CALL: str = str_append(&str, scss_emit_call(ast)); break;
    case SCSS_AST_CLASSNAME:
    case SCSS_AST_HASHNAME:
    case SCSS_AST_TAGNAME:
    case SCSS_AST_NAME: str = str_append(&str, scss_emit_name(ast)); break;
    case SCSS_AST_VAR: str = str_append(&str, scss_emit_var(ast)); break;
    case SCSS_AST_IMPORT: str = str_append(&str, scss_emit_import(ast)); break;
    case SCSS_AST_STRING: str = str_append(&str, scss_emit_string(ast)); break;
    case SCSS_AST_INT: str = str_append(&str, scss_emit_int(ast)); break;
    case SCSS_AST_FLOAT: str = str_append(&str, scss_emit_float(ast)); break;
    case SCSS_AST_BINOP: str = str_append(&str, scss_emit_binop(ast)); break;
    case SCSS_AST_COMPOUND: str = str_append(&str, scss_emit_compound(ast)); break;
    case SCSS_AST_NOOP: str = strdup(""); break;
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

  if (ast->typedata) {
    char* typedatastr = scss_emit(ast->typedata);

    if (typedatastr) {
      str = str_append(&str, typedatastr);
      free(typedatastr);
    }
  }

  if (ast->flags && str) {
    char* flagsstr = emit_spaced_list2(ast->flags);

    if (flagsstr) {
      str = str_append(&str, flagsstr);
      free(flagsstr);
    }
  }

  if (ast->capsulated)
    str = str_append(&str, ")");

  return str ? str : strdup("");
}

char* scss_emit_style_rule(scss_AST_T* ast)
{
  char* str = 0;
  if (ast->body && ast->body->list_value && ast->body->list_value->size) {

    char* footerstr = ast->footer && ast->footer->list_value ? scss_emit(ast->footer) : strdup(" ");

    char* leftstr = ast->list_value ? emit_spaced_list(ast->list_value) : strdup(" ");

    char* bodystr = ast->body ? scss_emit(ast->body) : strdup(" ");

    if (footerstr) {
      char* f = 0;
      f = str_append(&f, "\n");
      f = str_append(&f, footerstr);
      free(footerstr);
      footerstr = f;
    }

    TEMPLATE(style_rule, str, strlen(leftstr) + strlen(bodystr) + strlen(footerstr), leftstr,
             bodystr, footerstr);

    if (bodystr)
      free(bodystr);

    if (footerstr)
      free(footerstr);

    if (leftstr)
      free(leftstr);
  }

  if (ast->siblings) {
    for (unsigned int i = 0; i < ast->siblings->size; i++) {
      scss_AST_T* child = (scss_AST_T*)ast->siblings->items[i];
      char* childstr = 0;
      char* em = scss_emit(child);
      childstr = str_append(&childstr, em);
      str = str_append(&str, childstr);
    }
  }

  return str ? str : strdup("");
}

char* scss_emit_media_query(scss_AST_T* ast)
{
  char* str = 0;

  char* leftstr = ast->list_value ? emit_spaced_list(ast->list_value) : strdup(" ");

  char* bodystr = ast->body ? scss_emit(ast->body) : strdup(" ");

  TEMPLATE(media_query, str, strlen(leftstr) + strlen(bodystr), leftstr, bodystr);

  if (bodystr)
    free(bodystr);

  if (leftstr)
    free(leftstr);

  return str ? str : strdup("");
}

char* scss_emit_prop_dec(scss_AST_T* ast)
{
  char* str = 0;
  char* leftstr = ast->list_value && ast->list_value->size ? emit_spaced_list(ast->list_value)
                  : ast->left                              ? scss_emit(ast->left)
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

char* scss_emit_var(scss_AST_T* ast)
{
  char* str = 0;
  char* name = strdup(ast->name);
  str = str_append(&str, name);

  if (ast->value) {
    str = str_append(&str, ":");
    char* valuestr = scss_emit(ast->value);

    if (valuestr) {
      str = str_append(&str, valuestr);
      str = str_append(&str, ";");
      free(valuestr);
    }
  }

  return str;
}

char* scss_emit_import(scss_AST_T* ast)
{
  if (!ast->value)
    return strdup("");
  char* valuestr = scss_emit(ast->value);

  char* str = 0;

  TEMPLATE(import, str, strlen(valuestr), valuestr);

  str = str_append(&str, "\n");

  return str;
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

  if (ast->token && ast->token->type != SCSS_TOKEN_COLON_COLON)
    str = str_append(&str, " ");
  str = str_append(&str, tokstr);

  if (ast->token && ast->token->type != SCSS_TOKEN_COLON_COLON)
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

      if (child->type == SCSS_AST_IMPORT)
        str = str_append(&str, "\n");

      str = str_append(&str, childstr);

      if (childstr)
        free(childstr);
    }
  }

  return str ? str : strdup("");
}
