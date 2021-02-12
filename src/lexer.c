#include "include/lexer.h"
#include "include/string_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) a > b ? b : a

char* scss_lexer_str(scss_lexer_T* scss_lexer)
{
  char* str = calloc(2, sizeof(char));
  str[0] = scss_lexer->c;
  str[1] = '\0';

  return str;
}

scss_lexer_T* init_scss_lexer(char* source, const char* filepath)
{
  scss_lexer_T* scss_lexer = calloc(1, sizeof(struct SCSS_LEXER_STRUCT));
  scss_lexer->source = strdup(source);
  scss_lexer->source_size = strlen(source);
  scss_lexer->i = 0;
  scss_lexer->c = scss_lexer->source[scss_lexer->i];
  scss_lexer->cstr[0] = scss_lexer->c;
  scss_lexer->cstr[1] = '\0';
  scss_lexer->line = 1;
  scss_lexer->filepath = filepath;

  return scss_lexer;
}

void scss_lexer_advance(scss_lexer_T* scss_lexer)
{
  if (scss_lexer->c == '\n') {
    scss_lexer->line += 1;
  }

  if (scss_lexer->c != '\0' && scss_lexer->i < scss_lexer->source_size) {
    scss_lexer->i += 1;
    scss_lexer->c = scss_lexer->source[scss_lexer->i];

    scss_lexer->cstr[0] = scss_lexer->c;
    scss_lexer->cstr[1] = '\0';
  }
}

void scss_lexer_skip_whitespace(scss_lexer_T* scss_lexer)
{
  while ((scss_lexer->c == '\t' || scss_lexer->c == 10 || scss_lexer->c == ' ' ||
          scss_lexer->c == '\n' || scss_lexer->c == '\r') &&
         scss_lexer->c != 0) {
    scss_lexer_advance(scss_lexer);
  }
}

void scss_lexer_skip_comment(scss_lexer_T* scss_lexer)
{
  if (scss_lexer->c == '/') {
    if (scss_lexer_peek(scss_lexer, 1) == '/') {
      while (scss_lexer->c != '\n' && scss_lexer->c != 0)
        scss_lexer_advance(scss_lexer);
    } else if (scss_lexer_peek(scss_lexer, 1) == '*') {
      while (1) {
        if (scss_lexer->c == '*' && scss_lexer_peek(scss_lexer, 1) == '/') {
          scss_lexer_advance(scss_lexer);
          scss_lexer_advance(scss_lexer);
          break;
        }

        scss_lexer_advance(scss_lexer);

        if (scss_lexer->c == 0)
          break;
      }
    }
  }

  scss_lexer_skip_whitespace(scss_lexer);
}

char scss_lexer_peek(scss_lexer_T* scss_lexer, int offset)
{
  return scss_lexer->source[MIN(scss_lexer->i + offset, scss_lexer->source_size - 1)];
}

scss_token_T* scss_lexer_advance_token(scss_lexer_T* scss_lexer, scss_token_T* token)
{
  scss_lexer_advance(scss_lexer);
  return scss_ret_tok(scss_lexer, token);
}

