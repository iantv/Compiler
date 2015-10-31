#include <iostream>
#include <string.h>
#include "lexer.h"
#include "parser.h"

int main(int argc, const char *argv[]){
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
	if (argc == 3){
		lexer L(argv[2]);
		if (strcmp(argv[1], "-p") == 0){
			parser P(&L);
			P.parse();
			P.print(std::cout);
		}
		if (strcmp(argv[1], "-l") == 0){
			while (L.token_can_exist()){
				L.next();
				L.print();
			}
		}
	}

	system("pause");
	return 0;
}