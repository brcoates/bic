#ifndef ASM_H
#define ASM_H

#include <stdlib.h>

#include <include/seg.h>
#include <include/reg.h>
#include <include/list.h>
#include <include/parse.h>
#include <include/operand.h>

#define STACK_ALIGN 

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

typedef struct symbol symbol_t;
struct symbol {
	char* name;
	node_t* root_node;
	enum symbolflags {
		SF_UNKNOWN	= (1 << 0),
		SF_PROC 	= (1 << 1),
		SF_PROC_ARG	= (1 << 2),
		SF_VAR 		= (1 << 3),
		SF_INT 		= (1 << 4),
		SF_FLT		= (1 << 5),
		SF_STR 		= (1 << 6),
		SF_EXE		= (1 << 7),
		SF_DATA		= (1 << 8)
	} flags;
	segment_t* segment;
	enum {
		BT_UNKNOWN,
		BT_REGISTER,
		BT_SYMBOL
	} base_type;
	union base {
		reg_t* reg;
		symbol_t* symbol;
	} * base;
	unsigned long offset; // for proc args this will be the argument index
};
symbol_t* asm_symbol_add(char* name, enum symbolflags flags, unsigned long offset);
symbol_t* asm_symbol_lookup(char* name);
void asm_symbol_printtable();

typedef struct {
	list_t* symbols;
	list_t* registers;
	char* output;
} asm_state_t;
void asm_initstate();
reg_t* asm_getstatereg(regtype_t reg_type);

// Entrypoint
char* asm_codegen(parse_t* parse);

// Walk functions
void asm_walk(node_t* node);
void asm_walk_node(node_t* node);
void asm_walk_label(node_t* node);
void asm_walk_instruction(node_t* node);
void asm_walk_proc(node_t* node);
void asm_walk_call(node_t* node);

operand_t* asm_getoperandfromsymbol(symbol_t* sym_ident);
list_t* asm_getoperands(node_t* operands_node);
void asm_ins_scanoperands(node_t* node, char* opcode_name, list_t** operands, int max_args);
char* asm_ins_resolveoperandasm(operand_t* operand);
void asm_ins_mov(node_t* node);
void asm_ins_add(node_t* node);

void asm_label(symbol_t* sym_label);
void asm_appendasm(char* code);

#endif
