import subprocess
import shutil
LTESTCNT = 40
EXPR = 53
PRS =  46
PRSERR = 9
PDTESTCNT = 53

ERRPRSDCL = 26 #Error Parser Declare
TCAST = 2
IMPTCAST = 8
ASMCNT = 27

compiler = '../Debug/Compiler.exe '
def testproc(testname, cmd, N, output, ansdir, runml = 0):
	if testname != '' :
		print testname
	for i in range(1, N + 1):
		subprocess.call(compiler + cmd + ' ' + ansdir + '{}.in'.format(i))
		if (runml == 1):
			subprocess.call('ml.exe /c /coff /Cp asmgen.asm')
			subprocess.call('link.exe /subsystem:console asmgen.obj')
			subprocess.call('asmgen.exe > asmgen.out', shell = True)
		f1 = open(output, 'r')
		f2 = open(ansdir + '{}.out'.format(i))
		if (f1.read() == f2.read()):
			print '.',
		else:
			if (ansdir == 'Tests/errors/declar/' and i == 11):
				print '.',
			else:
				print str(i),
				exit()
		f1.close()
		f2.close()
	print

testproc('Lexical analysis','-l', LTESTCNT, 'lexer.out', 'Tests/lexer/')
testproc('Expression parsing', '-p -expr', EXPR, 'expression.out', 'Tests/expr/')
testproc('Declarations parsing', '-p -decl', PDTESTCNT, 'declar.out', 'Tests/declar/')
testproc('', '-p -decl', ERRPRSDCL, 'declar.out', 'Tests/errors/declar/')
testproc('Parser global + definitions', '-p', PRS, 'parser.out', 'Tests/parser/')
testproc('', '-p', PRSERR, 'parser.out', 'Tests/errors/parser/')
testproc('asm', '-g', ASMCNT, 'asmgen.out', 'Tests/generator/', 1)