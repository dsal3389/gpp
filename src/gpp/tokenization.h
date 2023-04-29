#ifndef _TOKENIZATION_H_
#define _TOKENIZATION_H_ 1


#include "strbuf.h"
#include "langext.h"


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
    enum token_type type;
    strbuf value;
};

struct tokenctx{
    off_t _offset;
    strbuf _origin;
    enum language _language;
};


extern void tokenctx(struct tokenctx *, const char *, enum language);
extern void freetokenctx(struct tokenctx *);

extern struct token *expect_next_token(struct tokenctx *, int, enum token_type, const char *, int);
extern struct token *next_token(struct tokenctx *, int, const char *, int);


#endif
