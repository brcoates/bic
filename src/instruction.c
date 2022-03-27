#include <stdarg.h>
#include <stdlib.h>
#include <include/list.h>
#include <include/instruction.h>

static list_t* ins_state; // list<instruction_t>

void ins_initstate() {
	ins_state = list_create();

	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r8, OT_r8));

	for (int i = 0; i < ins_state->count; i++) {
		instruction_t* ins = ins_state->items[i];
		printf("%s: (%d operands), first: %s\n", ins->opcode
}

instruction_t* ins_create(opcode_t opcode, int num_operands, ...) {
	va_list args;
	va_start(args, num_operands);

	instruction_t* ins = ins_alloc(opcode);
	for (int i = 0; i < num_operands; i++) {
		operandtype_t operand = va_arg(args, operandtype_t);
		list_additem(ins->operands, (void*) operand);
	}

	return ins;
}

instruction_t* ins_resolve(opcode_t opcode, int num_operands, ...) {
	va_list args;
	va_start(args, num_operands);

	list_t* operands = list_create();
	for (int i = 0; i < num_operands; i++) {
		operandtype_t operand = va_arg(args, operandtype_t);
		list_additem(operands, (void*) operand);
	}

	// go thru state and go find one.

	return NULL;
}

instruction_t* ins_alloc(opcode_t opcode) {
	instruction_t* ins = calloc(1, sizeof(instruction_t));
	ins->opcode = opcode;
	ins->operands = list_create();

	return ins;
}
