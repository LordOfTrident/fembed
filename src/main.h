#include <stdio.h>   /* printf, puts, putchar, fopen, fclose, fgetc */
#include <stdlib.h>  /* exit, EXIT_SUCCESS, EXIT_FAILURE, size_t */
#include <string.h>  /* strcmp */
#include <stdbool.h> /* bool, true, false */
#include <stdarg.h>  /* va_list, va_start, va_end, vsnprintf */

#include "platform.h"
#include "config.h"

/*
 * 0.1.0: First release
 *
 */

#ifdef PLATFORM_LINUX
#	define TYPE_COLOR    "\x1b[1m\x1b[91m"
#	define LITERAL_COLOR "\x1b[0m\x1b[96m"
#	define SPECIAL_COLOR "\x1b[1m\x1b[95m"
#	define DEFAULT_COLOR "\x1b[0m"
#else
#	define TYPE_COLOR
#	define LITERAL_COLOR
#	define SPECIAL_COLOR
#	define DEFAULT_COLOR
#endif

#define SPLIT_ON 10

void usage(void);
void version(void);

void error(const char *p_fmt, ...);
void try(const char *p_flag);

void gen_byte_arr(FILE *p_file);
void gen_str_arr(FILE *p_file);
