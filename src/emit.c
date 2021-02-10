#include "include/emit.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

#define AST scss_AST_T

char* scss_emit(scss_AST_T* ast)
{
  switch (ast->type)
  {
    case AST_STYLE_RULE: return scss_emit_style_rule(ast); break;
    case AST_PROP_DEC: return scss_emit_prop_dec(ast); break;
    case AST_NAME: return scss_emit_string(ast); break;
    case AST_STRING: return scss_emit_name(ast); break;
    case AST_INT: return scss_emit_int(ast); break;
    case AST_FLOAT: return scss_emit_float(ast); break;
    case AST_BINOP: return scss_emit_binop(ast); break;
    case AST_COMPOUND: return scss_emit_compound(ast); break;
    case AST_NOOP: return strdup(""); break;
    default: { printf("Cannot emit `%d`\n", ast->type); exit(1); }
  }

  return 0;
}

char* scss_emit_style_rule(scss_AST_T* ast)
{
  return strdup("style rule");
}

char* scss_emit_prop_dec(scss_AST_T* ast)
{
  return strdup("prop dec");
}

char* scss_emit_name(scss_AST_T* ast)
{
  return strdup("name");
}

char* scss_emit_string(scss_AST_T* ast)
{
  return strdup("string");
}

char* scss_emit_int(scss_AST_T* ast)
{
  return strdup("int");
}

char* scss_emit_float(scss_AST_T* ast)
{
  return strdup("float");
}

char* scss_emit_binop(scss_AST_T* ast)
{
  return strdup("binop");
}

char* scss_emit_compound(scss_AST_T* ast)
{
  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++)
  {
    AST* child = (AST*) ast->list_value->items[i];
    if (!child) continue;

    char* childstr = scss_emit(child);

    if (!childstr) continue;

    str = str_append(&str, childstr);
  }

  return str;
}
