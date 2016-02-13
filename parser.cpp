#include "parser.h"
#include "error.h"
#include "statements.h"
#include <typeinfo.h>
/*--------------------------------------class declar--------------------------------------*/
declar::declar(): id(nullptr), type(nullptr), def(false){ }

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

void declar::reset_type(sym_type *decl_type){ type = decl_type;	}

symbol *declar::get_id(){ return id; }
sym_type *declar::get_type(){ return type; }

declar::declar(const declar &dcl){
	id = dcl.id;
	name = dcl.name;
	type = dcl.type;
	def = dcl.def;
}

void declar::rebuild(declar &dcl){
	name = dcl.name;
	id = dcl.id;
	def = dcl.def;
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

void declar::set_name(string s){ if (s != "") id->name = s; }

bool declar::is_def(){ return def; }
const string &declar::get_name(){
	return id->name; 
}

bool declar::check_id(symbol *sym){ return id == sym; }

/*---------------------------------------class parser --------------------------------------*/
parser::parser(lexer *l): lxr(l), table(new sym_table()) { 	init_prelude(); tcast = true; }

vector<expr *> parser::parse_fargs(sym_table *sym_tbl){
	vector<expr *> args;
	token tk = lxr->next(); /* skip open bracket '(' */
	while (tk.type != TK_CLOSE_BRACKET){		
		args.push_back(expression(sym_tbl, 2)); /* because priority of comma as statement equal 1, but when listing arguments of function comma is separator */
		if (lxr->get().type == TK_CLOSE_BRACKET)
			break;
		tk = lxr->next(); /* skip comma	*/
	}
	if (lxr->get().type != TK_CLOSE_BRACKET)
		throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
	lxr->next(); /* skip close bracket ')' */
	return args;
}

expr *parser::parse_index(sym_table *sym_tbl){
	lxr->next(); /* skip open square bracket '[' */
	expr *ex = expression(sym_tbl, MIN_PRIORITY);
	if (lxr->next().type == TK_CLOSE_SQUARE_BRACKET)
		throw syntax_error(C2143, "missing \"]\" before \";\"", lxr->pos);
	return ex;
}

expr *parser::new_expr_bin_op(expr *ex1, expr *ex2, token tk){
	if (tcast == false || (ex1->type->name == ex2->type->name)) /* if type casting is disabled or operands has the same type */
		return new expr_bin_op(ex1, ex2, tk);		  /* create pointer to expr_bin_op without type checking and type casting */
	if (ex1->of_ctype(token_names[TK_DOUBLE]) && (ex2->of_ctype(token_names[TK_INT]) || ex2->of_ctype(token_names[TK_CHAR]))){
		ex2 = new expr_cast2type(token_names[TK_DOUBLE], ex2, prelude);
	} else if ((ex1->of_ctype(token_names[TK_INT]) || ex1->of_ctype(token_names[TK_CHAR])) && ex2->of_ctype(token_names[TK_DOUBLE])){
		ex1 = new expr_cast2type(token_names[TK_DOUBLE], ex1, prelude);
	} else if (ex1->of_ctype(token_names[TK_INT]) && ex2->of_ctype(token_names[TK_CHAR])){
		ex2 = new expr_cast2type(token_names[TK_INT], ex2, prelude);
	} else if (ex1->of_ctype(token_names[TK_CHAR]) && ex2->of_ctype(token_names[TK_INT])){
		ex1 = new expr_cast2type(token_names[TK_INT], ex1, prelude);
	}
	
	return new expr_bin_op(ex1, ex2, tk);;
}

expr *parser::expression(sym_table *sym_tbl, int priority){
	if (priority > MAX_PRIORITY) return factor(sym_tbl);		
	expr *ex = expression(sym_tbl, priority + 1);
	token tk = lxr->get();
	while (get_priority(tk) == priority){
		lxr->next();
		if (tk.type == TK_QUESTION){
			expr *second = expression(sym_tbl, 3);
			if (lxr->get().type != TK_COLON){
				throw syntax_error(C2143, "missing \":\" before \";\"", lxr->pos);
			}
			lxr->next();
			ex = new expr_tern_op(ex, second, expression(sym_tbl, 3), string("?:"));
		} else if (priority == 2){			
			ex = new_expr_bin_op(ex, expression(sym_tbl, priority), tk);
		} else if (tk.type == TK_INC || tk.type == TK_DEC){
			ex = new expr_postfix_unar_op(ex, tk);			
		} else {
			ex = new_expr_bin_op(ex, expression(sym_tbl, priority + 1), tk);
		}
		tk = lxr->get();
	}
	return ex;
}

expr * parser::new_expr_var(sym_table *sym_tbl, token tk){
	if (tcast == false) return new expr_var(tk, nullptr);
	if (sym_tbl == nullptr) throw error("symbol table is nullptr ", tk.pos);
	symbol * sym = sym_tbl->get_symbol(tk.get_src());
	if (sym == nullptr)
		throw error(tk.get_src() + " undefined ", tk.pos);
	return new expr_var(tk, sym->type);
}

expr *parser::factor(sym_table *sym_tbl){
	token tk = lxr->get();
	token tk_next = lxr->next();
	if (tk.type == TK_ID){
		expr *ex = new_expr_var(sym_tbl, tk);
		tk = tk_next;
		while (tk.type == TK_OPEN_BRACKET || tk.type == TK_OPEN_SQUARE_BRACKET || tk.type == TK_POINT || tk.type == TK_PTROP){
			if (lxr->get().type == TK_POINT || lxr->get().type == TK_PTROP){
				if (lxr->next().type == TK_ID){
					ex = new struct_access(ex, new expr_var(lxr->get(), nullptr), tk);
					lxr->next();
				}
			}
			if (lxr->get().type == TK_OPEN_SQUARE_BRACKET){
				ex = new expr_bin_op(ex, parse_index(sym_tbl), string("[]"));
			}
			if (lxr->get().type == TK_OPEN_BRACKET){
				ex = new function(ex, parse_fargs(sym_tbl));
			}
			tk = lxr->get();
		}
		return ex;
	}
	if (tk.type == TK_OPEN_BRACKET){
		expr *ex;
		bool tc = false; 
		if (tk_next.type == TK_DOUBLE || tk_next.type == TK_CHAR || tk_next.type == TK_INT){
			tc = true; lxr->next();
		} else {
			ex = expression(sym_tbl, MIN_PRIORITY);
		}
		if (lxr->get().type != TK_CLOSE_BRACKET){
			throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
		}
		lxr->next();
		if (tc){
			// redo!!!!!
			//ex = new expr_cast2type(tk_next.get_src(), factor());
		}
		while (lxr->get().type == TK_OPEN_BRACKET){
			ex = new function(ex, parse_fargs(sym_tbl));
		}
		return ex;
	}
	if (tk.is_literal()){
		if (tk.type == TK_STRING_LITERAL){
			return tcast ? new expr_literal(tk, new sym_const(new sym_pointer(prelude->get_type_specifier(token_names[TK_CHAR])))) : new expr_literal(tk);
		}
		string tname = tk.get_type_name();
		if (tname == "") throw 1;
		return tcast ? new expr_literal(tk, prelude->get_type_specifier(tname)) : new expr_literal(tk);
	}
	if (tk.type == TK_PLUS || tk.type == TK_MINUS || tk.type == TK_MUL || tk.type == TK_AND_BIT || tk.type == TK_NOT_BIT || tk.type == TK_INC || tk.type == TK_DEC){
		return new expr_prefix_unar_op(factor(sym_tbl) , tk);
	}
	if (tk.type == TK_SIZEOF){
		return new expr_prefix_unar_op(expression(sym_tbl, MIN_PRIORITY), tk);
	}
	return nullptr;
}

expr *parser::parse_expr(sym_table *sym_tbl){
	lxr->next();
	return expression(sym_tbl, MIN_PRIORITY);
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

void parser::parse_fparams(sym_table *lst, vector<string> *params){
	declar param;
	token tk = lxr->next(); /* skip open bracket '(' */
	while (tk.type != TK_CLOSE_BRACKET){
		if (tk.is_type_specifier()){
			param = parse_declare(lst);
			tk = lxr->get();
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
			params->push_back(param.get_name());
			lst->add_sym(new sym_var_param(param.get_name(), param.get_type()));
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

void parser::check_struct_member(symbol *member, string struct_tag, position pos){
	string s = typeid(*member).name();
	if (s != "class sym_struct")
		return;
	if (member->name == struct_tag)
		throw error(C3769, "\"" + struct_tag + "\" : a nested class cannot have the same name as the immediately enclosing class", lxr->pos);
	sym_table *table = (dynamic_cast<sym_struct *>(member))->table->prev;
	while (table->prev != nullptr){
		table = table->prev;
		if (table->global_exist(member->name)){
			sym_type *t = table->get_type_specifier(member->name);
			string str = typeid(*t).name();
			if (str == "class sym_struct"){
				throw error(C2020, member->name + ": struct redifinition", pos);
			}
		}
	}
}

symbol *parser::try_parse_struct_member_list(string &struct_tag, sym_table *sym_tbl){
	declar info;
	token tk = lxr->get();
	sym_table *slt = new sym_table(sym_tbl); /* struct local table */
	sym_type *t = new sym_struct(struct_tag, slt);
	sym_tbl->add_sym(t);
	if (tk.type == TK_OPEN_BRACE){
		tk = lxr->next(); /* skip open square bracket '{' */
		while (tk.type != TK_CLOSE_BRACE){
			symbol *sym = make_symbol(parse_declare(slt));
			slt->add_sym(sym);
			check_struct_member(sym, struct_tag, lxr->get().pos);
			tk = lxr->get();
			if (tk.type == TK_CLOSE_BRACE){
				break;
			} else if (tk.type == TK_SEMICOLON){
				tk = lxr->next();
			} else {
				string s = "\"" + tk.get_src() + "\" should be preceded by \";\"";
				throw syntax_error(C2144, s, tk.pos);
			}
		}
		if (tk.type == TK_CLOSE_BRACE && !slt->count_symbols())
			throw error(C2016, "C requires that a struct or union has at least one member", tk.pos);
	} else
		throw error(C2332, "struct: missing tag name", tk.pos);
	sym_tbl->del_sym(t);
	info.set_id(t);
	return make_symbol(info);
}

bool parser::is_expr_start(token tk, sym_table *sym_tbl){
	return tk.is_literal() || tk.is_operator() || sym_tbl->symbol_not_alias_exist(tk.get_src());
}

void parser::try_parse_statement(sym_table *sym_tbl, stmt_block *stmt_blck){
	token tk = lxr->get();
	if (is_expr_start(tk, sym_tbl)){
		//DO tests and corresponding call, but while it's comment
		if (sym_tbl->prev == nullptr){
			if (tk.is_literal())
				throw syntax_error(C2059, "\'constant\'", tk.pos);
			else if (tk.is_operator())
				throw syntax_error(C2059, tk.get_src(), tk.pos);
			else if (sym_tbl->symbol_not_alias_exist(tk.get_src())){
				//symbol *sym = sym_tbl->get_symbol(tk.get_src());
				//if (sym->type_eq("int") == false) DO throw only in case type_eq("int") == false and else it's OK..it's simplify declaration with type 'int'
				throw syntax_error(C2371, "\'" + tk.get_src() + "\': redefinition; different basic types", tk.pos);
			}
		}
		stmt_blck->push_back(new stmt_expr(expression(sym_tbl, MIN_PRIORITY)));
	} // else if
}

void parser::try_parse_statements_list(sym_table *sym_tbl, stmt_block *stmt_blck){
	token tk = lxr->next(); /* skip open brace '{' */
	while (tk.type != TK_CLOSE_BRACE){
		bool block = try_parse_block(sym_tbl, stmt_blck);
		try_parse_statement(sym_tbl, stmt_blck);
		bool func_def = try_parse_declarator(sym_tbl);
		tk = lxr->get();
		if (func_def || block) continue;
		check_semicolon();
		tk = lxr->get();
	}
	
	if (tk.type == TK_CLOSE_BRACE)
		lxr->next(); /* skip close brace '}' */
}

bool parser::try_parse_block(sym_table *sym_tbl_prev, stmt_block * stmt_prev_block){
	if (lxr->get().type != TK_OPEN_BRACE)
		return false;
	sym_table *sym_tbl = new sym_table(sym_tbl_prev);
	stmt_block *stmt_blck = new stmt_block(sym_tbl);
	try_parse_statements_list(sym_tbl, stmt_blck);
	stmt_prev_block->push_back(dynamic_cast<stmt *>(stmt_blck));
	return true;
}

stmt_block *parser::try_parse_body(sym_table *sym_tbl){
	if (!lxr->look_next_token(TK_OPEN_BRACE)){
		lxr->next();
		return nullptr;
	}
	lxr->next(); /* Current token is TK_OPEN_BRACE */
	stmt_block *stmt_blck = new stmt_block();
	try_parse_statements_list(sym_tbl, stmt_blck);
	return stmt_blck;
}

void parser::check_func_decl2errors(symbol **t, token tk){
	sym_function *cur_func = dynamic_cast<sym_function *>(*t);
	for (vector<sym_function *>::iterator it = table->functions.begin(); it != table->functions.end(); it++){
		if ((*it)->name == cur_func->name){
			if ((*it)->params == cur_func->params){
				if (equal((*it)->type, cur_func->type)){
					if ((*it)->block != nullptr && cur_func->block != nullptr){
						/* Necessary release print of all errors to test this error */
						throw error(C2084, "function \"" + cur_func->name + "\" already has a body", tk.pos);
					} else if ((*it)->block == nullptr && cur_func->block != nullptr){
						(*it)->block = cur_func->block;
						delete *t; *t = nullptr;
						break;
					} else {
						delete *t; *t = nullptr;
					}
				} else 
					throw error(C2556, cur_func->name + ": overloaded functions only differ by return type", tk.pos);
			}
		}
	}
}

void parser::check_decl2errors(sym_table *sym_tbl, symbol **t, token tk){
	string cur_type = typeid(**t).name();
	if (cur_type == "class sym_var" && sym_tbl->local_exist((*t)->name)){
		throw error(C2086, (*t)->type->name + " " + (*t)->name + ": redefinition", tk.pos);
	} else if (cur_type == "class sym_function"){
		check_func_decl2errors(t, tk);
	}
}

bool parser::try_parse_declarator(sym_table *sym_tbl){
	token tk = lxr->get();
	sym_type *stype = nullptr;
	bool func_def = false;
	if (tk.is_type_specifier()){
		declar dcl = parse_declare(sym_tbl);
		func_def = dcl.is_def();
		symbol *t = make_symbol(dcl);
		check_decl2errors(sym_tbl, &t, tk);
		if (t != nullptr){ 
			sym_tbl->add_sym(t);
			stype = t->type;
		}
	} else if (tk.is_storage_class_specifier() || tk.is_type_qualifier()){
		bool tconst = false, tdef = false;
		while (tk.is_type_qualifier() || tk.is_storage_class_specifier()){
			if (tdef && tk.is_storage_class_specifier()) throw error(C2159, "more than one storage class specified", tk.pos);
			tdef = tdef || tk.is_storage_class_specifier();
			tconst = tconst || tk.is_type_qualifier();
			tk = lxr->next();
		}
		table->add_sym(make_symbol(parse_declare(table, tdef, tconst)));
	}
	while (lxr->get().type == TK_COMMA && stype != nullptr){
		declar dcl = parse_declare(table);
		dcl.set_type(stype);
		table->add_sym(make_symbol(dcl));
	}
	return func_def;
}

declar parser::parse_declare(sym_table *sym_tbl){
	return parse_declare(sym_tbl, false, false);
}

declar parser::parse_declare(sym_table *sym_tbl, bool alias, bool constant){
	declar info;
	token tk = lxr->get();
	if (tk.is_type_specifier()){
		if (tk.type == TK_STRUCT){
			tk = lxr->next();
			string tag;
			if (tk.type == TK_ID){
				tag = tk.get_src();
				tk = lxr->next();
				if (tk.type == TK_ID){
					if (!sym_tbl->global_exist(tag) && !sym_tbl->local_exist(tag))
						throw error(C2079, "\"" + tk.get_src() + "\" uses undefined struct \"" + tag + "\"", tk.pos);
					info.set_type(sym_tbl->get_type_specifier(tag));
					info.rebuild(parse_dir_declare(sym_tbl, alias, false));
					info.set_name(tk.get_src());
					return info;
				}
			}
			symbol *t = try_parse_struct_member_list(tag, sym_tbl);
			string::iterator it = lxr->it;
			if (lxr->look_next_token(TK_SEMICOLON)){
				lxr->next();
				info.set_id(t);
				return info;
			}
			sym_tbl->add_sym(t);
			info.set_type(dynamic_cast<sym_type *>(t));
		} else {
			info.set_type(prelude->get_type_specifier(tk.get_src()));
		}
		tk = lxr->next();
		while (tk.is_storage_class_specifier() || tk.is_type_qualifier()){
			if (alias && tk.is_storage_class_specifier()) throw error(C2159, "more than one storage class specified", tk.pos);
			alias = (!alias) ? tk.is_storage_class_specifier() : alias;
			constant = (!constant) ? tk.is_type_qualifier() : constant;
			tk = lxr->next();
		}
		if (constant)
			info.reset_type(new sym_const(info.get_type()));
	} else if (sym_tbl->type_alias_exist(tk.get_src())){
		info.set_type(sym_tbl->get_type_by_synonym(tk.get_src()));
		tk = lxr->next();
	} else {
		tk = lxr->next();
	}
	if (alias && tk.is_storage_class_specifier()) throw error(C2159, "more than one storage class specified", tk.pos);
	while (tk.type == TK_MUL){
		info.reset_type(new sym_pointer(info.get_type()));
		tk = lxr->next();
		if (tk.type == TK_CONST){
			info.reset_type(new sym_const(info.get_type()));
			tk = lxr->next();
		}
		if (alias && tk.is_storage_class_specifier()) throw error(C2159, "more than one storage class specified", tk.pos);
	}
	info.rebuild(parse_dir_declare(sym_tbl, alias, constant));
	tk = lxr->get();
	if (tk.type == TK_TYPEDEF || tk.type == TK_CONST){
		throw error(C2143, "missing \";\" before \"" + tk.get_src() + "\"", tk.pos);
	}
	return info;
}

declar parser::parse_dir_declare(sym_table *sym_tbl, bool tdef, bool tconst){
	declar info = declar();
	bool dir_dcl= false;
	token tk = lxr->get();
	if (tk.type == TK_OPEN_BRACKET){
		info.rebuild(parse_declare(sym_tbl, tdef, false));
		tk = lxr->get();
		if (tk.type != TK_CLOSE_BRACKET)
			throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
		dir_dcl = true;
	} else if (tk.type == TK_ID){
		info.name = tk.get_src();
	}
	tk = lxr->next();
	if (tk.type != TK_OPEN_BRACKET && tk.type != TK_OPEN_SQUARE_BRACKET){
		if (tdef){
			info.set_id(new sym_alias(info.type));
			info.set_name(info.name);
		} else {
			info.set_id(new sym_var(info.name));
		}
	} else {
		while ((tk.type == TK_OPEN_BRACKET) || (tk.type == TK_OPEN_SQUARE_BRACKET)){
			if (tk.type == TK_OPEN_SQUARE_BRACKET){
				if (info.check_id(nullptr)){
					info.set_id(new sym_array(parse_size_of_array()));
					info.set_name(info.name);
				} else {
					if (dir_dcl)
						info.set_back_type(new sym_array(parse_size_of_array()));
					else
						info.set_type(new sym_array(parse_size_of_array()));		
				}
			} else if (tk.type == TK_OPEN_BRACKET){
				sym_table *st = new sym_table(sym_tbl);
				vector<string> params;
				parse_fparams(st, &params);
				if (info.check_id(nullptr)){
					sym_function * f = new sym_function(info.name, st, try_parse_body(st), params);
					info.set_id(f);
					info.def = f->block != nullptr;
					tk = lxr->get();
					continue;
				} else {
					string s = (info.get_type() == nullptr) ? typeid(*info.get_id()).name() : typeid(*info.get_type()).name();
					if (s == "class sym_array"){
						throw error(C2092, "element type of array cannot be function", lxr->pos);
					} else if (s == "class sym_function" || s == "class sym_func_type"){
						throw error(C2091, "function returns function", lxr->pos);
					}
					if (dir_dcl)
						info.set_back_type(new sym_func_type(nullptr, st, params));
					else
						info.set_type(new sym_func_type(nullptr, st, params));
				}
			}
			tk = lxr->next();
		}
	}
	return info;
}

void parser::check_semicolon(){
	token tk = lxr->get();
	if (tk.type == TK_SEMICOLON){
		tk = lxr->next(); 
	} else
		throw syntax_error(C2143, "missing \";\" before \"" + tk.get_src() + "\"", tk.pos);
}

void parser::parse(ostream &os){
	token tk = lxr->next();
	sym_type *stype = nullptr;
	table->prev = nullptr;
	while (tk.type != NOT_TK){
		try_parse_statement(table, nullptr); 
		bool func_def = try_parse_declarator(table);
		tk = lxr->get();
		if (func_def){
			if (tk.type == TK_SEMICOLON)
				tk = lxr->next();
			continue;
		}
		check_semicolon();
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