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
parser::parser(lexer *l): lxr(l), table(new sym_table()) { 	init_prelude(); tcast = true; point_of_entry = false; }

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
	if (tk.is_assign_op()){
		if (ex1->of_ctype(token_names[TK_DOUBLE]))
			ex2 = new expr_cast2type(token_names[TK_DOUBLE], ex2, prelude);
		else if (ex1->of_ctype(token_names[TK_INT]))
			ex2 = new expr_cast2type(token_names[TK_INT], ex2, prelude);
		else if (ex1->of_ctype(token_names[TK_CHAR]))
			ex2 = new expr_cast2type(token_names[TK_CHAR], ex2, prelude);
	} else {
		if (ex1->of_ctype(token_names[TK_DOUBLE]) && (ex2->of_ctype(token_names[TK_INT]) || ex2->of_ctype(token_names[TK_CHAR]))){
			ex2 = new expr_cast2type(token_names[TK_DOUBLE], ex2, prelude);
		} else if ((ex1->of_ctype(token_names[TK_INT]) || ex1->of_ctype(token_names[TK_CHAR])) && ex2->of_ctype(token_names[TK_DOUBLE])){
			ex1 = new expr_cast2type(token_names[TK_DOUBLE], ex1, prelude);
		} else if (ex1->of_ctype(token_names[TK_INT]) && ex2->of_ctype(token_names[TK_CHAR])){
			ex2 = new expr_cast2type(token_names[TK_INT], ex2, prelude);
		} else if (ex1->of_ctype(token_names[TK_CHAR]) && ex2->of_ctype(token_names[TK_INT])){
			ex1 = new expr_cast2type(token_names[TK_INT], ex1, prelude);
		}
	}
	return new expr_bin_op(ex1, ex2, tk);;
}

expr *parser::try_cast2type(expr *ex, sym_type *type, sym_table *sym_tbl){
	string et = ex->type->get_type_str_name(), nt = type->get_type_str_name();
	if (et == nt) return ex;
	
	if (prelude->local_exist(et)){
		if (prelude->local_exist(nt)){
			ex = new expr_cast2type(nt, ex, prelude); /* if each type is from prelude table */
		} else if (sym_tbl->type_exists_by_real_typename(nt)){
			string s = typeid(*type).name();
			if (typeid(*type) != typeid(sym_pointer)){
				throw error(C2440, "cannot convert \"" + et + "\" to \"" + nt+ "\"", lxr->get().pos);
			}
			ex = new expr_cast2type(type, ex);
		} else {
			throw error(C2440, "cannot convert \"" + et + "\" to \"" + nt+ "\"", lxr->get().pos);
		}
	} else if (sym_tbl->type_exists_by_real_typename(et)){
		if (prelude->local_exist(nt)){
			if (typeid(*(ex->type)) != typeid(sym_pointer))
				throw error(C2440, "cannot convert \"" + et + "\" to \"" + nt+ "\"", lxr->get().pos);
			ex = new expr_cast2type(nt, ex, prelude);
		} else if (sym_tbl->type_exists_by_real_typename(nt)){
			string s = typeid(*type).name();
			if (typeid(*type) != typeid(sym_pointer) || typeid(*(ex->type)) != typeid(sym_pointer)){
				throw error(C2440, "cannot convert \"" + et + "\" to \"" + nt+ "\"", lxr->get().pos);
			}
			ex = new expr_cast2type(type, ex);
		} else {
			throw error(C2440, "cannot convert \"" + et + "\" to \"" + nt+ "\"", lxr->get().pos);
		}
	}
	return ex;
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
	symbol *sym = sym_tbl->get_symbol(tk.get_src());
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
	if (lxr->get().type == TK_CLOSE_BRACKET)
		lxr->next();
	else
		throw syntax_error(C2143, "missing ')' before '" + lxr->get().get_src() + "'", lxr->get().pos);
}

