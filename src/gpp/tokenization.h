#ifndef _TOKENIZATION_H_
#define _TOKENIZATION_H_ 1


#include "strbuf.h"
#include "langext.h"


typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_SEPARATOR,
    TOKEN_OPERATOR,
    TOKEN_LITERAL,
    TOKEN_STRING,
    TOKEN_NUMBER,
} token_type;


struct token{
    const char *origin;
    token_type type;
    strbuf value;
};


extern int tokenize_string(struct token *, const char *, enum language);


#endif
