section .data
  equal db 'Equal', 0, 10
  len_equal equ $ - equal
  not_equal db 'Not Equal', 0, 10
  len_not_equal equ $ - not_equal
; Strings

; global variabless
yfdsfds dd 0
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
   push 0
   push 0

_start:

; start of yfdsfds
   mov r9, 10
   imul r9, 10
   push r9

   mov r9, 10
   imul r9, 10
   push r9

   pop r9
   pop r10
   add r9, r10
   push r9

   mov r9, 10
   imul r9, 10
   push r9

   pop rax
   mov r9, 10
   idiv r9
   push rax

   pop r9
   imul r9, 10
   push r9

   pop r9
   pop r10
   add r9, r10
   push r9

   pop r8
   mov [yfdsfds], r8

; end of yfdsfds


; start of x
   mov r9, 10
   imul r9, 10
   push r9

   mov r9, 10
   imul r9, 10
   push r9

   pop r9
   pop r10
   add r9, r10
   push r9

   mov r9, 10
   imul r9, 10
   push r9

   pop rax
   mov r9, 10
   idiv r9
   push rax

   pop r9
   imul r9, 10
   push r9

   pop r9
   pop r10
   add r9, r10
   push r9

   pop r8
   mov [x], r8

; end of x


   cmp DWORD [x], 300
   je print_equal
   jmp print_not_equal
