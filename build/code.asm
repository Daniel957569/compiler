section .data
  equal db 'Equal', 0, 10
  len_equal equ $ - equal
  not_equal db 'Not Equal', 0, 10
  len_not_equal equ $ - not_equal
  format_number db "%lld", 10, 0
  format_string db "%s", 10, 0
  format_true db "true", 10, 0
  format_false db "false", 10, 0

; Strings
the_result_is___string db "the result is: ", 0
Hello_World_string db "Hello World", 0

; global variabless

section .text
	extern printf
	global asm_start


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
   ret

print_not_equal: 
   mov rax, 1
   mov rdi, 1
   mov rsi, not_equal
   mov rdx, len_not_equal
   syscall
   ret

function_x:
   push rbp
   mov rbp, rsp
   sub rsp, 32

   mov [rbp - 24], r10
   mov [rbp - 16], r11
   mov QWORD [rbp - 8], 69

.L0:
   mov rax, QWORD [rbp - 24]
   push rax
   mov rax, QWORD [rbp - 8]
   push rax

   pop r8
   pop r9
   cmp r8, r9
   jle .L1
   mov rax, QWORD [rbp - 24]
   push rax
   pop r9
   mov r8, r9
   add r8, 1
   push r8

   pop r8
   mov [rbp - 24], r8

   mov rax, QWORD [rbp - 8]
   push rax
   mov rax, QWORD [rbp - 24]
   push rax
   pop r9
   pop r10
   add r9, r10
   push r9


   pop rax
   mov rsi, rax
   mov rdi, format_number
   xor rax, rax
   call printf
   mov rax, QWORD [rbp - 16]
   push rax

   pop rax
   mov rsi, rax
   mov rdi, format_string
   xor rax, rax
   call printf

   jmp .L0

.L1:

   mov rax, QWORD [rbp - 8]
   push rax
   pop rdx

   leave
   ret

function_main:
   push rbp
   mov rbp, rsp
   sub rsp, 16

   push 100
   pop r10
   push Hello_World_string
   pop r11
   call function_x
   mov [rbp - 8], rdx
   push the_result_is___string

   pop rax
   mov rsi, rax
   mov rdi, format_string
   xor rax, rax
   call printf
   mov rax, QWORD [rbp - 8]
   push rax

   pop rax
   mov rsi, rax
   mov rdi, format_number
   xor rax, rax
   call printf

   push 0
   pop rdx

   leave
   ret

asm_start:
   push rbp
   mov rbp, rsp

   call function_main
   call end
