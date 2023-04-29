#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "logging.h"
#include "strbuf.h"


#define _STRBUF_ALLOCATE(buf, size) \
    do { \
        if(buf->_allocated < size) \
            increase_strbuf(buf, size); \
    } while(0)

#define _STRBUF_LIST_ALLOCATE(buf, size) \
    do { \
        if(buf->_allocated < size) \
            increase_strbuf_list(buf, size); \
    } while(0)

static void increase_strbuf_list(strbuf_list *bufs, size_t size)
{
    unsigned long allocate_size = STRBUF_LIST_MIN_INCREASE_SIZE*ceil((double)size / (double)STRBUF_LIST_MIN_INCREASE_SIZE);
    unsigned long original_size = bufs->_allocated;

    if(!allocate_size)
        allocate_size = STRBUF_LIST_MIN_INCREASE_SIZE;

    bufs->_allocated = allocate_size;
    bufs->strings = realloc(bufs->strings, sizeof(void*)*bufs->_allocated);

    if(bufs->strings == NULL)
        die(
            "strbuf_list", "adding entries", 
            "error while adding %ld entries to strbuf list", allocate_size
        );

    for(unsigned long i=original_size; i<bufs->_allocated; i++){
        bufs->strings[i] = calloc(1, sizeof(strbuf));

        if(bufs->strings[i] == NULL)
            die(
                "strbuf_list", "allocating entry space", 
                "error accourd while allocating space for entry number %lu", i
            );
    }
}

/*
    set the `strbuf` allocated memory, expand it if required,
    the expended size is in multiplication of chunk size `STRBUF_MIN_INCREASE_SIZE`
*/
static void increase_strbuf(strbuf *buf, size_t size)
{
    unsigned long allocate_size = (STRBUF_MIN_INCREASE_SIZE*ceil((double)size / (double)STRBUF_MIN_INCREASE_SIZE));

    if(allocate_size == 0)
        allocate_size = STRBUF_MIN_INCREASE_SIZE;
    
    // add extra one for null terminator
    allocate_size++;

    buf->_allocated = allocate_size;
    buf->string = realloc(buf->string, sizeof(char)*buf->_allocated);

    if(buf->string == NULL)
       die("strbuf", "allocation", "failed reallocating new size for strbuf");
}

/* 
    set the strbuf string value 
*/
void strbuf_set(strbuf *buf, const char *text)
{
    buf->length = strlen(text);
    _STRBUF_ALLOCATE(buf, buf->length + 1);

    if(buf->length)
        strncpy(buf->string, text, buf->length);
    buf->string[buf->length] = 0;
}

/* 
    appends a string to the end of the string in the given `strbuf` 
*/
void strbuf_append(strbuf *buf, const char *text)
{
    unsigned long text_len = strlen(text);
    unsigned long strbuf_len = buf->length + text_len;

    if(!text_len)
        return;

    _STRBUF_ALLOCATE(buf, strbuf_len);
    strncpy(&buf->string[buf->length], text, text_len);
    buf->length = strbuf_len;
    buf->string[buf->length] = 0;
}

/* 
    pop character from strbuf in the given `index` position 
*/
char strbuf_pop_index(strbuf *buf, int index)
{
    if(index >= buf->length)
        return 0;
    
    char c = buf->string[index];

    memmove(
        &buf->string[index],
        &buf->string[index+1], 
        (buf->length - index)*sizeof(char)
    );

    buf->length--;
    buf->string[buf->length] = 0;
    return c;
}

/* 
    appends a single character to the end of the string
    in the strbuf
*/
void strbuf_append_char(strbuf *buf, unsigned char c)
{
    buf->length++;
    _STRBUF_ALLOCATE(buf, buf->length);
    buf->string[buf->length-1] = c;
    buf->string[buf->length] = 0;
}

/*
    strip the left side of the string, the space
    checking is done via `isspace`
*/
void strbuf_lstrip(strbuf *buf)
{
    if(!buf->length)
        return;

    char *c = buf->string;

    while(isspace(*c))
        c++;

    if(c == buf->string)
        return;

    buf->length -= (c - buf->string);
    memmove(buf->string, c, buf->length+1);
}

/*
    like `strbuf_lstrip` but for the right side
    of the string
*/
void strbuf_rstrip(strbuf *buf)
{
    if(!buf->length)
        return;

    char *c = &buf->string[buf->length-1];
    
    while(isspace(*c))
        c--;

    if(c == &buf->string[buf->length-1])
        return;
        
    buf->length = (c - buf->string);
    *(c+1) = 0;
}

/*
    free all the allocated memory inside `strbuf`
*/
void strbuf_free(strbuf *buf)
{
    buf->length = 0;
    buf->_allocated = 0;

    if(buf->string != NULL){
        free(buf->string);
        buf->string = NULL;
    }
}

/*
    create new `strbuf` at the end of the given `strbuf_list`
    and set to the given text value
*/
void strbuf_list_append(strbuf_list *bufls, const char *text)
{
    _STRBUF_LIST_ALLOCATE(bufls, bufls->length + 1);
    strbuf_set(bufls->strings[bufls->length], text);
    bufls->length++;
}

/*
    pop the `strbuf` at the given string, and freeing its memory
*/
void strbuf_list_pop_index(strbuf_list *bufls, int index)
{
    if(index > bufls->length)
        return;

    // free the poped strbuf, 
    // because we are never gonna see it again :)
    strbuf_free(bufls->strings[index]);

    if(index != bufls->length - 1)
        memmove(
            &bufls->strings[index], 
            &bufls->strings[index+1],
            (bufls->length - index)*sizeof(strbuf*)
        );
        
    bufls->_allocated--;
    bufls->length--;
}

/*
    reads lines from the given stream, each line is an index
    in `strbuf_list`
*/
void strbuf_list_from_stream(strbuf_list *bufls, FILE *stream)
{
    char buffer[1024];
    strbuf *line = NULL;

    // TODO: support reading lines longer then 1024 bytes
    while(fgets(buffer, sizeof(buffer), stream) != NULL){
        strbuf_list_append(bufls, buffer);
        line = bufls->strings[bufls->length-1];

        if(line->length && line->string[line->length-1] == '\n')
            strbuf_pop(line); 
    }
}

/*
    free `strbuf_list` and all the allocated memories
    that used for `strbuf`
*/
void strbuf_list_free(strbuf_list *bufls)
{
    for(unsigned long i=0; i<bufls->_allocated; i++)
        strbuf_free(bufls->strings[i]);

    if(bufls->strings != NULL){
        free(bufls->strings);
        bufls->strings = NULL;
    }
}

