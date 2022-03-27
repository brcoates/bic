#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <include/asm.h>
#include <include/reg.h>
#include <include/log.h>
#include <include/scan.h>
#include <include/parse.h>
#include <include/opcode.h>
#include <include/primitive.h>

#include <include/instruction.h>

#include <include/s_util.h>

static asm_state_t* asm_state = NULL;

char* asm_codegen(parse_t* parse) {
	ins_initstate();
	asm_initstate();
	assert(asm_state != NULL);

	node_t* curr = parse->node_head;//->body;
	asm_walk(curr);

	return asm_state->output;
}

void asm_walk(node_t* node) {
	// basically, we need to walk each node object.
	node_t* curr = node;
	do {
		asm_walk_node(curr);
		curr = curr->next;
	} while (curr != NULL);
}

void asm_walk_node(node_t* node) {
	switch (node->type) {
		case NT_BODY: 
			assert(node->body != NULL);
			asm_walk(node->body);
			break;

		case NT_INSTRUCTION:
			asm_walk_instruction(node);
			break;

		case NT_PROC:
			asm_walk_proc(node);
			break;

		case NT_LABEL:
			asm_walk_label(node);
			break;

		default:
			fprintf(stderr, "Unsupported feature, %s\n", parse_getnodetypename(node->type));
			exit(1);
	}
}

void asm_walk_label(node_t* node) {
	char buff[200];
	sprintf(buff, "\t%s:\n", node->token->str);
	asm_appendasm(buff);
}

void asm_walk_proc(node_t* node) {
	// first thing's first, lets go ahead and add this symbol
	symbol_t* proc_symbol = asm_symbol_add(node->token->str, SF_PROC);

	// now we've got this, we need to add symbols for the args
	node_t* curr = node->body;
	node_t* proc_args = NULL;
	while (curr != NULL && proc_args == NULL) {
		if (curr->type == NT_PROC_ARGS_LIST) proc_args = curr;
		curr = curr->next;
	}
	if (proc_args != NULL) {
		node_t* arg = proc_args->body;
		while (arg != NULL) {
			assert(arg->type == NT_PROC_ARG);
			
			// now let's grab the type of the parameter, and set the flags accordingly.
			enum symbolflags arg_flags = SF_PROC_ARG;

			// because these will be primitives, we can use the existing infrastructure for this.
			primitive_t primtype = prim_gettype(arg->body->token->str);
			switch (primtype) {
				case PRIM_INT: arg_flags |= SF_INT; break;
				case PRIM_FLT: arg_flags |= SF_FLT; break;
				case PRIM_STR: arg_flags |= SF_STR; break;
				default: 
					log_fatal(arg->token->line_num, "%s is not a valid primitive type\n", arg->token->str);
					exit(1);
					break;
			}

			asm_symbol_add(arg->token->str, arg_flags);

			arg = arg->next;
		}
	}

	// before we start emitting the asm, we need to actually spit out the label
	asm_label(proc_symbol);

	// let's setup our proc object... in order to do this, we first need to derive the number of args
	// this procedure will take. to do this, we will need to do some prelimary scanning work... once we have this we
	// can init the proc object and take it from there.
	stackframe_t* frame = asm_stackframe_create(asm_proc_getnumargs(node));
	asm_stackframe_enter(frame);
	
	// now we need to grab the body and walk this one, which should be the second body item (i.e. ->body->next)
	// but will loop like this just to be sure.
	node_t* proc_body = NULL;
	curr = node->body;
	while (curr != NULL && proc_body == NULL) {
		if (curr->type == NT_BODY) proc_body = curr;
		curr = curr->next;
	}
	if (proc_body != NULL) asm_walk(proc_body);

	asm_stackframe_exit(frame);
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

		case OP_ADD:
			asm_ins_add(node);
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

			case TT_NUM: {
				operand_t* operand_num = operand_create(operand_getnumtype(curr->token->str));

				// we will have to create a new token, that matches with a different string.
				operand_num->token = token_create(
					operand_getnumstr(curr->token->str), 
					curr->token->line_num,
					curr->token->type
				);

				list_additem(operands, operand_num);
				break;
			}

			default:
				fprintf(stderr, "Unsupported operand, %s\n", curr->token->str);
				exit(1);
				break;
		}
		curr = curr->next;
	} while (curr != NULL);

	return operands;
}

void asm_ins_scanoperands(node_t* node, char* opcode_name, list_t** operands, int max_args) {
	node_t* operands_node = node->body;
	assert(operands_node->type == NT_OPERAND_LIST);

	*operands = asm_getoperands(operands_node);
	if ((*operands)->count != max_args) {
		log_fatal(node->token->line_num, "%s opcode candidate requires %d arguments\n", opcode_name, max_args);
		exit(1);
	}
}

void asm_ins_mov(node_t* node) {
	list_t* operands;
	asm_ins_scanoperands(node, "mov", &operands, 2);

	// now we have the operands, let's go ahead and spit these back out
	// ideally we would have some validation here, but let's leave that for
	// another day.
	assert(operands != NULL);
	operand_t* op1 = operands->items[0];
	operand_t* op2 = operands->items[1];

	char code[200];
	sprintf(code, "mov %s, %s\n", op1->token->str, op2->token->str);
	asm_appendasm(code);
}

void asm_ins_add(node_t* node) {
	list_t* operands;
	asm_ins_scanoperands(node, "add", &operands, 2);

	assert(operands != NULL);
	operand_t* op1 = operands->items[0];
	operand_t* op2 = operands->items[1];
}

symbol_t* asm_symbol_add(char* name, enum symbolflags flags) {
	symbol_t* symbol = calloc(1, sizeof(symbol_t));
	symbol->name = name;
	symbol->flags = flags;

	return symbol;
}

symbol_t* asm_symbol_lookup(char* name) {
	for (int i = 0; i < asm_state->symbols->count; i++) {
		symbol_t* sym = asm_state->symbols->items[i];
		if (strcmp(sym->name, name) == 0) return sym;
	}
	return NULL;
}

/*reg_t* asm_regalloc() {
	for (int i = 0; i < asm_state->registers->count; i++) {
		bool is_inuse = false;
		for (int j = 0; j < asm_state->symbols->count && !is_inuse; j++) {
			symbol_t* sym = asm_state->symbols->items[j];
			is_inuse |= sym->stored_in_reg == asm_state->registers->items[i];
		}

		if (!is_inuse) return asm_state->registers->items[i];
	}
	return NULL;
}*/

stackframe_t* asm_stackframe_create(int num_args) {
	stackframe_t* frame = calloc(1, sizeof(stackframe_t));
	frame->size = num_args * 64ul;
	return frame;
}

void asm_stackframe_enter(stackframe_t* frame) {
	asm_appendasm("push rbp\nmov rsp, rbp\n");

	if (frame->size > 0) {
		char str[100];
		sprintf(str, "sub rsp, %lu\n", frame->size);
		asm_appendasm(str);
	}
}

void asm_stackframe_exit(stackframe_t* frame) {
	asm_appendasm("mov rsp, rbp\npop rbp\nret\n");
}

void asm_label(symbol_t* sym_label) {
	asm_appendasm("\t");
	asm_appendasm(sym_label->name);
	asm_appendasm(":\n");
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

scope_t* asm_scope_create(stackframe_t* frame, bool is_global) {
	scope_t* scope = calloc(1, sizeof(scope_t));
	scope->frame = frame;
	scope->is_global = is_global;

	return scope;
}
