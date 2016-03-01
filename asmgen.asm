.686
.model flat, stdcall

include \masm32\include\msvcrt.inc
includelib \masm32\lib\msvcrt.lib

.code
main:
	push 1
	pop eax
	ret
end main