void parser::check_struct_member(symbol *member, string struct_tag, position pos){
	if (typeid(*member) != typeid(sym_struct))
		return;
	if (member->name == struct_tag)
		throw error(C3769, "\"" + struct_tag + "\" : a nested class cannot have the same name as the immediately enclosing class", lxr->pos);

	sym_table *table = (dynamic_cast<sym_struct *>(member))->table;
	while (table->prev != nullptr){
		table = table->prev;
		if (table->global_exist(member->name)){
			if (typeid(*table->get_type_specifier(member->name)).name() == typeid(sym_struct).name()){
				throw error(C2020, member->name + ": struct redifinition", pos);
			}
		}
	}
}

void parser::try_parse_struct_member_list(sym_struct *my_struct){
	token tk = lxr->get();
	if (tk.type == TK_OPEN_BRACE){
		tk = lxr->next(); /* skip open square bracket '{' */
		while (tk.type != TK_CLOSE_BRACE){
			declar dcl = parse_declare(my_struct->table);
			if (dcl.id == nullptr && dcl.type != nullptr){
				check_struct_member(dcl.type, my_struct->name, lxr->get().pos);
			}
			symbol *sym = make_symbol(dcl);
			if (sym == nullptr){
				check_semicolon();
				tk = lxr->get();
				continue;
			}
			if (typeid(*sym) == typeid(sym_struct))
				sym->type = dynamic_cast<sym_type *>(sym);
			check_struct_member(sym->type, my_struct->name, lxr->get().pos);
			if (typeid(*sym) != typeid(sym_struct))
				my_struct->table->add_sym(sym);
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
		if (tk.type == TK_CLOSE_BRACE && !my_struct->table->count_symbols())
			throw error(C2016, "C requires that a struct or union has at least one member", tk.pos);
	}
}

bool parser::is_expr_start(token tk, sym_table *sym_tbl){
	return tk.is_literal() || tk.is_operator() || sym_tbl->symbol_not_alias_exist(tk.get_src());
}

void parser::try_parse_stmt_body(sym_table *sym_tbl, stmt_block *sym_blck, sym_function *owner, bool loop = false){
	if (lxr->next().type == TK_OPEN_BRACE)
		try_parse_statements_list(sym_tbl, sym_blck, owner, loop);
	else 
		try_parse_statement(sym_tbl, sym_blck, owner, loop);
}

void parser::try_parse_if_stmt(sym_table *sym_tbl, stmt_block *stmt_blck, sym_function *owner, bool loop){
	if (lxr->next().type != TK_OPEN_BRACKET)
		throw syntax_error(C2059, lxr->get().get_src() + "; it requares openning bracket", lxr->get().pos);
	lxr->next();
	stmt_if *new_if = new stmt_if(new stmt_expr(expression(sym_tbl, MIN_PRIORITY), EXECUTION_COND), new sym_table(sym_tbl), owner);

	if (lxr->get().type != TK_CLOSE_BRACKET)
		throw syntax_error(C2059, lxr->get().get_src() + "; it requares closing bracket", lxr->get().pos);
	try_parse_stmt_body(new_if->body_if_true->table, new_if->body_if_true, owner, loop);
	if (lxr->get().type == TK_ELSE){
		new_if->body_if_false = new stmt_block(new sym_table(sym_tbl), owner);
		try_parse_stmt_body(new_if->body_if_false->table, new_if->body_if_false, owner);
	}
	stmt_blck->push_back(new_if);
}

void parser::try_parse_while_stmt(sym_table *sym_tbl, stmt_block *stmt_blck, sym_function *owner){
	if (lxr->next().type != TK_OPEN_BRACKET)
		throw syntax_error(C2059, lxr->get().get_src() + "; it requares openning bracket", lxr->get().pos);
	lxr->next();
	stmt_while *new_while = new stmt_while(new stmt_expr(expression(sym_tbl, MIN_PRIORITY), CONTINUED_COND), new sym_table (sym_tbl), owner);
	if (lxr->get().type != TK_CLOSE_BRACKET)
		throw syntax_error(C2059, lxr->get().get_src() + "; it requares closing bracket", lxr->get().pos);
	try_parse_stmt_body(new_while->body->table, new_while->body, owner, true);
	stmt_blck->push_back(new_while);
}

void parser::try_parse_for_stmt(sym_table *sym_tbl, stmt_block *stmt_blck, sym_function *owner){
	if (lxr->next().type != TK_OPEN_BRACKET)
		throw syntax_error(C2059, lxr->get().get_src() + "; it requares openning bracket", lxr->get().pos);
	lxr->next();
	sym_table *for_table = new sym_table(sym_tbl);
	stmt_expr *init = nullptr, *cond = nullptr, *step = nullptr;

	/*if (is_decl_start())
		try_parse_declarator(for_table, stmt_blck);*/
	if (is_expr_start(lxr->get(), sym_tbl)) init = new stmt_expr(expression(sym_tbl, MIN_PRIORITY), NOT_COND);
	check_semicolon();
	if (is_expr_start(lxr->get(), sym_tbl)) cond = new stmt_expr(expression(sym_tbl, MIN_PRIORITY), END_COND);
	check_semicolon();
	if (is_expr_start(lxr->get(), sym_tbl)) step = new stmt_expr(expression(sym_tbl, MIN_PRIORITY), NOT_COND);
	if (lxr->get().type != TK_CLOSE_BRACKET)
		throw syntax_error(C2059, lxr->get().get_src() + "; it requares closing bracket", lxr->get().pos);
	
	stmt_for *new_for = new stmt_for(init, cond, step, new sym_table(sym_tbl), owner);

	try_parse_stmt_body(new_for->body->table, new_for->body, owner, true);
	stmt_blck->push_back(new_for);
}

void parser::try_parse_printf_stmt(sym_table *sym_tbl, stmt_block *stmt_blck){
	if (lxr->next().type != TK_OPEN_BRACKET)
		throw syntax_error(C2059, lxr->get().get_src() + "; it requares openning bracket", lxr->get().pos);
	stmt_printf *new_print = new stmt_printf(parse_fargs(sym_tbl));
	stmt_blck->push_back(new_print);
}

void parser::try_parse_statement(sym_table *sym_tbl, stmt_block *stmt_blck, sym_function *owner, bool loop = false){
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
		stmt_blck->push_back(new stmt_expr(expression(sym_tbl, MIN_PRIORITY), NOT_COND));
		check_semicolon();
	} else if (tk.type == TK_IF) {
		try_parse_if_stmt(sym_tbl, stmt_blck, owner, loop);
	} else if (tk.type == TK_WHILE){
		try_parse_while_stmt(sym_tbl, stmt_blck, owner);
	} else if (tk.type == TK_FOR){
		try_parse_for_stmt(sym_tbl, stmt_blck, owner);
	} else if (is_decl_start(sym_tbl)){
		try_parse_declarator(sym_tbl, stmt_blck);
	} else if (tk.type == TK_BREAK){
		if (!loop)
			throw error(C2044, "illegal '" + tk.get_src() + "'", tk.pos);
		stmt_blck->push_back(new stmt_break());		
		lxr->next();
		check_semicolon();
	} else if (tk.type == TK_CONTINUE){
		if (!loop)
			throw error(C2044, "illegal '" + tk.get_src() + "'", tk.pos);
		stmt_blck->push_back(new stmt_continue());
		lxr->next();
		check_semicolon();
	} else if (tk.type == TK_RETURN){
		lxr->next();
		if (is_expr_start(lxr->get(), sym_tbl)){
			expr *ex = try_cast2type(expression(sym_tbl, MIN_PRIORITY), owner->type, sym_tbl);
			stmt_blck->push_back(new stmt_return(new stmt_expr(ex, NOT_COND)));
		} else {
			stmt_blck->push_back(new stmt_return(nullptr));
		}
		check_semicolon();
	} else if (tk.type == TK_PRINTF){
		try_parse_printf_stmt(sym_tbl, stmt_blck);
		check_semicolon();
	}
}	

