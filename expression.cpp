#include "expression.h"

expr_bin_op::expr_bin_op(expr *l, expr *r, token t): left(l), right(r){ tk = t; }
expr_bin_op::expr_bin_op(expr *l, expr *r, string s): left(l), right(r){ op = s; }
expr_unar_op::expr_unar_op(expr *e, token t): ex(e){ tk = t; }
expr_literal::expr_literal(token t){ tk = t; }
expr_var::expr_var(token t){ tk = t; }
expr_tern_op::expr_tern_op(expr *l, expr *m, expr *r, string s): left(l), middle(m), right(r){ op = s; };
function::function(expr *id, const vector<expr *> &args): fid(id) { fargs = args; };
structure::structure(string &struct_name, expr_var *struct_field): field(struct_field){ name = struct_name.c_str(); };

void expr::print_level(ostream &os, int level){
	while (level){
		os << '\t';
		level--;
	}
}

void expr_bin_op::print(ostream &os, int level){
	right->print(os, level + 1);
	print_level(os, level);
	os << ((op == "") ? tk.get_src() : op) << endl;
	left->print(os, level + 1);
}

void expr_unar_op::print(ostream &os, int level){
	ex->print(os, level + 1);
	print_level(os, level);
	os << tk.get_src() << endl;
}

void expr_literal::print(ostream &os, int level){
	print_level(os, level);
	os << tk.get_src() << endl;
}

void expr_var::print(ostream &os, int level){
	print_level(os, level);
	os << tk.get_src() << endl;
}

void expr_tern_op::print(ostream &os, int level){
	right->print(os, level + 1);
	print_level(os, level);
	os << "?:";
	middle->print(os, 1);
	print_level(os, level);
	//os << '?' << endl;
	left->print(os, level + 1);
}

void function::print(ostream &os, int level){
	for (int i = fargs.size() - 1; i >= 0; i--){
		fargs[i]->print(os, level + 1);
	}
	print_level(os, level);
	os << "()" <<endl;
	fid->print(os, level + 1);
}

void structure::print(ostream &os, int level){
	field->print(os, level + 1);
	print_level(os, level);
	os << "." << endl;
	print_level(os, level + 1);
	os << name << endl;
}

int get_priority(token tk, bool unar){
	switch (tk.get_token_type()){
		case TK_ID:
		case TK_INT_VAL:
		case TK_DOUBLE_VAL:
		case TK_CHAR_VAL:
		case TK_STRING_LITERAL:
		case TK_POINT:
		case TK_PTROP:		return 16;

		case TK_DEC:
		case TK_INC:
		case TK_SIZEOF:
		case TK_NOT_BIT:
		case TK_NOT:		return 15;

		case TK_MUL:	  { return (unar) ? 15 : 13; }

		case TK_DIV:
		case TK_MOD:		return 13;
		
		case TK_PLUS:
		case TK_MINUS:	  { return (unar) ? 15 : 12; }

		case TK_SHL:
		case TK_SHR:		return 11;
		
		case TK_GT:
		case TK_LT:
		case TK_GE:
		case TK_LE:			return 10;
		
		case TK_EQ:
		case TK_NE:			return 9;

		case TK_AND_BIT:  { return (unar) ? 15: 8; }

		case TK_XOR_BIT:	return 7;

		case TK_OR_BIT:		return 6;

		case TK_AND_LOG:	return 5;

		case TK_OR_LOG:		return 4;

		case TK_QUESTION:	return 3;

		case TK_ASSIGN:
		case TK_PLUS_ASSIGN:
		case TK_MINUS_ASSIGN:
		case TK_MUL_ASSIGN:
		case TK_DIV_ASSIGN:
		case TK_MOD_ASSIGN:
		case TK_SHR_ASSIGN:
		case TK_SHL_ASSIGN:
		case TK_AND_ASSIGN:
		case TK_XOR_ASSIGN:
		case TK_OR_ASSIGN:	return 2;

		case TK_COMMA:		return 1;
		default:			return -1;
	}
}