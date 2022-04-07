#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <include/scan.h>
#include <include/reg.h>
#include <include/opcode.h>
#include <include/primitive.h>
#include <include/s_util.h>

scan_t* scan_file(FILE* fp) {

    list_t* tokens = list_create();

    int line_num = 1;
    bool in_str = false;
    char tok_str[100] = {0};
    char prev = '\0';
    char c;
    do {
        c = fgetc(fp);

        bool is_escapech = prev == '\\';
        bool tok_is_str = c == '\"' && !is_escapech;

        if (tok_is_str) in_str = !in_str;

        if (scan_isdelim(c) && strlen(tok_str) > 0 && !in_str) {
            char* str = calloc(strlen(tok_str) + (tok_is_str ? 2 : 1), sizeof(char));
            strcpy(str, tok_str);
            if (tok_is_str) {
                str[strlen(tok_str)] = '\"';
                str[strlen(tok_str) + 1] = '\0';
            }

			if (!scan_isstrwhitespace(str))
				list_additem(tokens, token_create(str, line_num, token_gettype(str)));

            tok_str[0] = '\0';

            // now add the delimiter
            if (!scan_iswhitespace(c) && !tok_is_str && c != EOF) {
                str = calloc(2, sizeof(char));
                str[0] = c;
                list_additem(tokens, token_create(str, line_num, token_gettype(str)));
            }
        } else if (in_str || !scan_iswhitespace(c) && c != EOF) {
            char str[2] = {c, '\0'};
            strcat(tok_str, str);
        }

		if (c == '\n') line_num++;

        prev = c;
    } while (c != EOF);

    scan_t* scan = calloc(1, sizeof(scan_t));
    scan->tokens = tokens;
    return scan;
}

bool scan_isdelim(char c) {
    if (c >= 'a' && c <= 'z') return false;
    if (c >= 'A' && c <= 'Z') return false;
    if (c >= '0' && c <= '9') return false;
    if (c == '_' || c == '@') return false;
    return true;
}

bool scan_iswhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool scan_isstrwhitespace(char* str) {
	for (int i = 0; i < strlen(str); i++) {
		if (!scan_iswhitespace(str[i])) return false;
	}
	return true;
}

token_t* token_create(char* str, int line_num, toktype_t type) {
    token_t* tok = calloc(1, sizeof(token_t));
    tok->str = str;
    tok->line_num = line_num;
    tok->type = type;

    return tok;
}

toktype_t token_gettype(const char* token) {
    size_t tok_len = strlen(token);
    if (tok_len == 0) {
        return TT_UNKNOWN;
    }

    char* lower_token = s_tolower((char*) token);

	if (opcode_isopcode((char*) token)) return TT_OPCODE;
	if (prim_isprimitive((char*) token)) return TT_KEYWORD_PRIMTYPE;

    if (token[0] == ';' && tok_len == 1) return TT_SEMICOLON;
    if (token[0] == ':' && tok_len == 1) return TT_COLON;
    if (token[0] == ',' && tok_len == 1) return TT_COMMA;
    if (tok_len > 1 && token[0] == '$') return TT_NUM;
    if (tok_len > 2 && token[1] == '/' && token[2] == '/') return TT_COMMENT;
    if (tok_len >= 2 && token[0] == '\"' && token[tok_len - 1] == '\"') return TT_STRING;

	static list_t* all_registers = NULL;
	if (all_registers == NULL) {
		all_registers = reg_getall();
	}
    for (int i = 0; i < all_registers->count; i++) {
		reg_t* reg = all_registers->items[i];
        if (reg->name != NULL && strcmp(lower_token, reg->name) == 0) return TT_REG;
    }

    // now we go over the keywords
    if (strcmp(lower_token, "if") == 0) return TT_KEYWORD_IF;
    if (strcmp(lower_token, "else") == 0) return TT_KEYWORD_ELSE;
    if (strcmp(lower_token, "endif") == 0) return TT_KEYWORD_ENDIF;
    if (strcmp(lower_token, "proc") == 0) return TT_KEYWORD_PROC;
    if (strcmp(lower_token, "begin") == 0) return TT_KEYWORD_BEGINPROC;
    if (strcmp(lower_token, "endproc") == 0) return TT_KEYWORD_ENDPROC;
	if (strcmp(lower_token, "call") == 0) return TT_KEYWORD_CALL;

    return TT_IDENT;
}

const char* token_gettypename(toktype_t type) {
    switch (type) {
        case TT_IDENT: return "TT_IDENT";
        case TT_REG: return "TT_REG";
        case TT_NUM: return "TT_NUM";
        case TT_COMMENT: return "TT_COMMENT";
        case TT_COMMA: return "TT_COMMA";
        case TT_COLON: return "TT_COLON";
        case TT_SEMICOLON: return "TT_SEMICOLON";
        case TT_KEYWORD_PROC: return "TT_KEYWORD_PROC";
        case TT_KEYWORD_BEGINPROC: return "TT_KEYWORD_BEGINPROC";
        case TT_KEYWORD_ENDPROC: return "TT_KEYWORD_ENDPROC";
        case TT_KEYWORD_IF: return "TT_KEYWORD_IF";
        case TT_KEYWORD_ELSE: return "TT_KEYWORD_ELSE";
        case TT_KEYWORD_ENDIF: return "TT_KEYWORD_ENDIF";
		case TT_OPCODE: return "TT_OPCODE";
		case TT_KEYWORD_PRIMTYPE: return "TT_KEYWORD_PRIMTYPE";
		case TT_KEYWORD_CALL: return "TT_KEYWORD_CALL";
		case TT_UNKNOWN: return "TT_UNKNOWN";
        default: return "??";
    }
}
