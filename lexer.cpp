#include <iostream>
#include "lexer.h"

static const char * const token_names[] = {
	"double", "int", "struct",
	"break", "else", "long", "switch",
	"case", "enum", "register", "typedef",
	"char", "return", "union",
	"const", "float", "short", "unsigned",
	"continue", "for", "signed", "void",
	"default", "sizeof", "do", "if", "while",

	"+", "-", "*", "/", "%",
	"|", "&", "=", "<", ">", "!",
	"==", "<=", ">=", "!=",
	"?", ":", ",", ";", ".",
	"[", "]", "(", ")", "{", "}",
	"++", "--", "||", "&&",

	"identifier", "int_val", "dbl_val", "chr_val", "str_lit"
};


token::token(int col, int row, token_t tk_type, const string tk_src){
	pos = position_t(row, col);
	type = tk_type;
	src.assign(tk_src);
}

token lexer::get(){
	return tk;
}

void lexer::scan_new_line(){
	string new_s;
	if (fin.eof()) return;
	getline(fin, new_s);
	if (new_s.length() == 0) return;
	s.assign(new_s);
	it = s.begin();
	++pos.row; pos.col = 1;
}

bool lexer::token_can_exist(){
	if (s.empty()) scan_new_line();
	return  (it != s.end() || !fin.eof());
}

void lexer::print(){
	if (tk.type != NOT_TK)
		cout << tk.pos.row << "\t" << tk.pos.col << "\t" << tk.src.c_str() << "\t\t" << token_names[tk.type] << endl;
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
	it++; /* skip the first char */
	while (*it != c){
		t.src += *it;
		it++;
		pos.col++;
	}
	it++;
	t.type = (c == '\"') ? TK_STRING_LITERAL : TK_CHAR_VAL;
	return t;
}

bool lexer::look_forward(const char c){
	string::iterator local_it = it;
	local_it++;
	return *local_it == c;
}

void lexer::skip_comment(){
	if (*it == '/'){ /* single-line comment */
		scan_new_line();
	} else if (*it == '*'){ /* multi-line comment */
		while (*it != '*' || *(it + 1) != '/'){
			skip_symbol();
			if (it == s.end())
				scan_new_line();
		}
		it += 2; pos.col += 2;
	}
}

inline void lexer::skip_symbol(){
	it++; pos.col++;
}

token lexer::next(){
	while (it == s.end() || *it == ' ' || *it == '\t'){
		if (it == s.end())
			scan_new_line();
		else
			skip_symbol(); /* skip spaces and tabs */
	}
	if (*it >= '0' && *it <= '9'){ 
		return tk = get_number();
	} else if ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z')){
		return tk = get_kwd_or_id();
	} else if (*it == '\"'){
		return tk = get_literal('\"');
	} else if (*it == '\''){
		return tk = get_literal('\'');
	} else if (*it == '+' || *it == '-' || *it == '|' || *it == '&' || *it == '='){
		token_t tt = NOT_TK;
		if (look_forward(*it)){
			switch (*it){
				case '+': { tt = TK_INC; break; }
				case '-': { tt = TK_DEC; break; }
				case '|': { tt = TK_OROR; break; }
				case '&': { tt = TK_ANDAND; break; }
				case '=': { tt = TK_EQ; break; }
			}
			skip_symbol();
			tk = token(pos.col - 1, pos.row,  tt,  token_names[tt]);
		} else{
			switch (*it){
				case '+': { tt = TK_PLUS; break; }
				case '-': { tt = TK_MINUS; break; }
				case '|': { tt = TK_OR; break; }
				case '&': { tt = TK_AND; break; }
				case '=': { tt = TK_ASSIGN; break; }
			}
			tk = token(pos.col, pos.row, tt, token_names[tt]);
		}
		skip_symbol();
		return tk;
	} else if (*it == '*'){
		skip_symbol();
		return tk = token(pos.col - 1, pos.row, TK_MUL, "*");
	} else if (*it == '/'){
		if (look_forward(*it) || look_forward('*')){
			it++;
			skip_comment();
			return next();
		} else {
			skip_symbol();
			return tk = token(pos.col - 1, pos.row, TK_DIV, "/");
		}
	} else if (*it == '%'){
		skip_symbol();
		return tk = token(pos.col - 1, pos.row, TK_MOD, "%");
	} else if (*it == '>' || *it == '<' || *it == '!'){
		token_t tt = NOT_TK;
		if (look_forward('=')){
			switch (*it){
				case '<': { tt = TK_LE; break; }
				case '>': { tt = TK_GE; break; }
				case '!': { tt = TK_NE; break; }
			}
			tk = token(pos.col, pos.row,  tt,  token_names[tt]);
			skip_symbol();
		} else{
			switch (*it){
				case '<': { tt = TK_LT; break; }
				case '>': { tt = TK_GT; break; }
				case '!': { tt = TK_NOT; break; }
			}
			tk = token(pos.col, pos.row, tt, token_names[tt]);
		}
		skip_symbol();
		return tk;
	} else if (*it == ',' || *it == ';' || *it == '.' || *it >= '[' || *it == ']' || *it == '(' || *it == ')' || *it == '{' || *it == '}' || *it == '?' || *it == ':'){
		token_t tt;
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
		skip_symbol();
		return tk = token(pos.col - 1, pos.row, tt, token_names[tt]);
	}
}

lexer::lexer(const char *filename): pos(position_t()){
	fin.open(filename);
	scan_new_line();
}