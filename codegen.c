#include <stdio.h>
#include "9cc.h"

int labelidx = 0;

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

void gen(Node *node) {
    int lidx = labelidx++;
    switch (node->kind) {
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax],  rdi\n");
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
        printf("    jmp .Lbegin%d\n", lidx);
        printf(".Lend%d:\n", lidx);
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
        printf(".Lend%d:\n", lidx);
        return;
    case ND_IF:
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", lidx);
        gen(node->rhs);
        printf(".Lend%d:\n", lidx);
        return;
    case ND_FOR:
        if (node->initstmt != NULL) {
            gen(node->initstmt);
        }
        printf(".Lbegin%d:\n", lidx);
        if (node->testexpr != NULL) {
            gen(node->testexpr);
        } else {
            // 条件式が未定義の場合は常にfor文の中の処理を実行させる？
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
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
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
