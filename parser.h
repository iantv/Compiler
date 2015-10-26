#ifndef PARSER
#define PARSER

#include "lexer.h"

class parser{
	lexer *lxr;
public:
	parser(lexer *l);
};

#endif