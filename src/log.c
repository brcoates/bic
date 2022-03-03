#include <stdio.h>
#include <stdarg.h>
#include <include/log.h>

void log_unexpected(char* expected, char* actual) {
    fprintf(stderr, "Unexpected identifier, expected %s, got %s\n", expected, actual);
}

void log_fatal(int line_num, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	printf("error line %d: ", line_num);
	vprintf(fmt, args);

	va_end(args);
}

