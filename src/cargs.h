/*
 * https://github.com/LordOfTrident/cargs
 *
 * #define CARGS_IMPLEMENTATION
 *
 */

#ifndef CARGS_H_HEADER_GUARD
#define CARGS_H_HEADER_GUARD

#include <stdio.h>   /* fprintf, stderr, FILE */
#include <stdlib.h>  /* size_t, malloc, free, strtol, strtoull, strtod */
#include <stdbool.h> /* bool, true, false */
#include <string.h>  /* strcmp, strlen, strdup, strcpy, strncpy */
#include <assert.h>  /* assert */

#define CARGS_VERSION_MAJOR 1
#define CARGS_VERSION_MINOR 0
#define CARGS_VERSION_PATCH 0

typedef struct {
	int          c;
	const char **v;
	char       **base;
} args_t;

args_t new_args(int argc, const char **argv);

bool arg_is_flag(     const char *arg);
bool arg_is_flag_long(const char *arg);
bool arg_is_flags_end(const char *arg);

enum {
	ARG_OK = 0,
	ARG_UNKNOWN,
	ARG_MISSING_VALUE,
	ARG_EXPECTED_CSTR,
	ARG_EXPECTED_CHAR,
	ARG_EXPECTED_INT,
	ARG_EXPECTED_SIZE,
	ARG_EXPECTED_FLOAT,
	ARG_EXPECTED_BOOL,
	ARG_OUT_OF_MEM,
};

const char *args_shift(      args_t *a);
int         args_parse_flags(args_t *a, int *where, args_t *stripped);

void flag_cstr( const char *short_name, const char *long_name, const char *desc, char  **var);
void flag_char( const char *short_name, const char *long_name, const char *desc, char   *var);
void flag_int(  const char *short_name, const char *long_name, const char *desc, int    *var);
void flag_size( const char *short_name, const char *long_name, const char *desc, size_t *var);
void flag_float(const char *short_name, const char *long_name, const char *desc, double *var);
void flag_bool( const char *short_name, const char *long_name, const char *desc, bool   *var);

void args_print_flags(FILE *file);
void args_print_usage(FILE *file, const char *app_name, const char *usage);

#endif

#ifdef CARGS_IMPLEMENTATION

args_t new_args(int argc, const char **argv) {
	return (args_t){.c = argc, .v = argv, .base = (char**)argv};
}

bool arg_is_flag(const char *arg) {
	return arg[0] == '-' && !arg_is_flags_end(arg);
}

bool arg_is_flag_long(const char *arg) {
	if (strlen(arg) > 2)
		return arg[0] == '-' && arg[1] == '-';
	else
		return false;
}

bool arg_is_flags_end(const char *arg) {
	return strcmp(arg, "--") == 0;
}

const char *args_shift(args_t *a) {
	if (a->c <= 0)
		return NULL;

	const char *arg = *a->v;

	-- a->c;
	++ a->v;

	return arg;
}

typedef enum {
	FLAG_CSTR = 0,
	FLAG_CHAR,
	FLAG_INT,
	FLAG_SIZE,
	FLAG_FLOAT,
	FLAG_BOOL,
} flag_type_t;

typedef struct {
	flag_type_t  type;

	union {
		char  **cstr;
		char   *ch;
		int    *int_;
		size_t *size;
		double *float_;
		bool   *bool_;
	} as;

	union {
		char  *cstr;
		char   ch;
		int    int_;
		size_t size;
		double float_;
		bool   bool_;
	} def;

	const char *short_name, *long_name, *desc;
} flag_t;

#define FLAGS_CAPACITY 128

flag_t flags[FLAGS_CAPACITY];
size_t flags_count = 0;

static flag_t *get_flag_by_short_name(const char *short_name) {
	if (short_name == NULL)
		return NULL;

	for (size_t i = 0; i < flags_count; ++ i) {
		if (strcmp(flags[i].short_name, short_name) == 0)
			return &flags[i];
	}

	return NULL;
}

static flag_t *get_flag_by_long_name(const char *long_name) {
	if (long_name == NULL)
		return NULL;

	for (size_t i = 0; i < flags_count; ++ i) {
		if (strcmp(flags[i].long_name, long_name) == 0)
			return &flags[i];
	}

	return NULL;
}

