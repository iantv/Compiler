.686
.model flat, stdcall

include \masm32\include\msvcrt.inc
includelib \masm32\lib\msvcrt.lib

.code
start:
	push 2
	push 1
	pop ebx
	pop eax
	xor edx,edx
	div ebx
	push eax
	pop eax
	ret
end start
