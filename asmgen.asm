.686
.model flat, stdcall

include \masm32\include\msvcrt.inc
includelib \masm32\lib\msvcrt.lib

STR_LITERAL macro value : req
	LOCAL lbl
	.data
	lbl BYTE value, 0
	.code
	EXITM <lbl>
endm

.data
	a_	dd	0
.code
start:
	mov ebp,esp
	push a_
	push 1
	pop ebx
	pop eax
	add eax,ebx
	push eax
	pop eax
	push eax
	push a_
	pop eax
	push eax
	push offset STR_LITERAL('%d %d')
	pop eax
	push eax
	call crt_printf
	mov esp,ebp
	ret
end start
