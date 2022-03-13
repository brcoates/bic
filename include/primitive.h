#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <stdbool.h>

typedef enum {
	PRIM_INT,
	PRIM_STR,
	PRIM_FLT,
	PRIM_UNKNOWN
} primitive_t;

primitive_t prim_gettype(char* str);
const char* prim_gettypename(primitive_t prim);

bool prim_isprimitive(char* str);

#endif
