#pragma once
#include <iostream>
#include <vector>
#include "lexer.h"
#include "sym_table.h"

#define MAX_PRIORITY 16
#define MIN_PRIORITY 1

int get_priority(token tk, bool unar = 0);

class expr{
protected:
	token tk;
	string op;
public:
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
	int operator<<(int);
};

class expr_bin_op: public expr{
protected:
	expr *left, *right;
public:
	expr_bin_op(expr *l, expr *r, token t);
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