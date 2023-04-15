#include <string.h>
#include <ctype.h>

#include "config.h"
#include "logging.h"
#include "strbuf.h"
#include "tokenization.h"
#include "langext.h"


#define is_identifier_char(c) \
    isalpha(c) || isdigit(c) || c == '_'
#define is_separator_char(c) \
    strchr(":;[]{}()/\\.,\n", c) != NULL
#define is_operator_char(c) \
    strchr("|+=-<>*$&^%!", c) != NULL
#define is_string_identifier_char(cptr) \
    (*c == '"' || *c == '\'' || *c == '`')


#define GET_WHOLE_TOKEN(chk_func, type_) \
    for(; chk_func(*c); c++) \
        strbuf_append_char(&token->value, *c); \
    token->type = type_; \


int tokenize_string(struct token *tokens_buf, const char *str)
{
    struct token *token = NULL;
    int found_tokens = 0;
    const char *c = str;

    for(; *c; found_tokens++){
        token = &tokens_buf[found_tokens];
        token->origin = str;

        strbuf_set(&token->value, "");

        if(isspace(*c)){
            GET_WHOLE_TOKEN(isspace, TOKEN_SPACE);
            continue;
        }

        if(!strncmp(c, config.op_prefix, strlen(config.op_prefix))){
            strbuf_set(&token->value, config.op_prefix);
            token->type = TOKEN_PREPROCESS;
            c += strlen(config.op_prefix);
            continue;
        }

        switch(*c){
            case 'A'...'Z':
            case 'a'...'z':
            case '_':
                GET_WHOLE_TOKEN(is_identifier_char, TOKEN_IDENTIFIER);
                break;
            case '0'...'9':
                GET_WHOLE_TOKEN(isdigit, TOKEN_NUMBER);
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
                GET_WHOLE_TOKEN(is_operator_char, TOKEN_OPERATOR)
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
                strbuf_append_char(&token->value, *c++);
                token->type = TOKEN_SEPARATOR;
                break;
            case '"':
            case '\'':
            case '`':
                do{
                    strbuf_append_char(&token->value, *c++);
                } while(!is_string_identifier_char(c));

                // append the closing quotation mark
                strbuf_append_char(&token->value, *c++);
                token->type = TOKEN_STRING;
                break;
            default:
                strbuf_append_char(&token->value, *c++);
                token->type = TOKEN_UNKNOWN;
                break;
        }
    }

    return found_tokens;
}
