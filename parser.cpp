#include "parser.h"
#include "error.h"
#include <typeinfo.h>
/*---class declar---*/
declar::declar(): id(nullptr), type(nullptr){ }

void declar::set_id(symbol *decl_id){ id = decl_id; }
void declar::set_type(sym_type *decl_type){ 
	sym_type *t = decl_type;
	if (t != nullptr){
		while (t->type){
			t = t->type;
		}
		t->type = type;
	}
	type = decl_type;
}

void declar::set_back_type(sym_type *decl_type){
	sym_type *t = type;
	if (t != nullptr){
		while (t->type){
			t = t->type;
		}
		t->type = decl_type;
	} else 
		type = decl_type;
}

void declar::reset_type(sym_type *decl_type){
	type = decl_type;	
}

symbol *declar::get_id(){ return id; }
sym_type *declar::get_type(){ return type; }

declar::declar(declar &dcl){
	id = dcl.id;
	type = dcl.type;
}

void declar::rebuild(declar &dcl){
	id = dcl.id;
	sym_type *t = type;
	if (dcl.type){
		sym_type *last = dcl.type;
		while (last->type){
			last = last->type;
		}
		last->type = t;
		type = dcl.type;
	} 
}

void declar::set_name(string s){
	id->name = s;
}

const string &declar::get_name(){
	return id->name; 
}

bool declar::check_id(symbol *sym){
	return id == sym;
}

/*---class parser ----*/
parser::parser(lexer *l): lxr(l), table(new sym_table()) { }

