#ifndef LEXER
#define LEXER 

#include <fstream>
#include <string>

#define FIRST_KWD 0

enum TokenType{
	/* Tokens of reserved words */
	TK_DOUBLE = FIRST_KWD, TK_INT, TK_STRUCT,
	TK_BREAK, TK_ELSE, TK_LONG, TK_SWITCH,
	TK_CASE, TK_ENUM, TK_REGISTER, TK_TYPEDEF, 
	TK_CHAR, TK_RETURN, TK_UNION,
	TK_CONST, TK_FLOAT, TK_SHORT, TK_UNSIGNED,
	TK_CONTINUE, TK_FOR, TK_SIGNED, TK_VOID,
	TK_DEFAULT, TK_SIZEOF, TK_DO, TK_IF, TK_WHILE, 
	/* Tokens of operators */
	TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_MOD,
	TK_OR, TK_AND, TK_ASSIGN, TK_LT, TK_GT, TK_NOT,
	TK_EQ, TK_LE, TK_GE, TK_NE,
	TK_QUESTION, TK_COLON, TK_COMMA, TK_SEMICOLON, TK_POINT,
	TK_OPEN_SQUARE_BRACKET, TK_CLOSE_SQUARE_BRACKET,
	TK_OPEN_BRACKET, TK_CLOSE_BRACKET,
	TK_OPEN_BRACE, TK_CLOSE_BRACE, 
	TK_INC, TK_DEC, TK_OROR, TK_ANDAND,
	/* Identificator and literals */
	TK_ID, TK_INT_VAL, TK_DOUBLE_VAL, TK_CHAR_VAL, TK_STRING_LITERAL,
	/* Other tokens */
	NOT_TK
};
#define LAST_KWD (TK_WHILE - FIRST_KWD)
using namespace std;

typedef struct position{
	int row, col;
	position(int r, int c){ row = r; col = c; };
	position(){ row = col = 0; };
	position (const struct position &p){ row = p.row; col = p.col; };
} position_t;

class Token{
	position_t pos;
	TokenType type;
	string src;
public:
	friend class Lexer;
	Token(int col, int row, TokenType tk_type, const string tk_src);
	Token(const Token& tk){ pos = tk.pos; type = tk.type; src.assign(tk.src); };
	Token(): pos(position(0, 0)), type(NOT_TK), src(""){};
};

class Lexer{
	ifstream fin;
	string s;
	string::iterator it;
	position_t pos;
	Token tk;
	void ScanNewString();
	bool isKeyWord(TokenType tk_type);
	bool isOperator(TokenType tk_type);
	bool isDefinedByUser(TokenType tk_type);
	bool LookForward(const char c);
	Token GetNumber();
	Token GetKeyWordOrIdent();
	Token GetLiteral(const char c);
	void SkipComment();
	void SkipSymbol();
public:
	Lexer(const char *filename);
	void Print();
	Token Next();
	Token Get();
	bool TokenCanExist();
};

#endif