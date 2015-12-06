#include <string>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "sym_table.h"

int main(int argc, const char *argv[]){
	if (argc == 1) {
		cout<<"fatal error: no input files\ncompilation terminated\n";
	}
	if (argc == 3){
		lexer L(argv[2]);
		
		if (strcmp(argv[1], "-p") == 0){
			parser P(&L);
			ofstream fout("parser.out");
			L.next();
			expr *e = P.parse_expr();
			e->print(fout, 0);
			fout.close();
		}
		if (strcmp(argv[1], "-l") == 0){
			ofstream fout("lexer.out");
			while (L.token_can_exist()){
				fout << L.next();
			}
			fout.close();
		}
	}
	return 0;
}