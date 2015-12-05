import subprocess
LTESTCNT = 15;
PTESTCNT = 50;

print 'Lexer tests:'
for i in range(1, LTESTCNT + 1):
	subprocess.call('../Debug/Compiler.exe -l Tests/lexer/{}.in'.format(i))
	f1 = open('lexer.out', 'r')
	f2 = open('Tests/lexer/{}.out'.format(i), 'r')
	print '.' if (f1.read() == f2.read()) else 'E',
	f1.close()
	f2.close()

print 
print 'Parser tests:'
for i in range(1, PTESTCNT + 1):
	subprocess.call('../Debug/Compiler.exe -p Tests/parser/{}.in'.format(i))
	f1 = open('parser.out', 'r')
	f2 = open('Tests/parser/{}.out'.format(i), 'r')
	print '.' if (f1.read() == f2.read()) else 'E',
	f1.close()
	f2.close()