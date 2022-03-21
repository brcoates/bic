#include <assert.h>
#include <stdlib.h>

#include <include/parse.h>
#include <include/opcode.h>
#include <include/scan.h>
#include <include/log.h>
#include <include/list.h>

static parse_state_t* parse_state = NULL;

void parse_setstate(scan_t* scan) {
	if (parse_state == NULL) {
		parse_state = calloc(1, sizeof(parse_state_t));
	}
	parse_state->parse_root = calloc(1, sizeof(parse_t));
	parse_state->scan = scan;
	parse_state->scan_idx = 0;
}

parse_t* parse(scan_t* scan) {
	parse_setstate(scan);

	parse_t* root = parse_state->parse_root;
	
	// the root of any program is a block, so let's parse that first, and go from there.
	root->node_head = parse_block();

	return root;
}

node_t* parse_block() {
	// basically a block consists of:
	// instructions | procedures
	
	node_t* root = parse_createnode(NT_BODY);
	node_t* next_node = root;
	bool is_first = true;

	do {
		node_t* curr_node = NULL;

		node_t* procs = parse_proc();
		if (procs != NULL) {
			curr_node = procs;
		}

		node_t* instructions = parse_instruction();
		if (instructions != NULL) {
			if (curr_node != NULL) {
				curr_node->next = instructions; 
			} else {
				curr_node = instructions;
			}
		}
		
		// this will allow us to continually chain nodes
		if (is_first) {
			next_node->body = curr_node;
		} else {
			next_node->next = curr_node;
		}
		next_node = curr_node;

		is_first = false;
	} while (next_node != NULL && parse_canmovenext());

	return root;
}

node_t* parse_instruction() {
	// first of all, let's check to see if the next node is in fact an opcode
	token_t* opcode = parse_current();
	if (!opcode_isopcode(opcode->str)) {
		if (opcode->line_num > 4) {
		}
		return NULL;
	}

	list_t* operands_list = list_create();
	bool is_first = true;
	do {
		if (is_first || parse_current()->type == TT_COMMA) {
			parse_next();
		}

		// next up, is our first instruction
		token_t* operand = parse_current();
		if (operand->type != TT_IDENT && operand->type != TT_REG && operand->type != TT_NUM) {
			log_unexpected("operand", (char*) token_gettypename(operand->type), operand->line_num);
			exit(1);
		}

		// add to list
		list_additem(operands_list, operand);

		is_first = false;
	} while (parse_canmovenext() && parse_next()->type == TT_COMMA);

	node_t* ins_node = parse_createnode(NT_INSTRUCTION);
	ins_node->token = opcode;
	ins_node->body = parse_createnode(NT_OPERAND_LIST);

	node_t* initial_operand = NULL;
	node_t* next_operand = NULL;

	for (int i = 0; i < operands_list->count; i++) {
		token_t* operand_token = (token_t*) operands_list->items[i];
		assert(operand_token != NULL);

		node_t* operand_node = parse_createnode(NT_OPERAND);
		assert(operand_node != NULL);
		operand_node->token = operand_token;

		if (initial_operand == NULL) {
			initial_operand = operand_node;
			next_operand = operand_node;
		} else {
			next_operand->next = operand_node;
			next_operand = operand_node;
		}
	}

	node_t* ins_list_node = ins_node->body;
	ins_list_node->body = initial_operand;

	return ins_node;
}

