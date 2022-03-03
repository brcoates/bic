#ifndef REG_H
#define REG_H

#include <stdlib.h>
#include <include/list.h>

typedef enum {
	R_RAX,
	R_RBX,
	R_RCX,
	R_RDX,
	R_R8,
	R_R9,
	R_R10,
	R_R11,
	R_R12,
	R_R13,
	R_R14,
	R_R15,
	R_UNKNOWN
} regtype_t;

typedef struct {
	char* name;
	regtype_t type;
	size_t sz;
} reg_t;

const char* reg_getname(regtype_t reg);
unsigned int reg_getsize(regtype_t reg);
regtype_t reg_gettype(char* str);

list_t* reg_getall();

reg_t* reg_create(regtype_t type);

#endif
