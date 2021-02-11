#ifndef SCSS_TOKEN_H
#define SCSS_TOKEN_H
typedef struct SCSS_TOKEN_STRUCT
{
  enum
  {
    TOKEN_ID,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_QUESTION,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_NOT,
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_PIPE,
    TOKEN_ESCAPE,
    TOKEN_SQUARED,
    TOKEN_TILDE,
    TOKEN_SEMI,
    TOKEN_COLON,
    TOKEN_STRING,
    TOKEN_INT,
    TOKEN_INT_MIN,
    TOKEN_FLOAT,
    TOKEN_HASH,
    TOKEN_ARROW_RIGHT,
    TOKEN_EQUALS,
    TOKEN_EQUALS_EQUALS,
    TOKEN_EQUALS_EQUALS_EQUALS,
    TOKEN_PLUS_EQUALS,
    TOKEN_MINUS_EQUALS,
    TOKEN_NOT_EQUALS,
    TOKEN_NOT_EQUALS_EQUALS,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_SPREAD,
    TOKEN_LT_EQUALS,
    TOKEN_GT_EQUALS,
    TOKEN_AND_AND,
    TOKEN_AND,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_IMPORTANT,
    TOKEN_BLOCK,
    TOKEN_EOF
  } type;
  char* value;
  char c;
} scss_token_T;

scss_token_T* init_token(char* value, int type);

void token_free(scss_token_T* token);

scss_token_T* token_clone(scss_token_T* token);
#endif
