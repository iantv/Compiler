#pragma once
#include <iostream>
#include <vector>
#include "lexer.h"
#include "asm_generator.h"

#define MAX_PRIORITY 16
#define MIN_PRIORITY 1

class sym_type;
class sym_table;
class expr_bin_op;
class expr_postfix_unar_op;
class expr_prefix_unar_op;
int get_priority(token tk, bool unar = 0);

class expr{
protected:
	sym_type *type; /* return value type */
	token tk;
	string op;
public:
	expr();
	friend class parser;
	friend class expr_bin_op;
	friend class expr_postfix_unar_op;
	friend class stmt_printf;
	friend class expr_prefix_unar_op;
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
	int operator<<(int);
	bool of_ctype(string s);
	virtual void generate(asm_cmd_list *){}
	virtual void generate_addr(asm_cmd_list *){}
};

class expr_bin_op: public expr{
protected:
	expr *left, *right;
public:	
	expr_bin_op(expr *l, expr *r, token t, sym_type *st);
	expr_bin_op(expr *l, expr *r, token t);
	expr_bin_op(expr *l, expr *r, string op, sym_type *st);
	expr_bin_op(expr *l, expr *r, string op);
	void print(ostream &os, int level) override;
	void generate(asm_cmd_list *) override;
	void generate_simple_bin_op(asm_cmd_list *, token_t); /* +, -, *, / */
	void generate_rel_bin_op(asm_cmd_list *);
	void generate_assign_bin_op(asm_cmd_list *);
};

class expr_prefix_unar_op: public expr{
	expr *ex;
public:
	expr_prefix_unar_op(expr *e, token t);
	void print(ostream &os, int level);
	void generate(asm_cmd_list *) override;
	void generate_addr(asm_cmd_list *) override;
};

class expr_postfix_unar_op: public expr{
	expr *ex;
public:
	expr_postfix_unar_op(expr *e, token t);
	void print(ostream &os, int level);
	void generate(asm_cmd_list *) override;
};

class expr_literal: public expr{
public:
	expr_literal(token t, sym_type *);
	expr_literal(token t);
	void print(ostream &os, int level) override;
	void generate(asm_cmd_list *) override;
};

class expr_local_var: public expr{
public:
	expr_local_var(token t, sym_type *);
	void print(ostream &os, int level) override;
	void generate(asm_cmd_list *) override;
	void generate_addr(asm_cmd_list *) override;
};

class expr_global_var: public expr{
public:
	expr_global_var(token t, sym_type *);
	void print(ostream &os, int level) override;
	void generate(asm_cmd_list *) override;
	void generate_addr(asm_cmd_list *) override;
};

class expr_tern_op: public expr{
	expr *left, *middle, *right;
public:
	friend class parser;
	expr_tern_op(expr* l, expr *m, expr *r, string s);
	void print(ostream &os, int level);
};

class expr_function:public expr{
	expr *fid;
	vector<expr *>fargs;
public:
	expr_function(expr *id, const vector<expr *> &args);
	void print(ostream &os, int level);
};

class struct_access:public expr{
	expr *left;
	token tk;
	expr *field;
public:
	struct_access(expr *l, expr *r, token t);
	void print(ostream &os, int level);
};

class expr_cast2type: public expr{
	expr *ex;
public:
	expr_cast2type(string, expr *, sym_table *);
	expr_cast2type(sym_type *, expr *);
	void print(ostream &os, int level);
};
