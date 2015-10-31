#include "parser.h"

/* methods of class expr */
expr_bin_op::expr_bin_op(expr *left, expr *right, token t):l(left), r(right){ tk = t; }
expr_const::expr_const(token t){ tk = t; }
expr_var::expr_var(token t){ tk = t; }

void expr::print_level(ostream &os, int level){
	for (; level; level--) os << '\t';
}

void expr_bin_op::print(ostream &os, int level){
	l->print(os, level + 1);
	print_level(os, level);
	r->print(os, level + 1);
}

void expr_const::print(ostream &os, int level){
	print_level(os, level);
}

void expr_var::print(ostream &os, int level){
	print_level(os, level);
	
}

/* methods of class parser */
parser::parser(lexer *l): lxr(l) {}

void parser::parse(){

}

void parser::print(ostream &os){
	e->print(os, 1);
}