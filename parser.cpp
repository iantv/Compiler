#include "parser.h"
#include "error.h"

parser::parser(lexer *l): lxr(l), table(new sym_table()) { }

vector<expr *> parser::parse_fargs(){
	vector<expr *> args;
	token tk = lxr->next(); /* skip TK_OPEN_B;RACKET */
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
	lxr->next(); /* skip TK_OPEN_SQUARE_BRACKET */
	expr *ex = expression(MIN_PRIORITY);
	if (lxr->next().type == TK_CLOSE_SQUARE_BRACKET)
		throw syntax_error(C2143, "missing \"]\" before \";\"", lxr->pos);
	return ex;
}

expr *parser::expression(int priority){
	if (priority > MAX_PRIORITY) return factor();		
	expr *ex = expression(priority + 1);
	token tk = lxr->get();
	while (get_priority(tk) == priority){
		lxr->next();
		if (tk.type == TK_QUESTION){
			expr *second = expression(3);
			if (lxr->get().type != TK_COLON){
				throw syntax_error(C2143, "missing \":\" before \";\"", lxr->pos);;
			}
			lxr->next();
			ex = new expr_tern_op(ex, second, expression(3), string("?:"));
		} else if (priority == 2){
			ex = new expr_bin_op(ex, expression(priority), tk);
		} else if (tk.type == TK_INC || tk.type == TK_DEC){
			ex = new expr_postfix_unar_op(ex, tk);			
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
					ex = new struct_access(ex, new expr_var(lxr->get()), tk);
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
		expr *ex = expression(MIN_PRIORITY);
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
	if (tk.type == TK_PLUS || tk.type == TK_MINUS || tk.type == TK_MUL || tk.type == TK_AND_BIT || tk.type == TK_NOT_BIT || tk.type == TK_INC || tk.type == TK_DEC){
		return new expr_prefix_unar_op(factor() , tk);
	}
	if (tk.type == TK_SIZEOF){
		return new expr_prefix_unar_op(expression(MIN_PRIORITY), tk);
	}
	return nullptr;
}

expr *parser::parse_expr(){
	return expression(MIN_PRIORITY);
}

symbol *add_elem_to_list(symbol *sym_list, symbol *sym2){
	symbol *t = sym2;
	t->next = sym_list;
	return t;
}

symbol *parser::parse_declare(sym_table *st){
	int cnt = 0; /* count of stars */
	symbol *list = nullptr;
	token tk = lxr->get();
	if (tk.is_type_specifier()){
		list = add_elem_to_list(list, new sym_type(tk.get_src()));
	} else if (tk.is_type_qualifier()){
		list = add_elem_to_list(list, new sym_const("const"));
		lxr->next();
		list = add_elem_to_list(list, parse_declare(st));
		return list;
	} else if (tk.is_storage_class_specifier()){
		lxr->next();
		return parse_declare(st);
	} else
		throw 1;

	while (lxr->next().type == TK_MUL)
		cnt++;
	list = add_elem_to_list(list, parse_dir_declare(st));
	
	while (cnt--){
		list = add_elem_to_list(list, new sym_pointer());
	} 
	if (tk.is_type_specifier()){
		list = add_elem_to_list(list, new sym_type(tk.get_src()));
	}
	return list;
}

symbol *parser::parse_dir_declare(sym_table *st){
	symbol *list = nullptr;
	if (lxr->get().type == TK_OPEN_BRACKET){
		list  = add_elem_to_list(list, parse_declare(st));
		if (lxr->get().type != TK_CLOSE_BRACKET){
			throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
		}
	} else if (lxr->get().type == TK_ID){
		list = add_elem_to_list(list, new sym_var(lxr->get().get_src()));
	} else {
		throw syntax_error(C2059, token_names[lxr->get().type], lxr->pos);
	}
	token tk = lxr->get();
	lxr->next();
	while ((tk.type == TK_OPEN_BRACKET && lxr->look_forward(1, char(token_names[TK_CLOSE_BRACKET]))) ||
		   (tk.type == TK_OPEN_SQUARE_BRACKET && lxr->look_forward(1, char(token_names[TK_CLOSE_SQUARE_BRACKET])))){
			   if (tk.type == TK_OPEN_BRACKET){
				   list = add_elem_to_list(list, new sym_function("()"));
			   } else {
				   list = add_elem_to_list(list, new sym_array("[]"));
			   }
	}
	return list;
}

void parser::parse(ostream &prs_os){
	init_gst();
	token tk = lxr->next();
	while (tk.type != NOT_TK){
		if (tk.is_type_specifier() || tk.is_type_qualifier() || tk.is_storage_class_specifier()){
			//table->add_sym(parse_declare(table));
			//DO prelude table, and parse chain and create symbol
		}else {
			expr *e = parse_expr();
			e->print(prs_os, 0);
		}
		tk = lxr->get();
	}
}

void parser::init_gst(){
	table->add_sym(new sym_type(token_names[TK_INT]));
	table->add_sym(new sym_type(token_names[TK_CHAR]));
	table->add_sym(new sym_type(token_names[TK_LONG]));
	table->add_sym(new sym_type(token_names[TK_SHORT]));
	table->add_sym(new sym_type(token_names[TK_DOUBLE]));
	table->add_sym(new sym_type(token_names[TK_FLOAT]));
	table->add_sym(new sym_type(token_names[TK_VOID]));
}