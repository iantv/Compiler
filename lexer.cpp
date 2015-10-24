#include <iostream>
#include "lexer.h"

static const char * const TokenNames[] = {
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


Token::Token(int col, int row, TokenType tk_type, const string tk_src){
	pos = position(row, col);
	type = tk_type;
	src.assign(tk_src);
}

Token Lexer::Get(){
	return tk;
}

inline bool Lexer::isKeyWord(TokenType tk_type){
	return tk_type >= TK_DOUBLE && tk_type <= TK_WHILE;
}

inline bool Lexer::isOperator(TokenType tk_type){
	return tk_type >= TK_PLUS && tk_type <= TK_CLOSE_BRACE;
}

inline bool Lexer::isDefinedByUser(TokenType tk_type){
	return tk_type >= TK_ID && tk_type <= TK_CHAR_VAL;
}
void Lexer::ScanNewString(){
	string new_s;
	if (fin.eof()) return;
	getline(fin, new_s);
	if (new_s.length() == 0) return;
	s.assign(new_s); s += '\0'; /* because getline() return line without '\0' */
	it = s.begin();
	++pos.row; pos.col = 1;
}

bool Lexer::TokenCanExist(){
	return  *it != '\0' || !fin.eof();
}

void Lexer::Print(){
	if (tk.type != NOT_TK)
		cout << tk.pos.row << "\t" << tk.pos.col << "\t" << tk.src.c_str() << "\t\t" << TokenNames[tk.type] << endl;
}

Token Lexer::GetNumber(){
	Token t;
	t.pos = pos;
	while ((*it >= '0' && *it <= '9') || *it == '.'){ /* int or double value */
		t.src += *it;
		if (*it == '.') t.type = TK_DOUBLE_VAL;
		SkipSymbol();
	}
	t.type = (t.type == NOT_TK) ? TK_INT_VAL : t.type;
	return t;
}

Token Lexer::GetKeyWordOrIdent(){
	Token t;
	t.pos = pos;
	while ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z') || (*it >= '0' && *it <= '9')){
		t.src += *it;
		SkipSymbol();
	}
	for (int i = FIRST_KWD; i <= LAST_KWD; ++i){
		if (t.src == TokenNames[i]){
			t.type = TokenType(i);
		}
	}
	t.type = (t.type == NOT_TK) ? TK_ID : t.type;
	return t;
}

Token Lexer::GetLiteral(const char c){
	Token t;
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
	if (t.src.length() > 4 && c == '\'')
		throw 1;
	t.type = (c == '\"') ? TK_STRING_LITERAL : TK_CHAR_VAL;
	return t;
}

bool Lexer::LookForward(const char c){
	string::iterator local_it = it;
	local_it++;
	return *local_it == c;
}

void Lexer::SkipComment(){
	if (*it == '/'){ /* single-line comment */
		ScanNewString();
	} else if (*it == '*'){ /* multi-line comment */
		while (*it != '*' || *(it + 1) != '/'){
			SkipSymbol();
			if (*it == '\0')
				ScanNewString();
		}
		it += 2; pos.col += 2;
	}
}

inline void Lexer::SkipSymbol(){
	it++; pos.col++;
}

Token Lexer::Next(){
	if (*it == '\0')
		ScanNewString();	
	while(*it == ' ' || *it == '\t')
		SkipSymbol(); /* skip spaces and tabs */

	if (*it >= '0' && *it <= '9'){ 
		return tk = GetNumber();
	} else if ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z')){
		return tk = GetKeyWordOrIdent();
	} else if (*it == '\"'){
		return tk = GetLiteral('\"');
	} else if (*it == '\''){
		return tk = GetLiteral('\'');
	} else if (*it == '+' || *it == '-' || *it == '|' || *it == '&' || *it == '='){
		TokenType tt = NOT_TK;
		if (LookForward(*it)){
			switch (*it){
				case '+': { tt = TK_INC; break; }
				case '-': { tt = TK_DEC; break; }
				case '|': { tt = TK_OROR; break; }
				case '&': { tt = TK_ANDAND; break; }
				case '=': { tt = TK_EQ; break; }
			}
			SkipSymbol();
			tk = Token(pos.col - 1, pos.row,  tt,  TokenNames[tt]);
		} else{
			switch (*it){
				case '+': { tt = TK_PLUS; break; }
				case '-': { tt = TK_MINUS; break; }
				case '|': { tt = TK_OR; break; }
				case '&': { tt = TK_AND; break; }
				case '=': { tt = TK_ASSIGN; break; }
			}
			tk = Token(pos.col, pos.row, tt, TokenNames[tt]);
		}
		SkipSymbol();
		return tk;
	} else if (*it == '*'){
		SkipSymbol();
		return tk = Token(pos.col - 1, pos.row, TK_MUL, "*");
	} else if (*it == '/'){
		if (LookForward(*it) || LookForward('*')){
			it++;
			SkipComment();
			return Next();
		} else {
			SkipSymbol();
			return tk = Token(pos.col - 1, pos.row, TK_DIV, "/");
		}
	} else if (*it == '%'){
		SkipSymbol();
		return tk = Token(pos.col - 1, pos.row, TK_MOD, "%");
	} else if (*it == '>' || *it == '<' || *it == '!'){
		TokenType tt = NOT_TK;
		if (LookForward('=')){
			switch (*it){
				case '<': { tt = TK_LE; break; }
				case '>': { tt = TK_GE; break; }
				case '!': { tt = TK_NE; break; }
			}
			tk = Token(pos.col, pos.row,  tt,  TokenNames[tt]);
			SkipSymbol();
		} else{
			switch (*it){
				case '<': { tt = TK_LT; break; }
				case '>': { tt = TK_GT; break; }
				case '!': { tt = TK_NOT; break; }
			}
			tk = Token(pos.col, pos.row, tt, TokenNames[tt]);
		}
		SkipSymbol();
		return tk;
	} else if (*it == ',' || *it == ';' || *it == '.' || *it >= '[' || *it == ']' || *it == '(' || *it == ')' || *it == '{' || *it == '}' || *it == '?' || *it == ':'){
		TokenType tt;
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
		SkipSymbol();
		return tk = Token(pos.col - 1, pos.row, tt, TokenNames[tt]);
	}
}

Lexer::Lexer(const char *filename): pos(position()){
	fin.open(filename);
	ScanNewString();
}