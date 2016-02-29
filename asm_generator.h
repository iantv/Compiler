#ifndef ASMGEN_H
#define ASMGEN_H
#include <vector>

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

using namespace std;


class asm_t{
public:
	asm_t(){};
	virtual string get_code(){ return ""; }
	virtual void print(ostream &) = 0;
};

class asm_cmd_list: public asm_t{
	vector<asm_t *> cmds;
public:
	void push_back(asm_t *cmd);
	void print(ostream &) override;
};
/*
class asm_function: public asm_t{
	
};*/

class asm_main_function: public asm_t{
	string name;
	asm_cmd_list *cmds;
public:
	asm_main_function(string);
	string get_code() override;
	void print(ostream &) override;
};

class parser;
class asm_code{
	string head;
	vector<asm_t *> code;
public:
	asm_code(parser &);
	void print(ostream &);
};

#endif