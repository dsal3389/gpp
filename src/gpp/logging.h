#ifndef _LOGGING_H_
#define _LOGGING_H_ 1


#include <stdio.h>
#include <stdlib.h>

#include "ansi.h"
#include "config.h"


#define LOGGING_CRITICAL_LEVEL      50
#define LOGGING_ERROR_LEVEL         40
#define LOGGING_WARNING_LEVEL       30
#define LOGGING_INFORMATIONAL_LEVEL 20
#define LOGGING_DEBUG_LEVEL         10
#define LOGGING_QUIET_LEVEL         0


#define STRINGIZING(x) #x
#define STR(x) STRINGIZING(x)


#define logprintf(...) \
    if(config.logging_level != LOGGING_QUIET_LEVEL) printf(__VA_ARGS__)

#define logeprintf(...) \
    if(config.logging_level != LOGGING_QUIET_LEVEL) fprintf(stderr, __VA_ARGS__)

#define __logging_fmt(prefix, type, suffix, message) \
    ANSI_BOLD_TEXT(prefix ": " type ": " suffix) "\n    " message "\n\n"

#define lpdebug_fmt(filename, linenum, message) \
    filename "#L" linenum  ": " ANSI_COLOR_TEXT(BG_GREEN, "debug") ": " message "\n"
#define lpinfo_fmt(prefix, suffix, message) \
    __logging_fmt(prefix, ANSI_COLOR_TEXT(BRIGHT_GREEN, "info"), suffix, message)
#define lpwarn_fmt(prefix, suffix, message) \
    __logging_fmt(prefix, ANSI_COLOR_TEXT(BRIGHT_YELLOW, "warning"), suffix, message)
#define lperror_fmt(prefix, suffix, message) \
    __logging_fmt(prefix, ANSI_COLOR_TEXT(BRIGHT_RED, "error"), suffix, message)
#define lpfatal_fmt(prefix, suffix, message) \
    __logging_fmt(prefix, ANSI_COLOR_TEXT(RED, "fatal"), suffix, message)

#define lpdebug(message, ...) \
    if(config.logging_level >= LOGGING_DEBUG_LEVEL) \
        logprintf(lpdebug_fmt(__FILE__, STR(__LINE__), message) __VA_OPT__(,) __VA_ARGS__);
#define lpinfo(prefix, suffix, message, ...) \
    if(config.logging_level <= LOGGING_INFORMATIONAL_LEVEL) \
        logprintf(lpinfo_fmt(prefix, suffix, message) __VA_OPT__(,) __VA_ARGS__)
#define lpwarn(prefix, suffix, message, ...) \
    if(config.logging_level <= LOGGING_WARNING_LEVEL) \
        logeprintf(lpwarn_fmt(prefix, suffix, message) __VA_OPT__(,) __VA_ARGS__)
#define lperror(prefix, suffix, message, ...) \
    if(config.logging_level <= LOGGING_ERROR_LEVEL) \
        logeprintf(lperror_fmt(prefix, suffix, message) __VA_OPT__(,) __VA_ARGS__)

#define die(prefix, suffix, message, ...) do { \
        if(config.logging_level <= LOGGING_CRITICAL_LEVEL) \
            logeprintf(lpfatal_fmt(prefix, suffix, message) __VA_OPT__(,) __VA_ARGS__); \
        exit(EXIT_FAILURE); \
    } while(0)
    

#endif
