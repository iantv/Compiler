#include "Tests.h"

#include "lexer.h"
#include "parser.h"

#include <string>
#include <fstream>

#define LTESTS	"Tests\\lexer\\";
#define IN ".in"
#define OUT ".out"

#define LCNT 7

#define OK "OK"
#define WA "WA"

char tk_name[10];
void tests::lexer_tests(){
	bool fail = 0;
	for (int i = 1; i <= LCNT; i++){
		string fin_name = LTESTS; fin_name += '0' + i; fin_name += IN;
		string fout_name = LTESTS; fout_name += '0' + i; fout_name += OUT;
		
		lexer L(fin_name.c_str());
		ifstream fout(fout_name.c_str(), ios::in);
		token tk, cur_tk;
		
		while (L.token_can_exist()){
			fout >> tk.pos.row >> tk.pos.col >> tk.src >> tk_name;
 			cur_tk = L.next();
			if (!(cur_tk.pos == tk.pos && cur_tk.src == tk.src && strcmp(tk_name, token_names[cur_tk.type]) == 0)){
				cout << i << ". " << WA << ": row = " << tk.pos.row << ", col = " << tk.pos.col << endl;
				fail = 1;
				break;
			}
		}
		if (fail) break;
		else cout << i << ". " << OK << endl;
		fout.close();
	}
}