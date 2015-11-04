#include "parser.h"

expr_bin_op::expr_bin_op(expr *left, expr *right, token t): l(left), r(right){ tk = t; }
expr_literal::expr_literal(token t){ tk = t; }
expr_var::expr_var(token t){ tk = t; }

void expr::print_level(ostream &os, int level){
	while (level){
		os << '\t';
		level--;
	}
}

void expr_bin_op::print(ostream &os, int level){
	r->print(os, level + 1);
	print_level(os, level);
	os << tk.get_src() << endl;
	l->print(os, level + 1);
}

void expr_literal::print(ostream &os, int level){
	print_level(os, level);
	os << tk.get_src() << endl;
}

void expr_var::print(ostream &os, int level){
	print_level(os, level);
	os << tk.get_src() << endl;
}

parser::parser(lexer *l): lxr(l) {}

expr *parser::expression(){
	expr *ex = term();
	token tk = lxr->get();
	while (tk.type == TK_PLUS || tk.type == TK_MINUS){
		lxr->next();
		ex = new expr_bin_op(ex, term(), tk);
		tk = lxr->get();
	}
	return ex;
}

expr *parser::term(){
	expr *ex = factor();
	token tk = lxr->get();
	while (tk.type == TK_MUL || tk.type == TK_DIV){
		lxr->next();
		ex = new expr_bin_op(ex, factor(), tk);
		tk = lxr->get();
	}
	return ex;
}

expr *parser::factor(){
	token tk = lxr->get();
	lxr->next();
	if (tk.type == TK_ID){
		return new expr_var(tk);
	}
	if (tk.type == TK_OPEN_BRACKET){
		expr *ex = expression();
		if (tk.type == TK_CLOSE_BRACKET)
			throw 1;
		lxr->next();
		return ex;
	}
	if (tk.type == TK_INT_VAL || tk.type == TK_DOUBLE_VAL || tk.type == TK_CHAR_VAL){
		return new expr_literal(tk);
	}
	return e;
}

void parser::parse(){
	lxr->next();
	e = expression();	
}

void parser::print(ostream &os){
	e->print(os, 0);
}
