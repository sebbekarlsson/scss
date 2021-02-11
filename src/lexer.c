#include "include/lexer.h"
#include "include/string_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) a > b ? b : a

char *lexer_str(lexer_T *lexer) {
  char *str = calloc(2, sizeof(char));
  str[0] = lexer->c;
  str[1] = '\0';

  return str;
}

lexer_T *init_lexer(char *source, const char *filepath) {
  lexer_T *lexer = calloc(1, sizeof(struct SCSS_LEXER_STRUCT));
  lexer->source = calloc(strlen(source) + 1, sizeof(char));
  strcpy(lexer->source, source);
  lexer->source_size = strlen(source);
  lexer->i = 0;
  lexer->c = lexer->source[lexer->i];
  lexer->cstr[0] = lexer->c;
  lexer->cstr[1] = '\0';
  lexer->line = 1;
  lexer->filepath = filepath;

  return lexer;
}

void lexer_advance(lexer_T *lexer) {
  if (lexer->c == '\n') {
    lexer->line += 1;
  }

  if (lexer->c != '\0' && lexer->i < lexer->source_size) {
    lexer->i += 1;
    lexer->c = lexer->source[lexer->i];

    lexer->cstr[0] = lexer->c;
    lexer->cstr[1] = '\0';
  }
}

void lexer_skip_whitespace(lexer_T *lexer) {
  while ((lexer->c == '\t' || lexer->c == 10 || lexer->c == ' ' ||
          lexer->c == '\n' || lexer->c == '\r') &&
         lexer->c != 0) {
    lexer_advance(lexer);
  }
}

void lexer_skip_comment(lexer_T *lexer) {
  if (lexer->c == '/') {
    if (lexer_peek(lexer, 1) == '/') {
      while (lexer->c != '\n' && lexer->c != 0)
        lexer_advance(lexer);
    } else if (lexer_peek(lexer, 1) == '*') {
      while (1) {
        if (lexer->c == '*' && lexer_peek(lexer, 1) == '/') {
          lexer_advance(lexer);
          lexer_advance(lexer);
          break;
        }

        lexer_advance(lexer);

        if (lexer->c == 0)
          break;
      }
    }
  }

  lexer_skip_whitespace(lexer);
}

char lexer_peek(lexer_T *lexer, int offset) {
  return lexer->source[MIN(lexer->i + offset, lexer->source_size - 1)];
}

scss_token_T *lexer_advance_token(lexer_T *lexer, scss_token_T *token) {
  lexer_advance(lexer);
  return ret_tok(lexer, token);
}

scss_token_T *lexer_next(lexer_T *lexer) {
  while (lexer->c != 0) {
    lexer_skip_whitespace(lexer);
    if (lexer->c == '\t' || lexer->c == 10 || lexer->c == ' ' ||
        lexer->c == '\n' || lexer->c == '\r') {
      lexer_skip_whitespace(lexer);
    }

    if (lexer->c == '/') {
      while (lexer_peek(lexer, 1) == '/' || lexer_peek(lexer, 1) == '*') {
        lexer_skip_comment(lexer);
        lexer_skip_whitespace(lexer);
        continue;
      }
    }

    if (isdigit(lexer->c) ||
        (lexer->c == '.' && isdigit(lexer_peek(lexer, 1)))) {
      return lexer_parse_number(lexer);
    }

    if (isalnum(lexer->c) || lexer->c == '_' || lexer->c == '$') {
      return lexer_parse_id(lexer);
    }

    if (lexer->c == '"' || lexer->c == '\'' || lexer->c == '`') {
      return lexer_parse_string(lexer);
    }

    if (lexer->c == '=') {
      if (lexer_peek(lexer, 1) == '>') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("=>"), TOKEN_ARROW_RIGHT));
        lexer_advance(lexer);
        return tok;
      } else if (lexer_peek(lexer, 1) == '=' && lexer_peek(lexer, 2) == '=') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("==="), TOKEN_EQUALS_EQUALS_EQUALS));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      } else if (lexer_peek(lexer, 1) == '=') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("=="), TOKEN_EQUALS_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '!') {
      if (lexer_peek(lexer, 1) == '=' && lexer_peek(lexer, 2) == '=') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("!=="), TOKEN_NOT_EQUALS_EQUALS));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      } else if (lexer_peek(lexer, 1) == '=') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("!="), TOKEN_NOT_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '+') {
      if (lexer_peek(lexer, 1) == '=') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("+="), TOKEN_PLUS_EQUALS));
        lexer_advance(lexer);
        return tok;
      } else if (lexer_peek(lexer, 1) == '+') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("++"), TOKEN_INCREMENT));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '.') {
      if (lexer_peek(lexer, 1) == '.' && lexer_peek(lexer, 2) == '.') {
        scss_token_T *tok =
            lexer_advance_token(lexer, init_token(strdup("..."), TOKEN_SPREAD));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      if (isalnum(lexer_peek(lexer, 1))) {
        return lexer_parse_id(lexer);
      }
    }

    if (lexer->c == '<') {
      if (lexer_peek(lexer, 1) == '=') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup(">="), TOKEN_LT_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '-') {
      if (lexer_peek(lexer, 1) == '=') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("-="), TOKEN_MINUS_EQUALS));
        lexer_advance(lexer);
        return tok;
      } else if (lexer_peek(lexer, 1) == '-') {
        scss_token_T *tok = lexer_advance_token(
            lexer, init_token(strdup("--"), TOKEN_DECREMENT));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '&') {
      if (lexer_peek(lexer, 1) == '&') {
        scss_token_T *tok =
            lexer_advance_token(lexer, init_token(strdup("&&"), TOKEN_AND_AND));
        return lexer_advance_token(lexer, tok);
      } else {
        scss_token_T *tok =
            lexer_advance_token(lexer, init_token(strdup("&"), TOKEN_AND));
        return tok;
      }
    }

    switch (lexer->c) {
    case '{':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_LBRACE));
    case '}':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_RBRACE));
    case '(':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_LPAREN));
    case ')':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_RPAREN));
    case '[':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_LBRACKET));
    case ']':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_RBRACKET));
    case ';':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_SEMI));
    case '=':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_EQUALS));
    case '<':
      return lexer_advance_token(lexer, init_token(lexer_str(lexer), TOKEN_LT));
    case '>':
      return lexer_advance_token(lexer, init_token(lexer_str(lexer), TOKEN_GT));
    case '.':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_DOT));
    case ',':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_COMMA));
    case ':':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_COLON));
    case '!':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_NOT));
    case '&':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_AND));
    case '+':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_PLUS));
    case '-':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_MINUS));
    case '/':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_DIV));
    case '%':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_MOD));
    case '|':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_PIPE));
    case '*':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_STAR));
    case '?':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_QUESTION));
    case '\\':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_ESCAPE));
    case '#':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_HASH));
    case '^':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_SQUARED));
    case '~':
      return lexer_advance_token(lexer,
                                 init_token(lexer_str(lexer), TOKEN_TILDE));
    case '\0':
      break;
    }

    if (lexer->c != 0) {
      printf("[Lexer]: Unexpected token (%s):%d: `%c` (%d)\n", lexer->filepath,
             lexer->line, lexer->c, (int)lexer->c);
      break;
    }
  }

  return ret_tok(lexer, init_token(lexer->cstr, TOKEN_EOF));
}

