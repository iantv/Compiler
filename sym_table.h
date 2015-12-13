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
	symbol *next;
	virtual void print(ostream &os) = 0;
public:
	friend class parser;
	friend class sym_table;
	friend ostream &operator<<(ostream &os, symbol &sym);
	friend symbol *add_elem_to_list(symbol *sym_list, symbol *sym2);
	symbol(){ name = ""; }
	symbol(const string &sym_name) { name = sym_name; }
};

/* SYM FUNCTON */

class sym_function: public symbol{
	void print(ostream &os) override;
	
public:
	sym_function(const string &sym_name) { name = sym_name; }
};

/* SYM TYPE */

class sym_type: public symbol{
	void print(ostream &os);
public:
	sym_type(){}
	sym_type(const string &sym_name) { name = sym_name; }
};

class sym_scalar: public sym_type{
	void print(ostream &os);
public:
	sym_scalar(){}
};

class sym_float: public sym_scalar{
	void print(ostream &os);
public:
	sym_float(){}
};

class sym_integer: public sym_scalar{
	void print(ostream &os);
public:
	sym_integer(){}
};

class sym_array: public sym_type{
	sym_type *elem_type;
	int length;
	void print(ostream &os) override;
public:
	sym_array(const string &sym_name){ name = sym_name; }
};

class sym_struct: public sym_type{
	void print(ostream &os) override;
public:
	sym_struct(){}
};

class sym_alias: public sym_type{
	void print(ostream &os) override;
public:
	sym_alias(){}
};

class sym_pointer: public sym_type{
	void print(ostream &os);
public:
	sym_pointer(const string &sym_name){ name = "pointer"; }
};

/* SYM VARIABLE */

class sym_var: public symbol{
	expr *init_expr;
	sym_type *type;
	void print(ostream &os) override;
public:
	sym_var(){}
	sym_var(const string &sym_name, sym_type *sym_vartype = nullptr, expr *sym_init_ex = nullptr);
};

class sym_var_param: public sym_var{
	void print(ostream &os) override;
public:
	sym_var_param(){}
};

class sym_var_const: public sym_var{
	void print(ostream &os) override;
public:
	sym_var_const(){}
};

class sym_var_local: public sym_var{
	void print(ostream &os) override;
public:
	sym_var_local(){}
};

class sym_var_global: public sym_var{
	void print(ostream &os) override;
public:
	sym_var_global(){}
};

/* SYM TABLE */

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
	bool local_exist(symbol *sym);
	bool global_exist(symbol *sym);
};

#endif