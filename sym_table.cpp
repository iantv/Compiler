#include "sym_table.h"

static void print_level(int level){
	while (level){
		cout << '\t';
		level--;
	}
}

ostream &operator<<(ostream &os, symbol &sym){
	sym.print(os);
	return os;
}

void sym_const::print(ostream &os){
	//print_level(level);
	os << "const: " << this->name << endl;		
}

void sym_function::print(ostream &os){
	//print_level(level);
	os << "function: " << name << endl;
}

void sym_var::print(ostream &os){
	//print_level(level);
	os << "variable: " << name << endl;	
}

void sym_table::add_sym(symbol *sym){
	symbols.insert(pair<string, symbol *>(sym->name, sym));
}

void sym_table::del_sym(symbol *sym){
	symbols.erase(sym->name);
}

ostream &operator<<(ostream &os, const sym_table st){
	print_level(st.level);
	cout << '{' << endl;
	for (auto it = st.symbols.begin(); it != st.symbols.end(); ++it){
		print_level(st.level + 1);
		cout << *((*it).second);
	}
	cout << '}' << endl;
	return os;
}
