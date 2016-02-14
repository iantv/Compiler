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
	table = nullptr;
	cond = nullptr;
	body = nullptr;
}

stmt_if::stmt_if(stmt_expr *ex, sym_table *sym_tbl){
	type = STMT_IF;
	table = sym_tbl;
	cond = ex;
	body = new stmt_block(sym_tbl);
}

void stmt_if::push_back(stmt *new_stmt){
	body->push_back(new_stmt);
}

void stmt_if::print(ostream &os, int level){
	print_level(os, level);
	os << "if: cond stmt" << endl;
	cond->print(os, level + 1);
	body->print(os, level + 1);
}