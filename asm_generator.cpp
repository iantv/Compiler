#include "asm_generator.h"
#include "parser.h"

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
	"fcom", "fcomi", "fcomip", "inc", "dec"
};

static const string asm_reg_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp" };

static const string asm_type_str[] = { "db", "dw", "dd", "dq" };

asm_code::asm_code(parser &prs){
	head = ".686\n.model flat, stdcall\n\ninclude \\masm32\\include\\msvcrt.inc\nincludelib \\masm32\\lib\\msvcrt.lib\n\n\
STR_LITERAL macro value : req\n\
	LOCAL lbl\n\
	.data\n\
	lbl BYTE value, 0\n\
	.code\n\
	EXITM <lbl>\n\
endm\n\n";

	for (auto it = prs.table->symbols.begin(); it != prs.table->symbols.end(); it++)
		(*it).second->generate(this);
	for (auto it = prs.table->functions.begin(); it != prs.table->functions.end(); it++)
		(*it)->generate(this);
}

void asm_code::print(ostream &os){
	os << head;
	os << ".data" << endl;
	for (auto it = data.begin(); it != data.end(); it++)
		(*it)->print(os);
	os << ".code" << endl;
	for (auto it = code.begin(); it != code.end(); it++){
		(*it)->print(os);
	}
}

void asm_code::add(asm_function *new_func){
	code.push_back(new_func);
}

void asm_code::add(asm_global_var *new_var){
	data.push_back(new_var);
}

/*-------------------------------class asm_function------------------------------*/

asm_main_function::asm_main_function(asm_cmd_list *fcmds){
	cmds = fcmds;
}

void asm_main_function::print(ostream &os){
	os << "start:" << endl;
	cmds->print(os);
	os << "end start" << endl;
}

asm_function::asm_function(string func_name, asm_cmd_list *fcmds){
	name = func_name;
	cmds = fcmds;
}

void asm_function::print(ostream &os){
	os << name << " proc" << endl;
	cmds->print(os);
	os << name << " endp" << endl;
}

/*-------------------------------class asm_global_var---------------------------------*/

asm_global_var::asm_global_var(string &name, asm_type_t type){
	gv_name = name;
	gv_type = type;
}

void asm_global_var::print(ostream &os){
	os << '\t' << gv_name << '\t' << asm_type_str[gv_type] << '\t' << 0 << endl;
}

/*-------------------------------class asm_cmd_list---------------------------------*/

void asm_cmd_list::push_back(asm_t *cmd){
	cmds.push_back(cmd);
}

void asm_cmd_list::print(ostream &os){
	for (auto it = cmds.begin(); it != cmds.end(); it++){
		os << '\t';
		(*it)->print(os);
	}
}


void asm_cmd_list::add(asm_op_t op){
	cmds.push_back(new asm_t(op));
}

void asm_cmd_list::add(asm_op_t op, string val){
	cmds.push_back(new asm_unar_op_t(op, new asm_operand_const(val)));
}

void asm_cmd_list::add(asm_op_t op, asm_reg_t reg){
	cmds.push_back(new asm_unar_op_t(op, new asm_operand_reg_t(reg)));
}

void asm_cmd_list::add(asm_op_t op, asm_reg_t reg1, asm_reg_t reg2){
	cmds.push_back(new asm_bin_op_t(op, new asm_operand_reg_t(reg1), new asm_operand_reg_t(reg2)));
}

void asm_cmd_list::add(asm_op_t op, asm_reg_t reg, string val){
	cmds.push_back(new asm_bin_op_t(op, new asm_operand_reg_t(reg), new asm_operand_const(val)));
}

void asm_cmd_list::add_offset(asm_op_t op, string value){
	cmds.push_back(new asm_unar_op_t(op, new asm_operand_offset(value)));
}

void asm_cmd_list::add_assign(asm_op_t op, asm_reg_t addr, asm_reg_t val){
	cmds.push_back(new asm_bin_op_t(op, new asm_operand_deref(addr), new asm_operand_reg_t(val)));
}

void asm_cmd_list::add_deref(asm_op_t op, asm_reg_t reg){
	cmds.push_back(new asm_unar_op_t(op, new asm_operand_deref(reg)));
}

void asm_cmd_list::add_deref(asm_op_t op, asm_reg_t reg, int offset){
	cmds.push_back(new asm_unar_op_t(op, new asm_operand_deref(reg, offset)));
}
string asm_cmd_list::get_label_name(){
	return "L_" + to_string(label_cnt++);
}

void asm_cmd_list::add_label(string lname){
	cmds.push_back(new asm_label_t(lname));
}

/*---------------------------------------class asm_t-----------------------------------------*/

asm_t::asm_t(asm_op_t asm_op){
	op = asm_op;
}

void asm_t::print(ostream &os){
	os << asm_op_str[op] << endl;
}

asm_label_t::asm_label_t(string label_name){
	name = label_name;
}

void asm_label_t::print(ostream &os){
	os << name << ':' << endl;
}

/*-----------------------------------class asm_operand----------------------------------------*/

asm_operand_reg_t::asm_operand_reg_t(asm_reg_t asm_reg){
	reg = asm_reg;
}

void asm_operand_reg_t::print(ostream &os){
	os << asm_reg_str[reg];
}

asm_operand_const::asm_operand_const(string value){
	val = value;
}

void asm_operand_const::print(ostream &os){
	os << val;
}

asm_operand_offset::asm_operand_offset(string variable){
	var = variable;
}

void asm_operand_offset::print(ostream &os){
	os << "offset " << var;
}

asm_operand_deref::asm_operand_deref(asm_reg_t asm_reg){
	reg = asm_reg;	
	offset = 0;
}

asm_operand_deref::asm_operand_deref(asm_reg_t asm_reg, int asm_offset){
	reg = asm_reg;	
	offset = asm_offset;
}

void asm_operand_deref::print(ostream &os){
	os << "[" << asm_reg_str[reg] << " + " << offset << "]";
}

/*------------------------------------class asm_unar_op_t----------------------------------*/

asm_unar_op_t::asm_unar_op_t(asm_op_t asm_op, asm_operand_t *asm_operand){
	op = asm_op;
	operand = asm_operand;
}

void asm_unar_op_t::print(ostream &os){
	os << asm_op_str[op] << ' ';
	operand->print(os);
	os << endl;
}

asm_bin_op_t::asm_bin_op_t(asm_op_t asm_op, asm_operand_t *asm_operand1, asm_operand_t *asm_operand2){
	op = asm_op;
	operand1 = asm_operand1;
	operand2 = asm_operand2;
}

void asm_bin_op_t::print(ostream &os){
	os << asm_op_str[op] << ' ';
	operand1->print(os);
	os << ',';
	operand2->print(os);
	os << endl;
}