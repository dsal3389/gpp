#ifndef _LANGEXT_H_
#define _LANGEXT_H_ 1

#define PYTHON_EXT     { "py", "pyw", NULL }
#define C_EXT          { "c", "h", "cpp", "cc", NULL }
#define JAVASCRIPT_EXT { "js", "jsx", NULL }
#define TYPESCRIPT_EXT { "ts", "tsx". NULL }


enum language{
    LANG_UNKNOWN,
    PYTHON,
    JAVASCRIPT,
    TYPESCRIPT,
    C,
};


extern enum language get_path_language_code(const char *);


#endif