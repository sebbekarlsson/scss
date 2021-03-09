#ifndef SCSS_LEXER_H
#define SCSS_LEXER_H
#include "token.h"

typedef struct SCSS_LEXER_STRUCT
{
  char* source;
  unsigned int source_size;
  char c;
  char cstr[2];
  unsigned int i;
  int line;
  const char* filepath;
  scss_token_T* prev_token;
} scss_lexer_T;

scss_lexer_T* init_scss_lexer(char* source, const char* filepath);

void scss_lexer_advance(scss_lexer_T* scss_lexer);

void scss_lexer_skip_whitespace(scss_lexer_T* scss_lexer);

void scss_lexer_skip_comment(scss_lexer_T* scss_lexer);

char scss_lexer_peek(scss_lexer_T* scss_lexer, int offset);

unsigned int scss_lexer_check_regex(scss_lexer_T* scss_lexer);

scss_token_T* scss_lexer_advance_token(scss_lexer_T* scss_lexer, scss_token_T* token);

scss_token_T* scss_lexer_next(scss_lexer_T* scss_lexer);

scss_token_T* scss_lexer_parse_id(scss_lexer_T* scss_lexer);

scss_token_T* scss_lexer_parse_rule(scss_lexer_T* scss_lexer);

scss_token_T* scss_lexer_parse_string(scss_lexer_T* scss_lexer);

scss_token_T* scss_lexer_parse_number(scss_lexer_T* scss_lexer);

scss_token_T* scss_lexer_parse_any(scss_lexer_T* scss_lexer, char stop_char, char stop_char2);

scss_token_T* scss_lexer_switch_id(scss_lexer_T* scss_lexer, scss_token_T* token);

scss_token_T* scss_ret_tok(scss_lexer_T* scss_lexer, scss_token_T* token);

scss_lexer_T* scss_lexer_copy(scss_lexer_T* scss_lexer);

scss_token_T* scss_lexer_peek_next_token(scss_lexer_T* scss_lexer);

void scss_lexer_free(scss_lexer_T* scss_lexer);

#endif
