#include <stdlib.h>

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
