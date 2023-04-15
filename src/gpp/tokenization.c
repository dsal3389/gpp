#include <string.h>
#include <ctype.h>

#include "config.h"
#include "logging.h"
#include "strbuf.h"
#include "tokenization.h"
#include "langext.h"


#define is_identifier_char(c_) \
    isalpha(c_) || isdigit(c_) || c_ == '_'
#define is_separator_char(c_) \
    strchr(":;[]{}()/\\.,\n", c_) != NULL
#define is_operator_char(c_) \
    strchr("|+=-<>*$&^%!", c_) != NULL
#define is_string_identifier_char(cptr_) \
    (*cptr_ == '"' || *cptr_ == '\'' || *cptr_ == '`')


#define GET_WHOLE_TOKEN(chk_func, type_, var_) \
    for(; chk_func(*var_); var_++) \
        strbuf_append_char(&token.value, *var_); \
    token.type = type_; \


struct token *expect_next_token(struct tokenctx *ctx, enum token_type type, const char *filename, unsigned int lnum)
{
    struct token *token = next_token(ctx);

    if(token->type != type)
        die(
            "tokenization", "unexpected token in file %s",
            "%u | %s", filename, lnum, token->origin
        );
    return token;
}

struct token *next_token(struct tokenctx *ctx)
{
    const char *ptr = &ctx->_origin[ctx->_offset];
    static struct token token;

    if(*ptr == 0)
        return NULL;

    token.origin = ctx->_origin;
    strbuf_set(&token.value, "");

    if(isspace(*ptr)){
        GET_WHOLE_TOKEN(isspace, TOKEN_SPACE, ptr);
    } else if (!strncmp(ptr, config.op_prefix, strlen(config.op_prefix))){
        strbuf_set(&token.value, config.op_prefix);
        token.type = TOKEN_PREPROCESS;
    } else {
        switch(*ptr){
            case 'A'...'Z':
            case 'a'...'z':
            case '_':
                GET_WHOLE_TOKEN(is_identifier_char, TOKEN_IDENTIFIER, ptr);
                break;
            case '0'...'9':
                GET_WHOLE_TOKEN(isdigit, TOKEN_NUMBER, ptr);
                break;
            case '|':
            case '+':
            case '=':
            case '-':
            case '<':
            case '>':
            case '*':
            case '$':
            case '&':
            case '^':
            case '%':
            case '!':
                GET_WHOLE_TOKEN(is_operator_char, TOKEN_OPERATOR, ptr);
                break;
            case '\n':
            case ':':
            case ';':
            case '.':
            case ',':
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '/':
            case '\\':
                strbuf_append_char(&token.value, *ptr++);
                token.type = TOKEN_SEPARATOR;
                break;
            case '"':
            case '\'':
            case '`':
                do{
                    strbuf_append_char(&token.value, *ptr++);
                } while(!is_string_identifier_char(ptr));

                // append the closing quotation mark
                strbuf_append_char(&token.value, *ptr++);
                token.type = TOKEN_STRING;
                break;
            default:
                strbuf_append_char(&token.value, *ptr++);
                token.type = TOKEN_UNKNOWN;
                break;
        }
    }

    ctx->_offset += token.value.length;
    return &token;
}
