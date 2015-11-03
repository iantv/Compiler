#ifndef LEXER
#define LEXER 

#include <fstream>
#include <string>

#define FIRST_KWD 0
/*
	"|", "&", "<<", ">>",
	"=", "+=", "-=", "*=", "/=", "%=", "^=", "|=", "&=", 
	
	"<", ">", "!",
*/
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
	TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_MOD, TK_XOR, TK_OR, TK_AND, TK_SHL, TK_SHR,
	TK_ASSIGN, TK_PLUS_ASSIGN, TK_MINUS_ASSIGN, TK_MUL_ASSIGN, TK_DIV_ASSIGN,
	TK_MOD_ASSIGN, TK_XOR_ASSIGN, TK_OR_ASSIGN, TK_AND_ASSIGN, TK_SHL_ASSIGN, TK_SHR_ASSIGN, 
	TK_INC, TK_DEC, TK_XORXOR, TK_OROR, TK_ANDAND,
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
using namespace std;

struct position{
	int row, col;
	position(int r, int c){ row = r; col = c; };
	position(){ row = col = 0; };
	position(const position &p){ row = p.row; col = p.col; };
};

class token{
	position pos;
	token_t type;
	string src;
public:
	friend class parser;
	friend class lexer;
	token(const token& tk){ pos = tk.pos; type = tk.type; src.assign(tk.src); };
	token(position tk_pos, token_t tk_type);
	token(): pos(position(0, 0)), type(NOT_TK), src(""){};
	//bool is_operator();
	string get_src(){ return src; };
	void print();
};

class lexer{
	ifstream fin;
	string s;
	string::iterator it;
	position pos;
	token tk;
	void scan_new_line();
	bool look_forward(const char c);
	token get_number();
	token get_kwd_or_id();
	token get_literal(const char c);
	void skip_comment();
public:
	lexer(const char *filename);
	token next();
	token get();
	bool token_can_exist();
	void skip_symbol();
	void tk_print();
};

#endif