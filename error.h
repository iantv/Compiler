#ifndef ERROR
#define ERROR

#include <string>
#include "lexer.h"

#define C2011 "C2011"
#define C2016 "C2016"
#define C2020 "C2020"
#define C2044 "C2044"
#define C2059 "C2059"
#define C2065 "C2065"
#define C2079 "C2079"
#define C2084 "C2084"
#define C2085 "C2085"
#define C2086 "C2086"
#define C2091 "C2091"
#define C2092 "C2092"

#define C2143 "C2143"
#define C2144 "C2144"
#define C2146 "C2146"
#define C2153 "C2153"
#define C2159 "C2159"

#define C2332 "C2332"
#define C2371 "C2371"

#define C2440 "C2440"

#define C2556 "C2556"

#define C3769 "C3769"
static const string pos_to_string(const position pos){
	return "line " + to_string(pos.row) + " column " + to_string(pos.col) + "\n"; 
};

class error: public exception{
public:
	string msg;
	error(const string err_msg, position err_pos): msg(err_msg) {};
	error(const string err_code, const string err_msg, position err_pos) { msg = "error " + err_code + ": " + err_msg + " in " + pos_to_string(err_pos);}  
};

class syntax_error: public error{
public:
	syntax_error(const string err_code, const string err_msg, position err_pos): error("error " + err_code + ": syntax error: " + err_msg + " in " + pos_to_string(err_pos), err_pos){}
};

class hex_error: public error{
public:
	hex_error(const string err_code, const string err_msg, position err_pos): error("error" + err_code + ": " + err_msg + pos_to_string(err_pos), err_pos){}
};

class octal_error: public error{
public:
	octal_error(const string err_code, const string err_msg, position err_pos): error("error" + err_code + ": " + err_msg + pos_to_string(err_pos), err_pos){}
};

#endif