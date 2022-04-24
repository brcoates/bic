#ifndef CODEGEN_H
#define CODEGEN_H

#include <include/list.h>
#include <include/parse.h>

typedef struct symbol symbol_t;

typedef struct {
	symbol_t* symbol;
	char* value;
} stringdef_t;
stringdef_t* cg_stringdef_create(symbol_t* symbol, char* string_value);
typedef struct {
	list_t* string_defs; // list<stringdef_t*>
} cg_context_t;
void cg_context_init();
void cg_context_addstringdef(stringdef_t* stringdef);

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
struct symbol {
	char* label;
	symbol_type_t symbol_type;
};
typedef struct {
	list_t* symbols; // list<symbol_t*>
} symbol_table_t;
symbol_t* cg_symbol_resolve(scope_t* scope, char* label);
void cg_symbol_add(scope_t* scope, symbol_t* symbol);
symbol_t* cg_symbol_create(symbol_type_t type, char* label);
const char* cg_symbol_gettypename(symbol_type_t type);

char* cg_symbol_newstringlabel(char* str); // this is to create new labels for literal strings to be allocated
// ---

void cg_init();
char* codegen(parse_t* parse_root);

void cg_walk(scope_t* root_scope, node_t* node);

#endif
