#include "sym_table.h"
#include <map>

expr::expr(){ type = nullptr; }
expr_bin_op::expr_bin_op(expr *l, expr *r, token t): left(l), right(r), expr_bin_op::expr() {
	type = l->type; /* Assign type of one of two operand, because type casting happens in the function which call this */
	tk = t; 
}
expr_bin_op::expr_bin_op(expr *l, expr *r, string s): left(l), right(r), expr_bin_op::expr(){ op = s; }
expr_prefix_unar_op::expr_prefix_unar_op(expr *e, token t): ex(e), expr_bin_op::expr(){ 
	type = e->type;
	tk = t; 
}

expr_postfix_unar_op::expr_postfix_unar_op(expr *e, token t): ex(e), expr_bin_op::expr(){ 
	type = e->type;
	tk = t; 
}

expr_literal::expr_literal(token t): expr_bin_op::expr(){ tk = t; }
expr_global_var::expr_global_var(string gname, sym_type *var_type): expr_bin_op::expr(){ name = gname; type = var_type; }
expr_local_var::expr_local_var(string lname, sym_type *var_type): expr_bin_op::expr(){ name = lname; type = var_type; }
expr_local_var::expr_local_var(symbol *local_sym){ sym = local_sym; type = sym->get_type(); name = sym->name; }
expr_tern_op::expr_tern_op(expr *l, expr *m, expr *r, string s): left(l), middle(m), right(r), expr_bin_op::expr(){ op = s; };
expr_function::expr_function(expr *id, const vector<expr *> &args): fid(id), expr_bin_op::expr(){ fargs = args; };
struct_access::struct_access(expr *l, expr *struct_field, token t): field(struct_field), left(l), expr_bin_op::expr(){ tk = t; };

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

void expr_prefix_unar_op::print(ostream &os, int level){
	ex->print(os, level + 1);
	print_level(os, level);
	os << tk.get_src() << endl;
}

void expr_postfix_unar_op::print(ostream &os, int level){
	print_level(os, level);
	os << tk.get_src() << endl;
	ex->print(os, level + 1);
}

void expr_literal::print(ostream &os, int level){
	print_level(os, level);
	os << tk.get_src() << endl;
}

void expr_global_var::print(ostream &os, int level){
	print_level(os, level);
	os << name << endl;
}

void expr_local_var::print(ostream &os, int level){
	print_level(os, level);
	os << name << endl;
}

void expr_tern_op::print(ostream &os, int level){
	right->print(os, level + 1);
	middle->print(os, level + 1);
	print_level(os, level);
	os << "?:" << endl;
	left->print(os, level + 1);
}

void expr_function::print(ostream &os, int level){
	for (int i = fargs.size() - 1; i >= 0; i--){
		fargs[i]->print(os, level + 1);
	}
	print_level(os, level);
	os << "()" <<endl;
	fid->print(os, level + 1);
}

