#!/bin/bash
echo -e "
.intel_syntax noprefix
.data
.globl literal0
literal0:
    .byte 50
    .byte 51
    .byte 50
    .byte 57
    .byte 48
    .byte 56
    .byte 57
    .byte 53
    .byte 54
    .byte 50
    .byte 56
    .byte 48
    .byte 49
    .byte 92
    .byte 110
    .byte 0
.globl main
.text
main:
    push rbp
    mov rbp, rsp
    sub rsp, 0
    lea rax, literal0[rip]
    push rax
    pop rdi
    mov rax, 0
    call printf
    push rax
    pop rax
    push 0
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    push rax
    mov rsp, rbp
    pop rbp
    ret
" > tmp1.s
cc -no-pie tmp1.s
./a.out
