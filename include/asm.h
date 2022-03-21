#ifndef ASM_H
#define ASM_H

#include <stdlib.h>

#include <include/list.h>
#include <include/parse.h>
#include <include/reg.h>

typedef struct {
	size_t size;
} stackframe_t;
stackframe_t* asm_stackframe_create(int num_args);
void asm_stackframe_enter(stackframe_t* frame);
void asm_stackframe_exit(stackframe_t* frame);

int asm_proc_getnumargs(node_t* node);

typedef struct {
	stackframe_t* frame;
	bool is_global;
} scope_t;
scope_t* asm_scope_create(stackframe_t* frame, bool is_global);

typedef struct {
	char* name;
	node_t* root_node;
	enum symbolflags {
		SF_PROC 	= (1 << 0),
		SF_PROC_ARG	= (1 << 1),
		SF_VAR 		= (1 << 2),
		SF_INT 		= (1 << 3),
		SF_FLT		= (1 << 4),
		SF_STR 		= (1 << 5)
	} flags;
	long address;
} symbol_t;
symbol_t* asm_symbol_add(char* name, enum symbolflags flags);
symbol_t* asm_symbol_lookup(char* name);
void asm_symbol_print_table();

typedef struct {
	list_t* symbols;
	list_t* registers;
	char* output;
} asm_state_t;
void asm_initstate();
//reg_t* asm_regalloc();

char* asm_codegen(parse_t* parse);

void asm_walk(node_t* node);
void asm_walk_node(node_t* node);
void asm_walk_instruction(node_t* node);
void asm_walk_proc(node_t* node);

list_t* asm_getoperands(node_t* operands_node);
void asm_ins_mov(node_t* node);

void asm_label(symbol_t* sym_label);
void asm_appendasm(char* code);

#endif
