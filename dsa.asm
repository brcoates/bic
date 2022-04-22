bits 64
global main

extern printf

section .text
	main:
push rbp
mov rsp, rbp
sub rsp, 64
mov rax, 1
add rax, rcx
add rax, rdx
add rax, r8
add rax, r9
add rax, [rbp + 8]
mov rsp, rbp
pop rbp
ret
