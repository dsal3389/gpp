#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "config.h"
#include "strbuf.h"
#include "logging.h"
#include "preprocessor.h"


static char *progname;


void usage()
{
    logeprintf(
        "%s [flags] ...files\n"
        "\n"
        "options:\n"
        "   -h, --help\tprint this message to STDOUT\n"
        "   -D, --debug\tprint debug messages to STDOUT\n"
        "   -q, --quiet\tdon't print anything to STDOUT, not even critical errors\n"
        "   -l, --follow-links\tfollow link to source files\n"
        "   -p, --prefix\tset preprocessor operation prefix (default `#`)\n"
        "   -o, --output\toutput directory path to put all the postprocessed files in (default `pgpp`)\n"
        "   -m, --minimize\tremove empty lines"
    , progname);
    exit(EXIT_FAILURE);
}


// used only in `parse_argv`, get the flag argument, `dst` is the variable where
// to store the results
#define GET_OPTION_VALUE(dst) do{ \
        if(*argc < 2) \
            die( \
                "arguments", "missing required value for flag", \
                "%s requires a value" \
            , **argv); \
        (*argc)--; \
        (*argv)++; \
        dst = **argv; \
        if(*dst == '-') \
            die( \
                "arguments", "given value to %s", \
                "given value to %s cannot start with `-`" \
            , *((*argv)-1), *((*argv)-1)); \
    } while(0)


void parse_argv(int *argc, char ***argv, strbuf_list *buffs)
{
    while(**argv){
        if(!strcmp(**argv, "-h") || !strcmp(**argv, "--help")) {
            usage();
        } else if(!strcmp(**argv, "-D") || !strcmp(**argv, "--debug")) {
            config.logging_level = LOGGING_DEBUG_LEVEL;
        } else if(!strcmp(**argv, "-l") || !strcmp(**argv, "--follow-links")){
            config.follow_links = 1;
        } else if(!strcmp(**argv, "-q") || !strcmp(**argv, "--quiet")){
            config.logging_level = LOGGING_QUIET_LEVEL;
        } else if(!strcmp(**argv, "-m") || !strcmp(**argv, "--minimize")) {
            config.minimize = 1;
        } else if(!strcmp(**argv, "-p") || !strcmp(**argv, "--prefix")){
            GET_OPTION_VALUE(config.op_prefix);
        } else if(!strcmp(**argv, "-o") || !strcmp(**argv, "--output")){
            GET_OPTION_VALUE(config.output_dir);
        } else {
            if(!strncmp(**argv, "--", 2) || !strncmp(**argv, "-", 1))
                die("arguments", "%s", "unknown argument passed", **argv);
            strbuf_list_append(buffs, **argv);
        }

        (*argc)--;
        (*argv)++;
    }
}

void unfold_directories(strbuf_list *entries, const char *path)
{
    DIR *dir = opendir(path);
    struct dirent *direntry;
    strbuf entrypath = STRBUF_INIT;

    if(dir == NULL)
        die(
            "%s", "opening directory %s", 
            "unexpected error accured when tried to open directory %s"
        , path, path, path);

    while((direntry=readdir(dir)) != NULL){
        if(!strcmp(direntry->d_name, ".") || !strcmp(direntry->d_name, ".."))
            continue;

        strbuf_set(&entrypath, path);
        strbuf_append_char(&entrypath, '/');
        strbuf_append(&entrypath, direntry->d_name);
        strbuf_list_append(entries, entrypath.string);
    }
    
    closedir(dir);
}

void run_on_entries(strbuf_list *entries)
{
    for(int i=0; i<entries->length; i++)
        preprocess_entry(entries->strings[i]->string);
}

void check_entries(strbuf_list *entries)
{
    struct stat entry_stat;
    char *path = NULL;
    int has_error = 0;

    for(int i=0; i<entries->length; i++){
        path = entries->strings[i]->string;
        
        if(ignore_path(path))
            continue;

        if(stat(path, &entry_stat) == -1){
            lperror(
                "%s", "invalid entry path", 
                "invalid entry was given %s, does not exists", path, path
            ); 
            has_error = 1;
            continue;
        }

        switch(entry_stat.st_mode & S_IFMT){
            case S_IFDIR:
                unfold_directories(entries, path);
                strbuf_list_pop_index(entries, i--);  // pop the current index because its a directory path
                break;
            case S_IFLNK:
                if(!config.follow_links){
                    lpinfo(
                        "%s", "link file, ignoring", 
                        "if you don't want %s to ignore links, use the `-l` or `--follow-links` flags"
                    , path, progname);
                    break;
                }
                // TODO: support link followup
                lpinfo(
                    "TODO", "follow links", 
                    "this feature is not ready yet."
                );
            case S_IFREG:
            case S_IFBLK:
                break;
            default:
                die(
                    "%s", "unsupported file type",
                    "please remove this file or add it to .gppignore file"
                , path);
        }
    }

    if(has_error)
        exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    strbuf_list entries = STRBUF_LIST_INIT;
    progname = *argv;
    argc--;
    argv++;

    if(!argc)
        usage();

    parse_argv(&argc, &argv, &entries);

    if(!entries.length)
        die("files", "missing-file-names", "%s requires file paths", progname);
    
    gpp_config_init();
    check_entries(&entries);
    run_on_entries(&entries);
    strbuf_list_free(&entries);
}

