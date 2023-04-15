#ifndef _TOKENIZATION_H_
#define _TOKENIZATION_H_ 1


#include "strbuf.h"
#include "langext.h"


#define TOKEN_CONTEXT(str, lang) { ._offset=0, ._origin=str, ._language=lang }


enum token_type{
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
};


struct token{
    const char *origin;
    enum token_type type;
    strbuf value;
};

struct tokenctx{
    off_t _offset;
    const char *_origin;
    enum language _language;
};


extern struct token *expect_next_token(struct tokenctx *, enum token_type, const char *, unsigned int);
extern struct token *next_token(struct tokenctx *);


#endif
