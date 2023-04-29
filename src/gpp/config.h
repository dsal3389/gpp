#ifndef _CONFIG_H_
#define _CONFIG_H_ 1


/*
    contains the initial configuration
    for `gpp`, the values match the `gpp_config` struct
*/
#define GPP_CONFIG_INIT { \
    .minimize=0,          \
    .logging_level=4,     \
    .follow_links=0,      \
    .op_prefix="#",       \
    .output_dir="pgpp",   \
}


/* the default .gppignore filename */
#define GPP_IGNORE_FILENAME ".gppignore"


/*
    contains all the configuration that can be changed
    in runtime

    gpp_config:
        minimize - if to delete empty lines
        logging_level - set the logging level
        follow_links - if we should resolved entry links
        op_prefix - the prefixes that define that this is a `gpp` line that should be processed
        output_dir - the output directory name that should contain the preprocessed files
*/
struct gpp_config{
    int minimize;
    int logging_level;
    int follow_links;
    char *op_prefix;
    char *output_dir;
};


/*
    used across the program files to check if a unique
    behaviour is required
*/
extern struct gpp_config config;

extern void gpp_config_init();
extern int ignore_path(const char *);


#endif
