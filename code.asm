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
   push 0
   mov rax, 10
   mov r9, 10
   idiv r9
   push rax

   pop r9
   mov r8, r9
   add r8, 10
   push r8

   mov r9, 20
   imul r9, 40
   push r9

   pop r9
   pop r10
   add r9, r10
   push r9

   mov r9, 20
   imul r9, 40
   push r9

   pop r9
   imul r9, 30
   push r9

   pop r9
   pop r10
   add r9, r10
   push r9

   pop r9
   mov r8, r9
   add r8, 20
   push r8

   pop r9
   mov r8, r9
   sub r8, 40
   push r8


   pop r8
   cmp r8, 24791
   je print_equal
   jmp print_not_equal
