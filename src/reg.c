#include <stdlib.h>
#include <include/reg.h>

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

list_t* reg_getall() {
	list_t* reg_list = list_create();
	for (int i = 0; i < R_R15; i++) list_additem(reg_list, reg_create(i));
	return reg_list;
}

reg_t* reg_create(regtype_t type) {
	reg_t* reg = calloc(1, sizeof(reg_t));
	reg->type = type;
	reg->name = (char*) reg_getname(type);
	reg->sz = 64; // all are 64 for the time being

	return reg;
}
