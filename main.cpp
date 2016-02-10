#include <string>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "sym_table.h"

int main(int argc, const char *argv[]){
	if (argc == 1){
		cout << "fatal error: no input files\ncompilation terminated\n";
	}
	ofstream fout;
	try {
		if (argc == 3){
			lexer L(argv[2]);
			if (strcmp(argv[1], "-p") == 0){
				parser P(&L);
				fout.open("parser.out");
				P.parse(fout);
				P.print_sym_table(fout);
				fout.close();
			} else if (strcmp(argv[1], "-l") == 0){
				fout.open("lexer.out");
				while (L.token_can_exist()){
					fout << L.next();
				}
				fout.close();
			}
		}
		if (argc == 4){
			lexer L(argv[3]);
			if (strcmp(argv[1], "-p") == 0)
				if (strcmp(argv[2], "-decl") == 0){
					parser P(&L);
					fout.open("declar.out");
					P.parse(fout);
					P.print_sym_table(fout);
					fout.close();
				} else if (strcmp(argv[2], "-expr") == 0){
					parser P(&L);
					fout.open("expression.out");
					P.tcast = false; /* Disable type checking */
					//P.prs_expr = true;
					P.parse_expr()->print(fout, 0);
					fout.close();
				} else if (strcmp(argv[2], "-tcast") == 0){
					parser P(&L);
					fout.open("typecast.out");
					P.parse_expr()->print(fout, 0);
					fout.close();
				}
		}
	} catch (error e){
		fout << e.msg;		
	} catch (...){
		fout << "Unexpected exception. ";
	}
	return 0;
}