vector<expr *> parser::parse_fargs(){
	vector<expr *> args;
	token tk = lxr->next(); /* skip open bracket '(' */
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
	lxr->next(); /* skip open square bracket '[' */
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

size_t parser::parse_size_of_array(){ /* Size is only const integer value */
	token tk = lxr->next();
	size_t t = 0;
	if (tk.type != TK_CLOSE_SQUARE_BRACKET){
		if (tk.type != TK_INT_VAL && tk.type != TK_CHAR_VAL )
			throw 1;
		t = atoi(tk.get_src().c_str());
		if (lxr->next().type != TK_CLOSE_SQUARE_BRACKET)
			throw syntax_error(C2143, "missing \"]\" before \";\"", lxr->pos);
	}
	return t;
}

void parser::parse_fparams(sym_table *lst){
	dcl_data param;
	token tk = lxr->next(); /* skip open bracket '(' */
	while (tk.type != TK_CLOSE_BRACKET){
		if (tk.is_type_specifier()){
			param.type = prelude->get_type_specifier(tk.get_src());
			if ((tk = lxr->next()).type == TK_ID){
				param.id = new sym_var_param(tk.get_src());
			}
			tk = lxr->next();
			if (tk.type != TK_COMMA){
				if (tk.type != TK_CLOSE_BRACKET){
					if (tk.is_type_specifier()){
						string s = "\"";
						s += tk.get_src();
						s += "\" should be preceded by \",\"";
						throw syntax_error(C2144, s, lxr->pos);
					} else if (tk.type == TK_ID){
						string s = "missing \",\" before identifier \"";
						s += tk.get_src(); s += "\"";
						throw syntax_error(C2146, s, lxr->pos);
					} else {
						string s = "missing \",\" before \"";
						s += tk.get_src(); s += "\""; 
						throw syntax_error(C2146, s, lxr->pos);
					}
				}
			} else
				tk = lxr->next();
			lst->add_sym(new sym_var_param(param.id->name, param.type));
		} else {
			if (tk.type == TK_ID){
				string s = tk.get_src();
				s += ": undeclared identifier";
				throw error(C2065, s, lxr->pos);
			} else {
				throw syntax_error(C2059, tk.get_src(), lxr->get().pos);
			}
		}
	}
}

symbol *parser::try_parse_struct(string &struct_tag, sym_table *sym_tbl){
	declar info;
	token tk = lxr->get();
	sym_table *slt = new sym_table(table); /* struct local table */
	if (tk.type == TK_OPEN_BRACE){
		tk = lxr->next(); /* skip open square bracket '{' */
		while (tk.type != TK_CLOSE_BRACE){
			slt->add_sym(make_symbol(parse_declare(slt)));
			tk = lxr->get();
			if (tk.type == TK_SEMICOLON)
				tk = lxr->next();
			else {
				string s = "\"" + tk.get_src() + "\" should be preceded by \";\"";
				throw syntax_error(C2144, s, tk.pos);
			}
		}
	} else throw 1;
	sym_type *st = new sym_struct(struct_tag, slt);
	info.set_id(st);
	return make_symbol(info);
}

declar parser::parse_declare(sym_table *sym_tbl){
	declar info;
	token tk = lxr->get();
	if (tk.is_type_specifier()){ // is_users_type(tk)
		if (tk.type == TK_STRUCT){
			tk = lxr->next();
			string tag;
			if (tk.type == TK_ID){
				tag = tk.get_src();
				tk = lxr->next();
				if (tk.type == TK_ID){
					if (sym_tbl->global_exist(tag) || sym_tbl->local_exist(tag)){
						info.set_type(sym_tbl->get_type_specifier(tag));
						info.rebuild(parse_dir_declare(sym_tbl));
						info.set_name(tk.get_src());
						return info;
					} else {
						throw error(C2079, "\"" + tk.get_src() + "\" uses undefined struct \"" + tag + "\"", tk.pos);
					}		
				}
			}
			symbol *t = try_parse_struct(tag, sym_tbl);
			string::iterator it = lxr->it;
			if (it != lxr->s.end() && (*it) == ';'){
				lxr->next();
				info.set_id(t);
				return info;
			}
			sym_tbl->add_sym(t);
			info.set_type(dynamic_cast<sym_type *>(t));
		} else {
			info.set_type(prelude->get_type_specifier(tk.get_src()));
		}
	}
	while (lxr->next().type == TK_MUL)
		info.reset_type(new sym_pointer(info.get_type()));
	info.rebuild(parse_dir_declare(sym_tbl));
	return info;
}

declar parser::parse_dir_declare(sym_table *sym_tbl){
	string name;
	declar info = declar();
	bool dir_dcl= false;
	if (lxr->get().type == TK_OPEN_BRACKET){
		info.rebuild(parse_declare(sym_tbl));
		if (lxr->get().type != TK_CLOSE_BRACKET)
			throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
		dir_dcl = true;
	} else if (lxr->get().type == TK_ID){
		name = lxr->get().get_src();
	}
	token tk = lxr->next();
	if (tk.type != TK_OPEN_BRACKET && tk.type != TK_OPEN_SQUARE_BRACKET){
		info.set_id(new sym_var(name));
	} else {
		while ((tk.type == TK_OPEN_BRACKET) || (tk.type == TK_OPEN_SQUARE_BRACKET)){
			if (tk.type == TK_OPEN_SQUARE_BRACKET){
				if (info.check_id(nullptr)){
					info.set_id(new sym_array(parse_size_of_array()));
					info.set_name(name);
				} else {
					if (dir_dcl)
						info.set_back_type(new sym_array(parse_size_of_array()));
					else
						info.set_type(new sym_array(parse_size_of_array()));		
				}
			} else if (tk.type == TK_OPEN_BRACKET){
				sym_table *st = new sym_table(sym_tbl);
				parse_fparams(st);
				if (info.check_id(nullptr)){
					info.set_id(new sym_function(name, st));
				} else {
					string s = (info.get_type() == nullptr) ? typeid(*info.get_id()).name() : typeid(*info.get_type()).name();
					if (s == "class sym_array"){
						throw error(C2092, "element type of array cannot be function", lxr->pos);
					} else if (s == "class sym_function" || s == "class sym_func_type"){
						throw error(C2091, "function returns function", lxr->pos);
					}
					if (dir_dcl)
						info.set_back_type(new sym_func_type(nullptr, st));
					else
						info.set_type(new sym_func_type(nullptr, st));
				}
			}
			tk = lxr->next();
		}
	}
	return info;
}

void parser::parse(ostream &os){
	init_prelude(); 	
	token tk = lxr->next();
	sym_type *stype = nullptr;
	while (tk.type != NOT_TK){
		if (tk.is_type_specifier() || tk.is_type_qualifier() || tk.is_storage_class_specifier()){
			symbol *t = make_symbol(parse_declare(table));
			table->add_sym(t);
			stype = t->type;
		} else if (tk.type == TK_SEMICOLON){
			tk = lxr->next();
			stype = nullptr;
			continue;
		} else if (tk.type == TK_COMMA && stype != nullptr){
			declar dcl = parse_declare(table);
			dcl.set_type(stype);
			table->add_sym(make_symbol(dcl));
		} else {
			expr *e = parse_expr();
			e->print(os, 0);
		}
		tk = lxr->get();
	}
}

void parser::init_prelude(){
	prelude = new sym_table();
	prelude->add_sym(new sym_type(token_names[TK_INT]));
	prelude->add_sym(new sym_type(token_names[TK_CHAR]));
	prelude->add_sym(new sym_type(token_names[TK_DOUBLE]));
	prelude->add_sym(new sym_type(token_names[TK_VOID]));
}

void parser::print_sym_table(ostream &os){
	os << (*table);
}