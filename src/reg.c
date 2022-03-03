#include <string.h>
#include <assert.h>

#include <include/reg.h>
#include <include/s_util.h>

const char* reg_getname(regtype_t reg) {
	switch (reg) {
		case R_RAX: return "rax";
		case R_RBX: return "rbx";
		case R_RCX: return "rcx";
		case R_RDX: return "rdx";
		case R_R8: return "r8";
        case R_R9: return "r9";
        case R_R10: return "r10";
        case R_R11: return "r11";
        case R_R12: return "r12";
        case R_R13: return "r13";
        case R_R14: return "r14";
        case R_R15: return "r15";
        default: return NULL;
	}
}

regtype_t reg_gettype(char* str) {
	char* str_lower = s_tolower(str);
	
	if (strcmp(str_lower, "rax")) return R_RAX;
	if (strcmp(str_lower, "rbx")) return R_RBX;
	if (strcmp(str_lower, "rcx")) return R_RCX;
	if (strcmp(str_lower, "rdx")) return R_RDX;
	if (strcmp(str_lower, "r8")) return R_R8;
	if (strcmp(str_lower, "r9")) return R_R9;
	if (strcmp(str_lower, "r10")) return R_R10;
	if (strcmp(str_lower, "r11")) return R_R11;
	if (strcmp(str_lower, "r12")) return R_R12;
	if (strcmp(str_lower, "r13")) return R_R13;
	if (strcmp(str_lower, "r14")) return R_R14;
	if (strcmp(str_lower, "r15")) return R_R15;

	return R_UNKNOWN;
}

list_t* reg_getall() {
	list_t* reg_list = list_create();
	for (int i = 0; i < R_UNKNOWN; i++) list_additem(reg_list, reg_create(i));
	return reg_list;
}

unsigned int reg_getsize(regtype_t type) {
	switch (type) {
		case R_RAX:
		case R_RBX:
		case R_RCX:
		case R_RDX:
		case R_R8: 
		case R_R9: 
		case R_R10:
		case R_R11:
		case R_R12:
		case R_R13:
		case R_R14:
		case R_R15:
			return 64;

		default:
			return 0;
	}
}

reg_t* reg_create(regtype_t type) {
	reg_t* reg = calloc(1, sizeof(reg_t));
	reg->type = type;
	reg->name = (char*) reg_getname(type);
	
	reg->sz = reg_getsize(type);
	assert(reg->sz > 0);

	return reg;
}
