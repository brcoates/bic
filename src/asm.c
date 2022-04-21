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

void asm_symbol_printtable_flag(int* num_flags, symbol_t* symbol, enum symbolflags flag, const char* flag_name);

static asm_state_t* asm_state = NULL;

char* asm_codegen(parse_t* parse) {
	ins_initstate(false);
	asm_initstate();
	assert(asm_state != NULL);

	node_t* curr = parse->node_head;
	asm_walk(curr);

	if (asm_state->code_output == NULL) {
		log_fatal("no executable output\n");
		exit(1);
	}

	char* output = s_alloc(asm_state->glob_output);

	if (asm_state->data_output != NULL) {
		output = s_append(output, "section .data\n");
		output = s_append(output, asm_state->data_output);
	}

	output = s_append(output, "\nsection .text\n");
	output = s_append(output, asm_state->code_output);

	return output;
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

		case NT_CALL: 
			asm_walk_call(node);
			break;

		case NT_DIRECTIVE:
			asm_walk_directive(node);
			break;

		default:
			log_fatal("unsupported feature, %s\n", parse_getnodetypename(node->type));
			exit(1);
	}
}

void asm_walk_directive(node_t* node) {
	// the only directive supported is extern, and this is validated during parsing, so we don't really need
	// to have anything fancy here, and it get's the job done. gg.
	char buff[200];
	sprintf(buff, "extern %s\n", prim_getstringvalue(node->body->token->str));

	asm_state->glob_output = s_append(asm_state->glob_output, buff);
}

void asm_walk_label(node_t* node) {
	char buff[200];
	sprintf(buff, "\t%s:\n", node->token->str);
	asm_code_append(buff);
}

void asm_walk_proc(node_t* node) {
	// first thing's first, lets go ahead and add this symbol
	symbol_t* proc_symbol = asm_symbol_add(node->token->str, SF_PROC, 0);

	// now we've got this, we need to add symbols for the args
	node_t* curr = node->body;
	node_t* proc_args = NULL;
	while (curr != NULL && proc_args == NULL) {
		if (curr->type == NT_PROC_ARGS_LIST) proc_args = curr;
		curr = curr->next;
	}
	if (proc_args != NULL) {
		node_t* arg = proc_args->body;
		int arg_idx = 0;
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
					log_compile_fatal(arg->token->line_num, "%s is not a valid primitive type\n", arg->token->str);
					exit(1);
					break;
			}

			symbol_t* arg_symbol = asm_symbol_add(arg->token->str, arg_flags, 0);
			arg_symbol->ordinal = arg_idx + 1;
			// arg_symbol->base_type = BT_REGISTER;
			// arg_symbol->base->reg = asm_getstatereg(R_RBP);

			arg = arg->next;

			arg_idx++;
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

void asm_walk_call(node_t* node) {
	// basically for a call, we need to figure out what args we have, and move these into the correct registers.
	// TODO: implement register preservation
	
}

void asm_walk_instruction(node_t* node) {
	opcode_t opcode = opcode_getopcodetype(node->token->str);
	if (opcode == OP_UNKNOWN) {
		log_fatal("unsupported opcode (%d) %s\n", opcode, node->token->str);
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
			log_fatal("unsupported codegen for opcode %s\n", opcode_gettypename(opcode));
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
				reg_t* reg = asm_getstatereg(reg_gettype(curr->token->str));

				operand_t* operand_reg = operand_create(operand_getregtype(reg));
				operand_reg->reg = reg;
				operand_reg->str = curr->token->str;

				list_additem(operands, operand_reg);
				break;
			}

			case TT_NUM: {
				operand_t* operand_num = operand_create(operand_getnumtype(curr->token->str));
				operand_num->str = operand_getnumstr(curr->token->str);

				list_additem(operands, operand_num);
				break;
			}

			case TT_IDENT: {
				// basically, when we have an identifier, it could be a number of different things which will change
				// how the identifier is handled (i.e. if it's a constant, then it will be handled differently to a
				// local variable, for instance.
				symbol_t* ident_symbol = asm_symbol_lookup(curr->token->str);
				if (ident_symbol == NULL) {
					log_fatal("symbol `%s` could not be resolved\n", curr->token->str);
					exit(1);
				}
				list_additem(operands, asm_getoperandfromsymbol(ident_symbol));
				break;
			}

			default:
				log_fatal("unsupported operand, %s\n", curr->token->str);
				exit(1);
				break;
		}
		curr = curr->next;
	} while (curr != NULL);

	return operands;
}

