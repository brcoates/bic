#ifndef OPERAND_H
#define OPERAND_H

#include <include/scan.h>
#include <include/reg.h>

/*
 * operand type structure:
 * CLASS	SIZE	INSTANCE
 * 00-FF	00-FF	0000-FFFF
 */

typedef enum {
	OC_REGISTER	= 0x00000000,
	OC_MEMORY	= 0x01000000,
	OC_IMM		= 0x02000000,
	OC_SEGOFF	= 0x04000000, // segoff/moffsXX operands specify a simple offset relative to the segment base
	OC_UNKNOWN 	= 0x08000000
} operandclass_t;

typedef enum {
	OS_8		= 0x00000000,
	OS_16		= 0x00010000,
	OS_32		= 0x00020000,
	OS_64		= 0x00040000,
	OS_UNKNOWN 	= 0x00080000
} operandsize_t;

typedef enum {
	OT_r8		= OC_REGISTER 	| OS_8 			| 0x00000000,
	OT_r8_al 	= OC_REGISTER 	| OS_8 			| 0x00000001,
	OT_r16 		= OC_REGISTER 	| OS_16 		| 0x00000000,
	OT_r16_ax 	= OC_REGISTER 	| OS_16 		| 0x00000001,
	OT_r32 		= OC_REGISTER 	| OS_32 		| 0x00000000,
	OT_r32_eax 	= OC_REGISTER 	| OS_32 		| 0x00000001,
	OT_r64		= OC_REGISTER 	| OS_64 		| 0x00000000,
	OT_r64_rax 	= OC_REGISTER 	| OS_64 		| 0x00000001,
	OT_m8 		= OC_MEMORY 	| OS_8 			| 0x00000000,
	OT_m16 		= OC_MEMORY 	| OS_16 		| 0x00000000,
	OT_m32 		= OC_MEMORY 	| OS_32 		| 0x00000000,
	OT_m64 		= OC_MEMORY 	| OS_64 		| 0x00000000,
	OT_imm8 	= OC_IMM 		| OS_8 			| 0x00000000,
	OT_imm16 	= OC_IMM		| OS_16 		| 0x00000000,
	OT_imm32 	= OC_IMM 		| OS_32 		| 0x00000000,
	OT_imm64 	= OC_IMM 		| OS_64 		| 0x00000000,
	OT_moffs8 	= OC_SEGOFF 	| OS_8 			| 0x00000000,
	OT_moffs16 	= OC_SEGOFF 	| OS_16 		| 0x00000000,
	OT_moffs32 	= OC_SEGOFF 	| OS_32 		| 0x00000000,
	OT_moffs64 	= OC_SEGOFF 	| OS_64 		| 0x00000000,
	OT_UNKNOWN 	= OC_UNKNOWN 	| OS_UNKNOWN 	| 0x00000000
} operandtype_t;

typedef struct {
	operandtype_t type;
	reg_t* reg;
	token_t* token;
} operand_t;

operandtype_t operand_getregtype(reg_t* reg);
operandtype_t operand_getnumtype(char* operand_str);
const char* operand_gettypename(operandtype_t type);

operand_t* operand_create(operandtype_t type);

char* operand_getnumstr(char* operand_str);

#endif
