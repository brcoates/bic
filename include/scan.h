#ifndef SCAN_H
#define SCAN_H

#include <stdbool.h>
#include <stdio.h>
#include <include/list.h>

typedef enum toktype {
    TT_UNKNOWN,
	TT_OPCODE,
	TT_IDENT,
	TT_REG,
	TT_NUM,
	TT_COMMENT,
	TT_STRING,
	TT_EOL,
    TT_COMMA,
    TT_COLON,
    TT_SEMICOLON,
    TT_KEYWORD_PROC,
    TT_KEYWORD_BEGINPROC,
    TT_KEYWORD_ENDPROC,
    TT_KEYWORD_IF,
    TT_KEYWORD_ELSE,
    TT_KEYWORD_ENDIF
} toktype_t;

typedef struct token {
    char* str;
    int line_num;
	toktype_t type;
} token_t;

typedef struct scan {
    list_t* tokens;
} scan_t;

scan_t* scan_file(FILE* fp);

bool scan_isdelim(char c);
bool scan_iswhitespace(char c);

token_t* token_create(char* str, int line_num, toktype_t type);

toktype_t token_gettype(const char* token);
const char* token_gettypename(toktype_t type);

#endif