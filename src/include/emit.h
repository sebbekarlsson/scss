#ifndef SCSS_EMIT_H
#define SCSS_EMIT_H
#include "AST.h"
char* scss_emit(scss_AST_T* ast);
char* scss_emit_style_rule(scss_AST_T* ast);
char* scss_emit_prop_dec(scss_AST_T* ast);
char* scss_emit_call(scss_AST_T* ast);
char* scss_emit_name(scss_AST_T* ast);
char* scss_emit_var(scss_AST_T* ast);
char* scss_emit_import(scss_AST_T* ast);
char* scss_emit_string(scss_AST_T* ast);
char* scss_emit_int(scss_AST_T* ast);
char* scss_emit_float(scss_AST_T* ast);
char* scss_emit_binop(scss_AST_T* ast);
char* scss_emit_compound(scss_AST_T* ast);
#endif
