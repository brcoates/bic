#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <include/list.h>
#include <include/operand.h>
#include <include/instruction.h>

static list_t* ins_state; // list<instruction_t>

void ins_initstate(bool debug_log) {
	ins_state = list_create();
	assert(ins_state != NULL);

	ins_init_mov();
	ins_init_add();
	ins_init_sub();
	ins_init_mul();
	ins_init_div();
	ins_init_cmp();
	ins_init_jmp();
	ins_init_push();
	ins_init_pop();

	if (debug_log) {
		for (int i = 0; i < ins_state->count; i++) {
			instruction_t* ins = ins_state->items[i];
			printf(
				"%s: (%d operands)", 
				opcode_gettypename(ins->opcode),
				ins->operands->count
			);
			for (int j = 0; j < ins->operands->count; j++) {
				printf(
					" %s",
					operand_gettypename(((instruction_operand_t*) ins->operands->items[j])->operand_type)
				);
			}
			printf("\n");
		}
	}
}

void ins_init_mov() {
	// === 8-bit ===
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r8, OT_imm8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r8, OT_m8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r8, OT_r8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m8, OT_r8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m8, OT_m8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m8, OT_r8));

	// === 16-bit ===
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r16, OT_imm8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r16, OT_imm16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r16, OT_m16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r16, OT_r16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m16, OT_imm8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m16, OT_imm16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m16, OT_m16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m16, OT_r16));

	// === 32-bit ===
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r32, OT_imm8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r32, OT_imm16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r32, OT_imm32));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r32, OT_m32));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r32, OT_r32));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m32, OT_imm8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m32, OT_imm16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m32, OT_imm32));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m32, OT_m32));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m32, OT_r32));

	// === 64-bit ===
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r64, OT_imm8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r64, OT_imm16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r64, OT_imm32));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r64, OT_imm64));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r64, OT_m64));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_r64, OT_r64));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m64, OT_imm8));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m64, OT_imm16));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m64, OT_imm32));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m64, OT_imm64));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m64, OT_m64));
	list_additem(ins_state, ins_create(OP_MOV, 2, OT_m64, OT_r64));
}

void ins_init_add() {
	// === 8-bit ===
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r8, OT_imm8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r8, OT_m8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r8, OT_r8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m8, OT_r8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m8, OT_m8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m8, OT_r8));

	// === 16-bit ===
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r16, OT_imm8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r16, OT_imm16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r16, OT_m16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r16, OT_r16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m16, OT_imm8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m16, OT_imm16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m16, OT_m16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m16, OT_r16));

	// === 32-bit ===
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r32, OT_imm8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r32, OT_imm16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r32, OT_imm32));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r32, OT_m32));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r32, OT_r32));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m32, OT_imm8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m32, OT_imm16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m32, OT_imm32));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m32, OT_m32));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m32, OT_r32));

	// === 64-bit ===
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r64, OT_imm8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r64, OT_imm16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r64, OT_imm32));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r64, OT_imm64));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r64, OT_m64));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_r64, OT_r64));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m64, OT_imm8));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m64, OT_imm16));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m64, OT_imm32));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m64, OT_imm64));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m64, OT_m64));
	list_additem(ins_state, ins_create(OP_ADD, 2, OT_m64, OT_r64));
}

void ins_init_sub() {
	// === 8-bit ===
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r8, OT_imm8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r8, OT_m8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r8, OT_r8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m8, OT_r8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m8, OT_m8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m8, OT_r8));

	// === 16-bit ===
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r16, OT_imm8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r16, OT_imm16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r16, OT_m16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r16, OT_r16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m16, OT_imm8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m16, OT_imm16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m16, OT_m16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m16, OT_r16));

	// === 32-bit ===
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r32, OT_imm8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r32, OT_imm16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r32, OT_imm32));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r32, OT_m32));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r32, OT_r32));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m32, OT_imm8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m32, OT_imm16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m32, OT_imm32));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m32, OT_m32));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m32, OT_r32));

	// === 64-bit ===
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r64, OT_imm8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r64, OT_imm16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r64, OT_imm32));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r64, OT_imm64));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r64, OT_m64));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_r64, OT_r64));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m64, OT_imm8));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m64, OT_imm16));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m64, OT_imm32));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m64, OT_imm64));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m64, OT_m64));
	list_additem(ins_state, ins_create(OP_SUB, 2, OT_m64, OT_r64));
}

