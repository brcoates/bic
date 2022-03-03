#ifndef LOG_H
#define LOG_H

void log_unexpected(char* expected, char* actual);
void log_fatal(int line_num, char* fmt, ...);

#endif
