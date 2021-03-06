#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <include/primitive.h>
#include <include/s_util.h>

primitive_t prim_gettype(char* str) {
	if (s_eqi(str, "int")) return PRIM_INT;
	if (s_eqi(str, "string")) return PRIM_STR;
	if (s_eqi(str, "float")) return PRIM_FLT;
	return PRIM_UNKNOWN;
}

const char* prim_gettypename(primitive_t prim) {
	switch (prim) {
		case PRIM_INT: return "int";
		case PRIM_STR: return "string";
		case PRIM_FLT: return "float";
		default: return "??";
	}
}

char* prim_getstringvalue(char* literal) {
	size_t len = strlen(literal);
	assert(len >= 2);
	assert(literal[0] == '"' && literal[len - 1] == '"');

	char* result = calloc(len - 1, sizeof(char));
	for (int i = 1; i < len - 1; i++) result[i - 1] = literal[i];

	return result;
}

bool prim_isprimitive(char* str) {
	return prim_gettype(str) != PRIM_UNKNOWN;
}
