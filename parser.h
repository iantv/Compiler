#ifndef PARSER
#define PARSER

#include "lexer.h"
#include "expression.h"
#include "sym_table.h"

#include <list>

typedef struct {
	symbol *id;
	sym_type *type;
}dcl_data;

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
	void parse_fparams(sym_table *lst); /* parse function's parametres */
	size_t parse_size_of_array();
	void parse_dir_declare(dcl_data &dcl);
	dcl_data parse_declare();
	void init_prelude(); /* init global symtable prelude */
public:
	parser(lexer *l);
	expr *parse_expr();
	void parse(ostream &os);
	void print_sym_table(ostream &os);
};

#endif