#ifndef _STRBUF_H_
#define _STRBUF_H_ 1


#include <stdio.h>


#define STRBUF_INIT      { ._allocated=0, .length=0, .string=NULL  }
#define STRBUF_LIST_INIT { ._allocated=0, .length=0, .strings=NULL }

#define STRBUF_MIN_INCREASE_SIZE 64
#define STRBUF_LIST_MIN_INCREASE_SIZE 4


typedef struct{
    unsigned int _allocated;
    unsigned int length;
    char *string;
} strbuf;


typedef struct{
    unsigned int _allocated;
    unsigned int length;
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

#define strbuf_strip(buf) do { \
        strbuf_lstrip(buf); \
        strbuf_rstrip(buf); \
    } while(0)
#define strbuf_pop(buf)        strbuf_pop_index(buf, buf->length-1)
#define strbuf_list_pop(bufls) strbuf_list_pop_index(bufls, bufls->length)


#endif
