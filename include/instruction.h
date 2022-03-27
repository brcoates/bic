#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <include/list.h>
#include <include/opcode.h>
#include <include/operand.h>

typedef struct {
	opcode_t opcode;
	list_t* operands; // list<operandtype_t>
} instruction_t;

void ins_initstate();

instruction_t* ins_alloc(opcode_t opcode);
instruction_t* ins_resolve(opcode_t opcode, int num_operands, ...);
instruction_t* ins_create(opcode_t opcode, int num_operands, ...);

#endif
