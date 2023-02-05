#include <stdio.h>   /* stdout, printf, puts, putchar, fopen, fclose, fgetc */
#include <stdlib.h>  /* exit, EXIT_SUCCESS, EXIT_FAILURE, size_t, malloc, free */
#include <string.h>  /* strcmp, strcpy */
#include <stdbool.h> /* bool, true, false */
#include <stdarg.h>  /* va_list, va_start, va_end, vsnprintf */
#include <assert.h>  /* assert */
#include <ctype.h>   /* isalnum */

#define APP_NAME    "fembed"
#define GITHUB_LINK "https://github.com/LordOfTrident/fembed"

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 2

/*
 * 0.1.0: First release
 * 0.1.1: Fix missing [] on byte array
 * 0.1.2: Remove redundant new line escapes in string arrays
 * 1.0.0: Switch to using cargs and colorer libraries, support windows, add an output to file option
 *
 */

#define TYPE_COLOR    COLOR_BRED
#define LITERAL_COLOR COLOR_BCYAN
#define SPECIAL_COLOR COLOR_BMAGENTA
#define COMMENT_COLOR COLOR_GREEN
#define NORMAL_COLOR  COLOR_BWHITE

#define SPLIT_ON 10

void usage(void);
void version(void);

void error(const char *fmt, ...);
void try(const char *flag);

void gen_byte_arr(FILE *file);
void gen_str_arr(FILE *file);
