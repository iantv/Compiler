#include <iostream>
#include "lexer.h"

int main(int argc, const char **argv){
	if (argc == 1) {
		cout<<"fatal error: no input files\ncompilation terminated\n";
	}
	if (argc == 2){
		lexer L(argv[1]);
		while (L.token_can_exist()){
			L.next();
			L.print();
		}
		
	}

	system("pause");
	return 0;
}