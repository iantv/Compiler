#pragma once
#include "expression.h"

enum stmt_t{ STMT_EXPR, STMT_BLOCK, STMT_IF };

class stmt{
protected:
	stmt_t type;
public:
	stmt(){};
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
};

enum cond_t{ EXECUTION_COND, END_COND, CONTINUED_COND, NOT_COND };
class stmt_expr: public stmt{
	expr *e;
	cond_t cond_type;
public:
	friend class parser;
	stmt_expr(expr *, cond_t);
	void print(ostream &os, int level) override;
};

class stmt_block: public stmt{
	sym_table *table;
	vector<stmt *> stmt_list;
public:
	friend class parser;
	stmt_block(vector<stmt *>);
	stmt_block();
	stmt_block(sym_table *);
	void push_back(stmt *);
	void print(ostream &os, int level) override;
};

class stmt_if: public stmt{
	sym_table *table_if_true;
	sym_table *table_if_false;
	stmt_expr *cond;
	stmt_block *body_if_true;
	stmt_block *body_if_false;
public:
	friend class parser;
	stmt_if();
	stmt_if(stmt_expr *, sym_table *);
	void push_back_if_true(stmt *);
	void push_back_if_false(stmt *);
	void print(ostream &os, int level) override;
};