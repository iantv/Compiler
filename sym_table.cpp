#include "sym_table.h"

ostream &operator<<(ostream &os, const sym_const sym){
	os << "const: " << sym.name << endl;	
}

ostream &operator<<(ostream &os, const sym_var sym){
	os << "variable: " << sym.name << endl;	
}

void sym_table::add_sym(const symbol &sym){
	symbols.insert(pair<string, symbol>(sym.name, sym));
}

void sym_table::del_sym(const symbol& sym){
	symbols.erase(sym.name);
}

ostream &operator<<(ostream &os, const sym_table st){
	cout << '[' << endl;
	for (auto it = st.symbols.begin(); it != st.symbols.end(); ++it){
		cout << (*it).second << endl;
	}
	cout << ']' << endl;
}