scss_token_T* scss_lexer_next(scss_lexer_T* scss_lexer)
{
  while (scss_lexer->c != 0) {
    scss_lexer_skip_whitespace(scss_lexer);
    if (scss_lexer->c == '\t' || scss_lexer->c == 10 || scss_lexer->c == ' ' ||
        scss_lexer->c == '\n' || scss_lexer->c == '\r') {
      scss_lexer_skip_whitespace(scss_lexer);
    }

    if (scss_lexer->c == '/') {
      while (scss_lexer_peek(scss_lexer, 1) == '/' || scss_lexer_peek(scss_lexer, 1) == '*') {
        scss_lexer_skip_comment(scss_lexer);
        scss_lexer_skip_whitespace(scss_lexer);
        continue;
      }
    }

    if (isdigit(scss_lexer->c) ||
        (scss_lexer->c == '.' && isdigit(scss_lexer_peek(scss_lexer, 1)))) {
      return scss_lexer_parse_number(scss_lexer);
    }

    if (isalnum(scss_lexer->c) || scss_lexer->c == '_' || scss_lexer->c == '$') {
      return scss_lexer_parse_id(scss_lexer);
    }

    if (scss_lexer->c == '"' || scss_lexer->c == '\'' || scss_lexer->c == '`') {
      return scss_lexer_parse_string(scss_lexer);
    }

    if (scss_lexer->c == '=') {
      if (scss_lexer_peek(scss_lexer, 1) == '>') {
        scss_token_T* tok = scss_lexer_advance_token(
          scss_lexer, init_scss_token(strdup("=>"), SCSS_TOKEN_ARROW_RIGHT));
        scss_lexer_advance(scss_lexer);
        return tok;
      } else if (scss_lexer_peek(scss_lexer, 1) == '=' && scss_lexer_peek(scss_lexer, 2) == '=') {
        scss_token_T* tok = scss_lexer_advance_token(
          scss_lexer, init_scss_token(strdup("==="), SCSS_TOKEN_EQUALS_EQUALS_EQUALS));
        scss_lexer_advance(scss_lexer);
        return scss_lexer_advance_token(scss_lexer, tok);
      } else if (scss_lexer_peek(scss_lexer, 1) == '=') {
        scss_token_T* tok = scss_lexer_advance_token(
          scss_lexer, init_scss_token(strdup("=="), SCSS_TOKEN_EQUALS_EQUALS));
        scss_lexer_advance(scss_lexer);
        return tok;
      }
    }

    if (scss_lexer->c == '!') {
      if (scss_lexer_peek(scss_lexer, 1) == '=' && scss_lexer_peek(scss_lexer, 2) == '=') {
        scss_token_T* tok = scss_lexer_advance_token(
          scss_lexer, init_scss_token(strdup("!=="), SCSS_TOKEN_NOT_EQUALS_EQUALS));
        scss_lexer_advance(scss_lexer);
        return scss_lexer_advance_token(scss_lexer, tok);
      } else if (scss_lexer_peek(scss_lexer, 1) == '=') {
        scss_token_T* tok = scss_lexer_advance_token(
          scss_lexer, init_scss_token(strdup("!="), SCSS_TOKEN_NOT_EQUALS));
        scss_lexer_advance(scss_lexer);
        return tok;
      }
    }

    if (scss_lexer->c == '+') {
      if (scss_lexer_peek(scss_lexer, 1) == '=') {
        scss_token_T* tok = scss_lexer_advance_token(
          scss_lexer, init_scss_token(strdup("+="), SCSS_TOKEN_PLUS_EQUALS));
        scss_lexer_advance(scss_lexer);
        return tok;
      } else if (scss_lexer_peek(scss_lexer, 1) == '+') {
        scss_token_T* tok =
          scss_lexer_advance_token(scss_lexer, init_scss_token(strdup("++"), SCSS_TOKEN_INCREMENT));
        scss_lexer_advance(scss_lexer);
        return tok;
      }
    }

    if (scss_lexer->c == '.') {
      if (scss_lexer_peek(scss_lexer, 1) == '.' && scss_lexer_peek(scss_lexer, 2) == '.') {
        scss_token_T* tok =
          scss_lexer_advance_token(scss_lexer, init_scss_token(strdup("..."), SCSS_TOKEN_SPREAD));
        scss_lexer_advance(scss_lexer);
        return scss_lexer_advance_token(scss_lexer, tok);
      }
      if (isalnum(scss_lexer_peek(scss_lexer, 1))) {
        return scss_lexer_parse_id(scss_lexer);
      }
    }

    if (scss_lexer->c == '<') {
      if (scss_lexer_peek(scss_lexer, 1) == '=') {
        scss_token_T* tok =
          scss_lexer_advance_token(scss_lexer, init_scss_token(strdup(">="), SCSS_TOKEN_LT_EQUALS));
        scss_lexer_advance(scss_lexer);
        return tok;
      }
    }

    if (scss_lexer->c == '-') {
      if (scss_lexer_peek(scss_lexer, 1) == '=') {
        scss_token_T* tok = scss_lexer_advance_token(
          scss_lexer, init_scss_token(strdup("-="), SCSS_TOKEN_MINUS_EQUALS));
        scss_lexer_advance(scss_lexer);
        return tok;
      } else if (scss_lexer_peek(scss_lexer, 1) == '-') {
        scss_token_T* tok =
          scss_lexer_advance_token(scss_lexer, init_scss_token(strdup("--"), SCSS_TOKEN_DECREMENT));
        scss_lexer_advance(scss_lexer);
        return tok;
      }
    }

    if (scss_lexer->c == '&') {
      if (scss_lexer_peek(scss_lexer, 1) == '&') {
        scss_token_T* tok =
          scss_lexer_advance_token(scss_lexer, init_scss_token(strdup("&&"), SCSS_TOKEN_AND_AND));
        return scss_lexer_advance_token(scss_lexer, tok);
      } else {
        scss_token_T* tok =
          scss_lexer_advance_token(scss_lexer, init_scss_token(strdup("&"), SCSS_TOKEN_AND));
        return tok;
      }
    }

    switch (scss_lexer->c) {
      case '{':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_LBRACE));
      case '}':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_RBRACE));
      case '(':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_LPAREN));
      case ')':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_RPAREN));
      case '[':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_LBRACKET));
      case ']':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_RBRACKET));
      case ';':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_SEMI));
      case '=':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_EQUALS));
      case '<':
        return scss_lexer_advance_token(scss_lexer,
                                        init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_LT));
      case '>':
        return scss_lexer_advance_token(scss_lexer,
                                        init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_GT));
      case '.':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_DOT));
      case ',':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_COMMA));
      case ':':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_COLON));
      case '!':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_NOT));
      case '&':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_AND));
      case '+':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_PLUS));
      case '-':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_MINUS));
      case '/':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_DIV));
      case '%':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_MOD));
      case '|':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_PIPE));
      case '*':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_STAR));
      case '?':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_QUESTION));
      case '\\':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_ESCAPE));
      case '#':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_HASH));
      case '^':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_SQUARED));
      case '~':
        return scss_lexer_advance_token(
          scss_lexer, init_scss_token(scss_lexer_str(scss_lexer), SCSS_TOKEN_TILDE));
      case '\0': break;
    }

    if (scss_lexer->c != 0) {
      printf("[Lexer]: Unexpected token (%s):%d: `%c` (%d)\n", scss_lexer->filepath,
             scss_lexer->line, scss_lexer->c, (int)scss_lexer->c);
      break;
    }
  }

  return scss_ret_tok(scss_lexer, init_scss_token(strdup(scss_lexer->cstr), SCSS_TOKEN_EOF));
}

