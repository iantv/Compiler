#ifndef ASMGEN_H
#define ASMGEN_H
enum asm_op_t{ 
	PUSH, POP, IMUL, DIV, ADD, SUB,
	NEG, NOT, OR, AND, XOR, SHL, SHR,
	CALL, MOV, RET, TEST, CMP,
	JZ, JNZ, JMP, JG, JGE, JL, JLE, JE, JNE, 
	FLD, FILD, FSTP, FADD, FSUB, FDIV, FMUL,
	FIADD, FISUB, FIDIV, FIMUL,
	JA, JB, JAE, JBE,
	FCOM, FCOMI, FCOMIP
};

enum asm_reg_t{
	EAX, EBX, ECX, EDX, EBP, ESP
};

enum asm_type_t{
	DB, DW, DD, DQ
};

class asm_code{
public:
	asm_code();
};

#endif