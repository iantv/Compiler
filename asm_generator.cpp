#include "asm_generator.h"
#include <string>

using namespace std;
static const string asm_op_str[] = {
	"push", "pop", "imul", "div", "add", "sub",
	"neg", "not", "or", "and", "xor", "shl", "shr",
	"call", "mov", "ret", "test", "cmp",
	"jz", "jnz", "jmp", "jg", "jge", "jl", "jle", "je", "jne",
	
	"fld", "fild", "fstp", "fadd", "fsub", "fdiv", "fmul",
	"fiadd", "fisub", "fidiv", "fimul",
	"ja", "jb", "jae", "jbe",
	"fcom", "fcomi", "fcomip"
};

static const string asm_reg_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp" };

static const string asm_type_str[] = { "db", "dw", "dd", "dq" };