scss_token_T* scss_lexer_parse_id(scss_lexer_T* scss_lexer)
{
  char* str = strdup("");

  if (scss_lexer->c == '.') {
    str = str_append(&str, scss_lexer->cstr);
    scss_lexer_advance(scss_lexer);
  }

  while (isalnum(scss_lexer->c) || isdigit(scss_lexer->c) || scss_lexer->c == '_' ||
         scss_lexer->c == '$' || scss_lexer->c == '-' || scss_lexer->c == '.') {
    str = str_append(&str, scss_lexer->cstr);
    scss_lexer_advance(scss_lexer);
  }

  return scss_lexer_switch_id(scss_lexer, init_scss_token(str, SCSS_TOKEN_ID));
}

scss_token_T* scss_lexer_parse_string(scss_lexer_T* scss_lexer)
{
  char* str = 0;
  char start = scss_lexer->c;

  scss_lexer_advance(scss_lexer);

  while (scss_lexer->c != 0 && scss_lexer->c != start) {
    if (scss_lexer->c == '\\') {
      str = str_append(&str, "\\");
      scss_lexer_advance(scss_lexer);
      str = str_append(&str, scss_lexer->cstr);
      scss_lexer_advance(scss_lexer);
      continue;
    }
    if (scss_lexer->c == start) {
      break;
    } else {
      if (scss_lexer->c == '"' && start == '\'') {
        str = str_append(&str, "\\");
      }
      str = str_append(&str, scss_lexer->cstr);
      scss_lexer_advance(scss_lexer);
    }
  }

  if (scss_lexer->c == start)
    scss_lexer_advance(scss_lexer);

  scss_token_T* token = init_scss_token(str ? str : strdup(""), SCSS_TOKEN_STRING);
  token->c = start;
  return scss_ret_tok(scss_lexer, token);
}

