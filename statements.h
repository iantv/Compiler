#pragma once
#include "expression.h"

class sym_function;
enum stmt_t{ STMT_EXPR, STMT_BLOCK, STMT_IF };

class stmt{
protected:
	stmt_t type;
public:
	stmt(){}
	virtual void print(ostream &os, int level) = 0;
	void print_level(ostream &os, int level);
	virtual void generate(asm_cmd_list *){}
};

enum cond_t{ EXECUTION_COND, END_COND, CONTINUED_COND, NOT_COND };
class stmt_expr: public stmt{
	expr *e;
	cond_t cond_type;
public:
	friend class parser;
	stmt_expr(expr *, cond_t);
	void print(ostream &os, int level) override;
	void generate(asm_cmd_list *) override;
};

class stmt_block: public stmt{
	sym_table *table;
	vector<stmt *> stmt_list;
	sym_function *owner;
public:
	friend class parser;
	stmt_block(vector<stmt *>);
	stmt_block();
	stmt_block(sym_table *, sym_function *);
	void push_back(stmt *);
	void print(ostream &os, int level) override;
	void generate(asm_cmd_list *) override;
};

class stmt_if: public stmt{
	stmt_expr *cond;
	stmt_block *body_if_true;
	stmt_block *body_if_false;
public:
	friend class parser;
	stmt_if();
	stmt_if(stmt_expr *, sym_table *, sym_function *);
	void push_back_if_true(stmt *);
	void push_back_if_false(stmt *);
	void print(ostream &os, int level) override;
};

class stmt_while: public stmt{
	stmt_block *body;
	stmt_expr *cond;
public:
	friend class parser;
	stmt_while(stmt_expr *, sym_table *, sym_function *);
	void push_back(stmt *);
	void print(ostream &os, int level);
};

class stmt_for: public stmt{
	stmt_block *body;
	sym_table *table;
	stmt_expr *init, *cond, *step;
public:
	friend class parser;
	stmt_for(stmt_expr *, stmt_expr *, stmt_expr *, sym_table *, sym_function *);
	void push_back(stmt *);
	void print(ostream &os, int level);
};

class stmt_break: public stmt{
public:
	stmt_break(){}
	void print(ostream &os, int level);
};

class stmt_continue: public stmt{
public:
	stmt_continue(){}
	void print(ostream &os, int level);
};

class stmt_return: public stmt{
	stmt_expr *ex;
public:
	stmt_return(stmt_expr *);
	void print(ostream &os, int level);
	void generate(asm_cmd_list *) override;
};

class stmt_printf: public stmt{
	string format;
	vector<stmt_expr *> args;
public:
	stmt_printf(vector<expr *> &);
	void print(ostream &os, int level);
	void generate(asm_cmd_list *) override;
}; 