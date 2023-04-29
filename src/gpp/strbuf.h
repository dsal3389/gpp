#ifndef _STRBUF_H_
#define _STRBUF_H_ 1


#include <stdio.h>


/* 
    those predefined values should be used on every new
    `strbuf` or `strbuf_list` variable
*/
#define STRBUF_INIT      { ._allocated=0, .length=0, .string=NULL  }
#define STRBUF_LIST_INIT { ._allocated=0, .length=0, .strings=NULL }

/*
    define the memory chunk size that should be allocated on each
    request from `strbuf` and `strbuf_list` for more memory
*/
#define STRBUF_MIN_INCREASE_SIZE 64
#define STRBUF_LIST_MIN_INCREASE_SIZE 8


/*
    contains the dynamic string information, like the
    length and the string itself, the `_allocated` defines how
    much bytes are actually allocated to this string before
    the need to request more memory with `calloc` `malloc` and `realloc`

    typedef strbuf:
        _allocated - how many bytes are allocated for the string before the need to allocate more
        length - the stored string length
        string - the actual string value
*/
typedef struct{
    unsigned long _allocated;
    unsigned long length;
    char *string;
} strbuf;


/*
    contains a list of `strbuf`, the `length` is the count of 
    how many elements are in the list, `_allocate` defines how many
    `strbufs` it can contain before calling `malloc` `calloc` or `realloc`

    typdef strbuf_list:
        _allocated - how many strbufs the current list can contain before the need to expand memory
        length - how many `strbufs` currently inside the list
        strings - list of pointers, each pointing to `strbuf`

*/
typedef struct{
    unsigned long _allocated;
    unsigned long length;
    strbuf **strings;
} strbuf_list;


extern void strbuf_free(strbuf *);
extern void strbuf_set(strbuf *, const char *);
extern void strbuf_append(strbuf *, const char *);
extern void strbuf_append_char(strbuf *, unsigned char);
extern char strbuf_pop_index(strbuf *, int);
extern void strbuf_lstrip(strbuf *);
extern void strbuf_rstrip(strbuf *);


extern void strbuf_list_append(strbuf_list *, const char *);
extern void strbuf_list_from_stream(strbuf_list *, FILE *);
extern void strbuf_list_pop_index(strbuf_list *, int);
extern void strbuf_list_free(strbuf_list *);

/*
    simple macros that are used as functions, those are
    better for code readability
*/
#define strbuf_strip(buf) do { \
        strbuf_lstrip(buf); \
        strbuf_rstrip(buf); \
    } while(0)
#define strbuf_pop(buf)        strbuf_pop_index(buf, buf->length-1)
#define strbuf_list_pop(bufls) strbuf_list_pop_index(bufls, bufls->length-1)


#endif
