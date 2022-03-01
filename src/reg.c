#include <stdlib.h>
#include <include/reg.h>

const char* reg_getname(reg_t reg) {
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
