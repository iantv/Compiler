#include "parser.h"

/* methods of class expr */
expr_bin_op::expr_bin_op(expr *left, expr *right, token t):l(left), r(right){ tk = t; }
expr_const::expr_const(token t){ tk = t; }
expr_var::expr_var(token t){ tk = t; }

void expr_bin_op::print(ostream &os, int level){
}

void expr_const::print(ostream &os, int level){
}

void expr_var::print(ostream &os, int level){
}

/* methods of class parser */
parser::parser(lexer *l): lxr(l) {}

void parser::parse(){
}

void parser::print(ostream &os){
	e->print(os, 1);
}