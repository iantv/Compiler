#include <iostream>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "Tests.h"

int main(int argc, const char *argv[]){
	tests T = tests();
 	T.lexer_tests();
 	cout << endl;
 	T.parser_tests();
	system("pause");
	return 0;

	if (argc == 1) {
		cout<<"fatal error: no input files\ncompilation terminated\n";
	}/*
	if (argc == 2){
		lexer L(argv[1]);
		while (L.token_can_exist()){
			std::cout << L.next();
			//L.print();
		}
		
	} */
	
	if (argc == 3){
		lexer L(argv[2]);
		/*if (strcmp(argv[1], "-p") == 0){
			parser P(&L);
			P.parse();
			P.print(std::cout);
		}*/
		if (strcmp(argv[1], "-l") == 0){
			string fout(argv[2]); 
			//int k = fout.find(".in");
			//freopen(strcat(fout.substr(k, 3).c_str(), ".a"), "w", stdout);
			while (L.token_can_exist()){
				std::cout << L.next();
			}
		}
	}

	system("pause");
	return 0;
}