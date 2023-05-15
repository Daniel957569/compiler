section .data
; Strings

; global variabless
x dd 0

section .text
	global _start

_start: 
   add rax, 10
   add rax, 10
   imul rax, 20
   mov rbx, 20
   idiv rax
   imul rax, 10
