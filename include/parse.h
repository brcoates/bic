#ifndef PARSE_H
#define PARSE_H

#include <include/scan.h>

typedef struct node node_t;

typedef enum nodetype {
    NT_BODY,
    NT_IF,
    NT_ELSE,
    NT_ENDIF,
    NT_PROC,
    NT_ENDPROC,
    NT_INSTRUCTION,
    NT_OPCODE,
    NT_OPERAND,
    NT_LABEL,
	NT_OPERAND_LIST,
	NT_LOCALVARDECL,
	NT_PROC_ARGS_LIST,
	NT_PROC_ARG,
	NT_PROC_ARG_DEF
} nodetype_t;

struct node {
    node_t* prev;
    node_t* next;
    node_t* body;

    nodetype_t type;
    token_t* token;
};

typedef struct parse {
    node_t* node_head;
} parse_t;

typedef struct parse_state {
	parse_t* parse_root;
	scan_t* scan;
	int scan_idx;
} parse_state_t;

void parse_setstate(scan_t* scan);
parse_t* parse(scan_t* scan);

node_t* parse_block();

node_t* parse_instruction();

node_t* parse_proc();
node_t* parse_proc_args();

const char* parse_getnodetypename(nodetype_t type);

token_t* parse_current();
token_t* parse_next();
token_t* parse_peeknext();
token_t* parse_prev();

bool parse_canmovenext();

// other helper funcs
node_t* parse_createnode(nodetype_t type);
void parse_consumewhitespace(bool look_ahead);

#endif
