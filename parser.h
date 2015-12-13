#ifndef PARSER
#define PARSER

#include "lexer.h"
#include "expression.h"
#include "sym_table.h"

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
	symbol *parse_dir_declare(sym_table *st);
	symbol *parse_declare(sym_table *st);
	void init_prelude(); /* init global symtable prelude */
	symbol *parse_identifier(sym_table *st);
public:
	parser(lexer *l);
	expr *parse_expr();
	void parse(ostream &prs_os);
};

#endif