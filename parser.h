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
	vector<stmt *> init_list;
	int id_name;
	bool global_init;
	expr *expression(sym_table *, int priority);
	expr *factor(sym_table *);
	expr *parse_function();
	expr *parse_index(sym_table *);
	expr *try_parse_tern_op();
	vector<expr *> parse_fargs(sym_table *); /* parse function's arguments */
	void parse_fparams(sym_table *lst, vector<string> *); /* parse function's parametres */
	
	size_t parse_size_of_array();
	declar parse_dir_declare(sym_table *sym_tbl, bool tdef, bool tconst);
	declar parse_declare(sym_table *sym_tbl, bool tdef, bool tconst);
	declar parse_declare(sym_table *sym_tbl);
	sym_type *try_parse_struct_decl(sym_table *, bool, bool);
	void try_parse_struct_member_list(sym_struct *);
	void check_struct_member(symbol *member, string struct_tag, position pos);
	void init_prelude(); /* init global symtable prelude */
	void try_type_cast(expr **, expr **);
	
	expr *new_expr_bin_op(expr *ex1, expr *ex2, token tk);
	expr *new_expr_var(sym_table *, token);
	
	void try_parse_block(sym_table *, stmt_block *, sym_function *, bool);
	stmt_block *try_parse_body(sym_table *, sym_function *, bool); /* parse function's body */
	void try_parse_init(symbol *, sym_table *, stmt_block *);
	void try_parse_statements_list(sym_table *, stmt_block *, sym_function *, bool);
	void try_parse_statement(sym_table *, stmt_block *, sym_function *, bool);
	void try_parse_declarator(sym_table *, stmt_block *);
	bool try_parse_definition(symbol *);
	
	bool is_expr_start(token, sym_table *);
	bool is_block_start();
	bool is_decl_start(sym_table *);

	void try_parse_if_stmt(sym_table *, stmt_block *, sym_function *, bool);
	void try_parse_stmt_body(sym_table *, stmt_block *, sym_function *, bool);
	void try_parse_while_stmt(sym_table *, stmt_block *, sym_function *);
	void try_parse_for_stmt(sym_table *, stmt_block *, sym_function *);
	
	expr *try_cast2type(expr *, sym_type *, sym_table *);

	void check_semicolon();
	void check_decl2errors(sym_table *, symbol **, token);
	void check_func_decl2errors(symbol **, token);
public:
	parser(lexer *l);
	expr *parse_expr(sym_table *);
	void parse(ostream &os);
	void print_sym_table(ostream &os);
	bool tcast;
	bool prs_expr;
	bool point_of_entry;
};

#endif