operand_t* asm_getoperandfromsymbol(symbol_t* sym_ident) {
	// if we are grabbing a constant value/ptr whatever, then we just need to return a basic operand
	// that will contain the name of that symbol, which has already been written to the data segment
	if (sym_ident->flags & SF_DATA) {
		operand_t* data_ident_operand = operand_create(OT_m64);
		data_ident_operand->str = sym_ident->name;
		return data_ident_operand;
	}

	// if we've got a proc argument, then we need to ensure the operand takes in
	if (sym_ident->flags & SF_PROC_ARG) {
		operand_t* arg_operand = operand_create(OT_r64);

		// ordering for function args are as follows:
		// rcx, rdx, r8, r9, ...stack
		if (sym_ident->ordinal > 4) {
			arg_operand->has_offset = true;
			arg_operand->offset = (sym_ident->ordinal - 4) * 8;
			arg_operand->reg = asm_getstatereg(R_RBP);
			arg_operand->str = sym_ident->name;
		} else {
			regtype_t reg_type;
			switch (sym_ident->ordinal) {
				case 1: reg_type = R_RCX; break;
				case 2: reg_type = R_RDX; break;
				case 3: reg_type = R_R8; break;
				case 4: reg_type = R_R9; break;
				default: 
					log_fatal("unexpected ordinal value %d\n", sym_ident->ordinal);
					exit(1);
					break;
			}

			sym_ident->stored_in_reg = reg_type;

			arg_operand->reg = asm_getstatereg(reg_type);
		}

		return arg_operand;
	}

	log_fatal("unsupported identifier symbol\n");
	exit(1);

	return NULL; // TODO: fill this in
}

