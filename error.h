#ifndef ERROR
#define ERROR

#include <string>
#include "lexer.h"

#define C2143 "C2143"
#define C2153 "C2153"

class error{
protected:
	string msg;
	position pos;
public:
	error(const string err_msg, position err_pos): msg(err_msg), pos(err_pos){};
};

static const string pos_to_string(const position pos){
	return "line " + to_string(pos.row) + "column " + to_string(pos.col) + "\n"; 
};

class syntax_error: error{
public:
	syntax_error(const string err_code, const string err_msg, position err_pos): error("error " + err_code + ": syntax error: " + err_msg + pos_to_string(err_pos), err_pos){}
};

class hex_error: error{
public:
	hex_error(const string err_code, const string err_msg, position err_pos): error("error" + err_code + ": " + err_msg + pos_to_string(err_pos), err_pos){}
};

class octal_error: error{
public:
	octal_error(const string err_code, const string err_msg, position err_pos): error("error" + err_code + ": " + err_msg + pos_to_string(err_pos), err_pos){}
};
#endif