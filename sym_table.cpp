#include "sym_table.h"

sym_function::sym_function(const string &sym_name, vector<sym_var_param>sym_fparams){
	name = sym_name; params = sym_fparams;
}

sym_var::sym_var(const string &sym_name, sym_type *sym_vartype, expr *sym_init_ex): init_expr(sym_init_ex) {
	name = sym_name; type = sym_vartype;
}

static void print_level(ostream &os, int level){
	while (level){
		os << '\t';
		level--;
	}
}

ostream &operator<<(ostream &os, symbol &sym){
	sym.print(os);
	return os;
}

void sym_scalar::print(ostream &os){
	os << "scalar ";
}

void sym_integer::print(ostream &os){
	os << "int ";
}

void sym_float::print(ostream &os){
	os << "float ";
}

void sym_pointer::print(ostream &os){
	os << "pointer to ";
}

void sym_func_type::print(ostream &os){
	os << "function returned ";
}

void sym_type::print(ostream &os){
	os << name << endl;
}

void sym_function::print(ostream &os){
	os << name << ": function returned " << (*type);
}

void sym_var::print(ostream &os){
	os << "variable: " << name << endl;	
}

void sym_var_param::print(ostream &os){
	os << "param: " << name;
}

void sym_struct::print(ostream &os){
	os << "struct ";
}

void sym_array::print(ostream &os){
	os << "array: ";
}
bool sym_table::local_exist(symbol *sym){
	if (symbols.size() == 0)
		return false;
	return symbols.find(sym->name) != symbols.end();
}

bool sym_table::global_exist(symbol *sym){
	sym_table *st = prev;
	while (st){
		if (local_exist(sym))
			return true;
		st = st->prev;
	}
	return false;
}

void sym_table::add_sym(symbol *sym){
	if (local_exist(sym))
		throw 1;
	symbols.insert(pair<string, symbol *>(sym->name, sym));
}

void sym_table::del_sym(symbol *sym){
	if (local_exist(sym))
		symbols.erase(sym->name);
}

ostream &operator<<(ostream &os, const sym_table st){
	print_level(os, st.level);
	os << '{' << endl;
	for (auto it = st.symbols.begin(); it != st.symbols.end(); ++it){
		print_level(os, st.level + 1);
		os << *((*it).second);
	}
	os << '}' << endl;
	return os;
}