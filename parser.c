#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

LVar *locals;

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var ; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *new_node_indent(int offset) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = offset;
    return node;
}

Node *assign() {
    Node *node = equality();
    if (consume("="))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;
}


Node *stmt() {
    Node *child;
    Node *node;
    if (consume("{")) {
        node = calloc(1, sizeof(Node));
        node->child = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        child = node->child;
        while (!consume("}")) {
            child->lhs = stmt();
            child->child = calloc(1, sizeof(Node));
            child = child->child;
        }
        return node;
    } else if (consume_kind(TK_FOR)) {
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        if(!consume(";")){
            node->initstmt = expr();
            expect(";");
        }
        if(!consume(";")){
            node->testexpr = expr();
            expect(";");
        }
        if(!consume(")")){
            node->updstmt = expr();
            expect(")");
        }
        node->lhs = stmt();
        return node;
    }else if (consume_kind(TK_IF)) {
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        // else文があるかを判定する
        if (consume_kind(TK_ELSE)) {
            node->kind = ND_IF_ELSE;
            Node *elsenode = calloc(1, sizeof(Node));
            elsenode->kind = ND_ELSE;
            elsenode->lhs = stmt();
            node->elsehs = elsenode;
        }
        return node;
    } else if (consume_kind(TK_WHILE)){
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        return node;
    } else if (consume_kind(TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
        return node;
    } else if (consume_kind(TK_INT)) {
        Type *top = calloc(1, sizeof(Type));
        top->ptr_to = calloc(1, sizeof(Type));
        Type *tmp = top->ptr_to;
        while (consume("*")) {
            // ポインタ型を定義する
            tmp->ty = PTR;
            tmp->ptr_to = calloc(1, sizeof(Type));
            tmp = tmp->ptr_to;
        }
        tmp->ty = INT;
        tmp->ptr_to = NULL;
        top = top->ptr_to;
        
        Token *tok = consume_indent();
        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->type = top;
        if(locals != NULL)
            lvar->offset = locals->offset + 8;
        else
            lvar->offset = 0;
        locals = lvar;
        expect(";");
        node = new_node_num(0);
        return node;
    } else {
        node = expr();
        expect(";");
        return node;
    }
}

void program() {
    int i = 0;
    while(!at_eof()) {
        code[i++] = define_function();
    }
    code[i] = NULL;
}

Node *define_function() {
    char t[64];
    expect_type("int");
    Node *func_node = calloc(1, sizeof(Node));
    Token *tok = consume_indent();
    func_node->kind = ND_FUNCDEF;
    func_node->name = tok->str;
    func_node->namelen = tok->len;
    int argnum = 0;
    expect("(");
    while(!consume(")")){
        expect_type("int");
        if(argnum >= 6)
            error("not implementation error!");
        // 引数をローカル変数と同様に扱う
        LVar *lvar = calloc(1, sizeof(LVar));
        Node *node = calloc(1, sizeof(Node));
        lvar->next = locals;
        lvar->name = token->str;
        lvar->len = token->len;
        if(locals != NULL)
            lvar->offset = locals->offset + 8;
        else
            lvar->offset = 8;
        node->kind = ND_LVAR;
        node->offset = lvar->offset;
        locals = lvar;
        func_node->arg[argnum++] = node;
        token = token->next;
        if(!consume(",")){
            expect(")");
            break;
        }
    }
    func_node->argnum = argnum;
    func_node->lhs = stmt();
    return func_node;
}

Node *expr() {
    return assign();
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_binary(ND_EQ, node, relational());
        if (consume("!="))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();
    
    for (;;) {
        if (consume("<"))
            node = new_binary(ND_LT, node, add());
        else if (consume("<="))
            node = new_binary(ND_LE, node, add());
        else if (consume(">"))
            node = new_binary(ND_LT, add(), node);
        else if (consume(">="))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }

}

Node *add() {
    Node *node = mul();

    for (;;) {
        if(consume("+"))
            node = new_binary(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary(){
    if (consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_node_num(0), unary());
    if (consume("&"))
        return new_binary(ND_ADDR, unary(), NULL);
    if (consume("*"))
        return new_binary(ND_DEREF, unary(), NULL);
    return primary();
}

Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_indent();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));

        if(startswith(tok->next->str, "(")) {
            char t[64];
            mysubstr(t, tok->str, 0, tok->len);
            node->name = tok->str;
            node->namelen = tok->len;
            node->kind = ND_FUNCALL;
            expect("(");
            int idx = 0;
            while(!consume(")")){
                if(idx >= 6)
                    error("not implementation error!");
                node->arg[idx] = add();
                idx++;
                if(!consume(",")){
                    expect(")");
                    break;
                }
            }
            return node;
        }

        LVar *lvar = find_lvar(tok);
        if(lvar) {
            node->kind = ND_LVAR;
            node->offset = lvar->offset;
            node->type = lvar->type;
        } else {
            char t[64];
            mysubstr(t, tok->str, 0, tok->len);
            error("%s is not defined.", t);
        }
        return node;
    }
    return new_node_num(expect_number());
}
