section .data
  equal db 'Equal', 0, 10
  len_equal equ $ - equal
  not_equal db 'Not Equal', 0, 10
  len_not_equal equ $ - not_equal
  format_number db "%d", 10, 0
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

   mov [rsp + 8], r10
   push 1
   xor rsi, rsi
   mov rdi, format_true
   call printf
   pop rdi
   push 10
   mov rsi, [rsp]
   mov rdi, format_number
   call printf
   pop rdi
   mov rax, QWORD [rsp + 8]
   push rax
   mov rsi, [rsp]
   mov rdi, format_string
   mov al, 0
   call printf
   pop rdi
   leave
   ret

function_main:
   push rbp
   mov rbp, rsp
   sub rsp, 16

   mov DWORD [rsp + 8], 20

   push string_hello
   pop r10
   call function_yooo
   leave
   ret

asm_start:

; start of hi
   mov rdi, string_yooo
   mov [hi], rdi
; end of hi


   call function_main
   call end
