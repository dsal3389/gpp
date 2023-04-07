#include <string.h>

#include "logging.h"
#include "langext.h"


struct extention_info{
    enum language language;
    const char *extls[16];
};


static struct extention_info extention_database[] = {
    { .language=PYTHON,     .extls=PYTHON_EXT },
    { .language=C,          .extls=C_EXT      },
    { .language=JAVASCRIPT, .extls=JAVASCRIPT_EXT },
};


#define EXTENTION_DB_LEN sizeof(extention_database) / sizeof(struct extention_info)


static int extention_in_list(const char *ext, const char **extls)
{
    const char **current_ext = extls;

    while(*current_ext != NULL){
        if(!strcmp(ext, *current_ext))
            return 1;
        current_ext++;
    }
    return 0;
}

/* guess the programming language base on the path extension string */
enum language get_path_language_code(const char *path)
{
    const char *ext = strrchr(path, '.');
    struct extention_info *extinfo = NULL;

    if(ext == NULL || ++ext == 0)
        return LANG_UNKNOWN;

    for(int i=0; i<EXTENTION_DB_LEN; i++){
        extinfo = &extention_database[i];

        if(extention_in_list(ext, extinfo->extls))
            return extinfo->language;
    }

    return LANG_UNKNOWN;
}
