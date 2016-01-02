#ifndef SYMTABLE
#define SYMTABLE

#include "lexer.h"
#include "expression.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;
class sym_type;
class sym_var_param;

class symbol{
protected:
	string name;
	virtual void print(ostream &os) = 0;
	sym_type *type;
public:
	friend class parser;
	friend class sym_table;
	friend ostream &operator<<(ostream &os, symbol &sym);
	virtual void print(ostream &os, int level) = 0;
	friend symbol *add_elem_to_list(symbol *sym_list, symbol *sym2);
	symbol(){ name = ""; }
	symbol(const string &sym_name) { name = sym_name; }
};

/* SYM FUNCTON */

class sym_function: public symbol{
	sym_table *table;
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_function(const string &sym_name, sym_table *lst); /* lst is pointer to Local Symbol Table */
};

/* SYM TYPE */

class sym_type: public symbol{
protected:
	void print(ostream &os);
public:
	void print(ostream &os, int level);
	sym_type(){}
	sym_type(const string &sym_name) { name = sym_name; }
};

class sym_scalar: public sym_type{
protected:
	void print(ostream &os);
public:
	void print(ostream &os, int level);
	sym_scalar(){}
};

class sym_float: public sym_scalar{
protected:
	void print(ostream &os) override;
public: 
	void print(ostream &os, int level) override;
	sym_float(const string &sym_name){ name = sym_name; }
};

class sym_integer: public sym_scalar{
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_integer(const string &sym_name){ name = sym_name; }
};

class sym_array: public sym_type{
	size_t length;
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_array(size_t size){ name = "array of "; length = size; }
};

class sym_struct: public sym_type{
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_struct(){}	// DO
};

class sym_alias: public sym_type{
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_alias(){} // DO
};

class sym_pointer: public sym_type{
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_pointer(sym_type *stype){ name = "pointer to"; type = stype; }
};

class sym_func_type: public sym_type{
	sym_table *table;
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_func_type(sym_type *stype, sym_table *lst){ name = ""; type = stype; table = lst; }
};

/* SYM VARIABLE */

class sym_var: public symbol{
protected:
	expr *init_expr;
	void print(ostream &os) override;
public:
	sym_var(){}
	void print(ostream &os, int level) override;
	sym_var(const string &sym_name, sym_type *sym_vartype = nullptr, expr *sym_init_ex = nullptr);
};

class sym_var_param: public sym_var{
	expr *init_val;
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_var_param(const string &sym_name, sym_type *sym_param_type = nullptr, expr *sym_init_val = nullptr);
};

class sym_var_const: public sym_var{
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_var_const(){}
};

class sym_var_local: public sym_var{
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
	sym_var_local(){}
};

class sym_var_global: public sym_var{
protected:
	void print(ostream &os) override;
public:
	void print(ostream &os, int level) override;
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
	friend class sym_function;
	friend class sym_func_type;
	void print(ostream &os, int level);
	void add_sym(symbol *sym);
	void del_sym(symbol *sym);
	bool local_exist(symbol *sym);
	bool global_exist(symbol *sym);
	sym_type *get_type_specifier(string);
};

#endif