scss_token_T* scss_lexer_parse_number(scss_lexer_T* scss_lexer)
{
  char* str = 0;
  int type = SCSS_TOKEN_INT;

  if (scss_lexer->c == '.') {
    str = str_append(&str, "0");
    str = str_append(&str, scss_lexer->cstr);
    scss_lexer_advance(scss_lexer);
  }

  if (isdigit(scss_lexer->c) &&
      (scss_lexer_peek(scss_lexer, 1) == 'e' || scss_lexer_peek(scss_lexer, 1) == 'E')) {
    type = SCSS_TOKEN_INT_MIN;

    while (isdigit(scss_lexer->c) || scss_lexer->c == 'e' || scss_lexer->c == 'E') {
      str = str_append(&str, scss_lexer->cstr);
      scss_lexer_advance(scss_lexer);
    }
  } else {
    while (isdigit(scss_lexer->c) || scss_lexer->c == 'e') {
      str = str_append(&str, scss_lexer->cstr);
      scss_lexer_advance(scss_lexer);
    }

    if (scss_lexer->c == '.') {
      type = SCSS_TOKEN_FLOAT;

      str = str_append(&str, scss_lexer->cstr);
      scss_lexer_advance(scss_lexer);

      while (isdigit(scss_lexer->c) || scss_lexer->c == 'e') {

        if (scss_lexer->c == 'e' && scss_lexer_peek(scss_lexer, 1) == '+') {
          str = str_append(&str, scss_lexer->cstr);
          str = str_append(&str, "+");
          scss_lexer_advance(scss_lexer);
          scss_lexer_advance(scss_lexer);

          continue;
        }

        str = str_append(&str, scss_lexer->cstr);
        scss_lexer_advance(scss_lexer);
      }
    }
  }

  return scss_ret_tok(scss_lexer, init_scss_token(str, type));
}

scss_token_T* scss_lexer_switch_id(scss_lexer_T* scss_lexer, scss_token_T* token)
{
  if (strcmp(token->value, "important") == 0)
    token->type = SCSS_TOKEN_IMPORTANT;

  return scss_ret_tok(scss_lexer, token);
}

scss_token_T* scss_ret_tok(scss_lexer_T* scss_lexer, scss_token_T* token)
{
  scss_lexer->prev_token = scss_token_clone(token);
  return token;
}

void scss_lexer_free(scss_lexer_T* scss_lexer)
{
  if (scss_lexer->source)
    free(scss_lexer->source);

  free(scss_lexer);
}

scss_lexer_T* scss_lexer_copy(scss_lexer_T* scss_lexer)
{
  scss_lexer_T* lex = init_scss_lexer(scss_lexer->source, scss_lexer->filepath);
  lex->c = scss_lexer->c;
  lex->i = scss_lexer->i;
  lex->line = scss_lexer->line;
  lex->prev_token = scss_token_clone(scss_lexer->prev_token);
  memcpy(lex->cstr, scss_lexer->cstr, sizeof(scss_lexer->cstr));

  return lex;
}

scss_token_T* scss_lexer_peek_next_token(scss_lexer_T* scss_lexer)
{
  scss_lexer_T* copy = scss_lexer_copy(scss_lexer);
  scss_token_T* next_token = scss_lexer_next(copy);
  scss_lexer_free(copy);

  return next_token;
}
