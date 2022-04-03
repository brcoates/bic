#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <include/log.h>

char* log_resolve_print_str(char* str);

void log_unexpected(char* expected, char* actual, int line_num) {
	log_fatal(
		line_num, 
		"unexpected token, expected %s, got %s\n",
		log_resolve_print_str(expected),
		log_resolve_print_str(actual)
	);
}

void log_fatal(int line_num, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "line %d: ", line_num);
	vfprintf(stderr, fmt, args);

	va_end(args);
}

char* log_resolve_print_str(char* str) {
	if (str == NULL) return "(null)";

	size_t len = strlen(str);
	if (len == 1 && str[0] == '\r') return "<CR>";
	if (len == 1 && str[0] == '\n') return "<LF>";
	if (len == 2 && str[0] == '\r' && str[1] == '\n') return "<CRLF>";

	return str;
}
