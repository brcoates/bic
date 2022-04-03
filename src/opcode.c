#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <include/opcode.h>
#include <include/s_util.h>

opcode_t opcode_getopcodetype(char* str) {
	char* str_lower = s_tolower(str);

	if (strcmp(str_lower, "add") == 0) return OP_ADD;
	if (strcmp(str_lower, "sub") == 0) return OP_SUB;
	if (strcmp(str_lower, "mul") == 0) return OP_MUL;
	if (strcmp(str_lower, "div") == 0) return OP_DIV;
	if (strcmp(str_lower, "mov") == 0) return OP_MOV;
	if (strcmp(str_lower, "cmp") == 0) return OP_CMP;
	if (strcmp(str_lower, "jmp") == 0) return OP_JMP;
	if (strcmp(str_lower, "jmp") == 0) return OP_PUSH;
	if (strcmp(str_lower, "jmp") == 0) return OP_POP;
	return OP_UNKNOWN;
}

const char* opcode_gettypename(opcode_t opcode) {
	switch (opcode) {
		case OP_ADD: return "ADD";
		case OP_SUB: return "SUB";
		case OP_MUL: return "MUL";
		case OP_DIV: return "DIV";
		case OP_MOV: return "MOV";
		case OP_CMP: return "CMP";
		case OP_JMP: return "JMP";
		case OP_PUSH: return "PUSH";
		case OP_POP: return "POP";
		default: return NULL;
	}
}

bool opcode_isopcode(char* str) {
	for (int i = 0; i < OP_UNKNOWN; i++) {
		char* opcode_str_lower = s_tolower((char*) opcode_gettypename(i));
		char* str_lower = s_tolower(str);

		if (strcmp(str_lower, opcode_str_lower) == 0) return true;
	}
	return false;
}
