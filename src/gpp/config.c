#include <stdio.h>
#include <string.h>

#include "strbuf.h"
#include "config.h"
#include "logging.h"


struct gpp_config config = GPP_CONFIG_INIT;
strbuf_list ignore_entries = STRBUF_LIST_INIT;


static void read_gppignore_file()
{
    FILE *stream = fopen(GPP_IGNORE_FILENAME, "r");
    strbuf *ignore_entry;

    if(stream == NULL)
        return;
    
    strbuf_list_from_stream(&ignore_entries, stream);

    // go on reverse order
    for(int i=ignore_entries.length-1; i>=0; i--){
        ignore_entry = ignore_entries.strings[i];
        strbuf_strip(ignore_entry);

        // remove emtpry lines or comments from the list
        if(!ignore_entry->length || *ignore_entry->string == '#')
            strbuf_list_pop_index(&ignore_entries, i);
    }

    fclose(stream);
}

int ignore_path(const char *path)
{
    if(!ignore_entries.length)
        return 0;

    // TODO: add globbing support
    for(int i=0; i<ignore_entries.length; i++)
        if(!strcmp(ignore_entries.strings[i]->string, path))
            return 1;
    return 0;
}

void gpp_config_init()
{
    read_gppignore_file();
    lpdebug("gpp config init complete");
}