node_t* parse_proc() {
	token_t* proc_kw_tok = parse_current();
	if (proc_kw_tok->type != TT_KEYWORD_PROC) {
		return NULL;
	}

	node_t* proc = parse_createnode(NT_PROC);

	token_t* proc_name = parse_next();
	if (proc_name->type != TT_IDENT) {
		log_unexpected("identifier", proc_name->str, proc_name->line_num);
		exit(1);
	}
	proc->token = proc_name;

	if (parse_next()->type != TT_COLON) {
		log_unexpected(":", parse_current()->str, parse_current()->line_num);
		exit(1);
	}

	// now let's go ahead and parse the args list...
	proc->body = parse_proc_args();

	if (parse_next()->type != TT_KEYWORD_BEGINPROC) {
		log_unexpected("begin", parse_current()->str, parse_current()->line_num);
		exit(1);
	}
	parse_next();

	// parse body of proc
	proc->body->next = parse_block();

	if (parse_current()->type != TT_KEYWORD_ENDPROC) {
		char buff[200];
		log_unexpected("endproc", parse_current()->str, parse_current()->line_num);
		exit(1);
	}

	return proc;
}

node_t* parse_proc_args() {
	node_t* args_list_node = parse_createnode(NT_PROC_ARGS_LIST);
	node_t* curr = NULL;

	// each arg will be a TT_KEYWORD_PRIMTYPE, TT_IDENT & TT_SEMICOLON
	while (parse_peeknext()->type == TT_KEYWORD_PRIMTYPE) {
		token_t* type_spec = parse_next();
		
		token_t* ident = parse_next();
		if (ident->type != TT_IDENT) {
			log_unexpected("identifier", parse_current()->str, parse_current()->line_num);
			exit(1);
		}

		// great, now we have the type and identifier, let's go ahead and make sure we end
		// with a semicolon
		if (parse_next()->type != TT_SEMICOLON) {
			log_unexpected(";", parse_current()->str, parse_current()->line_num);
			exit(1);
		}

		// now we know everything is all good, let's create our node and just add it!
		node_t* arg_node = parse_createnode(NT_PROC_ARG);
		arg_node->token = ident;
		arg_node->body = parse_createnode(NT_PROC_ARG_DEF);
		arg_node->body->token = type_spec;

		if (curr == NULL) {
			args_list_node->body = arg_node;
			curr = arg_node;
		} else {
			curr->next = arg_node;
			curr = arg_node;
		}
	}

	return args_list_node;
}

const char* parse_getnodetypename(nodetype_t type) {
	switch (type) {
		case NT_BODY: return "NT_BODY";
		case NT_IF: return "NNT_IF";
		case NT_ELSE: return "NT_ELSE";
		case NT_ENDIF: return "NT_ENDIF";
		case NT_PROC: return "NT_PROC";
		case NT_ENDPROC: return "NT_ENDPROC";
		case NT_INSTRUCTION: return "NT_INSTRUCTION";
		case NT_OPCODE: return "NT_OPCODE";
		case NT_OPERAND: return "NT_OPERAND";
		case NT_LABEL: return "NT_LABEL";
		case NT_OPERAND_LIST: return "NT_OPERAND_LIST";
		case NT_LOCALVARDECL: return "NT_LOCALVARDECL";
		case NT_PROC_ARGS_LIST: return "NT_PROC_ARGS_LIST";
		case NT_PROC_ARG: return "NT_PROC_ARG";
		case NT_PROC_ARG_DEF: return "NT_PROC_ARG_DEF";
		default: return NULL;
	}
}

token_t* parse_current() {
	return parse_state->scan->tokens->items[parse_state->scan_idx];
}

token_t* parse_next() {
	assert(parse_canmovenext());
	return parse_state->scan->tokens->items[++parse_state->scan_idx];
}

token_t* parse_peeknext() {
	assert(parse_canmovenext());
	return parse_state->scan->tokens->items[parse_state->scan_idx + 1];
}

token_t* parse_prev() {
	assert(parse_state->scan_idx - 1 >= 0);
	return parse_state->scan->tokens->items[parse_state->scan_idx - 1];
}

bool parse_canmovenext() {
	return parse_state->scan_idx + 1 < parse_state->scan->tokens->count;
}

node_t* parse_createnode(nodetype_t type) {
	node_t* node = calloc(1, sizeof(node_t));
	node->type = type;

	return node;
}
