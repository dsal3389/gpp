#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "strbuf.h"
#include "logging.h"
#include "preprocessor.h"
#include "tokenization.h"
#include "langext.h"


#define LINEBREAK_CHAR '\\'


struct preprocess_entry{
    enum language language;
    const char *path;
    strbuf_list content;
};


void merge_continued_lines(struct preprocess_entry *pe)
{
    strbuf *line, *next = NULL;
    char *first_brk = NULL, *last_brk = NULL;
    int merge = 1;

    // go over the lines in reverse order
    for(int i=pe->content.length-1; i >= 0; i--, merge=1){
        line = pe->content.strings[i];
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
        
        if(i + 1 >= pe->content.length){
            lpinfo(
                "file %s", "unexpected line break at the end of the file",
                "%d | %s", pe->path, i+1, line->string
            );
            continue;
        }
        
        // remove the LINEBREAK char
        // strbuf_pop(line);

        // get one line below, so we can merge
        // it into the current line
        next = pe->content.strings[i+1];

        if(next->length)
            // if the line below doesn't have any information
            // or string, in it, we can just delete it from the
            // content list
            strbuf_append(line, next->string);
        strbuf_list_pop_index(&pe->content, i+1);
    }
}


void tokenize_lines(struct preprocess_entry *pe)
{
    const char *line = NULL;
    struct token *token;

    for(int i=0; i<pe->content.length; i++){
        line = pe->content.strings[i]->string;
        struct tokenctx ctx = TOKEN_CONTEXT(line, pe->language);

        token = next_token(&ctx);
        if(token != NULL && token->type == TOKEN_PREPROCESS){
            token = expect_next_token(&ctx, TOKEN_SPACE, pe->path, i+1);
            printf("ttoken %s\n", token->value.string);
        }

        // while((token=next_token(&ctx)) != NULL){
        //     printf("token %s\n", token->value.string);
        // }
    }
}

void preprocess_entry(const char *path)
{
    FILE *stream = fopen(path, "r");
    struct preprocess_entry pe = {
        .path = path,
        .content = STRBUF_LIST_INIT,
        .language = get_path_language_code(path)
    };

    lpdebug(
        "preprocessing entry information: path=%s, language=%s"
    , pe.path, language_itoa(pe.language));

    if(stream == NULL)
        die("%s", "io", "couldn't open file %s", path, path);
    
    // LANG_UNKNOWN returned by `get_path_language_code`
    if(pe.language == LANG_UNKNOWN)
        lpwarn(
            "%s", "support programming langauge wasn't detected",
            "behaviour may be unexpected", pe.path
        );

    strbuf_list_from_stream(&pe.content, stream);
    merge_continued_lines(&pe);
    tokenize_lines(&pe);
    
    strbuf_list_free(&pe.content);
    fclose(stream);
}