void struct_access::print(ostream &os, int level){
	field->print(os, level + 1);
	print_level(os, level);
	os << tk.get_src() << endl;
	left->print(os, level + 1);
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

/*-----------------------------------------------EXPR::GENERATE------------------------------------------------*/

void expr_bin_op::generate_simple_bin_op(asm_cmd_list *cmds, token_t tk_t){
	left->generate(cmds);
	right->generate(cmds);
	cmds->add(POP, EBX);
	cmds->add(POP, EAX);
	if (tk_t == TK_PLUS){
		cmds->add(ADD, EAX, EBX);
	} else if (tk_t == TK_MINUS){
		cmds->add(SUB, EAX, EBX);
	} else if (tk_t == TK_MUL){
		cmds->add(IMUL, EAX, EBX);
	} else if (tk_t == TK_DIV){
		cmds->add(XOR, EDX, EDX);
		cmds->add(IDIV, EBX);
	} else if (tk_t == TK_MOD){
		cmds->add(XOR, EDX, EDX);
		cmds->add(IDIV, EBX);
		cmds->add(MOV, EAX, EDX);
	} else if (tk_t == TK_XOR_BIT){
		cmds->add(XOR, EAX, EBX);
	} else if (tk_t == TK_OR_BIT){
		cmds->add(OR, EAX, EBX);
	} else if (tk_t == TK_AND_BIT){
		cmds->add(AND, EAX, EBX);
	} else if (tk_t == TK_SHL){
		cmds->add(SHL, EAX, right->tk.get_src());
	} else if (tk_t == TK_SHR){
		cmds->add(SHR, EAX, right->tk.get_src());
	}
}

void expr_bin_op::generate_rel_bin_op(asm_cmd_list *cmds){
	left->generate(cmds);
	right->generate(cmds);
	cmds->add(POP, EBX);
	cmds->add(POP, EAX);
	cmds->add(CMP, EAX, EBX);
	string label1 = cmds->get_label_name();
	string label2 = cmds->get_label_name();
	switch (tk.get_type()){
		case TK_EQ: { cmds->add(JE, label1);	break; }
		case TK_GE: { cmds->add(JGE, label1);	break; }
		case TK_LE: { cmds->add(JLE, label1);	break; }
		case TK_GT: { cmds->add(JG, label1);	break; }
		case TK_LT: { cmds->add(JL, label1);	break; }
		case TK_NE: { cmds->add(JNE, label1);	break; }
	}
	cmds->add(PUSH, string("0"));
	cmds->add(JMP, label2);
	cmds->add_label(label1);
	cmds->add(PUSH, string("1"));
	cmds->add_label(label2);
}

void expr_bin_op::generate_assign_bin_op(asm_cmd_list *cmds){
	left->generate_addr(cmds);
	switch (tk.get_type()){
		case TK_ASSIGN:			{ right->generate(cmds); cmds->add(POP, EAX);	break; }
		case TK_PLUS_ASSIGN:	{ generate_simple_bin_op(cmds, TK_PLUS);		break; }
		case TK_MINUS_ASSIGN:	{ generate_simple_bin_op(cmds, TK_MINUS);		break; }
		case TK_MUL_ASSIGN:		{ generate_simple_bin_op(cmds, TK_MUL);			break; }
		case TK_DIV_ASSIGN:		{ generate_simple_bin_op(cmds, TK_DIV);			break; }
		case TK_MOD_ASSIGN:		{ generate_simple_bin_op(cmds, TK_MOD);			break; }
		case TK_XOR_ASSIGN:		{ generate_simple_bin_op(cmds, TK_XOR_BIT);		break; }
		case TK_OR_ASSIGN:		{ generate_simple_bin_op(cmds, TK_OR_BIT);		break; }
		case TK_AND_ASSIGN:		{ generate_simple_bin_op(cmds, TK_AND_BIT);		break; }
		case TK_SHL_ASSIGN:		{ generate_simple_bin_op(cmds, TK_SHL);			break; }
		case TK_SHR_ASSIGN:		{ generate_simple_bin_op(cmds, TK_SHR);			break; }
	}
	cmds->add(POP, EBX);
	cmds->add_assign(MOV, EBX, EAX);
}

void expr_bin_op::generate(asm_cmd_list *cmds){
	if (tk.is_assign_op()){
		generate_assign_bin_op(cmds);
	} else if (tk.is_rel_bin_op()) {
		generate_rel_bin_op(cmds);
		return;
	} else {
		generate_simple_bin_op(cmds, tk.get_type());
	}
	cmds->add(PUSH, EAX);
}

void expr_prefix_unar_op::generate(asm_cmd_list *cmds){
	if (tk == TK_INC || tk == TK_DEC){
		ex->generate_addr(cmds);
		ex->generate(cmds);
		cmds->add(POP, EBX);
		cmds->add(POP, EAX);
		if (tk == TK_INC)
			cmds->add(INC, EBX);
		else if (tk == TK_DEC)
			cmds->add(DEC, EBX);
		cmds->add_assign(MOV, EAX, EBX);
		cmds->add(MOV, EAX, EBX);
	} else if (tk == TK_AND_BIT){
		ex->generate_addr(cmds);
		return;
	} else if (tk == TK_MUL){
		ex->generate(cmds);
		cmds->add(POP, EAX);
		cmds->add_deref(PUSH, EAX);
		return;
	}
	cmds->add(PUSH, EAX);
}

void expr_prefix_unar_op::generate_addr(asm_cmd_list *cmds){
	if (tk == TK_MUL){
		ex->generate_addr(cmds);
		cmds->add(POP, EAX);
		cmds->add_deref(PUSH, EAX);
	}
}

void expr_postfix_unar_op::generate(asm_cmd_list *cmds){
	ex->generate_addr(cmds);
	ex->generate(cmds);
	cmds->add(POP, EBX);
	cmds->add(POP, EAX);
	cmds->add(PUSH, EBX);
	if (tk == TK_INC)
		cmds->add(INC, EBX);
	else if (tk == TK_DEC)
		cmds->add(DEC, EBX);
	cmds->add_assign(MOV, EAX, EBX);
}

void expr_literal::generate(asm_cmd_list *cmds){
	if (tk == TK_STRING_LITERAL){
		cmds->add_offset(PUSH, "STR_LITERAL(\'" + tk.get_src() + "\')");
	} else {
		cmds->add(PUSH, tk.get_src());
	}
}

void expr_global_var::generate(asm_cmd_list *cmds){
	cmds->add(PUSH, name + '_');	
}

void expr_global_var::generate_addr(asm_cmd_list * cmds){
	cmds->add_offset(PUSH, name + '_');
}

void expr_local_var::generate(asm_cmd_list *cmds){
	if (typeid(*sym) == typeid(sym_var_param)){
		int offset = dynamic_cast<sym_var_param *>(sym)->offset;
		cmds->add_deref(PUSH, EBP, offset);
	} else {
		generate_addr(cmds);
		cmds->add(POP, EAX);
		cmds->add_deref(PUSH, EAX);
	}
}

void expr_local_var::generate_addr(asm_cmd_list *cmds){
	int offset = 0;
	if (typeid(*sym) == typeid(sym_var_param)){
		sym_var_param *svp  = dynamic_cast<sym_var_param *>(sym);
		offset = svp->offset + svp->type->get_size();
	} else if (typeid(*sym) == typeid(sym_var)){
		sym_var *sv  = dynamic_cast<sym_var *>(sym);
		offset = sv->offset + sv->type->get_size();
		cmds->add(MOV, EAX, EBP);
		cmds->add(SUB, EAX, to_string(offset));
	}
	
	cmds->add(PUSH, EAX);
}

void expr_function::generate(asm_cmd_list *cmds){
	for (auto it = fargs.begin(); it != fargs.end(); it++)
		(*it)->generate(cmds);
	fid->generate_addr(cmds);
	cmds->add(POP, EAX);
	cmds->add(CALL, EAX);
	cmds->add(PUSH, EAX);
}

/*-------------------------------------------------Type casting-------------------------------------------------*/

bool expr::of_ctype(string s){
	return type->name == s;
}

expr_literal::expr_literal(token t, sym_type *st){
	tk = t, type = st;
};

expr_cast2type::expr_cast2type(string s, expr *e, sym_table *st): ex(e) { op = s; type = st->get_type_specifier(s); }

expr_cast2type::expr_cast2type(sym_type *new_type, expr *e): ex(e) {
	op  = new_type->get_type_str_name();
	type = new_type;
}

void expr_cast2type::print(ostream &os, int level){
	ex->print(os, level + 1);
	print_level(os, level);
	os << op << endl;
}
