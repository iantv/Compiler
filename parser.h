#ifndef PARSER
#define PARSER

#include "lexer.h"
#include "expression.h"

class parser{
	lexer *lxr;
	expr *expression(int priority);
	expr *factor();
public:
	parser(lexer *l);
	expr *parse_expr();
};

#endif