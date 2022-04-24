#ifndef CODEGEN_H
#define CODEGEN_H

#include <include/list.h>
#include <include/parse.h>

typedef struct cg_context cg_context_t;
struct cg_context {
	list_t* symbols;
	cg_context_t* parent;
	cg_context_t* next;
	cg_context_t* inner;
};
cg_context_t* cg_context_create();

// symbols ---
typedef enum {
	ST_PROC,
	ST_PARAM,
	ST_LOCALVAR,
	ST_LABEL,
	ST_UNKNOWN
} symbol_type_t;
typedef struct {
	char* label;
	symbol_type_t symbol_type;
} symbol_t;
typedef struct {
	list_t* symbols; // list<symbol_t*>
} symbol_table_t;
symbol_t* cg_symbol_resolve(cg_context_t* context, char* label);
void cg_symbol_add(cg_context_t* context, symbol_t* symbol);
symbol_t* cg_symbol_create(symbol_type_t type, char* label);
const char* cg_symbol_gettypename(symbol_type_t type);
// ---

void cg_init();
char* codegen(parse_t* parse_root);

void cg_walk(cg_context_t* root_context, node_t* node);

#endif
