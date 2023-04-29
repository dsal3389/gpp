#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "config.h"
#include "logging.h"
#include "strbuf.h"
#include "tokenization.h"
#include "langext.h"


#define is_identifier_char(c_) \
    isalpha(c_) || isdigit(c_) || c_ == '_'
#define is_separator_char(c_) \
    strchr(":;[]{}()\\.,\n", c_) != NULL
#define is_operator_char(c_) \
    strchr("|+=-<>*$&^%!/", c_) != NULL
#define is_string_identifier_char(cptr_) \
    (*cptr_ == '"' || *cptr_ == '\'' || *cptr_ == '`')


#define GET_WHOLE_TOKEN(chk_func, type_, var_) \
    for(; *var_ && (chk_func(*var_)); var_++) \
        strbuf_append_char(&token.value, *var_); \
    token.type = type_; \


void tokenctx(struct tokenctx *ctx, const char *origin, enum language language)
{
    ctx->_offset = 0;
    ctx->_language = language;
    strbuf_set(&ctx->_origin, origin);
}

void freetokenctx(struct tokenctx *ctx)
{
    ctx->_offset = 0;
    strbuf_free(&ctx->_origin);
}

struct token *expect_next_token(struct tokenctx *ctx, int skip_space, enum token_type type, const char *filename, int line_num)
{
    struct token *token = next_token(ctx, skip_space, filename, line_num);

    if(token == NULL)
        die(
            "tokenization", "unexpected end of token",
            "foo"
        );

    if(token->type != type)
        die(
            "tokenization", "unexpected token in file %s",
            "%s", filename, ctx->_origin.string
        );
    return token;
}

struct token *next_token(struct tokenctx *ctx, int skip_space, const char *filename, int line_num)
{
    const char *ptr = &ctx->_origin.string[ctx->_offset];
    char string_identifier;
    int escape_identifier;
    static struct token token;

    strbuf_set(&token.value, "");

    if(isspace(*ptr)){
        if(skip_space){
            while(isspace(*ptr)){
                ctx->_offset++;
                ptr++;
            }
        } else {
            GET_WHOLE_TOKEN(isspace, TOKEN_SPACE, ptr);
            goto NEXT_TOKEN_END;
        }
    }

    if(*ptr == 0)
        return NULL;
    
    if (!strncmp(ptr, config.op_prefix, strlen(config.op_prefix))){
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
            case '/':
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
            case '\\':
                strbuf_append_char(&token.value, *ptr++);
                token.type = TOKEN_SEPARATOR;
                break;
            case '"':
            case '\'':
            case '`':
                string_identifier = *ptr;
                escape_identifier = 0;
                do{
                    if(*ptr == '\\'){
                        // check if we need to escape the start string identifier
                        // because it have escape char before
                        while(*ptr == '\\'){
                            strbuf_append_char(&token.value, *ptr++);
                            escape_identifier = !escape_identifier;
                        }

                        // if we shouldn't escape the next identifier, then call `continue`
                        // so the do while loop could check if the current token is
                        // the starting string identifier
                        if(!escape_identifier)
                            continue;
                    }

                    strbuf_append_char(&token.value, *ptr++);
                } while(*ptr && *ptr != string_identifier);

                if(*ptr == 0)
                    die(
                        "tokenization", "string start with no closing at %s",
                        "%d | %s", filename, line_num, ctx->_origin.string
                    );

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

NEXT_TOKEN_END:
    ctx->_offset += token.value.length;
    return &token;
}
