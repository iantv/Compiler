#include <iostream>
#include "lexer.h"

int main(int argc, const char **argv){
	if (argc == 1) {
		cout<<"fatal error: no input files\ncompilation terminated\n";
	}
	if (argc == 2){
		Lexer L(argv[1]);
		while (L.TokenCanExist()){
			L.Next();
			L.Print();
		}
		
	}

	system("pause");
	return 0;
}