scss_token_T *lexer_parse_id(lexer_T *lexer) {
  char *str = strdup("");

  if (lexer->c == '.') {
    str = str_append(&str, lexer->cstr);
    lexer_advance(lexer);
  }

  while (isalnum(lexer->c) || isdigit(lexer->c) || lexer->c == '_' ||
         lexer->c == '$' || lexer->c == '-' || lexer->c == '.') {
    str = str_append(&str, lexer->cstr);
    lexer_advance(lexer);
  }

  return lexer_switch_id(lexer, init_token(str, TOKEN_ID));
}

scss_token_T *lexer_parse_string(lexer_T *lexer) {
  char *str = 0;
  char start = lexer->c;

  lexer_advance(lexer);

  while (lexer->c != 0 && lexer->c != start) {
    if (lexer->c == '\\') {
      str = str_append(&str, "\\");
      lexer_advance(lexer);
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
      continue;
    }
    if (lexer->c == start) {
      break;
    } else {
      if (lexer->c == '"' && start == '\'') {
        str = str_append(&str, "\\");
      }
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
    }
  }

  if (lexer->c == start)
    lexer_advance(lexer);

  scss_token_T *token = init_token(str ? str : strdup(""), TOKEN_STRING);
  token->c = start;
  return ret_tok(lexer, token);
}

scss_token_T *lexer_parse_number(lexer_T *lexer) {
  char *str = 0;
  int type = TOKEN_INT;

  if (lexer->c == '.') {
    str = str_append(&str, "0");
    str = str_append(&str, lexer->cstr);
    lexer_advance(lexer);
  }

  if (isdigit(lexer->c) &&
      (lexer_peek(lexer, 1) == 'e' || lexer_peek(lexer, 1) == 'E')) {
    type = TOKEN_INT_MIN;

    while (isdigit(lexer->c) || lexer->c == 'e' || lexer->c == 'E') {
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
    }
  } else {
    while (isdigit(lexer->c) || lexer->c == 'e') {
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
    }

    if (lexer->c == '.') {
      type = TOKEN_FLOAT;

      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);

      while (isdigit(lexer->c) || lexer->c == 'e') {

        if (lexer->c == 'e' && lexer_peek(lexer, 1) == '+') {
          str = str_append(&str, lexer->cstr);
          str = str_append(&str, "+");
          lexer_advance(lexer);
          lexer_advance(lexer);

          continue;
        }

        str = str_append(&str, lexer->cstr);
        lexer_advance(lexer);
      }
    }
  }

  return ret_tok(lexer, init_token(str, type));
}

scss_token_T *lexer_switch_id(lexer_T *lexer, scss_token_T *token) {
  if (strcmp(token->value, "important") == 0)
    token->type = TOKEN_IMPORTANT;

  return ret_tok(lexer, token);
}

scss_token_T *ret_tok(lexer_T *lexer, scss_token_T *token) {
  if (lexer->prev_token)
    token_free(lexer->prev_token);

  lexer->prev_token = token_clone(token);
  return token;
}

void lexer_free(lexer_T *lexer) {
  if (lexer->prev_token)
    token_free(lexer->prev_token);

  if (lexer->source)
    free(lexer->source);

  free(lexer);
}

lexer_T *lexer_copy(lexer_T *lexer) {
  lexer_T *lex = init_lexer(lexer->source, lexer->filepath);
  lex->c = lexer->c;
  lex->i = lexer->i;
  lex->line = lexer->line;
  lex->prev_token = token_clone(lexer->prev_token);
  memcpy(lex->cstr, lexer->cstr, sizeof(lexer->cstr));

  return lex;
}

scss_token_T *lexer_peek_next_token(lexer_T *lexer) {
  lexer_T *copy = lexer_copy(lexer);
  scss_token_T *next_token = lexer_next(copy);
  lexer_free(copy);

  return next_token;
}
