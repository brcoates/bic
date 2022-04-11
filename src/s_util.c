#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include <include/s_util.h>

char* s_append(char* dst, char* str) {
    assert(str != NULL);

    unsigned long dst_sz = dst == NULL ? 0 : strlen(dst);
    unsigned long str_sz = strlen(str);

    dst = realloc(dst, dst_sz + str_sz + 1);
    strcat(dst, str);

    return dst;
}

char* s_alloc(const char* initial_val) {
    assert(initial_val != NULL);

    char* str = calloc(strlen(initial_val) + 1, sizeof(char));
    strcpy(str, initial_val);

    return str;
}

char* s_tolower(char* str) {
	char* str_lower = calloc(strlen(str) + 1, sizeof(char));
	assert(str_lower != NULL);

	for (int i = 0; i < strlen(str); i++) {
		str_lower[i] = tolower(str[i]);
	}
	return str_lower;
}

bool s_eqi(char* str1, char* str2) {
	char* str1_low = s_tolower(str1);
	char* str2_low = s_tolower(str2);
	return s_eq(str1_low, str2_low);
}

bool s_eq(char* str1, char* str2) {
	return strcmp(str1, str2) == 0;
}


