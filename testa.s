.intel_syntax noprefix
.globl main
.text
main:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    push 43
    pop rax
    push rax
    mov rsp, rbp
    pop rbp
    ret
