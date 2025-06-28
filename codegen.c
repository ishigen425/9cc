#include <stdio.h>
#include <stdbool.h>
#include "9cc.h"

int labelidx = 0;
char *arglist[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// push variable address
void gen_variable(Node *node) {
    if (node->kind == ND_LVAR){
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n", node->offset);
        printf("    push rax\n");
    } else if (node->kind == ND_GVARREF) {
        char t[64];
        mysubstr(t, node->name, 0, node->namelen);
        printf("    lea rax, %s[rip]\n", t);
        printf("    push rax\n");
    } else if (node->kind == ND_LITERALREF) {
        printf("    lea rax, literal%d[rip]\n", node->offset);
        printf("    push rax\n");
    } else if (node->kind == ND_STRUCTREF_PTR) {
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    add rax, %d\n", node->offset);
        printf("    push rax\n");
    } else if (node->kind == ND_STRUCTREF) {
        printf("    pop rax\n");
        printf("    add rax, %d\n", node->offset);
        printf("    push rax\n");
    } else {
        error("The left-hand value of the assignment is not a variable.");
    }
}

void gen(Node *node) {
    if (node == NULL) return;
    int lidx = labelidx++;
    Node *child = node->child;
    char t[64];
    switch (node->kind) {
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_variable(node);
        if (node->lhs != NULL && is_struct_ref(node->lhs->kind)) {
            gen(node->lhs);
        }
        printf("    pop rax\n");
        if ((node->type != NULL && node->type->ty == CHAR)
            || (node->type != NULL && node->type->ty == BOOL)) {
            printf("    movzx rax, BYTE PTR [rax]\n");
        } else {
            printf("    mov rax, [rax]\n");
        }
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        if(node->lhs->kind == ND_DEREF){
            gen(node->lhs->lhs);
        }else{
            gen_variable(node->lhs);
            if (node->lhs->lhs != NULL && is_struct_ref(node->lhs->lhs->kind)) {
                gen(node->lhs->lhs);
            }
        }
        gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    case ND_WHILE:
        printf(".Lbegin%d:\n", lidx);
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", lidx);
        gen(node->rhs);
        printf("    pop rax\n");
        printf("    jmp .Lbegin%d\n", lidx);
        printf(".Lend%d:\n", lidx);
        printf("    push rax\n");
        return;
    case ND_IF_ELSE:
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lelse%d\n", lidx);
        gen(node->rhs);
        printf("    jmp .Lend%d\n", lidx);
        printf(".Lelse%d:\n", lidx);
        gen(node->elsehs->lhs);
        printf("    pop rax\n");
        printf(".Lend%d:\n", lidx);
        printf("    push rax\n");
        return;
    case ND_IF:
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", lidx);
        gen(node->rhs);
        printf("    pop rax\n");
        printf(".Lend%d:\n", lidx);
        printf("    push rax\n");
        return;
    case ND_FOR:
        if (node->initstmt != NULL) {
            gen(node->initstmt);
        }
        printf(".Lbegin%d:\n", lidx);
        if (node->testexpr != NULL) {
            gen(node->testexpr);
        } else {
            printf("    push 1\n");
        }
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", lidx);
        gen(node->lhs);
        if (node->updstmt != NULL) {
            gen(node->updstmt);
        }
        printf("    jmp .Lbegin%d\n", lidx);
        printf(".Lend%d:\n", lidx);
        printf("    push rax\n");
        return;
    case ND_BLOCK:
        while (child->lhs != NULL) {
            gen(child->lhs);
            printf("    pop rax\n");
            child = child->child;
        }
        printf("    push rax\n");
        return;
    case ND_FUNCALL:
        // When calling functions, the remainder of rsp divided by 16 must be 0.
        printf("    mov rax, rsp\n");
        printf("    mov rdi, 16\n");
        printf("    cqo\n");
        printf("    idiv rdi\n");
        printf("    sub rsp, rdx\n");
        printf("    push rdx\n");
        printf("    push rdx\n");
        mysubstr(t, node->name, 0, node->namelen);
        int idx = 0;
        // Use in order -> RDI, RSI, RDX, RCX, R8, R9
        while(node->arg[idx] != NULL){
            gen(node->arg[idx]);
            idx++;
        }
        for(int i = idx - 1; i >= 0; i--){
            printf("    pop %s\n", arglist[i]);
        };

        printf("    mov rax, 0\n");
        printf("    call %s\n", t);
        printf("    pop rdx\n");
        printf("    pop rdx\n");
        printf("    add rsp, rdx\n");
        // return value push stack
        printf("    push rax\n");
        return;
    case ND_FUNCDEF:
        mysubstr(t, node->name, 0, node->namelen);
        printf(".text\n");
        printf("%s:\n", t);
        // prolog
        printf("    push rbp\n");
        printf("    mov rbp, rsp\n");
        int variable_space = (node->argnum) * 8  + node->offset;
        if (variable_space % 16 != 0) variable_space += 8;
        printf("    sub rsp, %d\n", variable_space);
        for(int i = 0; i < node->argnum; i++) {
            gen_variable(node->arg[i]);
            printf("    pop rax\n");
            printf("    mov [rax], %s\n", arglist[i]);
            printf("    mov rax, 0\n");
        }

        gen(node->lhs);

        // epilog
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    case ND_ADDR:
        gen_variable(node->lhs);
        return;
    case ND_DEREF:
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_GVARDEF:
        mysubstr(t, node->name, 0, node->namelen);
        printf(".data\n");
        printf(".comm %s, %d, 4\n", t, node->offset);
        return;
    case ND_GVARREF:
        mysubstr(t, node->name, 0, node->namelen);
        printf("    lea rax, %s[rip]\n", t);
        printf("    push rax\n");
        if (node->lhs != NULL && is_struct_ref(node->lhs->kind)) {
            gen(node->lhs);
        }
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_LITERAL:
        printf(".data\n");
        printf(".globl literal%d\n", node->offset);
        printf("literal%d:\n", node->offset);
        mysubstr(t, node->name, 0, node->namelen);
        for(int i = 0; i < node->namelen; i++) {
            if(t[i] == '\\' && t[i+1] == 'n'){
                printf("    .byte 10\n");
                i++;
            }else{
                printf("    .byte %d\n", t[i]);
            }
        }
        printf("    .byte 0\n");
        return;
    case ND_STRUCTDEF:
        return;
    case ND_STRUCTREF_PTR:
    case ND_STRUCTREF:
        gen_variable(node);
        gen(node->lhs);
        return;
    case ND_NOT:
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    xor rax, 1\n");
        printf("    push rax\n");
        return;
    case ND_CAST:
        // For now, just generate the expression being cast
        // Cast to void pointer is essentially a no-op
        gen(node->lhs);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    case ND_MOD:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        printf("    push rdx\n");
        printf("    pop rax\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_AND:
        printf("    and rax, rdi\n");
        break;
    case ND_OR:
        printf("    or rax, rdi\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}
