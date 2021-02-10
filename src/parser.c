#include "include/parser.h"
#include "include/AST.h"
#include <stdio.h>
#include <string.h>

#define AST scss_AST_T
#define EAT(token_type) scss_parser_eat(parser, token_type)

scss_parser_T *init_scss_parser(lexer_T *lexer) {
  scss_parser_T *parser = calloc(1, sizeof(struct SCSS_PARSER_STRUCT));
  parser->lexer = lexer;
  parser->token = lexer_next(lexer);

  return parser;
}

void scss_parser_eat(scss_parser_T *parser, int token_type) {
  if (parser->token->type != token_type) {
    printf("[Parser.eat]: Unexpected token `%s` (%d), was expecting (%d)\n",
           parser->token->value, parser->token->type, token_type);
    exit(1);
  } else {
    parser->token = lexer_next(parser->lexer);
  }
}

/** ==== factorials ==== */

AST *scss_parser_parse_style_rule(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_STYLE_RULE);
  ast->list_value = init_list(sizeof(AST *));

  while (parser->token->type != TOKEN_LBRACE) {
    AST *child = scss_parser_parse_expr(parser);
    list_push(ast->list_value, child);
  }

  EAT(TOKEN_LBRACE);
  if (parser->token->type != TOKEN_RBRACE) {
    ast->body = scss_parser_parse_style_rule_body(parser);
  }

  EAT(TOKEN_RBRACE);

  return ast;
}

AST *scss_parser_parse_prop_dec(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_PROP_DEC);
  ast->list_value = init_list(sizeof(AST *));

  while (parser->token->type != TOKEN_LBRACE &&
         parser->token->type != TOKEN_COLON &&
         parser->token->type != TOKEN_SEMI &&
         parser->token->type != TOKEN_EOF) {
    AST *child = scss_parser_parse_expr(parser);
    list_push(ast->list_value, child);
  }

  if (parser->token->type == TOKEN_COLON) {
    EAT(TOKEN_COLON);
    ast->value = scss_parser_parse_expr(parser);
    EAT(TOKEN_SEMI);
  } else {
    ast->type = AST_STYLE_RULE;
    EAT(TOKEN_LBRACE);
    if (parser->token->type != TOKEN_RBRACE) {
      ast->body = scss_parser_parse_style_rule_body(parser);
    }

    EAT(TOKEN_RBRACE);
  }

  return ast;
}

AST *scss_parser_parse_name(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_NAME);
  ast->name = strdup(parser->token->value);
  scss_parser_eat(parser, TOKEN_ID);
  return ast;
}

AST *scss_parser_parse_string(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_STRING);
  ast->string_value = strdup(parser->token->value);
  scss_parser_eat(parser, TOKEN_STRING);
  return ast;
}

AST *scss_parser_parse_int(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_INT);
  ast->int_value = atoi(parser->token->value);
  ast->string_value = strdup(parser->token->value);
  scss_parser_eat(parser, TOKEN_INT);
  return ast;
}

AST *scss_parser_parse_float(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_FLOAT);
  ast->float_value = atof(parser->token->value);
  ast->string_value = strdup(parser->token->value);
  scss_parser_eat(parser, TOKEN_FLOAT);
  return ast;
}

AST *scss_parser_parse_factor(scss_parser_T *parser) {
  switch (parser->token->type) {
  case TOKEN_ID:
    return scss_parser_parse_name(parser);
    break;
  case TOKEN_STRING:
    return scss_parser_parse_string(parser);
    break;
  case TOKEN_INT:
    return scss_parser_parse_int(parser);
    break;
  case TOKEN_FLOAT:
    return scss_parser_parse_float(parser);
    break;
  default:
    return 0;
    break;
  }

  return init_scss_ast(AST_NOOP);
}

/** ==== terms ==== **/

AST *scss_parser_parse_term(scss_parser_T *parser) {
  AST *left = scss_parser_parse_factor(parser);

  return left;
}

/** ==== expression ==== **/

AST *scss_parser_parse_expr(scss_parser_T *parser) {
  AST *left = scss_parser_parse_term(parser);

  while (left &&
         (parser->token->type == TOKEN_GT || parser->token->type == TOKEN_LT)) {
    AST *binop = init_scss_ast(AST_BINOP);
    binop->left = left;
    binop->token = token_clone(parser->token);
    EAT(parser->token->type);
    binop->right = scss_parser_parse_expr(parser);
    left = binop;
  }

  return left;
}

/** ==== statement ==== **/

AST *scss_parser_parse_statement(scss_parser_T *parser) {
  switch (parser->token->type) {
  case TOKEN_ID:
    return scss_parser_parse_style_rule(parser);
  default: { /* noop */
  } break;
  }
  printf("[Parser.statement]: Unexpected token `%s` (%d)\n",
         parser->token->value, parser->token->type);
  exit(1);

  return 0;
}

/** ==== compound ==== */

AST *scss_parser_parse_compound(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_COMPOUND);
  ast->list_value = init_list(sizeof(AST *));

  AST *child = scss_parser_parse_statement(parser);
  list_push(ast->list_value, child);

  while (parser->token->type != TOKEN_EOF && child->type != AST_NOOP) {
    if (parser->token->type == TOKEN_SEMI)
      EAT(TOKEN_SEMI);

    child = scss_parser_parse_statement(parser);
    list_push(ast->list_value, child);
  }

  return ast;
}

AST *scss_parser_parse_style_rule_body(scss_parser_T *parser) {
  AST *ast = init_scss_ast(AST_COMPOUND);
  ast->list_value = init_list(sizeof(AST *));

  AST *child = scss_parser_parse_prop_dec(parser);
  list_push(ast->list_value, child);

  while (parser->token->type != TOKEN_RBRACE) {
    child = scss_parser_parse_prop_dec(parser);
    list_push(ast->list_value, child);
  }

  return ast;
}

/**
 * Entry point
 */
AST *scss_parser_parse(scss_parser_T *parser) {
  return scss_parser_parse_compound(parser);
}
