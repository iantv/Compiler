#include "statements.h"

void stmt::print_level(ostream &os, int level){
	while (level){
		os << "\t";
		level--;
	}
}

stmt_expr::stmt_expr(expr * ex){
	e = ex;
	type = STMT_EXPR;
}

void stmt_expr::print(ostream &os, int level){
	e->print_level(os, level);
	os << "expression:\n";
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
		os << (*table);
	}
	for (int i = 0; i < (int)stmt_list.size(); i++){
		stmt_list[i]->print(os, level);
	}
}