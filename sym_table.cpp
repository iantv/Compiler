#include "sym_table.h"
static void print_level(int level){
	while (level){
		cout << '\t';
		level--;
	}
}
ostream &operator<<(ostream &os, const symbol sym){ return os; }

ostream &operator<<(ostream &os, const sym_const sym){
	print_level(sym.level);
	os << "const: " << sym.name << endl;	
	return os;
}

ostream &operator<<(ostream &os, const sym_var sym){
	print_level(sym.level);
	os << "variable: " << sym.name << endl;	
	return os;
}

void sym_table::add_sym(const symbol &sym){
	symbols.insert(pair<string, symbol>(sym.name, sym));
}

void sym_table::del_sym(const symbol& sym){
	symbols.erase(sym.name);
}

ostream &operator<<(ostream &os, const sym_table st){
	print_level(st.level);
	cout << '[' << endl;
	for (auto it = st.symbols.begin(); it != st.symbols.end(); ++it){
		cout << (*it).second << endl;
	}
	cout << ']' << endl;
	return os;
}
