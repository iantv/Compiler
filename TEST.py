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
ASMCNT = 56

compiler = '../Debug/Compiler.exe '
def testproc(testname, cmd, N, output, ansdir, runml = 0):
	if testname != '' :
		print testname
	for i in range(1, N + 1):
		p = subprocess.Popen(compiler + cmd + ' ' + ansdir + '{}.in'.format(i))
		p.wait()
		if (runml == 1):
			bucket1 = open('bucket1', 'w')
			bucket2 = open('bucket2', 'w')
			bucket3 = open('asmgen.out', 'w')
			p = subprocess.call('ml.exe /c /coff /Cp /nologo asmgen.asm', shell = True, stdout = bucket1)
			p = subprocess.call('link.exe /subsystem:console asmgen.obj', shell = True, stdout = bucket2)
			p = subprocess.call('asmgen.exe', shell = True, stdout = bucket3)
			bucket1.close()
			bucket2.close()
			bucket3.close()
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