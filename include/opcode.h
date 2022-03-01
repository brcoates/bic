#ifndef OPCODE_H
#define OPCODE_H

#include <stdbool.h>

typedef enum opcode {
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOV,
	OP_CMP,
	OP_JMP,
	OP_UNKNOWN
} opcode_t;

opcode_t opcode_getopcodetype(char* str);
const char* opcode_gettypename(opcode_t opcode);

bool opcode_isopcode(char* str);

#endif
