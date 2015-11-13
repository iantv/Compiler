#include <iostream>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "Tests.h"
//#include <math.h>

int a, b, c, d, e, f, g, h, i, j, k;
int main(int argc, const char *argv[]){
	if (argc == 1) {
		cout<<"fatal error: no input files\ncompilation terminated\n";
	}
	b = c = d = e = f = g = h = i = j = k = 1;
	a=((((((((((b+=1)-=c)*=d)/=e)%=f)^=g)|=h)&=i)<<=j)>>=k);
	
	tests T = tests();
	T.lexer_tests();
	cout << endl;
	T.parser_tests();

	system("pause");
	return 0;
}