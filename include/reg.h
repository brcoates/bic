#ifndef REG_H
#define REG_H

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
	R_R15
} reg_t;

const char* reg_getname(reg_t reg);

#endif
