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


/*
    contains token information, 
    like type and string value

    token:
        type - the enum number token type
        value - strbuf that contains the token string value
*/
struct token{
    enum token_type type;
    strbuf value;
};

/*
    this structure is a token context, it saves what was
    the last processed index in the given text, thats how `next_token`
    knows how to continue from where it stopped

    tokenctx:
        _offset - the index that tells from which index to continue tokenizing the given string
        _origin - the string that should be tokenized
        _language - the language enum number, so we can to check for additional
                    tokenization
*/
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
