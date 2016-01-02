#include "parser.h"
#include "error.h"

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

size_t parser::parse_size_of_array(){
	// DO
	return size_t(0);
}

void parser::parse_fparams(sym_table *lst){
	dcl_data param;
	//expr *ex = nullptr;
	token tk = lxr->next(); /* skip ')' */
	while (tk.type != TK_CLOSE_BRACKET){
		if (tk.is_type_specifier()){
			param.type = prelude->get_type_specifier(tk.get_src());
			if ((tk = lxr->next()).type == TK_ID){
				param.id = new sym_var_param(tk.get_src());
			}
			tk = lxr->next();	/* */
			if (tk.type != TK_COMMA){
				if (tk.type != TK_CLOSE_BRACKET)
					throw 1;
			} else {
				tk = lxr->next(); 
			}
			lst->add_sym(new sym_var_param(param.id->name, param.type));
		} else throw 1;
	}
}

dcl_data parser::parse_declare(){
	dcl_data dcld;
	token tk = lxr->get();
	if (tk.is_type_specifier()){ // is_users_type(tk)
		dcld.type = prelude->get_type_specifier(tk.get_src());
	}
	while (lxr->next().type == TK_MUL)
		dcld.type = new sym_pointer(dcld.type);
	parse_dir_declare(dcld);
	return dcld;
}

void parser::parse_dir_declare(dcl_data &dcl){
	string name;
	if (lxr->get().type == TK_OPEN_BRACKET){
		dcl = parse_declare();
		if (lxr->get().type != TK_CLOSE_BRACKET)
			throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
	} else if (lxr->get().type == TK_ID){
		name = lxr->get().get_src();
	}
	token tk = lxr->next();
	if (tk.type != TK_OPEN_BRACKET && tk.type != TK_OPEN_SQUARE_BRACKET){
		dcl.id = new sym_var(name);
	} else {
		while ((tk.type == TK_OPEN_BRACKET) || (tk.type == TK_OPEN_SQUARE_BRACKET)){
			if (tk.type == TK_OPEN_BRACKET){
				sym_table *st = new sym_table(table);
				parse_fparams(st);
				if (name == "")
					dcl.type = new sym_func_type(dcl.type, st);
				else {
					dcl.id = new sym_function(name, st);
					name = "";
				}
				if (lxr->get().type != TK_CLOSE_BRACKET)
					throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
			}
			if (tk.type == TK_OPEN_SQUARE_BRACKET){
				dcl.type = new sym_array(parse_size_of_array());
				if (tk.type != TK_CLOSE_SQUARE_BRACKET)
					throw syntax_error(C2143, "missing \"]\" before \";\"", lxr->pos);
			}
			tk = lxr->next();
		}
	}

}

void parser::parse(ostream &os){
	init_prelude(); 	
	token tk = lxr->next();
	while (tk.type != NOT_TK){
		if (tk.is_type_specifier() || tk.is_type_qualifier() || tk.is_storage_class_specifier()){
			dcl_data t = parse_declare();
			t.id->type = t.type;
			table->add_sym(t.id);
		} else {
			expr *e = parse_expr();
			e->print(os, 0);
		}
		tk = lxr->next();
	}
}

void parser::init_prelude(){
	prelude = new sym_table();
	prelude->add_sym(new sym_integer(token_names[TK_INT]));
	prelude->add_sym(new sym_integer(token_names[TK_CHAR]));
	prelude->add_sym(new sym_type(token_names[TK_DOUBLE]));
	prelude->add_sym(new sym_type(token_names[TK_VOID]));
}

void parser::print_sym_table(ostream &os){
	os << (*table);
}