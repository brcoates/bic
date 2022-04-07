#ifndef LOG_H
#define LOG_H

typedef enum log_level {
	L_ERR = 33,
	L_FATAL = 31
} log_level_t;

void log_unexpected(char* expected, char* actual, int line_num);
void log_compile_fatal(int line_num, char* fmt, ...);
void log_fatal(char* fmt, ...);

#endif
