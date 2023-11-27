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

function_main:
   push rbp
   mov rbp, rsp
   sub rsp, 16

   mov QWORD [rbp - 8], 10


   push 0
   pop rdx

   leave
   ret

asm_start:
   push rbp
   mov rbp, rsp

   call function_main
   call end
