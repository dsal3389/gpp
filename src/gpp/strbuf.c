#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "strbuf.h"


#define _STRBUF_ALLOCATE(buf, size) \
    do { \
        if(buf->_allocated <= size) \
            increase_strbuf(buf, size); \
    } while(0)

#define _STRBUF_LIST_ALLOCATE(buf, size) \
    do { \
        if(buf->_allocated <= size) \
            increase_strbuf_list(buf, size); \
    } while (0)


static void increase_strbuf_list(strbuf_list *bufs, size_t size)
{
    if(size <= STRBUF_LIST_MIN_INCREASE_SIZE)
        size = STRBUF_LIST_MIN_INCREASE_SIZE*2;
    
    bufs->_allocated += size;
    bufs->strings = realloc(bufs->strings, sizeof(void*)*bufs->_allocated);

    if(bufs->strings == NULL)
        die(
            "strbuf_list", "adding entries", 
            "error while adding %lu entries to strbuf list", size
        );

    for(int i=(bufs->_allocated - size); i<bufs->_allocated; i++){
        bufs->strings[i] = calloc(1, sizeof(strbuf));

        if(bufs->strings[i] == NULL)
            die(
                "strbuf_list", "allocating entry space", 
                "error accourd while allocating space for entry number%d", i
            );
    }
}

static void increase_strbuf(strbuf *buf, size_t size)
{
    if(size <= STRBUF_MIN_INCREASE_SIZE)
        size = STRBUF_MIN_INCREASE_SIZE*2;

    buf->_allocated += size;
    buf->string = realloc(buf->string, buf->_allocated);

    if(buf->string == NULL)
       die("strbuf", "allocation", "failed reallocating new size for strbuf");
}

void strbuf_set(strbuf *buf, const char *text)
{
    unsigned int text_len = strlen(text);

    _STRBUF_ALLOCATE(buf, buf->length + 1);
    strcpy(buf->string, text);
    buf->length = text_len;
}

void strbuf_append(strbuf *buf, const char *text)
{
    unsigned int text_len = strlen(text);
    unsigned int strbuf_len = buf->length + text_len;

    _STRBUF_ALLOCATE(buf, strbuf_len + 1);
    strcpy(&buf->string[buf->length], text);
    buf->length = strbuf_len;
}

char strbuf_pop_index(strbuf *buf, int index)
{
    if(index > buf->length)
        return 0;
    
    char c = buf->string[index];
    memmove(
        &buf->string[index],
        &buf->string[index+1], 
        (buf->length - index)*sizeof(char)
    );
    buf->length--;
    return c;
}

void strbuf_append_char(strbuf *buf, unsigned char c)
{
    buf->length++;
    _STRBUF_ALLOCATE(buf, buf->length + 1);
    buf->string[buf->length-1] = c;
    buf->string[buf->length] = 0;
}

void strbuf_lstrip(strbuf *buf)
{
    if(!buf->length)
        return;

    char *c = buf->string;
    while(*c == ' ' || *c == '\t')
        c++;
    
    if(c == buf->string)
        return;

    buf->length -= (c - buf->string);
    memmove(buf->string, c, buf->length+1);
}

void strbuf_rstrip(strbuf *buf)
{
    if(!buf->length)
        return;

    char *c = &buf->string[buf->length-1];
    while(*c == ' ' || *c == '\t')
        c--;

    if(c == &buf->string[buf->length-1])
        return;
        
    buf->length = (c - buf->string);
    *(c+1) = 0;
}

void strbuf_free(strbuf *buf)
{
    if(buf->_allocated == 0)
        return;
    
    buf->length = 0;
    buf->_allocated = 0;
    free(buf->string);
}

void strbuf_list_append(strbuf_list *bufls, const char *text)
{
    _STRBUF_LIST_ALLOCATE(bufls, bufls->length + 1);
    strbuf_set(bufls->strings[bufls->length], text);
    bufls->length++;
}

void strbuf_list_pop_index(strbuf_list *bufls, int index)
{
    if(index > bufls->length)
        return;

    memmove(
        &bufls->strings[index], 
        &bufls->strings[index+1],
        (bufls->length - index)*sizeof(strbuf*)
    );
    bufls->length--;
}

void strbuf_list_from_stream(strbuf_list *bufls, FILE *stream)
{
    char buffer[1024];
    strbuf *line = NULL;

    while(fgets(buffer, sizeof(buffer), stream) != NULL){
        strbuf_list_append(bufls, buffer);
        line = bufls->strings[bufls->length-1];

        if(line->length && line->string[line->length-1] == '\n')
            strbuf_pop(line); 
    }
}
