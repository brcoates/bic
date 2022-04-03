#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <include/list.h>
#include <include/opcode.h>
#include <include/operand.h>

// really dislike that i have to do this, but an early design decision with the lists means that every list item must
// be a pointer, and, of course, i can't mash this with an enum (without being hacky) so don't really need a struct,
// but looks like i'll have to do this; at least for the time being
typedef struct {
	operandtype_t operand_type;
} instruction_operand_t;
instruction_operand_t* ins_operand_create(operandtype_t type);

typedef struct {
	opcode_t opcode;
	list_t* operands; // list<instruction_operand_t>
} instruction_t;

void ins_initstate(bool debug_log);

void ins_init_mov();
void ins_init_add();
void ins_init_sub();
void ins_init_mul();
void ins_init_div();
void ins_init_cmp();
void ins_init_jmp();
void ins_init_push();
void ins_init_pop();

instruction_t* ins_alloc(opcode_t opcode);
instruction_t* ins_resolve(opcode_t opcode, int num_operands, ...);
instruction_t* ins_resolve_fromlist(opcode_t opcode, list_t* operands);
instruction_t* ins_create(opcode_t opcode, int num_operands, ...);

#endif
