#include <iostream>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "Tests.h"

int main(int argc, const char *argv[]){
	if (argc == 1) {
		cout<<"fatal error: no input files\ncompilation terminated\n";
	}
	
	tests T(UT_LEXER);
	T.lexer_tests();
	cout << endl;
	T.parser_tests();

	system("pause");
	return 0;
}