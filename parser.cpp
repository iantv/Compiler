#include "parser.h"
#include "error.h"

parser::parser(lexer *l): lxr(l) {}

vector<expr *> parser::parse_fargs(){
	vector<expr *> args;
	token tk = lxr->next(); /* skip TK_OPEN_BRACKET */
	while (tk.type != TK_CLOSE_BRACKET){		
		args.push_back(expression(2)); /* because priority of comma as statement equal 1, but when listing arguments of function comma is separator */
		if (lxr->get().type == TK_CLOSE_BRACKET)
			break;
		tk = lxr->next(); /* skip comma	*/
	}
	if (lxr->get().type != TK_CLOSE_BRACKET)
		throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
	lxr->next(); /* skip close bracket ')' */
	return args;
}

expr *parser::parse_index(){
	int a[100];

	lxr->next(); /* skip TK_OPEN_SQUARE_BRACKET */
	expr *ex = expression(1);
	if (lxr->next().type == TK_CLOSE_SQUARE_BRACKET)
		throw syntax_error(C2143, "missing \"]\" before \";\"", lxr->pos);
	return ex;
}

expr *parser::expression(int priority){
	if (priority > MAX_PRIORITY) return factor();		
	expr *last = nullptr, *ex = expression(priority + 1);
	token tk = lxr->get();
	while (get_priority(tk) == priority){
		lxr->next();
		if (tk.type == TK_QUESTION){
			expr *second = expression(4);
			if (lxr->get().type != TK_COLON){
				throw syntax_error(C2143, "missing \":\" before \";\"", lxr->pos);;
			}
			lxr->next();
			ex = new expr_tern_op(ex, second, expression(4), string("?:"));
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
		expr *ex = new expr_var(tk);		
		tk = tk_next;
		while (tk.type == TK_OPEN_BRACKET || tk.type == TK_OPEN_SQUARE_BRACKET || tk.type == TK_POINT || tk.type == TK_PTROP){
			if (lxr->get().type == TK_POINT || lxr->get().type == TK_PTROP){
				if (lxr->next().type == TK_ID){
					ex = new structure(ex, new expr_var(lxr->get()), tk);
					lxr->next();
				}
			}
			if (lxr->get().type == TK_OPEN_SQUARE_BRACKET){
				ex = new expr_bin_op(ex, parse_index(), string("[]"));
			}
			if (lxr->get().type == TK_OPEN_BRACKET){
				ex = new function(ex, parse_fargs());
			}
			tk = lxr->get();
		}
		return ex;
	}
	if (tk.type == TK_OPEN_BRACKET){
		expr *ex = expression(1);
		if (lxr->get().type != TK_CLOSE_BRACKET){
			throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
		}
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
