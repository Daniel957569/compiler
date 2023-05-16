section .data
  equal db 'Equal', 0, 10
  len_equal equ $ - equal
  not_equal db 'Not Equal', 0, 10
  len_not_equal equ $ - not_equal
; Strings

; global variabless
x dd 0

section .text
	global _start


end:
   mov rax, 60
   xor rdi, rdi
   syscall

print_equal: 
   mov rax, 1
   mov rdi, 1
   mov rsi, equal
   mov rdx, len_equal
   syscall
   jmp end

print_not_equal: 
   mov rax, 1
   mov rdi, 1
   mov rsi, not_equal
   mov rdx, len_not_equal
   syscall
   jmp end

_start:
   mov rax, 10
   imul r9, rax, 11
   add r8, r9
   xor r9, r9

   mov rax, 1
   idiv r8
   add r8, rax

   imul r8, 10

   add r8, 50

   add r8, 10


   cmp r8, 1160
   je print_equal
   jmp print_not_equal
