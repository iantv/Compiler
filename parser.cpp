#include "parser.h"

parser::parser(lexer *l): lxr(l) {}

expr *parser::expression(int priority){
	if (priority > MAX_PRIORITY) return factor();		
	expr *ex = expression(priority + 1);
	token tk = lxr->get();
	while (get_priority(tk) == priority){
		lxr->next();
		ex = new expr_bin_op(ex, expression(priority + 1), tk);
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
		expr *ex = expression(1);
		if (tk.type == TK_CLOSE_BRACKET)
			throw 1;
		lxr->next();
		return ex;
	}
	if (tk.type == TK_INT_VAL || tk.type == TK_DOUBLE_VAL || tk.type == TK_CHAR_VAL){
		return new expr_literal(tk);
	}
	if (tk.type == TK_PLUS || tk.type == TK_MINUS || tk.type == TK_MUL || tk.type == TK_AND_BIT){
		return new expr_unar_op(factor() , tk);
	}
	return NULL;
}

expr *parser::parse_expr(){
	return expression(MIN_PRIORITY);
}
