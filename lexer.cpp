#include <iostream>
#include "error.h"
#include <stdarg.h>

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
	getline(fin, new_s);
	s.assign(new_s);
	it = s.begin();
	++pos.row; pos.col = 1;
}

bool lexer::token_can_exist(){
	if (s.empty())
		scan_new_line();
	return  (it != s.end() || !fin.eof());
}

token lexer::try_parse_hex_number(){
	token t; t.pos = pos;
	t.src += '0'; skip_symbol(); t.src += *it; skip_symbol();
	while (!s.empty() && (it != s.end() && (
			(*it >= '0' && *it <= '9') ||
			(*it >= 'A' && *it <= 'F') ||
			(*it >= 'a' && *it <= 'f')))){
			t.src += *it;
			skip_symbol();
	}
	if (look_forward(6, '`', '@', '#', '¹', '$', '_')){
		throw syntax_error(C2143, "missing \";\" before \"{\"", t.pos);
	}
	if (!strcmp(t.src.c_str(), "0x") && !strcmp(t.src.c_str(), "0X")){
		throw hex_error(C2153, "hex constants must have at least one hex digit", t.pos);
	}
	t.type = TK_INT_VAL;
	return t;
}

token lexer::get_number(){
	token t;
	t.pos = pos;
	char illegal_digit = 0;
	if (*it == '0' && look_forward(2, 'x', 'X')){
			return try_parse_hex_number();
	}
	bool octal = *it == '0';
	while (!s.empty() && (it != s.end() && ((*it >= '0' && *it <= '9') || (*it == '.' && t.type != TK_DOUBLE_VAL)))){
		t.src += *it;
		if (*it == '.'){
			t.type = TK_DOUBLE_VAL;
			illegal_digit = 0;
		}
		if (octal && (*it == '8' || *it == '9')){
			illegal_digit = *it;
		}
		skip_symbol();
	}
	if (look_forward(6, '`', '@', '#', '¹', '$', '_')){
		throw syntax_error(C2143, "missing \";\" before \"{\"", t.pos);
	}
	t.type = (t.type == NOT_TK) ? TK_INT_VAL : t.type;
	if (illegal_digit && t.type == TK_INT_VAL){
		string msg("illegal digit ");
		msg += illegal_digit;
		throw octal_error(C2153, msg, t.pos);
	}
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

bool lexer::look_forward(int n, const char c, ...){
	va_list vl; char t = c;
	string::iterator local_it = it;
	if (local_it == s.end())
		return false;
	local_it++;
	va_start(vl, c);
	t = c;
	for (int i = 0; i < n && local_it != s.end(); i++){
		if (*local_it == t){
			va_end(vl);
			return true;
		}
		t = va_arg(vl, char);
	}
	va_end(vl);
	return false; /* throw */
}

void lexer::skip_comment(){
	skip_symbol();	/* skip / char */
	if (*it == '/'){ /* single-line comment */
		scan_new_line();
	} else if (*it == '*'){ /* multi-line comment */
		while (1){
			if (it != s.end() && (*it == '*' && look_forward(1, '/')))
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
	while (it == s.end() || (*it) == ' ' || (*it) == '\t'){
		if (it == s.end()){
			scan_new_line();
		} else
			skip_symbol(); /* skip spaces and tabs */
		if (s.empty() && fin.eof()){
			return tk = token();
		}
	}
	token_t tt = NOT_TK;
	if ((*it >= '0' && *it <= '9') || (*it == '.' && look_forward(10, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'))){ 
		return tk = get_number();
	} else if ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z')){
		return tk = get_kwd_or_id();
	} else if (*it == '\"'){
		return tk = get_literal('\"');
	} else if (*it == '\''){
		return tk = get_literal('\'');
	} else if (*it == '+' || *it == '-' || *it == '^' || *it == '|' || *it == '&' || *it == '=' || *it == '>' || *it == '<'){
		if (*it == '-' && look_forward(1, '>')){
			tt = TK_PTROP;
			tk = token(pos, tt);
			skip_symbol();
		} else if (look_forward(1, *it)){
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
		} else if(look_forward(1, '=')){
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
		if (look_forward(1, '=')){
			tk = token(pos, TK_MUL_ASSIGN);
			skip_symbol();
		} else {
			tk = token(pos, TK_MUL);
		}
		skip_symbol();
		return tk;
	} else if (*it == '/'){
		if (look_forward(2, '/', '*')){
			skip_comment();
			return next();
		} else if(look_forward(1, '=')){
			tk = token(pos, TK_DIV_ASSIGN);
			skip_symbol();
		} else {
			tk = token(pos, TK_DIV);
		}
		skip_symbol();
		return tk;
	} else if (*it == '%'){
		if(look_forward(1, '=')){
			tk = token(pos, TK_MOD_ASSIGN);
			skip_symbol();
		} else {
			tk = token(pos, TK_MOD);
		}
		skip_symbol();
		return tk;
	} else if (*it == '!'){
		if (look_forward(1, '=')){
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

bool token::is_operator(){
	return (type >= TK_PLUS && type <= TK_CLOSE_SQUARE_BRACKET) && type != TK_SEMICOLON;
}

bool token::is_type_specifier(){
	return type == TK_INT || type == TK_DOUBLE || type == TK_FLOAT || type == TK_CHAR || type == TK_VOID || type == TK_SHORT || type == TK_LONG || type == TK_SIGNED || type == TK_UNSIGNED || type == TK_STRUCT || type == TK_UNION || type == TK_ENUM;
}

bool token::is_type_qualifier(){
	return type == TK_CONST;
}

bool token::is_storage_class_specifier(){
	return type == TK_TYPEDEF;
}

bool token::is_literal(){
	return type == TK_INT_VAL || type == TK_DOUBLE_VAL || type == TK_CHAR_VAL || type == TK_STRING_LITERAL;
}

string token::get_type_name(){
	switch (type){
		case TK_CHAR_VAL:	return token_names[TK_CHAR];
		case TK_INT_VAL:	return token_names[TK_INT];
		case TK_DOUBLE_VAL:	return token_names[TK_DOUBLE];
		case TK_STRING_LITERAL: return "const char *";
		default:			return "";
	}
}

bool lexer::look_next_token(token_t tk_t){
	string s1 = s;
	string::iterator it1 = it;
	position pos1 = pos;
	token tk1 = tk;

	bool r = next().type == tk_t;
	s = s1; it = it1; pos = pos1; tk = tk1;
	return r;
}