bool parser::is_block_start(){
	return lxr->get().type == TK_OPEN_BRACE;	
}

bool parser::is_decl_start(sym_table *sym_tbl){
	token tk = lxr->get();
	if (sym_tbl->global_exist(tk.src) || sym_tbl->local_exist(tk.src)){
		symbol *t = sym_tbl->get_symbol(tk.src);
		string s = typeid(*t).name();
		if (typeid(*t) == typeid(sym_alias) || typeid(*t) == typeid(sym_array) || typeid(*t) == typeid(sym_pointer) || typeid(*t) == typeid(sym_func_type))
			return true;
	}
	return tk.is_storage_class_specifier() || tk.is_type_qualifier() || tk.is_type_specifier();
}

void parser::try_parse_statements_list(sym_table *sym_tbl, stmt_block *stmt_blck, sym_function *owner, bool loop = false){
	token tk = lxr->next(); /* skip open brace '{' */
	while (tk.type != TK_CLOSE_BRACE){
		if (tk.type == TK_SEMICOLON){
			lxr->next();
		} else if (is_block_start()){
			try_parse_block(sym_tbl, stmt_blck, owner, loop);
		} else if (is_decl_start(sym_tbl)){
			try_parse_declarator(sym_tbl, stmt_blck);
		} else {
			try_parse_statement(sym_tbl, stmt_blck, owner, loop);
		}
		tk = lxr->get();
	}
	
	if (tk.type == TK_CLOSE_BRACE)
		lxr->next(); /* skip close brace '}' */
}

