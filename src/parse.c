#include <assert.h>
#include <stdlib.h>

#include <include/parse.h>
#include <include/opcode.h>
#include <include/scan.h>
#include <include/log.h>
#include <include/list.h>
#include <include/s_util.h>

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
	root->node_head = parse_block(BS_GLOB);

	return root;
}

node_t* parse_block(block_scope_t scope) {
	// basically a block consists of:
	// instructions | procedures | labels | calls
	
	node_t* root = parse_createnode(NT_BODY);
	node_t* next_node = root;
	bool is_first = true;

	do {
		node_t* curr_node = NULL;

		node_t* directive = parse_directive();
		if (directive != NULL) {
			curr_node = directive;
		}

		if (scope == BS_GLOB) {
			node_t* procs = parse_proc();
			if (procs != NULL) {
				if (curr_node != NULL) {
					curr_node->next = procs;
				} else {
					curr_node = procs;
				}
			}
		}

		node_t* instruction = parse_instruction();
		if (instruction != NULL) {
			if (curr_node != NULL) {
				curr_node->next = instruction;
			} else {
				curr_node = instruction;
			}
		}

		node_t* call = parse_call();
		if (call != NULL) {
			if (curr_node != NULL) {
				curr_node->next = call; 
			} else {
				curr_node = call;
			}
		}

		node_t* label = parse_label();
		if (label != NULL) {
			if (curr_node != NULL) {
				curr_node->next = label;
			} else {
				curr_node = label;
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

node_t* parse_label() {
	if (!parse_canmovenext()) return NULL;

	token_t* label_ident = parse_current();
	if (label_ident->type != TT_IDENT || parse_peeknext()->type != TT_COLON) return NULL;

	parse_next();
	parse_next();

	// create label
	node_t* label = parse_createnode(NT_LABEL);
	label->token = label_ident;

	return label;
}

node_t* parse_call() {
	if (!parse_canmovenext() || parse_current()->type != TT_KEYWORD_CALL) return NULL;

	token_t* symbol = parse_next();

	// ok great, now we have the details we can go ahead and grab the arguments/operands
	list_t* arguments = list_create();
	do {
		token_t* arg_tok = parse_next();
		if (arg_tok->type != TT_NUM && arg_tok->type != TT_REG && arg_tok->type != TT_IDENT && arg_tok->type != TT_STRING) {
			log_unexpected("number, string, register or identifier", arg_tok->str, arg_tok->line_num);
			exit(1);
		}
		list_additem(arguments, arg_tok);
	} while (parse_next()->type == TT_COMMA);

	// great, now we've got all the args & the name, let's go ahead and construct our node and return
	node_t* call_node = parse_createnode(NT_CALL);
	call_node->token = symbol;

	// now assign the arguments to the body...
	if (arguments->count > 0) {
		call_node->body = parse_createnode(NT_CALL_ARGS_LIST);
		node_t* args_list_node = call_node->body;
		
		node_t* next = NULL;
		for (int i = 0; i < arguments->count; i++) {
			token_t* arg_tok = arguments->items[i];

			node_t* arg_node = parse_createnode(NT_CALL_ARG);
			arg_node->token = arg_tok;
			
			if (args_list_node->body == NULL) {
				args_list_node->body = arg_node;
				next = arg_node;
			}
			else {
				next->next = arg_node;
				next = next->next;
			}
		}
	}
	
	return call_node;
}

node_t* parse_directive() {
	if (parse_current()->type != TT_DIRECTIVE) {
		return NULL;
	}

	// basically a directive is simply <directive> <argument>
	node_t* directive = parse_createnode(NT_DIRECTIVE);
	directive->token = parse_current();

	// seeing as we only have a single directive, just do a simple string match to validate.
	if (!s_eqi(directive->token->str, "#extern")) {
		log_compile_fatal(directive->token->line_num, "directive `%s` is not recognised\n", directive->token->str);
		exit(1);
	}

	// now, let's grab the argument. seeing as we only have one directive, all we need (for now) is just to ensure
	// this is a string. we can do fancy shit later on.
	token_t* directive_arg_token = parse_next();
	if (directive_arg_token->type != TT_STRING) {
		log_unexpected("string", directive_arg_token->str, directive_arg_token->line_num);
		exit(1);
	}

	node_t* directive_arg = parse_createnode(NT_DIRECTIVE_ARG);
	directive_arg->token = directive_arg_token;

	directive->body = directive_arg;

	parse_next(); // walk over this one

	return directive;
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
	proc->body->next = parse_block(BS_PROC);

	if (parse_current()->type != TT_KEYWORD_ENDPROC) {
		char buff[200];
		log_unexpected("endproc", parse_current()->str, parse_current()->line_num);
		exit(1);
	}
	if (parse_canmovenext())
		parse_next();

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
		case NT_CALL: return "NT_CALL";
		case NT_CALL_ARG: return "NT_CALL_ARG";
		case NT_CALL_ARGS_LIST: return "NT_CALL_ARGS_LIST";
		case NT_DIRECTIVE: return "NT_DIRECTIVE";
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

const char* parse_getscopename(block_scope_t scope) {
	switch (scope) {
		case BS_GLOB: return "BS_GLOB";
		case BS_PROC: return "BS_PROC";
		default: return "BS_UNKNOWN";
	}
}
