#include "main.h"

void usage(void) {
	puts("Github: "GITHUB_LINK"\n"
	     "Usage: "APP_NAME" [FILE] [OPTIONS]\n"
	     "Options:\n"
	     "  -h, --help     Show this message\n"
	     "  -v, --version  Print the version\n"
	     "  -s, --string   Embed into a string array");

	exit(EXIT_SUCCESS);
}

void version(void) {
	printf(APP_NAME" %i.%i.%i\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

	exit(EXIT_SUCCESS);
}

void error(const char *p_fmt, ...) {
	char    msg[256];
	va_list args;

	va_start(args, p_fmt);
	vsnprintf(msg, sizeof(msg), p_fmt, args);
	va_end(args);

	fprintf(stderr, "Error: %s\n", msg);
}

void try(const char *p_flag) {
	fprintf(stderr, "Try '"APP_NAME" %s'\n", p_flag);
}

void gen_byte_arr(FILE *p_file) {
	int    byte;
	size_t i;
	for (i = 0; (byte = fgetc(p_file)) != EOF; ++ i) {
		if (i % SPLIT_ON == 0) {
			if (i > 0)
				putchar('\n');

			putchar('\t');
		}

		printf(LITERAL_COLOR"0x%02X"DEFAULT_COLOR", ", byte);
	}

	putchar('\n');
}

static void start_str_line(void) {
	printf("\t"LITERAL_COLOR"\"");
}

static void end_str_line(void) {
	printf("\""DEFAULT_COLOR",\n");
}

void gen_str_arr(FILE *p_file) {
	start_str_line();

	int ch;
	for (size_t i = 0; (ch = fgetc(p_file)) != EOF; ++ i) {
		switch (ch) {
		case '\t': printf(SPECIAL_COLOR"\\t"LITERAL_COLOR);  break;
		case '\r': printf(SPECIAL_COLOR"\\r"LITERAL_COLOR);  break;
		case '\v': printf(SPECIAL_COLOR"\\v"LITERAL_COLOR);  break;
		case '\f': printf(SPECIAL_COLOR"\\f"LITERAL_COLOR);  break;
		case '\b': printf(SPECIAL_COLOR"\\b"LITERAL_COLOR);  break;
		case '\0': printf(SPECIAL_COLOR"\\0"LITERAL_COLOR);  break;
		case '"':  printf(SPECIAL_COLOR"\\\""LITERAL_COLOR); break;
		case '\\': printf(SPECIAL_COLOR"\\\\"LITERAL_COLOR); break;
		case '\n':
			printf(SPECIAL_COLOR"\\n"LITERAL_COLOR);
			end_str_line();
			start_str_line();

			break;

		default:
			if (ch >= ' ' && ch <= '~')
				printf("%c", (char)ch);
			else
				printf(SPECIAL_COLOR"\\x%02X"LITERAL_COLOR, ch);

			break;
		}
	}

	end_str_line();
}

int main(int p_argc, char **p_argv) {
	const char *path    = NULL;
	bool        str_arr = false;

	for (int i = 1; i < p_argc; ++ i) {
		if (strcmp(p_argv[i], "-h") == 0 || strcmp(p_argv[i], "--help") == 0)
			usage();
		else if (strcmp(p_argv[i], "-v") == 0 || strcmp(p_argv[i], "--version") == 0)
			version();
		else if (strcmp(p_argv[i], "-s") == 0 || strcmp(p_argv[i], "--string") == 0)
			str_arr = true;
		else if (path != NULL) {
			error("Unexpected argument '%s'", p_argv[i]);
			try("-h");

			exit(EXIT_FAILURE);
		} else
			path = p_argv[i];
	}

	if (path == NULL) {
		error("No input file specified");
		try("-h");

		exit(EXIT_FAILURE);
	}

	FILE *f = fopen(path, "r");
	if (f == NULL) {
		error("Could not open file '%s'", path);
		try("-h");

		exit(EXIT_FAILURE);
	}

	if (str_arr) {
		printf(TYPE_COLOR"const char"DEFAULT_COLOR" *embed[] = {\n");

		gen_str_arr(f);
	} else {
		printf(TYPE_COLOR"uint8_t"DEFAULT_COLOR" embed[] = {\n");

		gen_byte_arr(f);
	}

	printf("};\n");

	fclose(f);

	return 0;
}
