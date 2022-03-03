#ifndef ASM_H
#define ASM_H

#include <stdlib.h>

#include <include/list.h>
#include <include/parse.h>
#include <include/reg.h>

typedef struct symbol {
	char* name;
	node_t* root_node;
	reg_t* stored_in_reg;
	long address;
} symbol_t;

typedef struct asm_state {
	list_t* symbols;
	list_t* registers;
	char* output;
} asm_state_t;
void asm_initstate();

char* asm_codegen(parse_t* parse);

void asm_node(node_t* node);

void asm_walkinstruction(node_t* node);

list_t* asm_getoperands(node_t* operands_node);
void asm_ins_mov(node_t* node);

symbol_t* asm_lookupsymbol(char* name);
reg_t* asm_regalloc();

void asm_appendasm(char* code);

#endif
