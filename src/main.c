#include "main.h"

#define CARGS_IMPLEMENTATION
#include "cargs.h"

#define COLORER_IMPLEMENTATION
#include "colorer.h"

void usage(void) {
	puts("Github: "GITHUB_LINK);
	puts("Usage: "APP_NAME" [FILE] [OPTIONS]");
	puts("Options: ");
	args_print_flags(stdout);

	exit(EXIT_SUCCESS);
}

void version(void) {
	printf(APP_NAME" %i.%i.%i\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

	exit(EXIT_SUCCESS);
}

void error(const char *fmt, ...) {
	char    msg[256];
	va_list args;

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	fprintf(stderr, "Error: %s\n", msg);
}

void try(const char *flag) {
	fprintf(stderr, "Try '"APP_NAME" %s'\n", flag);
}

static char *pure_name(const char *path) {
	for (size_t i = strlen(path) - 1; i != (size_t)-1; -- i) {
		if (path[i] == '\\' || path[i] == '/') {
			path += i + 1;
			break;
		}
	}

	size_t len = strlen(path);
	char *str  = (char*)malloc(len + 1);
	if (str == NULL)
		return NULL;

	strcpy(str, path);
	for (size_t i = 0; i < len; ++ i) {
		if (str[i] == '.') {
			str[i] = '\0';
			break;
		} else if ((!isalnum(str[i]) && str[i] != '_') || (isdigit(str[i]) && i == 0))
			str[i] = '_';
	}

	return str;
}

static void embed_str(FILE *file, FILE *out, const char *path, const char *name) {
	color_fg(out, COMMENT_COLOR);
	fprintf(out, "/* %s */\n", path);

	color_fg(out, TYPE_COLOR);
	fprintf(out, "static const char");
	color_fg(out, NORMAL_COLOR);
	fprintf(out, " *%s[] = {\n", name);

	fprintf(out, "\t");
	color_fg(out, LITERAL_COLOR);
	fprintf(out, "\"");

	int ch;
	for (size_t i = 0; (ch = fgetc(file)) != EOF; ++ i) {
		if (ch >= ' ' && ch <= '~')
			printf("%c", (char)ch);
		else {
			color_fg(out, SPECIAL_COLOR);
			switch (ch) {
			case '\t': fprintf(out, "\\t");  break;
			case '\r': fprintf(out, "\\r");  break;
			case '\v': fprintf(out, "\\v");  break;
			case '\f': fprintf(out, "\\f");  break;
			case '\b': fprintf(out, "\\b");  break;
			case '\0': fprintf(out, "\\0");  break;
			case '"':  fprintf(out, "\\\""); break;
			case '\\': fprintf(out, "\\\\"); break;
			case '\n':
				color_fg(out, LITERAL_COLOR);
				fprintf(out, "\"");
				color_fg(out, NORMAL_COLOR);
				fprintf(out, ",\n");

				fprintf(out, "\t");
				color_fg(out, LITERAL_COLOR);
				fprintf(out, "\"");
				break;

			default:
				fprintf(out, "\\x%02X", ch);

				break;
			}
			color_fg(out, LITERAL_COLOR);
		}
	}

	fprintf(out, "\"");
	color_fg(out, NORMAL_COLOR);
	fprintf(out, ",\n};\n");
}

static void embed_bytes(FILE *file, FILE *out, const char *path, const char *name) {
	color_fg(out, COMMENT_COLOR);
	fprintf(out, "/* %s */\n", path);

	color_fg(out, TYPE_COLOR);
	fprintf(out, "static const char");
	color_fg(out, NORMAL_COLOR);
	fprintf(out, " *%s[] = {\n", name);

	int byte;
	for (size_t i = 0; (byte = fgetc(file)) != EOF; ++ i) {
		if (i % SPLIT_ON == 0) {
			if (i > 0)
				fprintf(out, "\n");

			fprintf(out, "\t");
		}

		color_fg(out, LITERAL_COLOR);
		fprintf(out, "0x%02X", byte);
		color_fg(out, NORMAL_COLOR);
		fprintf(out, ", ");
	}

	fprintf(out, "\n};\n");
}

static void embed(const char *in, const char *out, bool string) {
	FILE *f = fopen(in, "r");
	if (f == NULL) {
		error("Could not open file '%s'", in);
		exit(EXIT_FAILURE);
	}

	FILE *o;
	if (out == NULL)
		o = stdout;
	else {
		o = fopen(out, "w");
		if (o == NULL) {
			error("Could not write file '%s'", out);
			exit(EXIT_FAILURE);
		}
	}

	char *pure = pure_name(in);
	if (pure == NULL)
		assert(0 && "malloc() fail");

	if (string)
		embed_str(f, o, in, pure);
	else
		embed_bytes(f, o, in, pure);

	free(pure);
}

int main(int argc, const char **argv) {
	args_t a = new_args(argc, argv);
	args_shift(&a);

	char *out     = NULL;
	bool  ver     = false;
	bool  help    = false;
	bool  string  = false;

	flag_bool("v", "version", "Show the version",       &ver);
	flag_bool("h", "help",    "Show the usage",         &help);
	flag_bool("s", "string",  "Output as string array", &string);
	flag_cstr("o", "out",     "Path of output file",    &out);

	int    where;
	args_t stripped;
	int    err = args_parse_flags(&a, &where, &stripped);
	if (err != ARG_OK) {
		switch (err) {
		case ARG_OUT_OF_MEM:    assert(0 && "malloc() fail");
		case ARG_UNKNOWN:       error("Unknown flag '%s'",            a.v[where]); try("-h"); break;
		case ARG_MISSING_VALUE: error("Flag '%s' is a missing value", a.v[where]); try("-h"); break;

		default: error("Incorrect type for flag '%s'", a.v[where]); try("-h");
		}

		free(stripped.base);
		exit(EXIT_FAILURE);
	}

	if (help)
		usage();
	else if (ver)
		version();

	if (stripped.c != 1) {
		if (stripped.c < 1)
			error("No input file specified");
		else
			error("Unexpected argument '%s'", stripped.v[1]);

		try("-h");
		exit(EXIT_FAILURE);
	}

	embed(stripped.v[0], out, string);

	free(out);
	free(stripped.base);
	return EXIT_SUCCESS;
}
