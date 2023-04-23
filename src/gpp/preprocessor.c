#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "strbuf.h"
#include "logging.h"
#include "preprocessor.h"
#include "tokenization.h"
#include "langext.h"


#define LINEBREAK_CHAR '\\'


struct preprocessctx{
    const char *path;
    strbuf_list content;
    enum language language;
};


void merge_continued_lines(struct preprocessctx *pctx)
{
    strbuf *line, *next = NULL;
    char *first_brk = NULL, *last_brk = NULL;
    int merge = 1;

    // go over the lines in reverse order
    for(int i=pctx->content.length-1; i >= 0; i--, merge=1){
        line = pctx->content.strings[i];
        strbuf_rstrip(line);

        if(
            (first_brk=strchr(line->string, LINEBREAK_CHAR)) == NULL ||
            (last_brk=strrchr(first_brk, LINEBREAK_CHAR)) == NULL
        )
            continue;

        if(*(last_brk+1) != 0)
            continue;

        while(first_brk != last_brk){
            if(*first_brk == LINEBREAK_CHAR && *(first_brk+1) == LINEBREAK_CHAR)
                merge = !merge;
            first_brk++;
        }

        if(!merge)
            continue;
        
        if(i + 1 >= pctx->content.length){
            lpinfo(
                "file %s", "unexpected line break at the end of the file",
                "%d | %s", pctx->path, i+1, line->string
            );
            continue;
        }
        
        // remove the LINEBREAK char
        // strbuf_pop(line);

        // get one line below, so we can merge
        // it into the current line
        next = pctx->content.strings[i+1];

        if(next->length)
            // if the line below doesn't have any information
            // or string, in it, we can just delete it from the
            // content list
            strbuf_append(line, next->string);
        strbuf_list_pop_index(&pctx->content, i+1);
    }
}

void parse_lines(struct preprocessctx *pctx)
{
    struct tokenctx tctx;
    struct token *token = NULL;
    strbuf *line = NULL;

    for(int i=0; i<pctx->content.length; i++){
        line = pctx->content.strings[i];
        tokenctx(&tctx, line->string, pctx->language);
        
        while((token=next_token(&tctx, 1)) != NULL){
            switch(token->type){
                case TOKEN_UNKNOWN:
                    lpwarn(
                        "tokenization", "unexpected token at %s",
                        "%d | %s", pctx->path, i+1, line->string
                    );
                    break;
                case TOKEN_PREPROCESS:
                    lpdebug("here");
                    break;
                case TOKEN_SPACE:
                    lpdebug("space");
                    break;
                default:
                    lpdebug("default (%s)", token->value.string);
                    break;
            }
        }

        freetokenctx(&tctx);
    }
}

void preprocess_entry(const char *path)
{
    FILE *stream = fopen(path, "r");
    struct preprocessctx pctx = {
        .path = path,
        .content = STRBUF_LIST_INIT,
        .language = get_path_language_code(path)
    };

    lpdebug(
        "preprocessing entry information: path=%s, language=%s"
    , pctx.path, language_itoa(pctx.language));

    if(stream == NULL)
        die("%s", "io", "couldn't open file %s", path, path);
    
    // LANG_UNKNOWN returned by `get_path_language_code`
    if(pctx.language == LANG_UNKNOWN)
        lpwarn(
            "%s", "support programming langauge wasn't detected",
            "behaviour may be unexpected", pctx.path
        );

    strbuf_list_from_stream(&pctx.content, stream);
    merge_continued_lines(&pctx);
    parse_lines(&pctx);
    
    strbuf_list_free(&pctx.content);
    fclose(stream);
}
