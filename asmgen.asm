.686
.model flat, stdcall

include \masm32\include\msvcrt.inc
includelib \masm32\lib\msvcrt.lib

.data
	format	db	'%d', 0
.code
start:
	mov ebp,esp
	push 2
	push 5
	pop ebx
	pop eax
	sub eax,ebx
	push eax
	pop eax
	push eax
	push offset format
	call crt_printf
	mov esp,ebp
	ret
end start