void parser::try_parse_block(sym_table *sym_tbl_prev, stmt_block * stmt_prev_block, sym_function *owner, bool loop = false){
	if (lxr->get().type != TK_OPEN_BRACE)
		return;
	sym_table *sym_tbl = new sym_table(sym_tbl_prev);
	stmt_block *stmt_blck = new stmt_block(sym_tbl, owner);
	try_parse_statements_list(sym_tbl, stmt_blck, owner, loop);
	stmt_prev_block->push_back(stmt_blck);
}

stmt_block *parser::try_parse_body(sym_table *sym_tbl, sym_function *owner, bool loop = false){
	if (lxr->get().type != TK_OPEN_BRACE)
		return nullptr;
	stmt_block *stmt_blck = new stmt_block(sym_tbl, owner);
	if (global_init)
		stmt_blck->stmt_list = init_list;
	try_parse_statements_list(sym_tbl, stmt_blck, owner, loop);
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

void parser::check_struct_decl2errors(sym_table *sym_tbl, symbol **sym, token tk){
	sym_struct *struct1 = dynamic_cast<sym_struct *>(*sym);
	
	if (sym_tbl->local_exist(struct1->name)){
		sym_struct *struct2 = dynamic_cast<sym_struct *>(sym_tbl->get_symbol(struct1->name));
		if (struct1->table->local_is_empty()){
			if (struct2->table->local_is_empty()){
				delete *sym; *sym = nullptr;
			} else {
				delete *sym; *sym = nullptr;
			}
		} else {
			if (struct2->table->local_is_empty()){
				sym_tbl->del_sym(struct2);
			} else {
				throw error(C2011, "\'" + struct1->name + "\': \'struct\' redifinition", tk.pos);
			}
		}
	} else if (sym_tbl->global_exist(struct1->name)){
		sym_struct *struct2 = dynamic_cast<sym_struct *>(sym_tbl->get_symbol(struct1->name));
		if (struct1->table->local_is_empty()){
			if (struct2->table->local_is_empty()){
				//
			} else {
				delete *sym; *sym = nullptr;
			}
		} else {
			if (struct2->table->local_is_empty()){
				//
			} else {
				//
			}
		}
	}
}

sym_struct *parser::try_parse_struct_decl(sym_table *sym_tbl, bool alias, bool constant){
	token tk = lxr->next(); /* skip TK_STRUCT and next token assign to tk */
	sym_table *slt = new sym_table(sym_tbl); /* struct local table */
	sym_struct *t = new sym_struct("struct", slt);
	if (tk.type == TK_ID){ /* struct [name] { ... } || struct { ... }[var1][, [var2]]; || struct name {}; */
		t->name += " " + tk.get_src();
		if (lxr->look_next_token(TK_ID)){
			if (!sym_tbl->local_exist(t->name) && !sym_tbl->global_exist(t->name)){
				throw error(C2079, "\"" + lxr->next().get_src() + "\" uses undefined struct \"" + t->name + "\"", tk.pos );
			}
			sym_struct *temp = dynamic_cast<sym_struct *>(sym_tbl->get_symbol(t->name));
			if (temp->table->local_is_empty()){
				tk = lxr->next();
				throw error(C2079, "\"" + tk.get_src() + "\" uses undefined struct \"" + t->name + "\"", tk.pos );
			}
			return t;
		}
		if (lxr->look_next_token(TK_OPEN_BRACE))
			lxr->next();
	} else if (t->name == "struct"){
		t->name += " _" + to_string(id_name);
		id_name++;
	} 
	if (lxr->get().type != TK_OPEN_BRACE && alias){
		throw error(C2332, "struct: missing tag name", lxr->get().pos);
	}
	return t;
}

void parser::check_decl2errors(sym_table *sym_tbl, symbol **t, token tk){
	if (*t == nullptr)
		return;
	if (typeid(*(*t)) == typeid(sym_var)){
		if  (sym_tbl->local_exist((*t)->name))
			throw error(C2086, (*t)->type->name + " " + (*t)->name + ": redefinition", tk.pos);
	} else if (typeid(*(*t)) == typeid(sym_function)){
		check_func_decl2errors(t, tk);
	}
}

void parser::try_parse_init(symbol *sym, sym_table *sym_tbl, stmt_block *stmt_blck){
	if (lxr->get().type != TK_ASSIGN || typeid(*sym).name() != typeid(sym_var).name()) return;
	sym_var *t = dynamic_cast<sym_var *>(sym);
	token tk = lxr->get(); lxr->next();
	stmt * statement = new stmt_expr(new_expr_bin_op(new_expr_var(sym_tbl, t->var_token), expression(sym_tbl, 2), tk), NOT_COND);
	if (sym_tbl->prev == nullptr)
		init_list.push_back(statement);
	else
		stmt_blck->push_back(statement);
}

bool parser::try_parse_definition(symbol *t){
	if (lxr->get().type == TK_OPEN_BRACE){
		sym_function *f = dynamic_cast<sym_function *>(t);
		if (f->name == "main"){
			point_of_entry = true;
			global_init = true;
		}
		f->block = try_parse_body(f->table, f);
		global_init = false;
		return f->block != nullptr;
	}
	return false;
}

void parser::try_parse_declarator(sym_table *sym_tbl, stmt_block *stmt_blck = nullptr){
	token tk = lxr->get();
	sym_type *stype = nullptr;
	bool func_def = false;
	bool decl = false;
	if (tk.is_type_specifier() || (sym_tbl->global_exist(tk.src) || sym_tbl->local_exist(tk.src))){
		symbol *t = make_symbol(parse_declare(sym_tbl));
		func_def = try_parse_definition(t);
		check_decl2errors(sym_tbl, &t, tk);
		if (t != nullptr){
			sym_tbl->add_sym(t);
			try_parse_init(t, sym_tbl, stmt_blck);
			stype = t->type;
		}
		decl = true;
	} else if (tk.is_storage_class_specifier() || tk.is_type_qualifier()){
		bool tconst = false, tdef = false;
		while (tk.is_type_qualifier() || tk.is_storage_class_specifier()){
			if (tdef && tk.is_storage_class_specifier()) throw error(C2159, "more than one storage class specified", tk.pos);
			tdef = tdef || tk.is_storage_class_specifier();
			tconst = tconst || tk.is_type_qualifier();
			tk = lxr->next();
		}
		table->add_sym(make_symbol(parse_declare(table, tdef, tconst)));
		decl = true;
	}
	
	while (decl && lxr->get().type == TK_COMMA && stype != nullptr){
		declar dcl = parse_declare(table);
		dcl.set_type(stype);
		table->add_sym(make_symbol(dcl));
	}
	if (func_def == false)
		check_semicolon();
}

declar parser::parse_declare(sym_table *sym_tbl){
	return parse_declare(sym_tbl, false, false);
}

declar parser::parse_declare(sym_table *sym_tbl, bool alias, bool constant){
	declar info;
	token tk = lxr->get();
	if (tk.is_type_specifier()){
		if (tk.type == TK_STRUCT){
			sym_struct *t = try_parse_struct_decl(sym_tbl, alias, constant);
			try_parse_struct_member_list(t);
			symbol *sym = dynamic_cast<symbol *>(t);
			string s = t->name;
			check_struct_decl2errors(sym_tbl, &sym, lxr->get());
			info.set_type( sym == nullptr ? dynamic_cast<sym_type *>(sym_tbl->get_symbol(s)) : t);
			
			if (sym != nullptr) sym_tbl->add_sym(info.type);
			
			tk = lxr->next();
			if (tk.type == TK_SEMICOLON) /* do test: typedef struct;*/
				return info;	/* if it's type decalration */
		} else { 
			info.set_type(prelude->get_type_specifier(tk.get_src()));
			tk = lxr->next(); /* skip type from prelude */
		}
		while (tk.is_storage_class_specifier() || tk.is_type_qualifier()){
			if (alias && tk.is_storage_class_specifier()) throw error(C2159, "more than one storage class specified", tk.pos);
			alias = (!alias) ? tk.is_storage_class_specifier() : alias;
			constant = (!constant) ? tk.is_type_qualifier() : constant;
			tk = lxr->next();
		}
		if (constant) info.reset_type(new sym_const(info.get_type()));
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
	token tk_id;
	if (tk.type == TK_OPEN_BRACKET){
		info.rebuild(parse_declare(sym_tbl, tdef, false));
		tk = lxr->get();
		if (tk.type != TK_CLOSE_BRACKET)
			throw syntax_error(C2143, "missing \")\" before \";\"", lxr->pos);
		dir_dcl = true;
	} else if (tk.type == TK_ID){
		info.name = tk.get_src();
		tk_id = tk;
	}
	tk = lxr->next();
	if (tk.type != TK_OPEN_BRACKET && tk.type != TK_OPEN_SQUARE_BRACKET){
		if (tdef){
			info.set_id(new sym_alias(info.type));
			info.set_name(info.name);
		} else {
			info.set_id(new sym_var(info.name, nullptr, tk_id));
		}
	} else {
		while (((tk.type == TK_OPEN_BRACKET) || (tk.type == TK_OPEN_SQUARE_BRACKET)) && tk.type != NOT_TK){
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
				tk = lxr->next();
			} else if (tk.type == TK_OPEN_BRACKET){
				sym_table *st = new sym_table(sym_tbl);
				vector<string> params;
				parse_fparams(st, &params);
				if (info.check_id(nullptr)){
					sym_function *f = new sym_function(info.name, st, params, nullptr);
					info.set_id(f);
				} else {
					string s = (info.get_type() == nullptr) ? typeid(*info.get_id()).name() : typeid(*info.get_type()).name();
					if (s == typeid(sym_array).name()){
						throw error(C2092, "element type of array cannot be function", tk.pos);
					} else if (s == typeid(sym_function).name() || s == typeid(sym_func_type).name()){
						throw error(C2091, "function returns function", tk.pos);
					}
					if (dir_dcl)
						info.set_back_type(new sym_func_type(nullptr, st, params));
					else
						info.set_type(new sym_func_type(nullptr, st, params));
				}
			}
			tk = lxr->get();
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
	id_name = 0;
	table->prev = nullptr;
	while (tk.type != NOT_TK){
		if (tk.type == TK_SEMICOLON){
			tk = lxr->next();
		} else if (is_block_start()){
			try_parse_block(table, nullptr, nullptr);
		} else if (is_decl_start(table)){
			try_parse_declarator(table);
		} else {
			try_parse_statement(table, nullptr, nullptr);
		}
		tk = lxr->get();
	}
	if (!point_of_entry)
		throw error("entry point must be defined", position(0, 0));
}

void parser::init_prelude(){
	prelude = new sym_table();
	prelude->add_sym(new sym_type(token_names[TK_INT]));
	prelude->add_sym(new sym_type(token_names[TK_CHAR]));
	prelude->add_sym(new sym_type(token_names[TK_DOUBLE]));
	prelude->add_sym(new sym_type(token_names[TK_VOID]));
}

void parser::print_sym_table(ostream &os){
	table->print(os, 0);
}