#ifndef PARSER
#define PARSER

#include "lexer.h"
#include "expression.h"

class parser{
	lexer *lxr;
	expr *expression(int priority);
	expr *factor();
	expr *tern_op(expr *first);
	expr *parse_function();
	vector<expr *> parse_fargs(); /* parse function's arguments */
public:
	parser(lexer *l);
	expr *parse_expr();
};

#endif