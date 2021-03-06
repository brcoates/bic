#ifndef ASM_H
#define ASM_H

#include <stdlib.h>

#include <include/seg.h>
#include <include/reg.h>
#include <include/list.h>
#include <include/parse.h>
#include <include/operand.h>
#include <include/instruction.h>

typedef struct {
	size_t size;
	int num_reserves;
} stackframe_t;
stackframe_t* asm_stackframe_create(int num_args);
void asm_stackframe_resize(stackframe_t* frame);
void asm_stackframe_enter(stackframe_t* frame);
void asm_stackframe_exit(stackframe_t* frame);

int asm_proc_getnumargs(node_t* node);

typedef struct {
	stackframe_t* frame;
	bool is_global;
} scope_t;
scope_t* asm_scope_create(stackframe_t* frame, bool is_global);

typedef struct context context_t;
struct context {
	scope_t* scope;
	context_t* body;
};
context_t* asm_ctx_create(bool is_global_scope);
void asm_ctx_pushad(context_t* ctx);
void asm_ctx_popad(context_t* ctx);

typedef struct symbol symbol_t;
struct symbol {
	char* name;
	// node_t* root_node;
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
	long offset; 
	unsigned int ordinal; // keeping track of which argument #
	regtype_t stored_in_reg;
};
symbol_t* asm_symbol_add(char* name, enum symbolflags flags, unsigned long offset);
symbol_t* asm_symbol_lookup(char* name);
void asm_symbol_printtable();

typedef struct {
	list_t* symbols;
	list_t* registers;
	char* glob_output;
	char* code_output;
	char* data_output;

	list_t* reserved_registers;
	list_t* stack;
} asm_state_t;
void asm_initstate();
reg_t* asm_getstatereg(regtype_t reg_type);

// entrypoint
char* asm_codegen(parse_t* parse);

// walk functions
void asm_walk(node_t* node, context_t* context);
void asm_walk_node(node_t* node, context_t* context);
void asm_walk_directive(node_t* node, context_t* context);
void asm_walk_label(node_t* node, context_t* context);
void asm_walk_instruction(node_t* node, context_t* context);
void asm_walk_proc(node_t* node, context_t* context);
void asm_walk_call(node_t* node, context_t* context);

operand_t* asm_getoperandfromsymbol(symbol_t* sym_ident);
list_t* asm_getoperands(node_t* operands_node);
void asm_ins_scanoperands(node_t* node, char* opcode_name, list_t** operands, int max_args);
char* asm_ins_resolveoperandasm(operand_t* operand);
void asm_ins_mov(node_t* node);
void asm_ins_add(node_t* node);

const char* asm_ins_getsizespecifier(instruction_operand_t* operand);
char* asm_ins_asm(instruction_t* instruction, list_t* operands);

// stack functions
#define STACK_ALIGNMENT 16 /* this is mandated by the system-v abi for x86-64 calling conventions */
typedef struct {
	enum stack_item_type {
		SIT_REGISTER,
		SIT_SYMBOL
	} stack_item_type;
	union stack_item {
		reg_t* reg;
		symbol_t* symbol;
	} stack_item;
	long offset;
} stackitem_t;
void asm_stack_push(stackitem_t* item);
stackitem_t* asm_stack_pop();

// helper functions
void asm_pushad();
void asm_popfd();

bool asm_reg_isreserved(regtype_t reg);
bool asm_reg_reserve(regtype_t reg);
void asm_reg_clear(regtype_t reg);
reg_t* asm_reg_reservesize(size_t size);
void asm_reg_clearall();

// assembly text-writer functions
void asm_label(symbol_t* sym_label);
void asm_code_append(char* code);
void asm_data_append(char* code);

#endif
