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
	/*if (argc == 2){
		lexer L(argv[1]);
		while (L.token_can_exist()){
			L.next();
			L.tk_print();
		}
		
	} 
	if (argc == 3){
		lexer L(argv[2]);
		if (strcmp(argv[1], "-p") == 0){
			parser P(&L);
			if (L.token_can_exist()){
				L.next();
				expr *e = P.parse_expr();
				e->print(std::cout, 0);
			}
		}
		if (strcmp(argv[1], "-l") == 0){
			while (L.token_can_exist()){
				L.next();
				L.tk_print();
			}
		}
	}*/

	system("pause");
	return 0;
}