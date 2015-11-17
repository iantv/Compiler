#include <iostream>
#include "lexer.h"

token::token(position tk_pos, token_t tk_type){
	pos = tk_pos;
	type = tk_type;
	src.assign(token_names[type]);
}

token lexer::get(){
	return tk;
}

ostream &operator<<(ostream &os, const token tk){
	if (tk.type != NOT_TK)
		os << tk.pos.row << "\t" << tk.pos.col << "\t" << tk.src.c_str() << "\t\t" << token_names[tk.type] << endl;
	return os;
}

void lexer::scan_new_line(){
	string new_s;
	if (fin.eof()){
	//	return;
	}
	getline(fin, new_s);
	//if (new_s.length() == 0) return;
	s.assign(new_s);
	it = s.begin();
	++pos.row; pos.col = 1;
}

bool lexer::token_can_exist(){
	if (s.empty())
		scan_new_line();
	return  (it != s.end() || !fin.eof());
}

token lexer::get_number(){
	token t;
	t.pos = pos;
	while (!s.empty() && (it != s.end() && ((*it >= '0' && *it <= '9') || *it == '.'))){ /* int or double value */
		t.src += *it;
		if (*it == '.') t.type = TK_DOUBLE_VAL;
		skip_symbol();
	}
	t.type = (t.type == NOT_TK) ? TK_INT_VAL : t.type;
	return t;
}

token lexer::get_kwd_or_id(){
	token t;
	t.pos = pos;
	while (it != s.end() && ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z') || (*it >= '0' && *it <= '9'))){
		t.src += *it;
		skip_symbol();
	}
	for (int i = FIRST_KWD; i <= LAST_KWD; ++i){
		if (t.src == token_names[i]){
			t.type = token_t(i);
		}
	}
	t.type = (t.type == NOT_TK) ? TK_ID : t.type;
	return t;
}

token lexer::get_literal(const char c){
	token t;
	if (c != '\'' && c != '\"')
		return t;
	t.pos = pos;
	skip_symbol(); /* skip the first char */
	while (*it != c){
		t.src += *it;
		skip_symbol();
	}
	skip_symbol();
	t.type = (c == '\"') ? TK_STRING_LITERAL : TK_CHAR_VAL;
	return t;
}

bool lexer::look_forward(const char c){
	string::iterator local_it = it;
	local_it++;
	if (local_it != s.end())
		return *local_it == c;
	return false; /* throw */
}

void lexer::skip_comment(){
	skip_symbol();	/* skip / char */
	if (*it == '/'){ /* single-line comment */
		scan_new_line();
	} else if (*it == '*'){ /* multi-line comment */
		while (1){
			if (it != s.end() && (*it == '*' && look_forward('/')))
				break;
			if (it == s.end())
				scan_new_line();
			else
				skip_symbol();
		}
		skip_symbol();
		skip_symbol();
	}
}

inline void lexer::skip_symbol(){
	it++; pos.col++;
}

