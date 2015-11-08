#ifndef TESTS
#define TESTS

enum unit_t{
	UT_LEXER, UT_PARSER, UT_EXPRESSION
};

class tests{
	unit_t unit;
public:
	tests(unit_t unt): unit(unt) {};
	void lexer_tests();
};

#endif