#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include <include/operand.h>
#include <include/s_util.h>

operandtype_t operand_getregtype(reg_t* reg) {
	if (reg->sz == 8) return OT_r8;
	if (reg->sz == 16) return OT_r16;
	if (reg->sz == 32) return OT_r32;
	if (reg->sz == 64) return OT_r64;

	return OT_UNKNOWN;
}

operand_t* operand_create(operandtype_t type) {
	operand_t* operand = calloc(1, sizeof(operand_t));
	operand->type = type;
	
	return operand;
}

operandtype_t operand_getnumtype(char* operand_str) {
	char* int_str = operand_getnumstr(operand_str);

	// ok great, now we can convert this to a numerical type
	int val = atoi(int_str);
	if (val <= UCHAR_MAX) return OT_imm8;
	if (val <= USHRT_MAX) return OT_imm16;
	if (val <= UINT_MAX) return OT_imm32;
	if (val <= ULONG_MAX) return OT_imm64;
	return OT_UNKNOWN;
}

char* operand_getnumstr(char* operand_str) {
	// this will just substring past the $ indicator
	assert(operand_str != NULL && strlen(operand_str) > 0);
	assert(operand_str[0] == '$');

	char* int_str = calloc(strlen(operand_str), sizeof(char));
	for (int i = 1; i < strlen(operand_str); i++) {
		int_str[i - 1] = operand_str[i];
	}

	return int_str;
}

const char* operand_gettypename(operandtype_t type) {
	switch (type) {
		case OT_r8: return "r8";
		case OT_r8_al: return "al";
		case OT_r16: return "r16";
		case OT_r16_ax: return "ax";
		case OT_r32: return "r32";
		case OT_r32_eax: return "eax";
		case OT_r64: return "r64";
		case OT_r64_rax: return "rax";
		case OT_m8: return "m8";
		case OT_m16: return "m16";
		case OT_m32: return "m32";
		case OT_m64: return "m64";
		case OT_imm8: return "imm8";
		case OT_imm16: return "imm16";
		case OT_imm32: return "imm32";
		case OT_imm64: return "imm64";
		case OT_UNKNOWN: 
		default:
			return "??";
	}
}
