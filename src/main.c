#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <include/scan.h>
#include <include/parse.h>
#include <include/asm.h>

static struct options {
	struct debug_options {
		bool print_scans;
		bool print_parse;
		bool print_asm;
	} * debug_options;
} * options;

void print_node(node_t* root, char* prefix, int n, bool in_body);
void options_setup(int argc, char** argv);
void options_dispose();

int main(int argc, char** argv) {
	options_setup(argc, argv);

	FILE* fp = fopen("/Users/ben/dev/bic/data/test.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file.\n");
		exit(1);
	}

	scan_t* scan = scan_file(fp);
	if (options->debug_options->print_scans) {
		for (int i = 0; i < scan->tokens->count; i++) {
			token_t* token = scan->tokens->items[i];
			printf("%02d: %s (%s)\n", token->line_num, token->str, token_gettypename(token->type));
		}
		printf("\n");
	}

	parse_t* parse_root = parse(scan);
	if (options->debug_options->print_parse) {
		print_node(parse_root->node_head, "", 1, false);
		printf("\n");
	}

	char* asm_output = asm_codegen(parse_root);
	if (options->debug_options->print_asm) {
		printf("%s", asm_output);
	}

	options_dispose();

	return 0;
}

void print_node(node_t* root, char* prefix, int n, bool in_body) {
	size_t new_prefix_len = strlen(prefix) + 2;
	char* new_prefix = calloc(new_prefix_len, sizeof(char));
	for (int i = 0; i < new_prefix_len; i++) {
		new_prefix[i] = ' ';
	}

	if (in_body && n == 1) {
		printf(" [%d] ", n);
	} else if (in_body) {
		printf("%s ", prefix);
	} else {
		if (prefix == NULL || strlen(prefix) >= 1) printf("%s ", prefix);
		printf("[%d] ", n);
	}
	
	// basically, if we have a token, print this out and/or along with the node type
	if (root->token != NULL) printf("%s (%s)", root->token->str, token_gettypename(root->token->type));

	// now we grab the type
	printf(
		"%s%s\n", 
		root->token != NULL ? ": " : "",
		parse_getnodetypename(root->type)
	);

	// and do the body if we have one
	if (root->body != NULL) {
		printf("%s->", prefix);
		print_node(root->body, new_prefix, 1, true);
	}
	if (root->next != NULL) print_node(root->next, prefix, n + 1, false);

	free(new_prefix);
}

void options_setup(int argc, char** argv) {
	options = calloc(1, sizeof(struct options));
	options->debug_options = calloc(1, sizeof(struct debug_options));

	// TODO: parse command-line args
	options->debug_options->print_scans = true;
	options->debug_options->print_parse = true;
	options->debug_options->print_asm = true;
}

void options_dispose() {
	assert(options != NULL);
	assert(options->debug_options != NULL);

	free(options->debug_options);
	free(options);
}
