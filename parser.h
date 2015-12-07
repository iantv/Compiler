#ifndef PARSER
#define PARSER

#include "lexer.h"
#include "expression.h"

class parser{
	lexer *lxr;
	expr *expression(int priority);
	expr *factor();
	expr *parse_function();
	expr *parse_index();
	expr *try_parse_tern_op();
	vector<expr *> parse_fargs(); /* parse function's arguments */
	void parse_declare(ostream &os);
	void parse_dir_declare(ostream &os);
public:
	parser(lexer *l);
	void parse(ostream &prs_os);
	expr *parse_expr();
};

#endif