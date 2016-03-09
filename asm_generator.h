#ifndef ASMGEN_H
#define ASMGEN_H
#include <vector>

enum asm_op_t{ 
	PUSH, POP, IMUL, IDIV, ADD, SUB,
	NEG, NOT, OR, AND, XOR, SHL, SHR,
	CALL, MOV, RET, TEST, CMP,
	JZ, JNZ, JMP, JG, JGE, JL, JLE, JE, JNE, 
	FLD, FILD, FSTP, FADD, FSUB, FDIV, FMUL,
	FIADD, FISUB, FIDIV, FIMUL,
	JA, JB, JAE, JBE,
	FCOM, FCOMI, FCOMIP,
	INC, DEC
};

enum asm_reg_t{
	EAX, EBX, ECX, EDX, EBP, ESP
};

enum asm_type_t{
	DB, DW, DD, DQ
};

using namespace std;

class asm_operand_t{
public:
	asm_operand_t(){}
	virtual void print(ostream &){}
};

class asm_operand_reg_t: public asm_operand_t{
protected:
	asm_reg_t reg;
public:	
	asm_operand_reg_t(asm_reg_t);
	void print(ostream &) override;
};

class asm_operand_const: public asm_operand_t{
	string val;
public:
	asm_operand_const(string);
	void print(ostream &) override;
};

class asm_operand_deref: public asm_operand_t{
	asm_reg_t reg;
public:
	asm_operand_deref(asm_reg_t);
	void print(ostream &) override;
};

class asm_operand_offset: public asm_operand_t{
	string var;
public:
	asm_operand_offset(string);
	void print(ostream &) override;
};

class asm_t{
protected:
	asm_op_t op;
public:
	asm_t(){}
	asm_t(asm_op_t);
	virtual void print(ostream &);
};

class asm_label_t: public asm_t{
	string name;
public:
	asm_label_t(string);
	void print(ostream &) override;
};

class asm_unar_op_t: public asm_t{
	asm_operand_t *operand;
public:
	asm_unar_op_t(asm_op_t, asm_operand_t *);
	void print(ostream &) override;
};

class asm_bin_op_t: public asm_t{
	asm_operand_t *operand1, *operand2;
public:
	asm_bin_op_t(asm_op_t, asm_operand_t *, asm_operand_t *);
	void print(ostream &) override;
};

class asm_cmd_list;

class asm_function: public asm_t{
protected:
	string name;
	asm_cmd_list *cmds;
public:
	asm_function(){}
	asm_function(string, asm_cmd_list *);
	void print(ostream &) override;
};

class asm_main_function: public asm_function{
public:
	asm_main_function(asm_cmd_list *);
	void print(ostream &) override;
};

class asm_global_var: public asm_t{
	string gv_name;
	asm_type_t gv_type;
public:
	asm_global_var(string &, asm_type_t);
	void print(ostream &os) override;
};

class asm_cmd_list: public asm_t{
	vector<asm_t *> cmds;
	int label_cnt;
public:
	asm_cmd_list(){ label_cnt = 0; }
	void push_back(asm_t *cmd);
	void print(ostream &) override;

	void add(asm_op_t);	
	void add(asm_op_t, string);
	void add(asm_op_t, asm_reg_t);
	void add(asm_op_t, asm_reg_t, asm_reg_t);
	void add(asm_op_t, asm_reg_t, string);
	
	void add_offset(asm_op_t, string);
	void add_assign(asm_op_t, asm_reg_t, asm_reg_t);
	void add_deref(asm_op_t, asm_reg_t);
	string get_label_name();
	void add_label(string);
};

class parser;
class asm_code{
	string head;
	vector<asm_t *> code;
	vector<asm_t *> data;
public:
	asm_code(parser &);
	void print(ostream &);

	void add(asm_function *);
	void add(asm_global_var *);
};

#endif