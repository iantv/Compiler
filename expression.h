#ifndef EXPRESSION
#define EXPRESSION

#include <iostream>
#include "lexer.h"

#define MAX_PRIORITY 16
#define MIN_PRIORITY 1

int get_priority(token tk);

class expr{
protected:
	token tk;
public:
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
	int operator<<(int);
};

class expr_bin_op: public expr{
	expr *left, *right;
public:
	expr_bin_op(expr *l, expr *r, token t);
	void print(ostream &os, int level) override;
};

/*
class expr_unar_op: public expr{
	expr *ex;
public:
	expr_unar_op(expr *e, token t);
	void print(ostream &os, int level);
	int priority();
};*/

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

/* DO */
/*class expr_tern_op: public expr{
	expr *left, *middle, *right;
	token expected_tk; 
public:
	expr_tern_op(expr* l, expr *m, expr *r, token t);
	void print(ostream &os, int level);
};

class expr_rel_op: public expr_bin_op{
public:
	expr_rel_op(expr *l, expr *r, token t);
};*/

#endif