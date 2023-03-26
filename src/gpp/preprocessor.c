#include <stdio.h>
#include <string.h>

#include "logging.h"
#include "preprocessor.h"
#include "strbuf.h"


#define LINEBREAK_CHAR '\\'

#define TOKEN_ONELINER 1
#define TOKEN_FUNCTION 2


struct preprocess_entry{
    const char *path;
    FILE *stream;
    strbuf_list content;
};

struct token{
    strbuf *tokenline;
    int type;
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
        
        if(i + 1 >= pe->content.length)
            die(
                "file %s", "unexpected line break at the end of the file",
                "%d | %s", pe->path, i+1, line->string
            );
        
        // remove the LINEBREAK char
        strbuf_pop(line);

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

void preprocess_entry(const char *path)
{
    struct preprocess_entry pe = {
        .path = path,
        .stream = fopen(path, "r"),
        .content = STRBUF_LIST_INIT,
    };

    if(pe.stream == NULL)
        die("%s", "io", "couldn't open file %s", path, path);

    strbuf_list_from_stream(&pe.content, pe.stream);
    merge_continued_lines(&pe);

    // for(int i=0; i<pe.content.length; i++)
    //     lpdebug("%03d   | %s", i, pe.content.strings[i]->string);

    fclose(pe.stream);
}
