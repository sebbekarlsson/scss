#include "include/parser.h"
#include "include/AST.h"
#include "include/garbage.h"
#include <stdio.h>
#include <string.h>

#define AST scss_AST_T
#define EAT(token_type) scss_parser_eat(parser, token_type)

extern scss_garbage_T* GARBAGE;

scss_parser_T* init_scss_parser(scss_lexer_T* lexer)
{
  scss_parser_T* parser = calloc(1, sizeof(struct SCSS_PARSER_STRUCT));
  parser->lexer = lexer;
  parser->token = scss_lexer_next(lexer);

  return parser;
}

void scss_parser_free(scss_parser_T* parser)
{
  free(parser);
}

void scss_parser_eat(scss_parser_T* parser, int token_type)
{
  if (parser->token->type != token_type) {
    printf("[Parser.eat]: Unexpected token `%s` (%d), was expecting (%d)\n", parser->token->value,
           parser->token->type, token_type);
    exit(1);
  } else {
    parser->token = scss_lexer_next(parser->lexer);
  }
}

/** ==== factorials ==== */

AST* scss_parser_parse_style_rule(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_STYLE_RULE);

  AST* b = 0;

  while (parser->token->type != SCSS_TOKEN_LBRACE && parser->token->type != SCSS_TOKEN_COLON &&
         parser->token->type != SCSS_TOKEN_EOF) {
    AST* child = scss_parser_parse_expr(parser);
    if (!b)
      b = child;
    list_push(ast->list_value, child);
  }

  AST* c = 0;

  AST* prev = b;
  while (parser->token->type == SCSS_TOKEN_COLON) {
    EAT(SCSS_TOKEN_COLON);
    AST* child = scss_parser_parse_expr(parser);
    if (!c)
      c = child;

    if (prev && child->type == SCSS_AST_CALL) {
      list_push(prev->options, child);
    } else {
      list_push(ast->list_value, child);
    }

    prev = child;
  }

  if (parser->token->type == SCSS_TOKEN_LBRACE) {
    EAT(SCSS_TOKEN_LBRACE);
    if (parser->token->type != SCSS_TOKEN_RBRACE) {
      ast->body = scss_parser_parse_style_rule_body(parser);
    }

    EAT(SCSS_TOKEN_RBRACE);
  } else if (c) {
    ast->type = SCSS_AST_PROP_DEC;
    ast->left = b;
    if (ast->list_value)
      list_free_shallow(ast->list_value);
    ast->list_value = 0;
    ast->value = c;

    if (parser->token->type == SCSS_TOKEN_SEMI)
      EAT(SCSS_TOKEN_SEMI);

    return ast;
  }

  return ast;
}

AST* scss_parser_parse_prop_dec(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_PROP_DEC);

  AST* child = scss_parser_parse_expr(parser);
  list_push(ast->list_value, child);

  EAT(SCSS_TOKEN_COLON);
  ast->value = scss_parser_parse_expr(parser);
  EAT(SCSS_TOKEN_SEMI);

  return ast;
}

AST* scss_parser_parse_name(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_NAME);
  ast->name = strdup(parser->token->value);
  scss_parser_eat(parser, SCSS_TOKEN_ID);

  if (parser->token->type == SCSS_TOKEN_LPAREN) {
    EAT(SCSS_TOKEN_LPAREN);
    ast->type = SCSS_AST_CALL;

    if (parser->token->type != SCSS_TOKEN_RPAREN) {
      AST* child = scss_parser_parse_statement(parser);
      list_push(ast->args, child);

      while (parser->token->type == SCSS_TOKEN_COMMA) {
        if (parser->token->type == SCSS_TOKEN_COMMA)
          EAT(SCSS_TOKEN_COMMA);

        child = scss_parser_parse_statement(parser);
        list_push(ast->args, child);
      }
    }

    EAT(SCSS_TOKEN_RPAREN);
  }

  return ast;
}