void asm_ins_scanoperands(node_t* node, char* opcode_name, list_t** operands, int max_args) {
	node_t* operands_node = node->body;
	assert(operands_node->type == NT_OPERAND_LIST);

	*operands = asm_getoperands(operands_node);
	assert(*operands != NULL);
	if ((*operands)->count != max_args) {
		log_compile_fatal(node->token->line_num, "%s opcode candidate requires %d arguments, got %d\n", 
				opcode_name, max_args, (*operands)->count);
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
	sprintf(code, "mov %s, %s\n", asm_ins_resolveoperandasm(op1), asm_ins_resolveoperandasm(op2));
	asm_code_append(code);
}

void asm_ins_add(node_t* node) {
	list_t* operands;
	asm_ins_scanoperands(node, "add", &operands, 2);

	assert(operands != NULL);
	operand_t* op1 = operands->items[0];
	operand_t* op2 = operands->items[1];

	char code[200];
	sprintf(code, "add %s, %s\n", asm_ins_resolveoperandasm(op1), asm_ins_resolveoperandasm(op2));
	asm_code_append(code);
}

char* asm_ins_resolveoperandasm(operand_t* operand) {
	assert(operand != NULL);

	// if we're dealing with a memory/label reference OR an immediate numerical value, then
	// the value for our operand will be a standard string, which can be grabbed from `str`
	if (operand->type & OC_MEMORY || operand->type & OC_IMM) {
		return operand->str;
	}

	// if not, then let's see if we're using a register
	if (operand->type & OC_REGISTER && !operand->has_offset) {
		assert(operand->reg != NULL);
		return operand->reg->name;
	}

	// handling done a little differently with 
	if (operand->type & OC_REGISTER && operand->has_offset) {
		// ok this case all that needs to be done is to basically: [$register + $offset]
		char buff[200];
		if (operand->offset != 0) {
			sprintf(buff, "[%s%s%ld]", 
				operand->reg->name, 
				operand->offset < 0 ? "" : " + ",
				operand->offset);
		} else {
			sprintf(buff, "[%s]", operand->reg->name);
		}

		char* buff_s = calloc(strlen(buff) + 1, sizeof(char));
		strcpy(buff_s, buff);
		return buff_s;
	}

	log_fatal("operand `%s` (%s) has no assembly generation path\n", operand->str, operand_gettypename(operand->type));
	exit(1);
}

symbol_t* asm_symbol_add(char* name, enum symbolflags flags, unsigned long offset) {
	if (asm_symbol_lookup(name) != NULL) {
		log_fatal("redefinition of symbol with name `%s`\n", name);
		exit(1);
	}

	symbol_t* symbol = calloc(1, sizeof(symbol_t));
	symbol->base = calloc(1, sizeof(union base));
	symbol->name = name;
	symbol->flags = flags;
	symbol->offset = offset;
	symbol->stored_in_reg = R_UNKNOWN;

	list_additem(asm_state->symbols, symbol);

	return symbol;
}

symbol_t* asm_symbol_lookup(char* name) {
	for (int i = 0; i < asm_state->symbols->count; i++) {
		symbol_t* sym = asm_state->symbols->items[i];
		if (s_eq(sym->name, name)) return sym;
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
	if (num_args > 4) {
		frame->size = (num_args - 4) * 64u;
	}
	return frame;
}

void asm_stackframe_enter(stackframe_t* frame) {
	asm_code_append("push rbp\nmov rsp, rbp\n");

	if (frame->size > 0) {
		char str[100];
		sprintf(str, "sub rsp, %lu\n", frame->size);
		asm_code_append(str);
	}
}

void asm_stackframe_exit(stackframe_t* frame) {
	asm_code_append("mov rsp, rbp\npop rbp\nret\n");
}

void asm_label(symbol_t* sym_label) {
	char label_buff[200];
	sprintf(label_buff, "\t%s:\n", sym_label->name);
	
	if (sym_label->flags & SF_DATA) asm_data_append(label_buff);
	else asm_code_append(label_buff);
}

void asm_code_append(char* code) {
	asm_state->code_output = s_append(asm_state->code_output, code);
}

void asm_data_append(char* code) {
	asm_state->data_output = s_append(asm_state->data_output, code);
}

void asm_initstate() {
	if (asm_state != NULL) {
		free(asm_state);
	}

	asm_state = calloc(1, sizeof(asm_state_t));
	asm_state->glob_output = s_alloc("bits 64\ndefault rel\nglobal main\n\n");
	asm_state->code_output = NULL; // s_alloc("\tsection .text\n");
	asm_state->data_output = NULL; // s_alloc("\tsection .data\n");
	asm_state->symbols = list_create();
	asm_state->registers = reg_getall();
	asm_state->reserved_registers = list_create();
}

void asm_reg_clear(regtype_t regtype) {}

bool asm_reg_reserve(regtype_t regtype) {}

reg_t* asm_reg_reservesize(size_t size) {
	for (int i = 0; i < asm_state->registers->count; i++) {
		reg_t* reg = asm_state->registers->items[i];
		if (reg->sz == size) {
			// now check if it's in use
			bool in_use = false;
			for (int j = 0; j < asm_state->reserved_registers->count && !in_use; j++) {
				reg_t* reserved_reg = asm_state->reserved_registers->items[j];
				if (reserved_reg->type == reg->type) in_use = true;
			}
			if (!in_use) return reg;
		}
	}
	return NULL;
}

void asm_reg_clearall() {
	asm_state->reserved_registers->count = 0;
}

scope_t* asm_scope_create(stackframe_t* frame, bool is_global) {
	scope_t* scope = calloc(1, sizeof(scope_t));
	scope->frame = frame;
	scope->is_global = is_global;

	return scope;
}

reg_t* asm_getstatereg(regtype_t reg_type) {
	for (int i = 0; i < asm_state->registers->count; i++) {
		reg_t* reg = asm_state->registers->items[i];
		if (reg_type == reg->type) return reg;
	}
	return NULL;
}

void asm_symbol_printtable() {
	for (int i = 0; i < asm_state->symbols->count; i++) {
		symbol_t* sym = asm_state->symbols->items[i];
		printf("symbol { name = '%s'; flags = ", sym->name);

		int num_flags = 0;
		if (sym->flags & SF_UNKNOWN) asm_symbol_printtable_flag(&num_flags, sym, SF_UNKNOWN, "SF_");
		if (sym->flags & SF_PROC) asm_symbol_printtable_flag(&num_flags, sym, SF_PROC, "SF_PROC");
		if (sym->flags & SF_PROC_ARG) asm_symbol_printtable_flag(&num_flags, sym, SF_PROC_ARG, "SF_PROC_ARG");
		if (sym->flags & SF_VAR) asm_symbol_printtable_flag(&num_flags, sym, SF_VAR, "SF_VAR");
		if (sym->flags & SF_INT) asm_symbol_printtable_flag(&num_flags, sym, SF_INT, "SF_INT");
		if (sym->flags & SF_FLT) asm_symbol_printtable_flag(&num_flags, sym, SF_FLT, "SF_FLT");
		if (sym->flags & SF_STR) asm_symbol_printtable_flag(&num_flags, sym, SF_STR, "SF_STR");
		if (sym->flags & SF_EXE) asm_symbol_printtable_flag(&num_flags, sym, SF_EXE, "SF_EXE");
		if (sym->flags & SF_DATA) asm_symbol_printtable_flag(&num_flags, sym, SF_DATA, "SF_DATA");

		char* base_type_name;
		if (sym->base_type == BT_UNKNOWN) base_type_name = "??";
		else if (sym->base_type == BT_REGISTER) base_type_name = "reg";
		else if (sym->base_type == BT_SYMBOL) base_type_name = "symbol";
		else base_type_name = "internal error";
		printf("; base(%s) = ", base_type_name);

		if (sym->base_type == BT_REGISTER) {
			printf("%%%s(%lu)", sym->base->reg->name, sym->base->reg->sz);
		} else if (sym->base_type == BT_SYMBOL) {
			printf("[%s]", sym->base->symbol->name);
		} else {
			printf("??");
		}

		printf("; offset = %lu; }\n", sym->offset);
	}
}

void asm_symbol_printtable_flag(int* num_flags, symbol_t* symbol, enum symbolflags flag, const char* flag_name) {
	bool is_match = symbol->flags & flag;
	if (is_match) {
		if (*num_flags > 0) printf(" | ");
		printf("%s", flag_name);
		*num_flags = *num_flags + 1;
	}
}
