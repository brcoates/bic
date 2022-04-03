#ifndef OPCODE_H
#define OPCODE_H

#include <stdbool.h>
#include <include/list.h>
#include <include/operand.h>

typedef enum opcode {
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOV,
	OP_CMP,
	OP_JMP,
	OP_PUSH,
	OP_POP,
	OP_UNKNOWN
} opcode_t;

opcode_t opcode_getopcodetype(char* str);
const char* opcode_gettypename(opcode_t opcode);

bool opcode_isopcode(char* str);

list_t* opcode_goperands(opcode_t op);

#endif
