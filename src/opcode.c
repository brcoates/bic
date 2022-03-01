#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <include/opcode.h>
#include <include/s_util.h>

opcode_t opcode_getopcodetype(char* str) {
	char* str_lower = s_tolower(str);

	if (strcmp(str_lower, "ADD") == 0) return OP_ADD;
	if (strcmp(str_lower, "SUB") == 0) return OP_SUB;
	if (strcmp(str_lower, "MUL") == 0) return OP_MUL;
	if (strcmp(str_lower, "DIV") == 0) return OP_DIV;
	if (strcmp(str_lower, "MOV") == 0) return OP_MOV;
	if (strcmp(str_lower, "CMP") == 0) return OP_CMP;
	if (strcmp(str_lower, "JMP") == 0) return OP_JMP;
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
