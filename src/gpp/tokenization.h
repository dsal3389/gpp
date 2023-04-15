#ifndef _TOKENIZATION_H_
#define _TOKENIZATION_H_ 1


#include "strbuf.h"


typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_IDENTIFIER,
    TOKEN_SEPARATOR,
    TOKEN_OPERATOR,
    TOKEN_LITERAL,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_EMPTY,
    TOKEN_SPACE,
    TOKEN_PREPROCESS,
} token_type;


struct token{
    const char *origin;
    token_type type;
    strbuf value;
};

struct tokens_info{
    unsigned int tcount;
    struct token *tokens;
};


extern int tokenize_string(struct token *, const char *);


#endif
