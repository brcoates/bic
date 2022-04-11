#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <include/scan.h>
#include <include/parse.h>
#include <include/asm.h>
#include <include/log.h>
#include <include/s_util.h>

static struct options {
	struct debug_options {
		bool print_scans;
		bool print_parse;
		bool print_asm;
	} * debug_options;
	char* input_filename;
} * options;

void print_node(node_t* root, char* prefix, int n, bool in_body);

void options_printhelp();
void options_setup(int argc, char** argv);
void options_dispose();
bool options_isoption(char* str);
bool options_isswitch(char* str);

int main(int argc, char** argv) {
	options_setup(argc, argv);

	if (options->input_filename == NULL) {
		log_fatal("no input file specified\n");
		exit(1);
	}
	FILE* fp = fopen(options->input_filename, "r");
	if (fp == NULL) {
		log_fatal("error opening file.\n");
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

	// set input filename
	if (argc > 1 && !options_isoption(argv[argc - 1]) && !options_isswitch(argv[argc - 1])) {
		options->input_filename = argv[argc - 1];
	}

	options->debug_options->print_scans = false;
	options->debug_options->print_parse = false; 
	options->debug_options->print_asm = true;
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			char* arg = argv[i];
			assert(arg != NULL);

			if (s_eqi(arg, "--debug-scan")) options->debug_options->print_scans = true;
			else if (s_eqi(arg, "--debug-parse")) options->debug_options->print_parse = true;
			else if (s_eqi(arg, "--no-asm")) options->debug_options->print_asm = false;
			else if (s_eqi(arg, "--help")) options_printhelp();
		}
	}
}

bool options_isoption(char* str) {
	// TODO: check for actual options
	assert(str != NULL);
	return strlen(str) > 0 ? str[0] == '-' : false;
}

bool options_isswitch(char* str) {
	// TODO: check for actual switches
	assert(str != NULL);
	return strlen(str) > 0 ? str[0] == '-' : false;
}

void options_dispose() {
	assert(options != NULL);
	assert(options->debug_options != NULL);

	free(options->debug_options);
	free(options);
}

void options_printhelp() {
	const char* help_message =
		"bic [--debug-scan] [--debug-parse] [--no-asm] [--help] <input filename>\n"
		"	--debug-scan:	Print scan debugging\n"
		"	--debug-parse: 	Print parse debugging\n"
		"	--no-asm:		Disable assembly output\n"
		"	--help:			Print this help message";

	printf("%s\n", help_message);
}
