ml.exe /c /coff /Cp asmgen.asm
link /subsystem:console asmgen.obj
asmgen.exe > asmgen.out
