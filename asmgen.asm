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
	a_	db	0
.code
start:
	mov ebp,esp
	push offset STR_LITERAL('Hello\tworld!')
	pop eax
	push eax
	call crt_printf
	mov esp,ebp
	ret
end start
