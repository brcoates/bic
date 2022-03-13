#ifndef ASM_H
#define ASM_H

#include <stdlib.h>

#include <include/list.h>
#include <include/parse.h>
#include <include/reg.h>

typedef struct {
	int num_args;
	int num_locals;
	int balance;
} stackframe_t;
stackframe_t* asm_stackframe_create();
void asm_stackframe_enter(stackframe_t* frame);
void asm_stackframe_exit(stackframe_t* frame);

typedef struct {
	stackframe_t* frame;
} proc_t;
proc_t* asm_proc_create(int num_args);
int asm_proc_getnumargs(node_t* node);

typedef struct {
	char* name;
	node_t* root_node;
	reg_t* stored_in_reg;
	long address;
} symbol_t;
symbol_t* asm_symbol_create(char* name);
symbol_t* asm_symbol_lookup(char* name);

typedef struct {
	list_t* symbols;
	list_t* registers;
	char* output;
} asm_state_t;
void asm_initstate();
reg_t* asm_regalloc();

char* asm_codegen(parse_t* parse);

void asm_node(node_t* node);
void asm_walk_instruction(node_t* node);
void asm_walk_proc(node_t* node);

list_t* asm_getoperands(node_t* operands_node);
void asm_ins_mov(node_t* node);

void asm_appendasm(char* code);

#endif
