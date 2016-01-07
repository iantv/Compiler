#pragma once
#include <iostream>
#include <vector>
#include "lexer.h"

#define MAX_PRIORITY 16
#define MIN_PRIORITY 1

class sym_type;
int get_priority(token tk, bool unar = 0);

class expr{
protected:
	sym_type *type; /* return value type */
	token tk;
	string op;
public:
	expr();
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
	int operator<<(int);
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
};

class expr_prefix_unar_op: public expr{
	expr *ex;
public:
	expr_prefix_unar_op(expr *e, token t);
	void print(ostream &os, int level);
};

class expr_postfix_unar_op: public expr{
	expr *ex;
public:
	expr_postfix_unar_op(expr *e, token t);
	void print(ostream &os, int level);
};

class expr_literal: public expr{
public:
	expr_literal(token t, sym_type *);
	expr_literal(token t);
	void print(ostream &os, int level) override;

};

class expr_var: public expr{
public:
	expr_var(token t);
	void print(ostream &os, int level) override;
};

class expr_tern_op: public expr{
	expr *left, *middle, *right;
public:
	friend class parser;
	expr_tern_op(expr* l, expr *m, expr *r, string s);
	void print(ostream &os, int level);
};

class function:public expr{
	expr *fid;
	vector<expr *>fargs;
public:
	function(expr *id, const vector<expr *> &args);
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

class expr_cast2double: public expr{
	expr *ex;
public:
	expr_cast2double(expr *);
	void print(ostream &os, int level);
};
