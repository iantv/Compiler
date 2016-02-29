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
	"fcom", "fcomi", "fcomip"
};

static const string asm_reg_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp" };

static const string asm_type_str[] = { "db", "dw", "dd", "dq" };

asm_code::asm_code(parser &prs){
	head = ".686\n.model flat, stdcall\n\ninclude \\masm32\\include\\msvcrt.inc\nincludelib \\masm32\\lib\\msvcrt.lib\n\n";
	// do global vars
	/*for (auto it = prs.table->symbols.begin(); it != prs.table->symbols.end(); it++)
		code.push_back((*it).second->generate());*/
	for (auto it = prs.table->functions.begin(); it != prs.table->functions.end(); it++){
		code.push_back((*it)->generate());
	}
}

void asm_code::print(ostream &os){
	os << head;
	os << ".code" << endl;
	for (auto it = code.begin(); it != code.end(); it++){
		(*it)->print(os);
	}
}

asm_main_function::asm_main_function(){}

string asm_main_function::get_code(){
	return "main:\n" /* + cmds->get_code() + */ "\tret\nend main";
}

void asm_main_function::print(ostream &os){
	os << get_code();
}

asm_function::asm_function(string func_name){
	name = func_name;
}

string asm_function::get_code(){
	return name + " proc\n" /* + cmds->get_code() + */ "\tret\n" + name + " endp\n";
}

void asm_function::print(ostream &os){
	os << get_code();
}