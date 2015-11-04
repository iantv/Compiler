#ifndef PARSER
#define PARSER

#include "lexer.h"
#include <stack>

class expr{
protected:
	token tk;
public:
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
};

class expr_bin_op: public expr{
	expr *l, *r;
public:
	expr_bin_op(expr *left, expr *right, token t);
	void print(ostream &os, int level);
};

class expr_literal: public expr{
public:
	expr_literal(token t);
	void print(ostream &os, int level);
};

class expr_var: public expr{
public:
	expr_var(token t);
	void print(ostream &os, int level);
};

class parser{
	expr *e;
	lexer *lxr;
	expr *expression();
	expr *term();
	expr *factor();
public:
	parser(lexer *l);
	void parse();	
	void print(ostream &os);
};

#endif