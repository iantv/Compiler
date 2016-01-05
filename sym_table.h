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
class sym_var;

class symbol{
protected:
	string name;
	sym_type *type;
public:
	friend class parser;
	friend class sym_table;
	friend class declar;
	friend class sym_var;
	friend symbol *make_symbol(declar &dcl);
	virtual void print(ostream &os, int level) = 0;
	friend symbol *add_elem_to_list(symbol *sym_list, symbol *sym2);
	symbol(){ name = ""; type = nullptr; }
	symbol(const string &sym_name) { name = sym_name; }
};

symbol *make_symbol(declar &);

/* SYM FUNCTON */

class sym_function: public symbol{
	sym_table *table;
public:
	void print(ostream &os, int level) override;
	sym_function(const string &sym_name, sym_table *lst); /* lst is pointer to Local Symbol Table */
};

/* SYM TYPE */

class sym_type: public symbol{
public:
	friend class declar;
	void print(ostream &os, int level);
	sym_type(){}
	sym_type(const string &sym_name) { name = sym_name; type = nullptr; }
};

class sym_array: public sym_type{
	size_t length;
public:
	void print(ostream &os, int level) override;
	sym_array(size_t size){ length = size; }
};

class sym_struct: public sym_type{
	sym_table *table;
public:
	friend parser;
	void print(ostream &os, int level) override;
	sym_struct(const string &sym_name, sym_table *lst);
};

class sym_alias: public sym_type{
public:
	void print(ostream &os, int level) override;
	sym_alias(sym_type *stype){ type = stype; }
};

class sym_pointer: public sym_type{
public:
	void print(ostream &os, int level) override;
	sym_pointer(sym_type *stype){ type = stype; }
};

class sym_func_type: public sym_type{
	sym_table *table;
public:
	void print(ostream &os, int level) override;
	sym_func_type(sym_type *stype, sym_table *lst){ name = ""; type = stype; table = lst; }
};

class sym_const: public sym_type{
public:
	void print(ostream &os, int level) override;
	sym_const(sym_type *stype){ type = stype; }
};
/* SYM VARIABLE */

class sym_var: public symbol{
protected:
	expr *init_expr;
public:
	sym_var(){}
	void print(ostream &os, int level) override;
	sym_var(const string &sym_name, sym_type *sym_vartype = nullptr, expr *sym_init_ex = nullptr);
};

class sym_var_param: public sym_var{
	expr *init_val;
public:
	void print(ostream &os, int level) override;
	sym_var_param(const string &sym_name, sym_type *sym_param_type = nullptr, expr *sym_init_val = nullptr);
};

class sym_var_const: public sym_var{
public:
	void print(ostream &os, int level) override;
	sym_var_const(){}
};

class sym_var_local: public sym_var{
public:
	void print(ostream &os, int level) override;
	sym_var_local(){}
};

class sym_var_global: public sym_var{
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
	friend class parser;
	void print(ostream &os, int level);
	void add_sym(symbol *sym);
	void del_sym(symbol *sym);
	bool local_exist(string &name);
	bool global_exist(string &name);
	sym_type *get_type_specifier(string);
	bool type_synonym_exist(string);
	sym_type *get_type_by_synonym(string s);
};

#endif