AST* scss_parser_parse_rule(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_NAME);
  ast->name = strdup(parser->token->value);
  scss_parser_eat(parser, SCSS_TOKEN_RULE);

  if (parser->token->type == SCSS_TOKEN_LPAREN) {
    EAT(SCSS_TOKEN_LPAREN);
    ast->type = SCSS_AST_CALL;

    if (parser->token->type != SCSS_TOKEN_RPAREN) {
      AST* child = scss_parser_parse_factor(parser);
      list_push(ast->args, child);

      while (parser->token->type == SCSS_TOKEN_COMMA) {
        if (parser->token->type == SCSS_TOKEN_COMMA)
          EAT(SCSS_TOKEN_COMMA);

        child = scss_parser_parse_factor(parser);
        list_push(ast->args, child);
      }
    }

    EAT(SCSS_TOKEN_RPAREN);
  }

  return ast;
}

AST* scss_parser_parse_var(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_VAR);
  ast->name = strdup(parser->token->value);
  scss_parser_eat(parser, SCSS_TOKEN_VAR);

  if (parser->token->type == SCSS_TOKEN_LPAREN) {
    EAT(SCSS_TOKEN_LPAREN);
    ast->type = SCSS_AST_CALL;

    if (parser->token->type != SCSS_TOKEN_RPAREN) {
      AST* child = scss_parser_parse_factor(parser);
      list_push(ast->args, child);

      while (parser->token->type == SCSS_TOKEN_COMMA) {
        if (parser->token->type == SCSS_TOKEN_COMMA)
          EAT(SCSS_TOKEN_COMMA);

        child = scss_parser_parse_factor(parser);
        list_push(ast->args, child);
      }
    }

    EAT(SCSS_TOKEN_RPAREN);
  }

  return ast;
}

scss_AST_T* scss_parser_parse_import(scss_parser_T* parser)
{
  EAT(SCSS_TOKEN_IMPORT);
  AST* ast = init_scss_ast(SCSS_AST_IMPORT);
  ast->value = scss_parser_parse_factor(parser);
  return ast;
}

AST* scss_parser_parse_string(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_STRING);
  ast->string_value = strdup(parser->token->value);
  scss_parser_eat(parser, SCSS_TOKEN_STRING);
  return ast;
}

AST* scss_parser_parse_int(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_INT);
  ast->int_value = atoi(parser->token->value);
  ast->string_value = strdup(parser->token->value);
  scss_parser_eat(parser, SCSS_TOKEN_INT);
  return ast;
}

AST* scss_parser_parse_float(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_FLOAT);
  ast->float_value = atof(parser->token->value);
  ast->string_value = strdup(parser->token->value);
  scss_parser_eat(parser, SCSS_TOKEN_FLOAT);
  return ast;
}

AST* scss_parser_parse_factor(scss_parser_T* parser)
{
  AST* left = 0;

  if (parser->token->type == SCSS_TOKEN_LPAREN) {
    EAT(SCSS_TOKEN_LPAREN);
    left = scss_parser_parse_style_rule_body(parser);
    left->capsulated = 1;
    EAT(SCSS_TOKEN_RPAREN);
    return left;
  }

  switch (parser->token->type) {
    case SCSS_TOKEN_RULE: left = scss_parser_parse_rule(parser); break;
    case SCSS_TOKEN_VAR: left = scss_parser_parse_var(parser); break;
    case SCSS_TOKEN_ID: left = scss_parser_parse_name(parser); break;
    case SCSS_TOKEN_STRING: left = scss_parser_parse_string(parser); break;
    case SCSS_TOKEN_INT: left = scss_parser_parse_int(parser); break;
    case SCSS_TOKEN_FLOAT: left = scss_parser_parse_float(parser); break;
    default: left = init_scss_ast(SCSS_AST_NOOP); break;
  }

  if (left && (left->type == SCSS_AST_INT || left->type == SCSS_AST_FLOAT)) {
    if (parser->token->type == SCSS_TOKEN_ID) {
      left->typedata = scss_parser_parse_factor(parser);
    }

    if (parser->token->type != SCSS_TOKEN_SEMI && parser->token->type != SCSS_TOKEN_LBRACE) {
      if (!left->flags)
        left->flags = init_list(sizeof(AST*));

      while (parser->token->type != SCSS_TOKEN_SEMI && parser->token->type != SCSS_TOKEN_LBRACE &&
             parser->token->type == SCSS_TOKEN_ID) {
        list_push(left->flags, scss_parser_parse_factor(parser));
      }
    }
  }

  return left;
}

