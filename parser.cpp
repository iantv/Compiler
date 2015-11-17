#include "parser.h"

parser::parser(lexer *l): lxr(l) {}

expr *parser::tern_op(expr *first){
	expr *middle = factor();
	if (lxr->get().type != TK_COLON)
		throw 1;
	expr *last = factor();
	return new expr_tern_op(first, middle, last, "?:");
};

vector<expr *> parser::parse_fargs(){
	vector<expr *> args;
	token tk = lxr->next();
	while (tk.type != TK_CLOSE_BRACKET){		
		args.push_back(expression(2)); /* because priority of comma as statement equal 1, but when listing arguments of function comma is separator */
		if (lxr->get().type == TK_CLOSE_BRACKET)
			break;
		tk = lxr->next(); /* skip comma	*/
	}
	if (lxr->get().type != TK_CLOSE_BRACKET)
		throw 1;
	lxr->next(); /* skip close bracket ')' */
	return args;
}

expr *parser::expression(int priority){
	if (priority > MAX_PRIORITY) return factor();		
	expr *ex = expression(priority + 1);
	token tk = lxr->get();
	while (get_priority(tk) == priority){
		lxr->next();

		if (priority == 3){
			ex = tern_op(ex);
		} else
			ex = new expr_bin_op(ex, expression(priority + 1), tk);
		tk = lxr->get();
	}
	return ex;
}

expr *parser::factor(){
	token tk = lxr->get();
	token tk_next = lxr->next();
	if (tk.type == TK_ID){
		expr *ex;
		if (tk_next.type == TK_OPEN_SQUARE_BRACKET){
			lxr->next();
			ex = expression(1);
			if (lxr->next().type == TK_CLOSE_SQUARE_BRACKET){
				throw 1;
			}
			return new expr_bin_op(new expr_var(tk), ex, string("[]"));
		}
		ex = new expr_var(tk);;		
		while (lxr->get().type == TK_OPEN_BRACKET){
			ex = new function(ex, parse_fargs());
		}
		while (lxr->get().type == TK_POINT){
			//lxr->next();
			//expr *ex = factor();
			return new structure(ex, factor(), string("."));
		}
		return ex;
	}
	if (tk.type == TK_OPEN_BRACKET){
		expr *ex = expression(1);
		if (lxr->get().type != TK_CLOSE_BRACKET)
			throw 1;
		lxr->next();
		while (lxr->get().type == TK_OPEN_BRACKET){
			ex = new function(ex, parse_fargs());
		}
		return ex;
	}
	if (tk.type == TK_INT_VAL || tk.type == TK_DOUBLE_VAL || tk.type == TK_CHAR_VAL){
		return new expr_literal(tk);
	}
	if (tk.type == TK_PLUS || tk.type == TK_MINUS || tk.type == TK_MUL || tk.type == TK_AND_BIT || tk.type == TK_NOT_BIT){
		return new expr_unar_op(factor() , tk);
	}
	if (tk.type == TK_SIZEOF){
		return new expr_unar_op(expression(1), tk);
	}
	return NULL;
}

expr *parser::parse_expr(){
	return expression(MIN_PRIORITY);
}
