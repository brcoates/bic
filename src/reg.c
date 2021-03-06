#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <include/reg.h>
#include <include/s_util.h>

const char* reg_getname(regtype_t reg) {
	switch (reg) {
		case R_RAX: return "rax";
		case R_RBX: return "rbx";
		case R_RCX: return "rcx";
		case R_RDX: return "rdx";
		case R_RBP: return "rbp";
		case R_RSP: return "rsp";
		case R_EAX: return "eax";
		case R_EBX: return "ebx";
		case R_ECX: return "ecx";
		case R_EDX: return "edx";
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
	
	if (s_eqi(str_lower, "rax")) return R_RAX;
	if (s_eqi(str_lower, "rbx")) return R_RBX;
	if (s_eqi(str_lower, "rcx")) return R_RCX;
	if (s_eqi(str_lower, "rdx")) return R_RDX;
	if (s_eqi(str_lower, "rbp")) return R_RBP;
	if (s_eqi(str_lower, "rsp")) return R_RSP;
	if (s_eqi(str_lower, "eax")) return R_EAX;
	if (s_eqi(str_lower, "ebx")) return R_EBX;
	if (s_eqi(str_lower, "ecx")) return R_ECX;
	if (s_eqi(str_lower, "edx")) return R_EDX;
	if (s_eqi(str_lower, "r8")) return R_R8;
	if (s_eqi(str_lower, "r9")) return R_R9;
	if (s_eqi(str_lower, "r10")) return R_R10;
	if (s_eqi(str_lower, "r11")) return R_R11;
	if (s_eqi(str_lower, "r12")) return R_R12;
	if (s_eqi(str_lower, "r13")) return R_R13;
	if (s_eqi(str_lower, "r14")) return R_R14;
	if (s_eqi(str_lower, "r15")) return R_R15;

	return R_UNKNOWN;
}

list_t* reg_getall() {
	list_t* reg_list = list_create();
	list_additem(reg_list, reg_create(R_RAX));
	list_additem(reg_list, reg_create(R_RBX));
	list_additem(reg_list, reg_create(R_RCX));
	list_additem(reg_list, reg_create(R_RDX));
	list_additem(reg_list, reg_create(R_RBP));
	list_additem(reg_list, reg_create(R_RSP));
	list_additem(reg_list, reg_create(R_EAX));
	list_additem(reg_list, reg_create(R_EBX));
	list_additem(reg_list, reg_create(R_ECX));
	list_additem(reg_list, reg_create(R_EDX));
	list_additem(reg_list, reg_create(R_R8));
	list_additem(reg_list, reg_create(R_R9));
	list_additem(reg_list, reg_create(R_R10));
	list_additem(reg_list, reg_create(R_R11));
	list_additem(reg_list, reg_create(R_R12));
	list_additem(reg_list, reg_create(R_R13));
	list_additem(reg_list, reg_create(R_R14));
	list_additem(reg_list, reg_create(R_R15));
	return reg_list;
}

unsigned int reg_getsize(regtype_t type) {

	if (type & RS_8) return 8;
	if (type & RS_16) return 16;
	if (type & RS_32) return 32;
	if (type & RS_64) return 64;
	return 0;
}

bool reg_isgeneralpurpose(regtype_t reg) {
	return reg & RC_GP;
}

reg_t* reg_create(regtype_t type) {
	reg_t* reg = calloc(1, sizeof(reg_t));
	reg->type = type;
	reg->name = (char*) reg_getname(type);
	
	reg->sz = reg_getsize(type);
	assert(reg->sz > 0);

	return reg;
}
