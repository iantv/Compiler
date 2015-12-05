#ifndef SYMTABLE
#define SYMTABLE

#include "lexer.h"
#include "expression.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;
class sym_type;

class symbol{
protected:
	string name;
	sym_type *type;
	virtual void print(ostream &os) = 0;
public:
	friend class sym_table;
	friend ostream &operator<<(ostream &os, symbol &sym);
	symbol(){ name = ""; }
	symbol(const string &sym_name) { name = sym_name; }
};

class sym_type: public symbol{
	void print(ostream &os) override;
public:
	sym_type(const string &sym_name) { name = sym_name; }
};

class sym_const: public symbol{
	expr *init_expr;
	void print(ostream &os) override;
public:
	sym_const(const string &sym_name): init_expr(nullptr) { name = sym_name; }
};

class sym_function: public symbol{
	void print(ostream &os) override;
public:
	sym_function(const string &sym_name) { name = sym_name; }
};

class sym_var: public symbol{
	expr *init_expr;
	void print(ostream &os) override;
public:
	sym_var(const string &sym_name): init_expr(nullptr) { name = sym_name; }
};

class sym_table{
	map<string, symbol *> symbols;
	sym_table *prev;
	int level;
public:
	sym_table(){ prev = nullptr; level = 0; }
	sym_table(sym_table *sym_table_prev): prev(sym_table_prev){ level = prev->level + 1; }
	friend ostream &operator<<(ostream &os, const sym_table st);
	void add_sym(symbol *sym);
	void del_sym(symbol *sym);
};

#endif