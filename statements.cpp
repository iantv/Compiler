#include "statements.h"

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
}

stmt_block::stmt_block(vector<stmt *> list){
	stmt_list = list;
	type = STMT_BLOCK;
}

void stmt_block::push_back(stmt *new_stmt){
	stmt_list.push_back(new_stmt);
}

void stmt_block::print(ostream &os, int level){
	for (int i = 0; i < stmt_list.size(); i++){
		stmt_list[i]->print(os, level);
	}
}