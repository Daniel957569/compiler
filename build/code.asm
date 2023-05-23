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
string_hello db "hello", 0
string_yooo db "yooo", 0

; global variabless
hi dq 0

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

function_yooo:
   push rbp
   mov rbp, rsp
   sub rsp, 16

   mov [rbp - 16], r10
   mov QWORD [rbp - 8], 0

.L0:
   mov rax, QWORD [rbp - 8]
   push rax
   push 10

   pop r8
   pop r9
   cmp r9, r8
   jge .L1
   mov rax, QWORD [rbp - 8]
   push rax

   pop rax
   mov rsi, rax
   mov rdi, format_number
   call printf
   mov rax, QWORD [rbp - 8]
   push rax
   pop r9
   mov r8, r9
   add r8, 1
   push r8

   pop r8
   mov [rbp - 8], r8


   jmp .L0

.L1:

   push 10
   pop rdx

   leave
   ret

function_main:
   push rbp
   mov rbp, rsp
   sub rsp, 8


   push string_hello
   pop r10

   call function_yooo
   push rdx
   pop rdx
   mov [rbp - 8], rdx
   mov rax, QWORD [rbp - 8]
   push rax

   pop rax
   mov rsi, rax
   mov rdi, format_number
   call printf

   push 0
   pop rdx

   leave
   ret

asm_start:

; start of hi
   mov rdi, string_yooo
   mov [hi], rdi
; end of hi


   call function_main
   call end
