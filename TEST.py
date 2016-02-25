import subprocess
LTESTCNT = 40
EXPR = 53
PRS =  43
PRSERR = 9
PDTESTCNT = 53

ERRPRSDCL = 26 #Error Parser Declare
TCAST = 2
IMPTCAST = 8
def testproc(testname, cmd, N, output, ansdir):
	if testname != '' :
		print testname
	for i in range(1, N + 1):
		subprocess.call('../Debug/Compiler.exe ' + cmd + ' ' + ansdir + '{}.in'.format(i))
		f1 = open(output, 'r')
		f2 = open(ansdir + '{}.out'.format(i))
		if (f1.read() == f2.read()):
			print '.',
		else:
			if (ansdir == 'Tests/errors/declar/' and i == 11) or (ansdir == 'Tests/parser/' and i == 9):
				print '.',
			else:
				print str(i) + 'E',
		f1.close()
		f2.close()
	print

testproc('Lexical analysis','-l', LTESTCNT, 'lexer.out', 'Tests/lexer/')
testproc('Expression parsing', '-p -expr', EXPR, 'expression.out', 'Tests/expr/')
testproc('Declarations parsing', '-p -decl', PDTESTCNT, 'declar.out', 'Tests/declar/')
testproc('', '-p -decl', ERRPRSDCL, 'declar.out', 'Tests/errors/declar/')
testproc('Parser global + definitions', '-p', PRS, 'parser.out', 'Tests/parser/')
testproc('', '-p', PRSERR, 'parser.out', 'Tests/errors/parser/')