#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

typedef enum {
    LEX_EOF,
    LEX_NEWLINE,
    LEX_BEGIN_PROGRAM,
    LEX_END_PROGRAM,
    LEX_BEGIN_IF,
    LEX_END_IF,
    LEX_SHOW,
    LEX_INT,
    LEX_FLOAT,
    LEX_ID,
    LEX_INT_NUM,
    LEX_FLOAT_NUM,
    LEX_EQ,
    LEX_NEQ,
    LEX_LT,
    LEX_GT,
    LEX_LE,
    LEX_GE,
    LEX_PLUS,
    LEX_MINUS,
    LEX_MULT,
    LEX_DIV,
    LEX_LPAREN,
    LEX_RPAREN,
    LEX_LBRACE,
    LEX_RBRACE,
    LEX_SEMICOLON,
    LEX_ASSIGN,
    LEX_UNKNOWN
} LexTokenType;

typedef struct {
    LexTokenType type;
    char value[32];
} LexToken;

void skip_whitespace(const uint8_t *source, int *position);
LexToken get_next_token(const uint8_t *source, int *position);

#endif