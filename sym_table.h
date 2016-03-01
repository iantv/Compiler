#pragma once
#include "lexer.h"
#include "expression.h"
#include <string>
#include <map>
#include <iostream>
#include "asm_generator.h"

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
	friend class expr;
	friend symbol *make_symbol(declar &dcl);
	virtual void print(ostream &os, int level) = 0;
	friend symbol *add_elem_to_list(symbol *sym_list, symbol *sym2);
	symbol(){ name = ""; type = nullptr; }
	symbol(const string &sym_name) { name = sym_name; }
	bool type_eq(string type_name);
	sym_type *get_type(){ return type; }
	virtual void generate(asm_code *) { return; }
};

symbol *make_symbol(declar &);

class stmt_block;
class sym_function: public symbol{
	sym_table *table;
	stmt_block *block;
	vector<string> params;
public:
	friend class parser;
	void print(ostream &os, int level) override;
	sym_function(const string &sym_name, sym_table *lst, vector<string> &param_list, stmt_block *); /* lst is pointer to Local Symbol Table */
	void generate(asm_code *) override;
};

class sym_type: public symbol{
public:
	friend class declar;
	friend bool equal(sym_type *, sym_type *);
	void print(ostream &os, int level);
	sym_type(){}
	sym_type(const string &sym_name) { name = sym_name; type = nullptr; }
	virtual string get_type_str_name();
};

class sym_array: public sym_type{
	size_t length;
public:
	void print(ostream &os, int level) override;
	sym_array(size_t size){ length = size; }
	string get_type_str_name() override;
};

class sym_struct: public sym_type{
	sym_table *table;
public:
	friend parser;
	void print(ostream &os, int level) override;
	sym_struct(const string &sym_name, sym_table *lst);
	string get_type_str_name() override;
};

class sym_alias: public sym_type{
public:
	void print(ostream &os, int level) override;
	sym_alias(sym_type *stype){ type = stype; }
	string get_type_str_name() override;
};

class sym_pointer: public sym_type{
public:
	void print(ostream &os, int level) override;
	sym_pointer(sym_type *stype){ type = stype; }
	string get_type_str_name() override;
};

class sym_func_type: public sym_type{
	sym_table *table;
	vector<string> params;
public:
	void print(ostream &os, int level) override;
	sym_func_type(sym_type *stype, sym_table *lst, vector<string> &param_list);
	string get_type_str_name() override;
};

class sym_const: public sym_type{
public:
	void print(ostream &os, int level) override;
	sym_const(sym_type *stype){ type = stype; }
	string get_type_str_name() override;
};

class sym_var: public symbol{
protected:
	token var_token;
public:
	friend class parser;
	sym_var(){}
	void print(ostream &os, int level) override;
	sym_var(const string &sym_name, sym_type *sym_vartype = nullptr);
	sym_var(const string &, sym_type *, token);
};

class sym_var_param: public sym_var{
public:
	void print(ostream &os, int level) override;
	sym_var_param(const string &sym_name, sym_type *sym_param_type = nullptr);
};

class sym_table{
	map<string, symbol *> symbols;
	vector<sym_function *> functions;
	sym_table *prev;
	int level;
public:
	sym_table(){ prev = nullptr; level = 0; }
	sym_table(sym_table *sym_table_prev): prev(sym_table_prev){ level = prev->level + 1; }
	friend class sym_function;
	friend class sym_func_type;
	friend class parser;
	friend class asm_code;
	void print(ostream &os, int level);
	void add_sym(symbol *sym);
	void del_sym(symbol *sym);
	bool local_exist(string &name);
	bool global_exist(string &name);
	bool type_exists_by_real_typename(string &type_name);
	bool type_by_name(string &);
	bool local_is_empty();
	symbol *get_symbol(string);
	sym_type *get_type_specifier(string);
	bool type_alias_exist(string);
	sym_type *get_type_by_synonym(string s);
	bool symbol_not_alias_exist(string s);
	int count_symbols(){ return symbols.size(); }
};