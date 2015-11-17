#include "Tests.h"

#include "lexer.h"
#include "parser.h"

#include <string>
#include <fstream>

#define LTESTS	"C:\\\\Users\\Tatiana\\Documents\\Visual Studio 2012\\Projects\\Compiler\\Compiler\\Tests\\lexer\\"
#define PTESTS	"C:\\\\Users\\Tatiana\\Documents\\Visual Studio 2012\\Projects\\Compiler\\Compiler\\Tests\\parser\\"
#define IN ".in"
#define OUT ".out"

#define LCNT 15
#define PCNT 26

#define OK "OK"
#define WA "WA"

string int2str(int a){
	string res = "";
	int i = 0;
	while (a > 9){
		res = int2str(a % 10) + res;
		a /= 10;
	}
	res = char('0' + a) + res;
	return res;
}

char tk_name[10];
void tests::lexer_tests(){
	bool fail = 0;
	token tk, cur_tk;
	cout << "LEXER:" << endl;
	for (int i = 1; i <= LCNT; i++){
		string fin_name = LTESTS; fin_name += int2str(i); fin_name += IN;
		string fout_name = LTESTS; fout_name += int2str(i); fout_name += OUT;
		
		lexer L(fin_name.c_str());
		ifstream fout(fout_name.c_str(), ios::in);
		while (L.token_can_exist()){
			tk = token();
			fout >> tk.pos.row >> tk.pos.col >> tk.src >> tk_name;
 			cur_tk = L.next();
			if (!(cur_tk.pos == tk.pos && cur_tk.src == tk.src && strcmp(tk_name, token_names[cur_tk.type]) == 0)){
				cout << i << ". " << WA << ": row = " << tk.pos.row << ", col = " << tk.pos.col << endl;
				fail = 1;
				break;
			}
		}
		if (fail) break;
		cout << i << ". " << OK << endl;
		fout.close();
	}
}

void tests::parser_tests(){
	bool fail = 0;
	token tk, cur_tk;
	cout << "PARSER:" << endl;
	for (int i = 1; i <= PCNT; i++){
		ofstream ans("C:\\\\Users\\Tatiana\\Documents\\Visual Studio 2012\\Projects\\Compiler\\Compiler\\Tests\\parser\\ans.a");
		ifstream ans2("C:\\\\Users\\Tatiana\\Documents\\Visual Studio 2012\\Projects\\Compiler\\Compiler\\Tests\\parser\\ans.a");
		
		if (i == 18) continue;
		
		string fin_name = PTESTS; fin_name += int2str(i); fin_name += IN;
		string fout_name = PTESTS; fout_name += int2str(i); fout_name += OUT;
		
		lexer L(fin_name.c_str());
		parser P(&L);
		
		ifstream fout(fout_name.c_str(), ios::in);

		L.next();
		expr *e = P.parse_expr();
		e->print(ans, 0);
		ans.close();
		
		string s1((istreambuf_iterator<char>(fout)), istreambuf_iterator<char>());
		string s2((istreambuf_iterator<char>(ans2)), istreambuf_iterator<char>());
		cout << i << ". " << ((s1 == s2) ? "OK" : "WA") << endl;
		
		ans2.close();
		fout.close();
	}
}
