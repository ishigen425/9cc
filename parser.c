#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

LVar *locals;
int locals_num;
GVar *globals;
GVar *literals;
int literals_def_idx = 0;

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var ; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

GVar *find_gvar(Token *tok) {
    for (GVar *var = globals; var ; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

GVar *find_gvar_literals(Token *tok) {
    for (GVar *var = literals; var ; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

int next_offset() {
    if(locals == NULL)
        return 8;
    return locals->offset + 8;
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

void program() {
    int i = 0;
    while(!at_eof()) {
        code[i++] = define_function_gvar();
    }
    code[i] = NULL;
}

Node *define_function_gvar() {
    locals = NULL;
    locals_num = 0;
    char t[64];
    TypeKind ty;
    if (consume_kind(TK_INT))
        ty = INT;
    if (consume_kind(TK_CHAR))
        ty = CHAR;
    Token *tok = consume_indent();
    if (consume("(")) {
        Node *func_node = calloc(1, sizeof(Node));
        func_node->kind = ND_FUNCDEF;
        func_node->name = tok->str;
        func_node->namelen = tok->len;
        int argnum = 0;
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
            lvar->offset = next_offset();
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
        func_node->localsnum = locals_num;
        return func_node;
    } else {
        Type *top = calloc(1, sizeof(Type));
        top->ptr_to = calloc(1, sizeof(Type));
        Type *tmp = top->ptr_to;
        while (consume("*")) {
            // ポインタ型を定義する
            tmp->ty = PTR;
            tmp->ptr_to = calloc(1, sizeof(Type));
            tmp = tmp->ptr_to;
            tok = consume_indent();
        }
        tmp->ty = ty;
        tmp->ptr_to = NULL;
        top = top->ptr_to;
        GVar *gvar = calloc(1, sizeof(LVar));
        gvar->next = globals;
        gvar->name = tok->str;
        gvar->len = tok->len;
        if (consume("[")) {
            tmp = calloc(1, sizeof(Type));
            tmp->ptr_to = top;
            tmp->ty = ARRAY;
            tmp->array_size = expect_number();
            expect("]");
            gvar->type = tmp;
        } else {
            gvar->type = top;
        }
        globals = gvar;
        expect(";");
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_GVARDEF;
        node->type = gvar->type;
        node->name = gvar->name;
        node->namelen = gvar->len;
        return node;
    }
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
        if (consume("[")) {
            tmp = calloc(1, sizeof(Type));
            tmp->ptr_to = top;
            tmp->ty = ARRAY;
            tmp->array_size = expect_number();
            expect("]");
            lvar->type = tmp;
            if (tmp->ptr_to->ty == INT){
                locals_num += tmp->array_size;
            } else {
                locals_num += tmp->array_size;
            }
            lvar->offset = next_offset() + (tmp->array_size-1) * 8;
        } else {
            lvar->type = top;
            locals_num++;
            lvar->offset = next_offset();
        }
        locals = lvar;
        expect(";");
        node = new_node_num(0);
        return node;
    } else if (consume_kind(TK_CHAR)) {
        Type *top = calloc(1, sizeof(Type));
        top->ptr_to = calloc(1, sizeof(Type));
        Type *tmp = top->ptr_to;
        while (consume("*")) {
            // ポインタ型を定義する
            tmp->ty = PTR;
            tmp->ptr_to = calloc(1, sizeof(Type));
            tmp = tmp->ptr_to;
        }
        tmp->ty = CHAR;
        tmp->ptr_to = NULL;
        top = top->ptr_to;
        
        Token *tok = consume_indent();
        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = next_offset();
        if (consume("[")) {
            tmp = calloc(1, sizeof(Type));
            tmp->ptr_to = top;
            tmp->ty = ARRAY;
            tmp->array_size = expect_number();
            expect("]");
            lvar->type = tmp;
            locals_num += tmp->array_size;
        } else {
            lvar->type = top;
            locals_num++;
        }
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

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("="))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;
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
        if (node->type != NULL && node->type->ty == PTR && consume("+"))
            node = new_binary(ND_ADD, node, mul_ptr(node->type));
        else if (node->type != NULL && node->type->ty == ARRAY && consume("+"))
            node = new_binary(ND_ADD, node, mul_ptr(node->type));
        else if (node->lhs != NULL && node->lhs->type != NULL && node->lhs->type->ty == ARRAY && node->kind == ND_ADDR && consume("+")){
            Type *type = calloc(1, sizeof(Type));
            type->ptr_to = calloc(1, sizeof(Type));
            type->ptr_to->ty = INT;
            node = new_binary(ND_ADD, node, mul_ptr(type));
        }
        else if (node->type != NULL && node->type->ty == PTR && consume("-"))
            node = new_binary(ND_SUB, node, mul_ptr(node->type));
        else if (node->lhs != NULL && node->lhs->type != NULL && node->lhs->type->ty == ARRAY && node->kind == ND_ADDR && consume("-")){
            Type *type = calloc(1, sizeof(Type));
            type->ptr_to = calloc(1, sizeof(Type));
            type->ptr_to->ty = INT;
            node = new_binary(ND_SUB, node, mul_ptr(type));
        }
        else if(consume("+"))
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

Node *mul_ptr(Type *type) {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return new_binary(ND_MUL, node, new_node_num(8));
    }
}

Type *arg_type;

Node *unary(){
    if (consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_node_num(0), unary());
    if (consume("&"))
        return new_binary(ND_ADDR, unary(), NULL);
    if (consume("*"))
        return new_binary(ND_DEREF, unary(), NULL);
    if (consume_kind(TK_SIZEOF)){
        arg_type = NULL;
        Node *node = primary();
        if (arg_type != NULL && arg_type->ty == PTR){
            node = new_node_num(8);
        } else if (arg_type != NULL && arg_type->ty == ARRAY) {
            node = new_node_num(arg_type->array_size);
        } else if (arg_type != NULL && arg_type->ty == CHAR) {
            node = new_node_num(1);
        } else {
            node = new_node_num(4);
        }
        arg_type = NULL;
        return node;
    }   
    return primary();
}


Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_indent();
    Node *node = calloc(1, sizeof(Node));
    if (tok) {

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
        GVar *gvar = find_gvar(tok);
        if(lvar) {
            node->kind = ND_LVAR;
            node->offset = lvar->offset;
            node->type = lvar->type;
            arg_type = lvar->type;
            if (consume("[")){
                node = new_binary(ND_DEREF, new_binary(ND_ADD, new_binary(ND_ADDR, node, NULL), new_binary(ND_MUL, new_node_num(8), equality())), NULL);
                expect("]");
                return node;
            }
            if (lvar->type != NULL && lvar->type->ty == ARRAY) {
                return new_binary(ND_ADDR, node, NULL);
            }
        } else if(gvar) {
            node->kind = ND_GVARREF;
            node->name = gvar->name;
            node->namelen = gvar->len;
            if (consume("[")){
                node = new_binary(ND_DEREF, new_binary(ND_ADD, new_binary(ND_ADDR, node, NULL), new_binary(ND_MUL, new_node_num(8), equality())), NULL);
                expect("]");
                return node;
            }
            if (gvar->type != NULL && gvar->type->ty == ARRAY) {
                return new_binary(ND_ADDR, node, NULL);
            }
        } else {
            char t[64];
            mysubstr(t, tok->str, 0, tok->len);
            error("%s is not defined.", t);
        }
        return node;
    }
    if (token->kind == TK_STR) {
        GVar *gvar = calloc(1, sizeof(LVar));
        gvar->next = globals;
        gvar->name = token->str;
        gvar->len = token->len;
        node->kind = ND_LITERALREF;
        node->name = gvar->name;
        node->namelen = gvar->len;
        node->offset = literals_def_idx;
        Node *def_node = calloc(1, sizeof(Node));
        def_node->kind = ND_LITERAL;
        def_node->name = gvar->name;
        def_node->namelen = gvar->len;
        def_node->offset = literals_def_idx;
        literals_def[literals_def_idx++] = def_node;
        token = token->next;
        return new_binary(ND_ADDR, node, NULL);
    }

    return new_node_num(expect_number());
}
