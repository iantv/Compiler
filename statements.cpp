#include "statements.h"
#include "parser.h"

void stmt::print_level(ostream &os, int level){
	while (level){
		os << "\t";
		level--;
	}
}

stmt_expr::stmt_expr(expr * ex, cond_t cond = NOT_COND){
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

stmt_block::stmt_block(){
	type = STMT_BLOCK;
	table = nullptr;
}

stmt_block::stmt_block(sym_table *sym_tbl){
	type = STMT_BLOCK;
	table = sym_tbl;
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
		os << "block:" << endl;
		table->print(os, level + 1);
	}
	for (int i = 0; i < (int)stmt_list.size(); i++){
		stmt_list[i]->print(os, level);
	}
}

stmt_if::stmt_if(){
	type = STMT_IF;
	table_if_true = nullptr;
	cond = nullptr;
	body_if_true = nullptr;
	body_if_false = nullptr;
}

stmt_if::stmt_if(stmt_expr *ex, sym_table *sym_tbl){
	type = STMT_IF;
	table_if_true = sym_tbl;
	table_if_false = nullptr;
	cond = ex;
	body_if_true = new stmt_block(sym_tbl);
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