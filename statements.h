#pragma once
#include "parser.h"
#include "expression.h"

enum stmt_t{ STMT_EXPR, STMT_BLOCK };

class stmt{
protected:
	stmt_t type;
public:
	stmt(){};
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
};

class stmt_expr: public stmt{
	expr *e;
public:
	friend class parser;
	stmt_expr(expr *);
	void print(ostream &os, int level) override;
};

class stmt_block: public stmt{
	sym_table *table;
	vector<stmt *> stmt_list;
public:
	stmt_block(vector<stmt *>);
	stmt_block();
	stmt_block(sym_table *);
	void push_back(stmt *);
	void print(ostream &os, int level) override;
};