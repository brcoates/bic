#ifndef S_UTIL_H
#define S_UTIL_H

#include <stdbool.h>

char* s_append(char* dst, char* str);
char* s_alloc(const char* initial_val);

char* s_tolower(char* str);

bool s_eqi(char* str1, char* str2);
bool s_eq(char* str1, char* str2);

#endif
