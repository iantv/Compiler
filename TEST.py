import subprocess
LTESTCNT = 40
PTESTCNT = 50

PDTESTCNT = 23

ERRPRSDCL = 8 #Error Parser Declare
def testproc(testname, cmd, N, output, ansdir):
	print testname
	for i in range(1, N + 1):
		subprocess.call('../Debug/Compiler.exe ' + cmd + ' ' + ansdir + '{}.in'.format(i))
		f1 = open(output, 'r')
		f2 = open(ansdir + '{}.out'.format(i))
		print '.' if (f1.read() == f2.read()) else 'E',
		f1.close()
		f2.close()
	print

testproc('Lexer','-l', LTESTCNT, 'lexer.out', 'Tests/lexer/')
testproc('Parser of expression', '-p', PTESTCNT, 'parser.out', 'Tests/parser/')
testproc('Parser of declarations', '-p -decl', PDTESTCNT, 'parser.out', 'Tests/declar/')
testproc('Errors of declarrations parsing', '-p -decl', ERRPRSDCL, 'parser.out', 'Tests/errors/declar/')