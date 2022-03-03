#ifndef OPERAND_H
#define OPERAND_H

#include <include/scan.h>
#include <include/reg.h>

typedef enum {
	OT_r8,
	OT_r16,
	OT_r32,
	OT_r64,
	OT_m8,
	OT_m16,
	OT_m32,
	OT_m64,
	OT_imm8,
	OT_imm16,
	OT_imm32,
	OT_imm64,
	OT_UNKNOWN
} operandtype_t;

typedef struct {
	operandtype_t type;
	reg_t* reg;
	token_t* token;
} operand_t;

operandtype_t operand_getregtype(reg_t* reg);
operand_t* operand_create(operandtype_t type);

#endif
