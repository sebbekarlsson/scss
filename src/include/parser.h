#ifndef SCSS_PARSER_H
#define SCSS_PARSER_H
#include "AST.h"
#include "lexer.h"
#include "token.h"
typedef struct SCSS_PARSER_STRUCT
{
  scss_lexer_T* lexer;
  scss_token_T* token;
} scss_parser_T;

scss_parser_T* init_scss_parser(scss_lexer_T* lexer);

void scss_parser_free(scss_parser_T* parser);

void scss_parser_eat(scss_parser_T* parser, int token_type);

scss_AST_T* scss_parser_parse(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_name(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_string(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_int(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_float(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_statement(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_factor(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_term(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_expr(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_compound(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_style_rule_body(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_style_rule_compound(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_style_rule(scss_parser_T* parser);

scss_AST_T* scss_parser_parse_prop_dec(scss_parser_T* parser);
#endif