token lexer::next(){
	while (it == s.end() || *it == ' ' || *it == '\t'){
		if (it == s.end()){
			scan_new_line();
		} else
			skip_symbol(); /* skip spaces and tabs */
		if (s.empty() && fin.eof()){
			return tk = token();
		}
	}
	
	token_t tt = NOT_TK;
	if (*it >= '0' && *it <= '9'){ 
		return tk = get_number();
	}/*
	if (size_t p = s.find("sizeof")){ //	DO sizeof
		if (p && p < s.length() - 6 && s.substr(p, 6) == "sizeof"){
			tk = token(pos, TK_SIZEOF);
			return tk;
		}
	} */
	if ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z')){
		return tk = get_kwd_or_id();
	} else if (*it == '\"'){
		return tk = get_literal('\"');
	} else if (*it == '\''){
		return tk = get_literal('\'');
	} else if (*it == '+' || *it == '-' || *it == '^' || *it == '|' || *it == '&' || *it == '=' || *it == '>' || *it == '<'){
		if (*it == '-' && look_forward('>')){
			tt = TK_PTROP;
			tk = token(pos, tt);
			skip_symbol();
		} else if (look_forward(*it)){
			switch (*it){
				case '+': { tt = TK_INC; break; }
				case '-': { tt = TK_DEC; break; }
				case '^': { tt = TK_XOR_LOG; break; }
				case '|': { tt = TK_OR_LOG; break; }
				case '&': { tt = TK_AND_LOG; break; }
				case '=': { tt = TK_EQ; break; }
				case '>': {
					tt = TK_SHR;
					if ((it + 2) != s.end() && (*(it + 2) == '=')){
						skip_symbol();
						tt = TK_SHR_ASSIGN;
						pos.col--; it--;
					}
					break; 	  
				}
				case '<': {
					tt = TK_SHL;
					if ((it + 2) != s.end() && (*(it + 2) == '=')){
						skip_symbol();
						tt = TK_SHL_ASSIGN;
						pos.col--; it--;
					}
					break;
				}
			}
			tk = token(pos,  tt);
			skip_symbol();
			if (tt == TK_SHR_ASSIGN || tt == TK_SHL_ASSIGN)
				skip_symbol();	/* skip the third char '=' */
		} else if(look_forward('=')){
			switch (*it){
				case '+': { tt = TK_PLUS_ASSIGN; break; }
				case '-': { tt = TK_MINUS_ASSIGN; break; }
				case '^': { tt = TK_XOR_ASSIGN; break; }
				case '|': { tt = TK_OR_ASSIGN; break; }
				case '&': { tt = TK_AND_ASSIGN; break; }
				case '>': { tt = TK_GE; break; }
				case '<': { tt = TK_LE; break; }
			}
			tk = token(pos,  tt);
			skip_symbol();
		} else {
			switch (*it){
				case '+': { tt = TK_PLUS; break; }
				case '-': { tt = TK_MINUS; break; }
				case '^': { tt = TK_XOR_BIT; break; }
				case '|': { tt = TK_OR_BIT; break; }
				case '&': { tt = TK_AND_BIT; break; }
				case '=': { tt = TK_ASSIGN; break; }
				case '>': { tt = TK_GT; break; }
				case '<': { tt = TK_LT; break; }
			}
			tk = token(pos, tt);
		}
		skip_symbol();
		return tk;
	} else if (*it == '~'){
		tk = token(pos, TK_NOT_BIT);
		skip_symbol();
		return tk;
	} else if (*it == '*'){
		if (look_forward('=')){
			tk = token(pos, TK_MUL_ASSIGN);
			skip_symbol();
		} else {
			tk = token(pos, TK_MUL);
		}
		skip_symbol();
		return tk;
	} else if (*it == '/'){
		if (look_forward('/') || look_forward('*')){
			skip_comment();
			return next();
		} else if(look_forward('=')){
			tk = token(pos, TK_DIV_ASSIGN);
			skip_symbol();
		} else {
			tk = token(pos, TK_DIV);
		}
		skip_symbol();
		return tk;
	} else if (*it == '%'){
		if(look_forward('=')){
			tk = token(pos, TK_MOD_ASSIGN);
			skip_symbol();
		} else {
			tk = token(pos, TK_MOD);
		}
		skip_symbol();
		return tk;
	} else if (*it == '!'){
		if (look_forward('=')){
			tk = token(pos, TK_NE);
			skip_symbol();
		} else{
			tk = token(pos, TK_NOT);
		}
		skip_symbol();
		return tk;
	} else if (*it == ',' || *it == ';' || *it == '.' || *it >= '[' || *it == ']' || *it == '(' || *it == ')' || *it == '{' || *it == '}' || *it == '?' || *it == ':'){
		switch (*it){
			case '?': { tt = TK_QUESTION; break; }
			case ':': { tt = TK_COLON; break; }
			case ',': { tt = TK_COMMA; break; }
			case ';': { tt = TK_SEMICOLON; break; }
			case '.': { tt = TK_POINT; break; }
			case '[': { tt = TK_OPEN_SQUARE_BRACKET; break; }
			case ']': { tt = TK_CLOSE_SQUARE_BRACKET; break; }
			case '(': { tt = TK_OPEN_BRACKET; break; }
			case ')': { tt = TK_CLOSE_BRACKET; break; }
			case '{': { tt = TK_OPEN_BRACE; break; }
			case '}': { tt = TK_CLOSE_BRACE; break; }
		}
		tk = token(pos, tt);
		skip_symbol();
		return tk;
	}
	return tk = token();
}

lexer::lexer(const char *filename): pos(position()){
	s = ""; tk = token();
	fin.open(filename, ios::in);
	scan_new_line();
}