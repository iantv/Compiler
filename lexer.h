#ifndef LEXER
#define LEXER 

#include <fstream>
#include <string>
#define FIRST_KWD 0

enum token_t{
	/* Tokens of reserved words */
	TK_DOUBLE = FIRST_KWD, TK_INT, TK_STRUCT,
	TK_BREAK, TK_ELSE, TK_LONG, TK_SWITCH,
	TK_CASE, TK_ENUM, TK_REGISTER, TK_TYPEDEF, 
	TK_CHAR, TK_RETURN, TK_UNION,
	TK_CONST, TK_FLOAT, TK_SHORT, TK_UNSIGNED,
	TK_CONTINUE, TK_FOR, TK_SIGNED, TK_VOID,
	TK_DEFAULT, TK_SIZEOF, TK_DO, TK_IF, TK_WHILE, 
	/* Tokens of statements */
	TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_MOD, TK_XOR_BIT, TK_OR_BIT, TK_AND_BIT, TK_NOT_BIT, TK_SHL, TK_SHR,
	TK_ASSIGN, TK_PLUS_ASSIGN, TK_MINUS_ASSIGN, TK_MUL_ASSIGN, TK_DIV_ASSIGN,
	TK_MOD_ASSIGN, TK_XOR_ASSIGN, TK_OR_ASSIGN, TK_AND_ASSIGN, TK_SHL_ASSIGN, TK_SHR_ASSIGN, 
	TK_INC, TK_DEC, TK_XOR_LOG, TK_OR_LOG, TK_AND_LOG, TK_PTROP, 
	TK_LT, TK_GT, TK_NOT, TK_EQ, TK_LE, TK_GE, TK_NE,

	TK_QUESTION, TK_COLON, TK_COMMA, TK_SEMICOLON, TK_POINT,
	TK_OPEN_SQUARE_BRACKET, TK_CLOSE_SQUARE_BRACKET,
	TK_OPEN_BRACKET, TK_CLOSE_BRACKET, TK_OPEN_BRACE, TK_CLOSE_BRACE, 
	/* Identificator and literals */
	TK_ID, TK_INT_VAL, TK_DOUBLE_VAL, TK_CHAR_VAL, TK_STRING_LITERAL,
	/* Other tokens */
	NOT_TK
};
#define LAST_KWD (TK_WHILE - FIRST_KWD)

static const char * const token_names[] = {
	"double", "int", "struct",
	"break", "else", "long", "switch",
	"case", "enum", "register", "typedef",
	"char", "return", "union",
	"const", "float", "short", "unsigned",
	"continue", "for", "signed", "void",
	"default", "sizeof", "do", "if", "while", 
	
	"+", "-", "*", "/", "%", "^", "|", "&", "~", "<<", ">>", 
	"=", "+=", "-=", "*=", "/=", "%=", "^=", "|=", "&=", "<<=", ">>=",
	"++", "--", "^^", "||", "&&", "->",
	"<", ">", "!", "==", "<=", ">=", "!=",
	"?", ":", ",", ";", ".", "[", "]", "(", ")", "{", "}",

	"identifier", "int_val", "dbl_val", "chr_val", "str_lit", ""
};

using namespace std;

struct position{
	int row, col;
	position(int r, int c){ row = r; col = c; };
	position(){ row = col = 0; };
	position(const position &p){ row = p.row; col = p.col; };
	bool operator==(const position &p){ return row == p.row && col == p.col; };
};

class token{
	position pos;
	token_t type;
	string src;
public:
	friend class parser;
	friend class lexer;
	friend class tests;
	token(const token& tk){ pos = tk.pos; type = tk.type; src.assign(tk.src); };
	token(position tk_pos, token_t tk_type);
	token(): pos(position(0, 0)), type(NOT_TK), src(""){};
	const string &get_src(){ return src; };
	string set_src(const char *s){ src = s; return src; }
	token_t get_token_type() { return type; };
	bool is_operator();
	friend ostream &operator<<(ostream &os, const token tk);
	bool is_type_specifier();
	bool is_type_qualifier();
	bool is_storage_class_specifier();
	bool is_literal();
	string get_type_name();
	bool operator==(token &tk){ return pos == tk.pos && type == tk.type && src == tk.get_src(); };
	bool is_assign_op();
};

class lexer{
	ifstream fin;
	string s;
	string::iterator it;
	position pos;
	token tk;
	void scan_new_line();
	bool look_forward(int n, const char c, ...); /* look next symbol after (*it) */
	token get_number();
	token try_parse_hex_number();
	token get_kwd_or_id();
	token get_literal(const char c);
	void skip_comment();
public:
	friend class parser;
	lexer(const char *filename);
	token next();
	token get();
	bool look_next_token(token_t); 
	bool token_can_exist();
	void skip_symbol();
};

#endif