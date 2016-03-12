#include "statements.h"
#include "parser.h"

void stmt::print_level(ostream &os, int level){
	while (level){
		os << "\t";
		level--;
	}
}

/*--------------------------------------------STMT_EXPR--------------------------------------------*/

stmt_expr::stmt_expr(expr *ex, cond_t cond = NOT_COND){
	e = ex;
	type = STMT_EXPR;
	cond_type = cond;
}

void stmt_expr::print(ostream &os, int level){
	print_level(os, level);
	if (cond_type == EXECUTION_COND) { os << "exec cond:"		<< endl; }
	if (cond_type == END_COND)		 { os << "end cond:"		<< endl; }
	if (cond_type == CONTINUED_COND) { os << "continued cond:"	<< endl; }
	if (cond_type == NOT_COND)		 { os << "expression:"		<< endl; }
	e->print(os, level + 1);
}

void stmt_expr::generate(asm_cmd_list *cmds){
	e->generate(cmds);
	cmds->add(POP, EAX);
}

/*--------------------------------------------STMT_BLOCK--------------------------------------------*/

stmt_block::stmt_block(){
	type = STMT_BLOCK;
	table = nullptr;
}

stmt_block::stmt_block(sym_table *sym_tbl, sym_function *func_owner){
	type = STMT_BLOCK;
	table = sym_tbl;
	owner = func_owner;
}

stmt_block::stmt_block(vector<stmt *> list){
	stmt_list = list;
	type = STMT_BLOCK;
}

void stmt_block::push_back(stmt *new_stmt){
	stmt_list.push_back(new_stmt);
}

void stmt_block::print(ostream &os, int level){
	if (table != nullptr){
		print_level(os, level);
		os << "{" << endl;
		table->print(os, level + 1);
	}
	for (int i = 0; i < (int)stmt_list.size(); i++){
		stmt_list[i]->print(os, level + 1);
	}
	print_level(os, level);
	os << "}" << endl;
}

void stmt_block::generate(asm_cmd_list *cmds){
	int nbyte = 0;
	for (auto it = table->symbols.begin(); it != table->symbols.end(); it++){
		nbyte += (*it).second->type->get_size();
	}
	cmds->add(SUB, ESP, to_string(nbyte));
	for (auto it = stmt_list.begin(); it != stmt_list.end(); it++){
		(*it)->generate(cmds);
		if (typeid(*it) == typeid(stmt_return)){
			cmds->add(JMP, owner->name + "_return");
		}
	}
}

/*--------------------------------------------STMT_IF--------------------------------------------*/

stmt_if::stmt_if(){
	type = STMT_IF;
	cond = nullptr;
	body_if_true = nullptr;
	body_if_false = nullptr;
}

stmt_if::stmt_if(stmt_expr *ex, sym_table *sym_tbl, sym_function *owner){
	type = STMT_IF;
	cond = ex;
	body_if_true = new stmt_block(sym_tbl, owner);
	body_if_false = nullptr;
}

void stmt_if::push_back_if_true(stmt *new_stmt){
	body_if_true->push_back(new_stmt);
}

void stmt_if::push_back_if_false(stmt *new_stmt){
	body_if_false->push_back(new_stmt);
}

void stmt_if::print(ostream &os, int level){
	print_level(os, level);
	os << "if: cond stmt" << endl;
	cond->print(os, level + 1);
	body_if_true->print(os, level + 1);
	if (body_if_false != nullptr){
		print_level(os, level);
		os << "else:" << endl;
		body_if_false->print(os, level + 1);
	}
}

/*--------------------------------------------STMT_WHILE--------------------------------------------*/

stmt_while::stmt_while(stmt_expr *ex, sym_table *sym_tbl, sym_function *owner){
	cond = ex;
	body = new stmt_block(sym_tbl, owner);
}

void stmt_while::push_back(stmt *new_stmt){
	body->push_back(new_stmt);
}

void stmt_while::print(ostream &os, int level){
	print_level(os, level);
	os << "while: loop" << endl;
	cond->print(os, level + 1);
	body->print(os, level + 1);
}

/*--------------------------------------------STMT_FOR--------------------------------------------*/

stmt_for::stmt_for(stmt_expr *initialization, stmt_expr *condition, stmt_expr *loop_step, sym_table *sym_tbl, sym_function *owner){
	init = initialization;
	cond = condition;
	step = loop_step;
	table = sym_tbl;
	body = new stmt_block(new sym_table(sym_tbl), owner);
}

void stmt_for::push_back(stmt *new_stmt){
	body->push_back(new_stmt);
}

void stmt_for::print(ostream &os, int level){
	print_level(os, level);
	os << "for: loop" << endl;
	if (init != nullptr){
		print_level(os, level + 1);
		os << "init:"  << endl;
		init->print(os, level + 2);
	}
	if (cond != nullptr)
		cond->print(os, level + 1);
	if (step != nullptr){
		print_level(os, level + 1);
		os << "step:" << endl;
		step->print(os, level + 2);
	}
	body->print(os, level + 1);
}

/*--------------------------------------------STMT_BREAK--------------------------------------------*/

void stmt_break::print(ostream &os, int level){
	print_level(os, level);
	os << "break" << endl;
}

/*--------------------------------------------STMT_CONTINUE--------------------------------------------*/

void stmt_continue::print(ostream &os, int level){
	print_level(os, level);
	os << "continue" << endl;
}

/*--------------------------------------------STMT_RETURN--------------------------------------------*/

stmt_return::stmt_return(stmt_expr *ret_expr){
	ex = ret_expr;
}

void stmt_return::print(ostream &os, int level){
	print_level(os, level);
	os << "return";
	if (ex != nullptr){
		os << ":" << endl;
		ex->print(os, level + 1);
	} else 
		os << endl;
}

void stmt_return::generate(asm_cmd_list *cmds){
	ex->generate(cmds);
}

/*--------------------------------------------STMT_PRINTF--------------------------------------------*/

stmt_printf::stmt_printf(vector<expr *> &fargs){
	if (fargs[0]->tk.get_type_name() != "const char *")
		throw 1; 
	for (int i = 0; i < (int)fargs.size(); i++)
		args.push_back(new stmt_expr(fargs[i]));
}

void stmt_printf::print(ostream &os, int level){
	print_level(os, level);
	os << "printf:" << endl;
	for (int i = 0; i < (int)args.size(); i++)
		args[i]->print(os, level + 2);
}

void stmt_printf::generate(asm_cmd_list *cmds){
	for (auto it = args.rbegin(); it != args.rend(); it++){
		(*it)->generate(cmds);
		cmds->add(PUSH, EAX);
	}
	cmds->add(CALL, "crt_printf");
}