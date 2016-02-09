#ifndef PARSER
#define PARSER

#include "lexer.h"
#include "expression.h"
#include "sym_table.h"
#include "statements.h"

#include <list>

class declar{
	symbol *id;
	sym_type *type;
	string name;
	bool def; // + definition for functions and blocks
public:
	declar();
	declar(const declar &);
	void set_id(symbol *);
	void set_type(sym_type *);
	void set_back_type(sym_type *);
	void set_name(string);
	sym_type *get_type();
	symbol *get_id();
	const string &get_name();
	void rebuild(declar &);
	void reset_type(sym_type *);
	bool check_id(symbol *);
	bool is_def();
	friend symbol *make_symbol(declar &dcl);
	friend class parser;
};


class parser{
	lexer *lxr;
	sym_table *table;
	sym_table *prelude;
	expr *expression(int priority);
	expr *factor();
	expr *parse_function();
	expr *parse_index();
	expr *try_parse_tern_op();
	vector<expr *> parse_fargs(); /* parse function's arguments */
	void parse_fparams(sym_table *lst, vector<string> *); /* parse function's parametres */
	size_t parse_size_of_array();
	declar parse_dir_declare(sym_table *sym_tbl, bool tdef, bool tconst);
	declar parse_declare(sym_table *sym_tbl, bool tdef, bool tconst);
	declar parse_declare(sym_table *sym_tbl);
	symbol *try_parse_struct_member_list(string &struct_tag, sym_table *sym_tbl);
	void check_struct_member(symbol *member, string struct_tag, position pos);
	void init_prelude(); /* init global symtable prelude */
	void try_type_cast(expr **, expr **);
	expr *new_expr_bin_op(expr *ex1, expr *ex2, token tk);
	bool try_parse_block(sym_table *, stmt_block *);
	void try_parse_body(sym_table *, stmt_block *); /* parse function's or block's body */
	
	void try_parse_statements_list(sym_table *, stmt_block *stmt_blck);
	void try_parse_statement(sym_table *, stmt_block *stmt_blck);
	bool try_parse_declarator(sym_table *);
	bool is_expr_start(token, sym_table *);
public:
	parser(lexer *l);
	expr *parse_expr();
	void parse(ostream &os);
	void print_sym_table(ostream &os);
	bool tcast;
};

#endif