#ifndef REG_H
#define REG_H

#include <stdlib.h>
#include <include/list.h>

typedef enum {
	RS_8 		= 0x00000000,
	RS_16		= 0x01000000,
	RS_32		= 0x02000000,
	RS_64		= 0x04000000,
	RS_UNKNOWN 	= 0x08000000
} regsize_t;

typedef enum {
	R_RAX 		= RS_64 		| 0x00000000,
	R_RBX 		= RS_64 		| 0x00000001,
	R_RCX 		= RS_64 		| 0x00000002,
	R_RDX 		= RS_64 		| 0x00000004,
	R_RBP 		= RS_64 		| 0x00000008,
	R_RSP 		= RS_64 		| 0x00000010,
	R_EAX 		= RS_32 		| 0x00000000,
	R_EBX 		= RS_32 		| 0x00000001,
	R_ECX 		= RS_32 		| 0x00000002,
	R_EDX 		= RS_32 		| 0x00000004,
	R_R8 		= RS_64 		| 0x00000020,
	R_R9 		= RS_64 		| 0x00000040,
	R_R10 		= RS_64 		| 0x00000080,
	R_R11 		= RS_64 		| 0x00000100,
	R_R12 		= RS_64 		| 0x00000200,
	R_R13 		= RS_64 		| 0x00000400,
	R_R14 		= RS_64 		| 0x00000800,
	R_R15 		= RS_64 		| 0x00001000,
	R_UNKNOWN 	= RS_UNKNOWN 	| 0x00000000
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