void flag_cstr(const char *short_name, const char *long_name, const char *desc, char **var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_CSTR;
	flags[flags_count].as.cstr    = var;
	flags[flags_count].def.cstr   = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_char(const char *short_name, const char *long_name, const char *desc, char *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_CHAR;
	flags[flags_count].as.ch      = var;
	flags[flags_count].def.ch     = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_int(const char *short_name, const char *long_name, const char *desc, int *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_INT;
	flags[flags_count].as.int_    = var;
	flags[flags_count].def.int_   = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;

}

void flag_size(const char *short_name, const char *long_name, const char *desc, size_t *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_SIZE;
	flags[flags_count].as.size    = var;
	flags[flags_count].def.size   = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_float(const char *short_name, const char *long_name, const char *desc, double *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_FLOAT;
	flags[flags_count].as.float_  = var;
	flags[flags_count].def.float_ = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_bool(const char *short_name, const char *long_name, const char *desc, bool *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_BOOL;
	flags[flags_count].as.bool_   = var;
	flags[flags_count].def.bool_  = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

static int flag_set(flag_t *f, const char *val) {
	switch (f->type) {
	case FLAG_CSTR: {
		*f->as.cstr = (char*)malloc(strlen(val) + 1);
		strcpy(*f->as.cstr, val);
	} break;

	case FLAG_CHAR:
		if (strlen(val) != 1)
			return ARG_EXPECTED_CHAR;

		*f->as.ch = val[0];
		break;

	case FLAG_INT: {
		char *ptr;
		int n = (int)strtol(val, &ptr, 10);
		if (*ptr != '\0')
			return ARG_EXPECTED_INT;

		*f->as.int_ = n;
	} break;

	case FLAG_SIZE: {
		char *ptr;
		size_t n = (size_t)strtoull(val, &ptr, 10);
		if (*ptr != '\0')
			return ARG_EXPECTED_SIZE;

		*f->as.size = n;
	} break;

	case FLAG_FLOAT: {
		char *ptr;
		double n = (double)strtod(val, &ptr);
		if (*ptr != '\0')
			return ARG_EXPECTED_FLOAT;

		*f->as.float_ = n;
	} break;

	case FLAG_BOOL:
		if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0)
			*f->as.bool_ = true;
		else if (strcmp(val, "false") == 0 || strcmp(val, "0") == 0)
			*f->as.bool_ = false;
		else
			return ARG_EXPECTED_BOOL;
	}

	return ARG_OK;
}

int args_parse_flags(args_t *a, int *where, args_t *stripped) {
	if (stripped != NULL) {
		stripped->c    = 0;
		stripped->base = (char**)malloc(sizeof(*stripped) * a->c);
		if (stripped->base == NULL)
			return ARG_OUT_OF_MEM;

		stripped->v = (const char**)stripped->base;
	}

	bool flags_end = false;
	for (int i = 0; i < a->c; ++ i) {
		if (arg_is_flags_end(a->v[i]) && !flags_end) {
			if (stripped == NULL)
				return ARG_OK;
			else {
				flags_end = true;
				continue;
			}
		} else if (!arg_is_flag(a->v[i]) || flags_end) {
			if (stripped != NULL)
				stripped->v[stripped->c ++] = a->v[i];

			continue;
		}

		if (where != NULL)
			*where = i;

		bool is_long = arg_is_flag_long(a->v[i]);
		const char *arg = a->v[i] + is_long + 1;

		const char *tmp   = arg;
		size_t      count = 0;
		while (*arg != '\0' && *arg != '=') {
			++ count;
			++ arg;
		}

		char *name = (char*)malloc(count + 1);
		if (name == NULL)
			return ARG_OUT_OF_MEM;

		strncpy(name, tmp, count);
		name[count] = '\0';

		flag_t *flag = is_long? get_flag_by_long_name(name) : get_flag_by_short_name(name);
		if (flag == NULL)
			return ARG_UNKNOWN;

		char *val = NULL;
		if (*arg == '=') {
			++ arg;

			tmp   = arg;
			count = 0;
			while (*arg != '\0') {
				++ count;
				++ arg;
			}

			val = (char*)malloc(count + 1);
			if (val == NULL)
				return ARG_OUT_OF_MEM;

			strncpy(val, tmp, count);
			val[count] = '\0';
		}

		if (val == NULL) {
			if (flag->type == FLAG_BOOL)
				*flag->as.bool_ = true;
			else {
				++ i;
				if (i >= a->c)
					return ARG_MISSING_VALUE;

				val = (char*)a->v[i];

				int err = flag_set(flag, val);
				if (err != ARG_OK)
					return err;
			}
		} else {
			int err = flag_set(flag, val);
			if (err != ARG_OK)
				return err;

			free(val);
		}

		free(name);
	}

	return ARG_OK;
}

void args_print_flags(FILE *file) {
	int longest = 0;
	for (size_t i = 0; i < flags_count; ++ i) {
		flag_t *f = &flags[i];

		int len;
		if (f->short_name == NULL)
			len = snprintf(NULL, 0, "  --%s", f->long_name);
		else if (f->long_name == NULL)
			len = snprintf(NULL, 0, "  -%s", f->short_name);
		else
			len = snprintf(NULL, 0, "  -%s, --%s", f->short_name, f->long_name);

		if (len > longest)
			longest = len;
	}

	for (size_t i = 0; i < flags_count; ++ i) {
		flag_t *f = &flags[i];

		int len;
		if (f->short_name == NULL)
			len = fprintf(file, "  --%s", f->long_name);
		else if (f->long_name == NULL)
			len = fprintf(file, "  -%s", f->short_name);
		else
			len = fprintf(file, "  -%s, --%s", f->short_name, f->long_name);

		for (int i = len; i < longest; ++ i)
			fputc(' ', file);

		fprintf(file, "    %s", f->desc);

		if ((f->type == FLAG_BOOL && !f->def.bool_) ||
		    (f->type == FLAG_CSTR && f->def.cstr == NULL)) {
			fprintf(file, "\n");
			continue;
		}

		fprintf(file, " (default '");
		switch (f->type) {
		case FLAG_CSTR:  fprintf(file, "%s",  f->def.cstr);   break;
		case FLAG_CHAR:  fprintf(file, "%c",  f->def.ch);     break;
		case FLAG_INT:   fprintf(file, "%i",  f->def.int_);   break;
		case FLAG_SIZE:  fprintf(file, "%zu", f->def.size);   break;
		case FLAG_FLOAT: fprintf(file, "%f",  f->def.float_); break;
		case FLAG_BOOL:  fprintf(file, f->def.bool_? "true" : "false"); break;
		}
		fprintf(file, "')\n");
	}
}

void args_print_usage(FILE *file, const char *app_name, const char *usage) {
	fprintf(file, "Usage: %s %s\nOptions:\n", app_name, usage);
	args_print_flags(file);
}

#endif
