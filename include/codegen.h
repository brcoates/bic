#ifndef CODEGEN_H
#define CODEGEN_H

#include <include/list.h>
#include <include/parse.h>

typedef struct scope scope_t;
struct scope {
	list_t* symbols;
	scope_t* parent;
	scope_t* next;
	scope_t* inner;
};
scope_t* cg_scope_create();

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
symbol_t* cg_symbol_resolve(scope_t* scope, char* label);
void cg_symbol_add(scope_t* scope, symbol_t* symbol);
symbol_t* cg_symbol_create(symbol_type_t type, char* label);
const char* cg_symbol_gettypename(symbol_type_t type);
// ---

void cg_init();
char* codegen(parse_t* parse_root);

void cg_walk(scope_t* root_scope, node_t* node);

#endif
