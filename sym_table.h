#ifndef SYMTABLE
#define SYMTABLE

#include "lexer.h"
#include "expression.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;

class sym_type: public symbol{
public:
	sym_type(const string &sym_name) { name = sym_name; }
};

class symbol{
protected:
	string name;
	sym_type *type;
public:
	friend class sym_table;
	friend virtual ostream &operator<<(ostream &os, const symbol sym) = 0;
	symbol(){ name = ""; }
	symbol(const string &sym_name) { name = sym_name; }
};

class sym_const: public symbol{
	expr *init_expr;
public:
	friend ostream &operator<<(ostream &os, const sym_const sym);
	sym_const(const string &sym_name) { symbol::symbol(sym_name); }
};

class sym_function: public symbol{
public:
	sym_function(const string &sym_name) { symbol::symbol(sym_name); }
};

class sym_var: public symbol{
	expr *init_expr;
public:
	friend ostream &operator<<(ostream &os, const sym_var sym);
	sym_var(const string &sym_name): init_expr(nullptr) { symbol::symbol(sym_name); }
};

class sym_table{
	map<string, symbol> symbols;
	sym_table *prev;
public:
	sym_table(){ prev = nullptr; }
	sym_table(sym_table *sym_table_prev): prev(sym_table_prev){}
	friend ostream &operator<<(ostream &os, const sym_table st);
	void add_sym(const symbol& sym);
	void del_sym(const symbol& sym);
};

#endif