#include "sym_table.h"
#include "statements.h"
#include "parser.h"

symbol *make_symbol(declar &dcl){
	symbol *t = dcl.get_id();
	t->type = dcl.get_type();
	return t;
}

sym_function::sym_function(const string &sym_name, sym_table *lst, stmt_block *stmt_blck){
	name = sym_name;
	table = lst;
	block = stmt_blck;
}

sym_struct::sym_struct(const string &sym_name, sym_table *lst){
	name = sym_name;
	table = lst;
}

sym_var::sym_var(const string &sym_name, sym_type *sym_vartype, expr *sym_init_ex):
	init_expr(sym_init_ex) {
	name = sym_name; type = sym_vartype;
}

sym_var_param::sym_var_param(const string &sym_name, sym_type *sym_param_type, expr *sym_init_val):
	init_val(sym_init_val){
	name = sym_name; type = sym_param_type; 
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
	table->print(os, level + 1);
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
	if (init_expr != nullptr){
		init_expr->print(os, MIN_PRIORITY);
	}
}

void sym_var_param::print(ostream &os, int level){
	print_level(os, level);
	os << name << ": parameter" << endl;
	type->print(os, level + 1);
}

void sym_struct::print(ostream &os, int level){
	print_level(os, level);
	os << "struct";
	if (name != "")
		os << " " << name << endl;
	else
		os << endl;
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

bool sym_table::local_exist(string &name){
	if (symbols.size() == 0)
		return false;
	return symbols.find(name) != symbols.end();
}

sym_type* sym_table::get_type_specifier(string s){
	map<string, symbol *>::iterator it;
	sym_type *t = nullptr;
	sym_table *st = prev;

	it = symbols.find(s.c_str());
	if (it == symbols.end()){
		while (st){
			t = st->get_type_specifier(s);
			if (t == nullptr)
				st = st->prev;
			else 
				return t;
		}
	} else
		return dynamic_cast<sym_type *>(it->second);
	return nullptr;
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

void sym_table::add_sym(symbol *sym){
	if (local_exist(sym->name))
		throw 1;
	symbols.insert(pair<string, symbol *>(sym->name, sym));
}

void sym_table::del_sym(symbol *sym){
	if (local_exist(sym->name))
		symbols.erase(sym->name);
}

ostream &operator<<(ostream &os, const sym_table st){
	for (auto it = st.symbols.begin(); it != st.symbols.end(); ++it){
		it->second->print(os, st.level);
	}
	return os;
}

void sym_table::print(ostream &os, int level){
	for (auto it = symbols.begin(); it != symbols.end(); ++it){
		it->second->print(os, level);
	}
}

sym_type *sym_table::get_type_by_synonym(string s){
	sym_type *t = get_type_specifier(s);
	string id = typeid(*t).name();
	if (id == "class sym_alias"){
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