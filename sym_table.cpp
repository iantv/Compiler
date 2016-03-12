#include "sym_table.h"
#include "statements.h"
#include "parser.h"

bool symbol::type_eq(string type_name){
	return type->name == type_name;
}

symbol *make_symbol(declar &dcl){
	symbol *t = dcl.get_id();
	if (t != nullptr)
		t->type = dcl.get_type();
	return t;
}

sym_function::sym_function(const string &sym_name, sym_table *lst, vector<string> &param_list, stmt_block *stmt_blck = nullptr){
	name = sym_name;
	table = lst;
	params = param_list;
	block = stmt_blck;
}

sym_func_type::sym_func_type(sym_type *stype, sym_table *lst, vector<string> &param_list){
	name = "";
	type = stype;
	table = lst;
	params = param_list; 
}

sym_struct::sym_struct(const string &sym_name, sym_table *lst){
	name = sym_name;
	table = lst;
}

sym_const::sym_const(sym_type *const_type){
	type = const_type;
}

sym_var::sym_var(const string &sym_name, sym_type *sym_vartype){
	name = sym_name; type = sym_vartype;
}

sym_var::sym_var(const string &sym_name, sym_type *sym_vartype, token tk){
	name = sym_name; type = sym_vartype; var_token = tk;
}

sym_var_param::sym_var_param(const string &sym_name, sym_type *sym_param_type){
	name = sym_name; type = sym_param_type;
	string tname = type->get_type_str_name();
	if (tname == token_names[TK_DOUBLE]){
		size = 8;
	} else if (tname == token_names[TK_INT]){
		size = 4;
	} else if (tname == token_names[TK_CHAR]){
		size = 1;
	}
}

int sym_var_param::set_offset(int param_offset){
	return offset = param_offset;
}

int sym_var_param::get_size(){
	return size;
}
sym_var_global::sym_var_global(const string &sym_name, sym_type *sym_param_type, token tk){
	name = sym_name; type = sym_param_type;  var_token = tk;
}

static void print_level(ostream &os, int level){
	while (level){
		os << '\t';
		level--;
	}
}

void sym_pointer::print(ostream &os, int level){
	print_level(os, level);
	os << name << "pointer(s) to" << endl;
	type->print(os, level + 1);
}

void sym_func_type::print(ostream &os, int level){
	print_level(os, level);
	os << name << "function" << endl;
	for (int i = 0; i < (int)params.size(); i++)
		table->get_symbol(params[i])->print(os, level + 1);
	table->print(os, level + 1);
	print_level(os, level);
	os << "returns" << endl;
	type->print(os, level  + 1);
}

void sym_type::print(ostream &os, int level){
	print_level(os, level);
	os << name << endl;
}

void sym_function::print(ostream &os, int level){
	print_level(os, level);
	os << name << ": function" << endl;
	for (int i = 0; i < (int)params.size(); i++)
		table->get_symbol(params[i])->print(os, level + 1);
	if (block != nullptr)
		block->print(os, level + 1);
	print_level(os, level);
	os << "returns" << endl;
	type->print(os, level  + 1);
}

void sym_var::print(ostream &os, int level){
	print_level(os, level);
	os << name << ": ";	
	int lvl = level;
	if (type->type == nullptr){
		os << "variable" << endl;
		lvl += 1;
	}
	type->print(os, lvl);
}

void sym_var_param::print(ostream &os, int level){
	print_level(os, level);
	os << name << ": parameter" << endl;
	type->print(os, level + 1);
}

void sym_struct::print(ostream &os, int level){
	print_level(os, level);
	os << name << endl;
	table->print(os, level + 1);
} 

void sym_array::print(ostream &os, int level){
	print_level(os, level);
	os << name << ((name == "") ? "" : ": ") << "array of";
	if (length)
		os << " " << length << " of" << endl;
	else 
		os << endl;
	type->print(os, level + 1);
}

void sym_alias::print(ostream &os, int level){
	print_level(os, level);
	os << name << ": alias for" << endl;
	type->print(os, level + 1);
}

void sym_const::print(ostream &os, int level){
	print_level(os, level);
	os << "constant" << endl;
	type->print(os, level + 1);
}

string sym_type::get_type_str_name(){
	return name;
}

sym_type::sym_type(const string &sym_name){
	name = sym_name;
	type = nullptr;
	if (name == token_names[TK_DOUBLE]){
		size = 8;
	} else if (name == token_names[TK_INT]){
		size = 4;
	} else if (name == token_names[TK_CHAR]){
		size = 1;
	}
}

string sym_array::get_type_str_name(){
	return "array of " + type->get_type_str_name();
}

string sym_struct::get_type_str_name(){
	return name;
}

string sym_alias::get_type_str_name(){
	return type->get_type_str_name();
}

string sym_pointer::get_type_str_name(){
	return type->get_type_str_name() + " *";
}

string sym_func_type::get_type_str_name(){
	string par;
	for (auto it = params.begin(); it != params.end(); ){
		sym_type *t = table->get_symbol(*it)->get_type();
		par += t->get_type_str_name();
		it++;
		if (it != params.end()) par += ", ";
	}
	return type->get_type_str_name() + " " + name + "(" + par + ")";
}

string sym_const::get_type_str_name(){
	return "constant " + type->get_type_str_name();
}

/*------------------------------------------------SYM_TABLE------------------------------------------------*/

