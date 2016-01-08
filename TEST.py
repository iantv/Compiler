import subprocess
LTESTCNT = 40
PTESTCNT = 50

PDTESTCNT = 49

ERRPRSDCL = 25 #Error Parser Declare
TCAST = 2
IMPTCAST = 1
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

testproc('Lexical analysis','-l', LTESTCNT, 'lexer.out', 'Tests/lexer/')
testproc('Expression parsing', '-p -expr', PTESTCNT, 'expression.out', 'Tests/expr/')
testproc('Declarations parsing', '-p -decl', PDTESTCNT, 'declar.out', 'Tests/declar/')
testproc('\terrors:', '-p -decl', ERRPRSDCL, 'declar.out', 'Tests/errors/declar/')
#testproc('Type casting', '-p -tcast', TCAST, 'typecast.out', 'Tests/typecast/')
testproc('Type casting', '-p -tcast', IMPTCAST, 'typecast.out', 'Tests/implicit_typecast/')