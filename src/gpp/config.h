#ifndef _CONFIG_H_
#define _CONFIG_H_ 1


#define GPP_CONFIG_INIT { \
    .minimize=0,          \
    .logging_level=4,     \
    .follow_links=0,      \
    .op_prefix="#",       \
    .output_dir="pgpp",   \
}


#define GPP_IGNORE_FILENAME ".gppignore"


struct gpp_config{
    int minimize;
    int logging_level;
    int follow_links;
    char *op_prefix;
    char *output_dir;
};


extern struct gpp_config config;
extern void gpp_config_init();
extern int ignore_path(const char *);


#endif