bool sym_table::local_exist(string &name){
	if (symbols.size() == 0 && functions.size() == 0)
		return false;
	for (auto it = functions.begin(); it != functions.end(); it++)
		if ((*it)->name == name)
			return true;
	return symbols.find(name) != symbols.end();
}

symbol *sym_table::get_symbol(string s){
	map<string, symbol *>::iterator it = symbols.find(s.c_str());
	vector<sym_function *>::iterator fit = functions.begin();
	if (it != symbols.end())
		return it->second;
	for (;fit != functions.end(); fit++)
		if ((*fit)->name == s)
			return (*fit);
	if (prev == nullptr)
		return nullptr;
	return prev->get_symbol(s);
}

sym_type* sym_table::get_type_specifier(string s){
	/*symbol *t = get_symbol(s);
	string id = typeid(*t).name();*/ //DO correct!!!
	return dynamic_cast<sym_type *>(get_symbol(s));
};

bool sym_table::global_exist(string &name){
	sym_table *st = prev;
	while (st){
		if (st->local_exist(name))
			return true;
		st = st->prev;
	}
	return false;
}

bool sym_table::type_exists_by_real_typename(string &type_name){
	map<string, symbol *>::iterator it;
	for (it = symbols.begin(); it != symbols.end(); it++){
		symbol *sym = (*it).second;
		if (typeid(*sym) == typeid(sym_alias) || typeid(*sym) == typeid(sym_const) || typeid(*sym) == typeid(sym_array) || typeid(*sym) == typeid(sym_struct) || typeid(*sym) == typeid(sym_pointer) || typeid(*sym) == typeid(sym_func_type)){
			sym_type *st = dynamic_cast<sym_type *>(sym);
			if (st->get_type_str_name() == type_name)
				return true;
		}
	}
	if (prev == nullptr) return false;
	return prev->type_exists_by_real_typename(type_name);
}

bool sym_table::type_by_name(string &type_name){
	map<string, symbol *>::iterator it;
	for (it = symbols.begin(); it != symbols.end(); it++){
		symbol *sym = (*it).second;
		if (typeid(*sym) == typeid(sym_alias) || typeid(*sym) == typeid(sym_const) || typeid(*sym) == typeid(sym_array) || typeid(*sym) == typeid(sym_struct) || typeid(*sym) == typeid(sym_pointer) || typeid(*sym) == typeid(sym_func_type)){
			sym_type *st = dynamic_cast<sym_type *>(sym);
			if (sym->name == type_name)
				return true;
		}
	}
	return false;
}

void sym_table::add_sym(symbol *sym){
	if (typeid(*sym).name() == typeid(sym_function).name()){
		functions.push_back(dynamic_cast<sym_function *>(sym));
		return;
	}
	if (local_exist(sym->name))
		throw 1;
	symbols.insert(pair<string, symbol *>(sym->name, sym));
}

void sym_table::del_sym(symbol *sym){
	if (local_exist(sym->name))
		symbols.erase(sym->name);
}

void sym_table::print(ostream &os, int level){
	for (int i = 0; i < (int)functions.size(); i++){
		functions[i]->print(os, level);
	}
	for (auto it = symbols.begin(); it != symbols.end(); ++it){
		symbol *t = it->second;
		if (typeid(*t).name() == typeid(sym_var_param).name())
			continue;
		t->print(os, level);
	}
}

sym_type *sym_table::get_type_by_synonym(string s){
	sym_type *t = get_type_specifier(s);
	if (t == nullptr) return nullptr;
	string id = typeid(*t).name();
	if (id == typeid(sym_alias).name()){
		return t->type;
	}
	return nullptr;
}

bool sym_table::type_alias_exist(string s){
	if (local_exist(s) || global_exist(s)){
		return get_type_by_synonym(s) != nullptr;
	}
	return false;
}

bool sym_table::symbol_not_alias_exist(string s){
	if (local_exist(s) || global_exist(s)){
		return get_type_by_synonym(s) == nullptr;
	}
	return false;
}

bool sym_table::local_is_empty(){
	return symbols.size() == 0;
}

bool equal(sym_type *sym1, sym_type *sym2){
	sym_type *t1 = sym1, *t2 = sym2;
	while (t1 != nullptr && t2 != nullptr && t1->type && t2->type && (t1->name == t2->name)){
		t1 = t1->type;
		t2 = t2->type;
	}
	if (t1 == t2)
		return true;
	return false;
}

/*------------------------------GENERATE METHODS------------------------------*/

void sym_function::generate(asm_code *code){
	asm_cmd_list *cmds = new asm_cmd_list();
	cmds->add(PUSH, EBP);
	cmds->add(MOV, EBP, ESP);
	block->generate(cmds);
	cmds->add_label(name + "_return");
	cmds->add(MOV, ESP, EBP);
	cmds->add(POP, EBP);
	cmds->add(RET); // Do later for returning value
	if (name == "main"){
		code->add(new asm_main_function(cmds));
		return;
	}
	code->add(new asm_function(name, cmds));
}

void sym_var_global::generate(asm_code *code){
	if (var_token.get_type_name() == "char"){
		code->add(new asm_global_var(name + '_', DB));
	} else {
		code->add(new asm_global_var(name + '_', DD));
	}
}

void sym_var_param::generate(asm_cmd_list *cmds){
	cmds->add_deref(PUSH, EBP, offset);
}