void ins_init_mul() {
	// === 8-bit ===
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r8, OT_imm8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r8, OT_m8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r8, OT_r8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m8, OT_r8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m8, OT_m8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m8, OT_r8));

	// === 16-bit ===
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r16, OT_imm8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r16, OT_imm16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r16, OT_m16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r16, OT_r16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m16, OT_imm8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m16, OT_imm16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m16, OT_m16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m16, OT_r16));

	// === 32-bit ===
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r32, OT_imm8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r32, OT_imm16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r32, OT_imm32));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r32, OT_m32));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r32, OT_r32));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m32, OT_imm8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m32, OT_imm16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m32, OT_imm32));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m32, OT_m32));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m32, OT_r32));

	// === 64-bit ===
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r64, OT_imm8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r64, OT_imm16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r64, OT_imm32));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r64, OT_imm64));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r64, OT_m64));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_r64, OT_r64));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m64, OT_imm8));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m64, OT_imm16));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m64, OT_imm32));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m64, OT_imm64));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m64, OT_m64));
	list_additem(ins_state, ins_create(OP_MUL, 2, OT_m64, OT_r64));
}

void ins_init_div() {
	// === 8-bit ===
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r8, OT_imm8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r8, OT_m8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r8, OT_r8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m8, OT_r8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m8, OT_m8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m8, OT_r8));

	// === 16-bit ===
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r16, OT_imm8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r16, OT_imm16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r16, OT_m16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r16, OT_r16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m16, OT_imm8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m16, OT_imm16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m16, OT_m16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m16, OT_r16));

	// === 32-bit ===
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r32, OT_imm8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r32, OT_imm16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r32, OT_imm32));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r32, OT_m32));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r32, OT_r32));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m32, OT_imm8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m32, OT_imm16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m32, OT_imm32));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m32, OT_m32));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m32, OT_r32));

	// === 64-bit ===
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r64, OT_imm8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r64, OT_imm16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r64, OT_imm32));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r64, OT_imm64));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r64, OT_m64));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_r64, OT_r64));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m64, OT_imm8));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m64, OT_imm16));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m64, OT_imm32));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m64, OT_imm64));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m64, OT_m64));
	list_additem(ins_state, ins_create(OP_DIV, 2, OT_m64, OT_r64));
}

void ins_init_cmp() {}

void ins_init_jmp() {}

void ins_init_push() {
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_r8));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_r16));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_r32));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_r64));

	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_m8));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_m16));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_m32));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_m64));

	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_imm8));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_imm16));
	list_additem(ins_state, ins_create(OP_PUSH, 1, OT_imm32));
}

void ins_init_pop() {
	list_additem(ins_state, ins_create(OP_POP, 1, OT_r8));
	list_additem(ins_state, ins_create(OP_POP, 1, OT_r16));
	list_additem(ins_state, ins_create(OP_POP, 1, OT_r32));
	list_additem(ins_state, ins_create(OP_POP, 1, OT_r64));

	list_additem(ins_state, ins_create(OP_POP, 1, OT_m8));
	list_additem(ins_state, ins_create(OP_POP, 1, OT_m16));
	list_additem(ins_state, ins_create(OP_POP, 1, OT_m32));
	list_additem(ins_state, ins_create(OP_POP, 1, OT_m64));
}

instruction_t* ins_create(opcode_t opcode, int num_operands, ...) {
	va_list args;
	va_start(args, num_operands);

	instruction_t* ins = ins_alloc(opcode);
	for (int i = 0; i < num_operands; i++) {
		operandtype_t operandtype = va_arg(args, operandtype_t);
		instruction_operand_t* operand = ins_operand_create(operandtype);
		list_additem(ins->operands, (void*) operand);
	}
	va_end(args);

	return ins;
}

instruction_t* ins_resolve(opcode_t opcode, int num_operands, ...) {
	va_list args;
	va_start(args, num_operands);

	list_t* operands = list_create();
	for (int i = 0; i < num_operands; i++) {
		operandtype_t operandtype = va_arg(args, operandtype_t);
		instruction_operand_t* operand = ins_operand_create(operandtype);
		list_additem(operands, (void*) operand);
	}
	va_end(args);

	return ins_resolve_fromlist(opcode, operands);
}

instruction_t* ins_resolve_fromlist(opcode_t opcode, list_t* operands) {
	// great, now we have a list of instruction_operand_t's - we can now go ahead and 
	// see if there are any that match this + opcode
	instruction_t* match = NULL;
	for (int i = 0; i < ins_state->count && match == NULL; i++) {
		instruction_t* ins = ins_state->items[i];
		if (ins->opcode != opcode) continue;
		if (ins->operands->count != operands->count) continue;

		bool is_same = true;
		for (int j = 0; j < ins->operands->count && is_same; j++) {
			instruction_operand_t* operand1 = ins->operands->items[j];
			instruction_operand_t* operand2 = operands->items[j];
			is_same &= operand1->operand_type == operand2->operand_type;
		}

		if (is_same) match = ins;
	}

	return match;
}

instruction_t* ins_alloc(opcode_t opcode) {
	instruction_t* ins = calloc(1, sizeof(instruction_t));
	ins->opcode = opcode;
	ins->operands = list_create();

	return ins;
}

instruction_operand_t* ins_operand_create(operandtype_t type) {
	instruction_operand_t* ins_operand = calloc(1, sizeof(instruction_operand_t)); 
	ins_operand->operand_type = type;

	return ins_operand;
}