/** ==== terms ==== **/

AST* scss_parser_parse_term(scss_parser_T* parser)
{
  AST* left = scss_parser_parse_factor(parser);

  return left;
}

/** ==== expression ==== **/

AST* scss_parser_parse_expr(scss_parser_T* parser)
{
  AST* left = scss_parser_parse_term(parser);

  while (left &&
         (parser->token->type == SCSS_TOKEN_GT || parser->token->type == SCSS_TOKEN_LT ||
          parser->token->type == SCSS_TOKEN_TILDE || parser->token->type == SCSS_TOKEN_PLUS ||
          parser->token->type == SCSS_TOKEN_AND)) {
    AST* binop = init_scss_ast(SCSS_AST_BINOP);
    binop->left = left;
    binop->token = scss_token_clone(parser->token);
    EAT(parser->token->type);
    binop->right = scss_parser_parse_expr(parser);
    left = binop;
  }

  while (left && parser->token->type == SCSS_TOKEN_COMMA) {
    AST* binop = init_scss_ast(SCSS_AST_BINOP);
    binop->left = left;
    binop->token = scss_token_clone(parser->token);
    EAT(parser->token->type);
    binop->right = scss_parser_parse_expr(parser);
    left = binop;
  }

  return left;
}

/** ==== statement ==== **/

AST* scss_parser_parse_statement(scss_parser_T* parser)
{
  AST* left = 0;

  switch (parser->token->type) {

    case SCSS_TOKEN_AND:
    case SCSS_TOKEN_STRING:
    case SCSS_TOKEN_RULE:
    case SCSS_TOKEN_VAR:
    case SCSS_TOKEN_ID: left = scss_parser_parse_style_rule(parser); break;
    case SCSS_TOKEN_IMPORT: left = scss_parser_parse_import(parser); break;
    default: { /* noop */
    } break;
  }

  if (parser->token->type == SCSS_TOKEN_COLON && left) {
    EAT(SCSS_TOKEN_COLON);
    AST* ast_prop = init_scss_ast(SCSS_AST_PROP_DEC);
    ast_prop->left = left;
    if (left->name) {
      ast_prop->name = strdup(left->name);
    }
    ast_prop->value = scss_parser_parse_expr(parser);
    left = ast_prop;
  }

  if (!left) {
    printf("[Parser.statement]: Unexpected token `%s` (%d)\n", parser->token->value,
           parser->token->type);
    exit(1);
  }

  return left;
}

/** ==== compound ==== */

AST* scss_parser_parse_compound(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_COMPOUND);

  AST* child = scss_parser_parse_statement(parser);
  list_push(ast->list_value, child);

  while (parser->token->type != SCSS_TOKEN_EOF && child->type != SCSS_AST_NOOP) {
    if (parser->token->type == SCSS_TOKEN_SEMI)
      EAT(SCSS_TOKEN_SEMI);

    child = scss_parser_parse_statement(parser);
    list_push(ast->list_value, child);
  }

  return ast;
}

AST* scss_parser_parse_style_rule_body(scss_parser_T* parser)
{
  AST* ast = init_scss_ast(SCSS_AST_COMPOUND);

  AST* child = scss_parser_parse_statement(parser);
  list_push(ast->list_value, child);

  while (parser->token->type != SCSS_TOKEN_RBRACE && parser->token->type != SCSS_TOKEN_EOF &&
         parser->token->type != SCSS_TOKEN_RPAREN) {
    child = scss_parser_parse_statement(parser);
    list_push(ast->list_value, child);
  }

  return ast;
}

/**
 * Entry point
 */
AST* scss_parser_parse(scss_parser_T* parser)
{
  return scss_parser_parse_compound(parser);
}
