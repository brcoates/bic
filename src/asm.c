#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <include/asm.h>
#include <include/reg.h>
#include <include/log.h>
#include <include/scan.h>
#include <include/parse.h>
#include <include/opcode.h>

#include <include/s_util.h>

static asm_state_t* asm_state = NULL;

char* asm_codegen(parse_t* parse) {
	asm_initstate();
	assert(asm_state != NULL);

	// basically, we need to walk each node object.
	node_t* curr = parse->node_head->body;
	do {
		asm_node(curr);
		curr = curr->next;
	} while (curr != NULL);

	return asm_state->output;
}

void asm_node(node_t* node) {
	switch (node->type) {
		case NT_BODY: 
			assert(node->body != NULL);
			asm_node(node->body);
			break;

		case NT_INSTRUCTION:
			asm_walk_instruction(node);
			break;

		case NT_PROC:
			asm_walk_proc(node);
			break;

		default:
			fprintf(stderr, "Unsupported feature, %s\n", parse_getnodetypename(node->type));
			exit(1);
	}
}

void asm_walk_proc(node_t* node) {
	// first of all, let's setup our proc object... in order to do this, we first need to derive the number of args
	// this procedure will take. to do this, we will need to do some prelimary scanning work... once we have this we
	// can init the proc object and take it from there.
	proc_t* proc = asm_proc_create(asm_proc_getnumargs(node));
}

int asm_proc_getnumargs(node_t* node) {
	if (node->type == NT_PROC) return asm_proc_getnumargs(node->body);
	if (node->type == NT_PROC_ARGS_LIST) {
		// great, now we're here, each child of body, then ->next will be an arg.
		int num_args = 0;
		node_t* next = NULL;
		do {
			if (next != NULL && next->type == NT_PROC_ARG) num_args = num_args + 1;

			if (next == NULL) next = node->body;
			else next = next->next;
		} while (next != NULL);
		return num_args;
	} else {
		assert(node->next != NULL);
		return asm_proc_getnumargs(node->next);
	}
}

void asm_walk_instruction(node_t* node) {
	opcode_t opcode = opcode_getopcodetype(node->token->str);
	if (opcode == OP_UNKNOWN) {
		fprintf(stderr, "Unsupported opcode (%d) %s\n", opcode, node->token->str);
		exit(1);
	}

	// depending on what opcode we have, will depend on how we proceed...
	switch (opcode) {
		case OP_MOV:
			asm_ins_mov(node);
			break;

		default:
			fprintf(stderr, "Unsupported codegen for opcode %s\n", opcode_gettypename(opcode));
			exit(1);
			return;
	}
}

list_t* asm_getoperands(node_t* operands_node) {
	assert(operands_node->type == NT_OPERAND_LIST);

	list_t* operands = list_create();
	node_t* curr = operands_node->body;
	do {
		assert(curr->token != NULL);
		switch (curr->token->type) {
			case TT_REG: {
				reg_t* reg = reg_create(reg_gettype(curr->token->str));
				
				operand_t* operand_reg = operand_create(operand_getregtype(reg));
				operand_reg->reg = reg;
				operand_reg->token = curr->token;

				list_additem(operands, operand_reg);
				break;
			}

			default:
				fprintf(stderr, "Unsupported opcode, %s\n", curr->token->str);
				exit(1);
				break;
		}
		curr = curr->next;
	} while (curr != NULL);

	return operands;
}

void asm_ins_mov(node_t* node) {
	node_t* operands_node = node->body;
	assert(operands_node->type == NT_OPERAND_LIST);

	list_t* operands = asm_getoperands(operands_node);
	if (operands->count != 2) {
		log_fatal(node->token->line_num, "MOV operand requires 2 arguments\n");
		exit(1);
	}

	// now we have the operands, let's go ahead and spit these back out
	// ideally we would have some validation here, but let's leave that for
	// another day.
	operand_t* op1 = operands->items[0];
	operand_t* op2 = operands->items[1];

	char code[200];
	sprintf(code, "mov %s, %s\n", op1->token->str, op2->token->str);
	asm_appendasm(code);
}

symbol_t* asm_symbol_create(char* name) {
	symbol_t* symbol = calloc(1, sizeof(symbol_t));
	symbol->name = name;

	return symbol;
}

symbol_t* asm_symbol_lookup(char* name) {
	for (int i = 0; i < asm_state->symbols->count; i++) {
		symbol_t* sym = asm_state->symbols->items[i];
		if (strcmp(sym->name, name) == 0) return sym;
	}
	return NULL;
}

reg_t* asm_regalloc() {
	for (int i = 0; i < asm_state->registers->count; i++) {
		bool is_inuse = false;
		for (int j = 0; j < asm_state->symbols->count && !is_inuse; j++) {
			symbol_t* sym = asm_state->symbols->items[j];
			is_inuse |= sym->stored_in_reg == asm_state->registers->items[i];
		}

		if (!is_inuse) return asm_state->registers->items[i];
	}
	return NULL;
}

void asm_stackframe_enter(stackframe_t* frame) {
	asm_appendasm("push rbp\nmov rsp, rbp\n");

	char str[100];
	sprintf(str, "sub rsp, %d\n", frame->num_args);
	asm_appendasm(str);
}

proc_t* asm_proc_create(int num_args) {
	proc_t* proc = calloc(1, sizeof(proc_t));
	proc->frame = asm_stackframe_create();

	proc->frame->num_args = num_args;
	
	return proc;
}

void asm_stackframe_exit(stackframe_t* frame) {
	asm_appendasm("mov rsp, rbp\npop rbp\n");
}

void asm_appendasm(char* code) {
	asm_state->output = s_append(asm_state->output, code);
}

void asm_initstate() {
	if (asm_state != NULL) {
		free(asm_state);
	}

	asm_state = calloc(1, sizeof(asm_state_t));
	asm_state->output = NULL;
	asm_state->symbols = list_create();
	asm_state->registers = reg_getall();
}

stackframe_t* asm_stackframe_create() {
	stackframe_t* frame = calloc(1, sizeof(stackframe_t));
	return frame;
}
