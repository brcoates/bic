#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <include/scan.h>
#include <include/parse.h>
#include <include/asm.h>

void print_node(node_t* root, char* prefix, int n);

int main(int argc, char** argv) {
	FILE* fp = fopen("/Users/ben/dev/bic/data/test.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file.\n");
		exit(1);
	}

	scan_t* scan = scan_file(fp);

	for (int i = 0; i < scan->tokens->count; i++) {
		token_t* token = scan->tokens->items[i];
		printf("%02d: %s (%s)\n", token->line_num, token->str, token_gettypename(token->type));
	}
	printf("\n");

	printf("Parsing...\n");
	parse_t* parse_root = parse(scan);
	print_node(parse_root->node_head, "", 0);

	char* asm_output = asm_codegen(parse_root);
	printf("\nasm:\n%s", asm_output);

	return 0;
}

void print_node(node_t* root, char* prefix, int n) {
	size_t new_prefix_len = strlen(prefix) + 2;
	char* new_prefix = calloc(new_prefix_len, sizeof(char));
	for (int i = 0; i < new_prefix_len; i++) {
		new_prefix[i] = ' ';
	}

	printf("%s%d ", prefix, n);

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
		printf("%s->\n", prefix);
		print_node(root->body, new_prefix, 0);
	}
	if (root->next != NULL) print_node(root->next, prefix, n + 1);

	free(new_prefix);
}
