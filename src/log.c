#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include <include/log.h>

char* log_resolve_print_str(char* str);
char* log_resolve_level_str(log_level_t level, bool is_bold);

void log_unexpected(char* expected, char* actual, int line_num) {
	log_compile_fatal(
		line_num, 
		"unexpected token, expected %s, got %s\n",
		log_resolve_print_str(expected),
		log_resolve_print_str(actual)
	);
}

void log_compile_fatal(int line_num, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "%s line %d: ", log_resolve_level_str(L_FATAL, true), line_num);
	vfprintf(stderr, fmt, args);

	va_end(args);
}

void log_fatal(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "%s ", log_resolve_level_str(L_FATAL, true));
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

char* log_resolve_level_str(log_level_t level, bool is_bold) {
	char* level_name;
	switch (level) {
		case L_ERR: level_name = "error"; break;
		case L_FATAL: level_name = "fatal"; break;
	}

	char buff[200];
	sprintf(buff, "\033[%d;%dm%s:\033[0m", 
		is_bold ? 1 : 0,
		level,
		level_name);

	// let's go ahead, now we know how long the text is, and just alloc the mem. the array will die, but that's fine
	char* buff_str = calloc(strlen(buff) + 1, sizeof(char));
	strcpy(buff_str, buff);

	return buff_str;
}
