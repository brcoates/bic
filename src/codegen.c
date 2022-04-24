#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <include/codegen.h>
#include <include/s_util.h>

void print_symbol_table(scope_t* scope, int depth, int child_idx);

static scope_t* root_scope = NULL;
static cg_context_t* context = NULL;

void cg_init() {
	root_scope = cg_scope_create();
	cg_context_init();
}

void cg_context_init() {
	context = malloc(sizeof(cg_context_t));
	context->string_defs = list_create();
}

char* codegen(parse_t* parse_root) {
	cg_init();
	cg_walk(root_scope, parse_root->node_head);

	print_symbol_table(root_scope, 0, 0);

	return "not implemented";
}

void print_symbol_table(scope_t* scope, int depth, int child_idx) {
	printf("TABLE (depth = %d; idx = %d):\n", depth, child_idx);
	for (int i = 0; i < scope->symbols->count; i++) {
		symbol_t* symbol = scope->symbols->items[i];
		printf("\t%s (%s)\n", symbol->label, cg_symbol_gettypename(symbol->symbol_type));
	}
	if (scope->inner != NULL) print_symbol_table(scope->inner, depth + 1, 0);
	if (scope->next != NULL) print_symbol_table(scope->next, depth, child_idx + 1);
}

void cg_walk(scope_t* root_scope, node_t* node) {
	// here we need to basically just walk all nodes recursively (bottom down, pre-order)... the goal of this is
	// essentially:
	// 1. identify all symbols
	// 2. establish scope for each symbol
	
	if (node == NULL) return;

	scope_t* ctx = root_scope;

	if (node->type != NT_DIRECTIVE) {
		symbol_type_t symbol_type;
		char* label = NULL;
		symbol_t* symbol = NULL;
		
		switch (node->type) {
			case NT_PROC: symbol_type = ST_PROC; break;
			case NT_PROC_ARG: symbol_type = ST_PARAM; break;
			case NT_LOCALVARDECL: symbol_type = ST_LOCALVAR; break;
			case NT_LABEL: symbol_type = ST_LABEL; break;
			default:
				// the only thing to consider is the 
				if (node->token != NULL && node->token->type == TT_STRING) {
					label = cg_symbol_newstringlabel(node->token->str);
					symbol_type = ST_LABEL;

					symbol = cg_symbol_create(symbol_type, label);
					cg_context_addstringdef(cg_stringdef_create(symbol, node->token->str));
				} else {
					symbol_type = ST_UNKNOWN;
				}
				break;
		}
		if (symbol_type != ST_UNKNOWN) {
			if (label == NULL) {
				assert(node->token != NULL && node->token->str != NULL);
				label = node->token->str;
			}

			// TODO: Fix this, i'm really not happy with this, we're re-doing the symbol assignment as above and to be
			// honest, this whole function is a bit of a mess... would be great to come back and refactor this one.
			if (symbol == NULL) {
				symbol = cg_symbol_create(symbol_type, label);
			}
			cg_symbol_add(ctx, symbol);
		}

		// if we have a proc declaration, let's go ahead and create a scope for this one...
		if (symbol_type == ST_PROC) {
			scope_t* proc_scope = cg_scope_create();

			if (ctx->inner == NULL) {
				ctx->inner = proc_scope;
			} else {
				// if we already have an inner, then we need to append us to the end. let's go find the tail.
				scope_t* tail = ctx->inner;
				while (tail->next != NULL) tail = tail->next;

				tail->next = proc_scope;
			}
			proc_scope->parent = ctx;
			ctx = proc_scope;
		} 

		cg_walk(ctx, node->body);

		if (symbol_type == ST_PROC) {
			assert(ctx->parent != NULL);
			ctx = ctx->parent;
		}
	}

	cg_walk(ctx, node->next);
}

scope_t* cg_scope_create() {
	scope_t* scope = calloc(1, sizeof(scope_t));
	scope->symbols = list_create();
	return scope;
}

symbol_t* cg_symbol_resolve(scope_t* scope, char* label) {
	for (int i = 0; i < scope->symbols->count; i++) {
		symbol_t* symbol = scope->symbols->items[i];
		assert(symbol != NULL);
		if (s_eq(label, symbol->label)) {
			return symbol;
		}
	}
	if (scope->inner != NULL) return cg_symbol_resolve(scope->inner, label);
	if (scope->next != NULL) return cg_symbol_resolve(scope->next, label);
	return NULL;
}

void cg_symbol_add(scope_t* scope, symbol_t* symbol) {
	list_additem(scope->symbols, symbol);
}

symbol_t* cg_symbol_create(symbol_type_t type, char* label) {
	symbol_t* symbol = calloc(1, sizeof(symbol_t));
	symbol->label = label;
	symbol->symbol_type = type;

	return symbol;
}

const char* cg_symbol_gettypename(symbol_type_t type) {
	switch (type) {
		case ST_PROC: return "ST_PROC";
		case ST_PARAM: return "ST_PARAM";
		case ST_LOCALVAR: return "ST_LOCALVAR";
		case ST_LABEL: return "ST_LABEL";
		case ST_UNKNOWN: return "ST_UNKNOWN";
	}
}

char* cg_symbol_newstringlabel(char* str) {
	static unsigned int string_idx = 0;

	char buff[200];
	sprintf(buff, "str_%lu_%d", strlen(str), ++string_idx);

	return s_alloc(buff);
}

stringdef_t* cg_stringdef_create(symbol_t* symbol, char* string_value) {
	stringdef_t* stringdef = malloc(sizeof(stringdef_t));
	stringdef->symbol = symbol;
	stringdef->value = string_value;

	return stringdef;
}

void cg_context_addstringdef(stringdef_t* stringdef) {
	list_additem(context->string_defs, stringdef);
}
