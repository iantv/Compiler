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
	"fcom", "fcomi", "fcomip", "offset"
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

	// do global vars
	/*for (auto it = prs.table->symbols.begin(); it != prs.table->symbols.end(); it++)
		code.push_back((*it).second->generate());*/
	for (auto it = prs.table->functions.begin(); it != prs.table->functions.end(); it++){
		(*it)->generate(this);
	}
}

void asm_code::print(ostream &os){
	os << head;
	/*os << ".data" << endl;
	for (auto it = gvar.begin(); it != gvar.end(); it++)
		(*it)->print(os);*/
	os << ".code" << endl;
	for (auto it = code.begin(); it != code.end(); it++){
		(*it)->print(os);
	}
}

void asm_code::add(asm_function *new_func){
	code.push_back(new_func);
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
	cmds.push_back(new asm_t(asm_op_str[op]));
}

void asm_cmd_list::add(asm_op_t op, string val){
	cmds.push_back(new asm_t(asm_op_str[op] + ' ' + val));
}

void asm_cmd_list::add(asm_op_t op, asm_reg_t reg){
	cmds.push_back(new asm_t(asm_op_str[op] + ' ' + asm_reg_str[reg]));
}

void asm_cmd_list::add(asm_op_t op, asm_reg_t reg1, asm_reg_t reg2){
	cmds.push_back(new asm_t(asm_op_str[op] + ' ' + asm_reg_str[reg1] + ',' + asm_reg_str[reg2]));
}

void asm_cmd_list::add(asm_op_t op, asm_op_t offset, string fmt){
	cmds.push_back(new asm_t(asm_op_str[op] + ' ' + asm_op_str[offset] + ' ' + fmt));
}
/*-----------------------------------class asm_cmd----------------------------------*/

asm_t::asm_t(string cmd_str){
	cmd = cmd_str;
}

void asm_t::print(ostream &os){
	os << cmd << endl;
}