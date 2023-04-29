#ifndef _LANGEXT_H_
#define _LANGEXT_H_ 1


/*
    contains the extension for each language,
    those predefined values will later be used in `langext.c`
    in `extention_database`
*/
#define PYTHON_EXT     { "py", "pyw", NULL }
#define C_EXT          { "c", "h", "cpp", "cc", NULL }
#define JAVASCRIPT_EXT { "js", "jsx", NULL }
#define TYPESCRIPT_EXT { "ts", "tsx", NULL }


enum language{
    LANG_UNKNOWN,
    PYTHON,
    JAVASCRIPT,
    TYPESCRIPT,
    C,
};


extern const char *language_itoa(enum language);
extern enum language get_path_language_code(const char *);


#endif
