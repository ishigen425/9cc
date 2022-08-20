#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

int literals_def_idx = 0;
int _offset = 0;

int get_next_offset(int val) {
    if (locals != NULL) {
        if (_offset < locals->offset) _offset = locals->offset;
    }
    if (val <= 8){
        return _offset = _offset + 8;
    } else {
        return _offset = _offset + val;
    }
}

bool at_eof() {
    return token->kind == TK_EOF;
}

LVar *declared_lvar(TypeKind kind, int kind_size){
    Type *top = calloc(1, sizeof(Type));
    top->ptr_to = calloc(1, sizeof(Type));
    Type *tmp = top->ptr_to;
    while (consume("*")) {
        // ポインタ型を定義する
        tmp->ty = PTR;
        tmp->ptr_to = calloc(1, sizeof(Type));
        tmp = tmp->ptr_to;
    }
    tmp->ty = kind;
    tmp->ptr_to = NULL;
    top = top->ptr_to;
    
    Token *tok = consume_indent();
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;
    if (consume("[")) {
        tmp = calloc(1, sizeof(Type));
        tmp->ptr_to = top;
        tmp->ty = ARRAY;
        tmp->array_size = expect_number();
        expect("]");
        lvar->type = tmp;
        lvar->offset = get_next_offset(kind_size) + (tmp->array_size-1) * kind_size;
        lvar->offset += ((tmp->array_size-1) / 8 + 1) * 8;
    } else {
        lvar->type = top;
        lvar->offset = get_next_offset(kind_size);
    }
    return lvar;
}

LVar *declare_structs() {
    Token *struct_name_tok = consume_indent();
    Node *struct_node = find_defined_structs(struct_name_tok);

    Type *top = calloc(1, sizeof(Type));
    top->ptr_to = calloc(1, sizeof(Type));
    Type *tmp = top->ptr_to;
    while (consume("*")) {
        // ポインタ型を定義する
        tmp->ty = PTR;
        tmp->ptr_to = calloc(1, sizeof(Type));
        tmp = tmp->ptr_to;
    }
    tmp->ty = STRUCT;
    tmp->ptr_to = NULL;
    tmp->type_name = struct_node->name;
    tmp->type_name_len = struct_node->namelen;
    top = top->ptr_to;
    
    Token *tok = consume_indent();

    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->offset = get_next_offset(struct_node->offset);
    lvar->type = top;
    return lvar;
}

Node *defined_struct(Token *tok) {
    int offset = 0;
    int localnum = 0;
    Node *variabls = NULL;
    expect("{");
    while (!consume("}")) {
        LVar *lvar = calloc(1, sizeof(LVar));
        if (consume_kind(TK_INT)) {
            lvar = declared_lvar(INT, 8);
        } else if (consume_kind(TK_CHAR)) {
            lvar = declared_lvar(CHAR, 1);
        } else if (consume_kind(TK_BOOL)) {
            lvar = declared_lvar(BOOL, 1);
        } else if (consume_kind(TK_STRUCT)) {
            Token *child_tok = consume_indent();
            Node *childe_struct_node = find_defined_structs(child_tok);
            if(childe_struct_node == NULL) {
                char *t = calloc(50, sizeof(char));
                mysubstr(t, child_tok->str, 0, child_tok->len);
                error("%s is not defined.", t);
            }
            lvar = declared_lvar(STRUCT, childe_struct_node->offset);
            lvar->type->type_name = child_tok->str;
            lvar->type->type_name_len = child_tok->len;
        } else {
            lvar = calloc(1, sizeof(LVar));
        }
        Node *lvar_node = calloc(1, sizeof(Node));
        lvar_node->kind = ND_LVAR;
        lvar_node->offset = offset;
        lvar_node->name = lvar->name;
        lvar_node->namelen = lvar->len;
        lvar_node->type = lvar->type;
        lvar_node->child = variabls;
        variabls = lvar_node;
        offset += lvar->offset;
        expect(";");

    }
    expect(";");
    Node *defined_struct_node = calloc(1, sizeof(Node));
    defined_struct_node->lhs = variabls;
    defined_struct_node->kind = ND_STRUCTDEF;
    defined_struct_node->name = tok->str;
    defined_struct_node->namelen = tok->len;
    defined_struct_node->offset = offset;
    defined_struct_node->child = defined_structs;
    defined_structs = defined_struct_node;

    return defined_struct_node;
}

void *defined_enum() {
    expect("{");
    int index = 0;
    while (!consume("}")) {
        EnumDef *enum_def = calloc(1, sizeof(EnumDef));
        Token *tok = consume_indent();
        enum_def->str = tok->str;
        enum_def->len = tok->len;
        enum_def->index = index++;
        enum_def->next = defined_enums;
        defined_enums = enum_def;
        expect(",");
    }
    Token *name = consume_indent();
    expect(";");
}

Node *declared_gvar(Token *tok, TokenKind ty) {
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
    if (ty == INT) node->offset = 8;
    else if (ty == CHAR) node->offset = 1;
    if (tmp->ty == ARRAY) {
        node->offset *= tmp->array_size;
    }
    return node;
}

Node *declared_structs_gvar(Token *tok, TokenKind ty) {
    char *tokname = calloc(100, 1);
    int namelen = tok->len;
    memcpy(tokname, tok->str, namelen);
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
    tmp->type_name = tokname;
    tmp->type_name_len = namelen;
    tmp->ptr_to = NULL;
    top = top->ptr_to;

    GVar *gvar = calloc(1, sizeof(GVar));
    gvar->next = globals;
    gvar->name = tok->str;
    gvar->len = tok->len;
    if (top->ty == STRUCT) {
        Token *struct_tok = consume_indent();
        gvar->name = struct_tok->str;
        gvar->len = struct_tok->len;
    }
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
    if (node->type->ty == INT) node->offset = 8;
    else if (node->type->ty == CHAR) node->offset = 1;
    if (node->type->ty == ARRAY) {
        node->offset *= tmp->array_size;
    } else if (node->type->ty == STRUCT) {
        Token *search_tok = calloc(1, sizeof(Token));
        search_tok->str = tokname;
        search_tok->len = namelen;
        Node *defined_struct_node = find_defined_structs(search_tok);
        node->offset = defined_struct_node->offset;
    } else if (node->type->ty == PTR) {
        node->offset = 8;
    }
    return node;
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
    _offset = 0;
    char t[64];
    TypeKind ty;
    if (consume_kind(TK_INT))
        ty = INT;
    if (consume_kind(TK_CHAR))
        ty = CHAR;
    if (consume_kind(TK_BOOL))
        ty = BOOL;
    if (consume_kind(TK_STRUCT)) {
        Token *tok = consume_indent();
        if (find_defined_structs(tok) != NULL) {
            return declared_structs_gvar(tok, STRUCT);
        }
        return defined_struct(tok);
    }
    if (consume_kind(TK_TYPEDEF)) {
        if (consume_kind(TK_ENUM)) {
            defined_enum();
            return calloc(1, sizeof(Node));
        }
    }
    Token *tok = consume_indent();
    if (consume("(")) {
        Node *func_node = calloc(1, sizeof(Node));
        func_node->kind = ND_FUNCDEF;
        func_node->name = tok->str;
        func_node->namelen = tok->len;
        int argnum = 0;
        while(!consume(")")){
            if(argnum >= 6)
                error("not implementation error!");
            // 引数をローカル変数と同様に扱う
            LVar *lvar;
            if (consume_kind(TK_INT)) {
                lvar = declared_lvar(INT, 8);
            } else if (consume_kind(TK_CHAR)) {
                lvar = declared_lvar(CHAR, 1);
            } else if (consume_kind(TK_BOOL)) {
                lvar = declared_lvar(BOOL, 1);
            }
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_LVAR;
            node->offset = lvar->offset;
            lvar->next = locals;
            locals = lvar;
            func_node->arg[argnum++] = node;
            if(!consume(",")){
                expect(")");
                break;
            }
        }
        func_node->argnum = argnum;
        func_node->lhs = stmt();
        func_node->offset = get_next_offset(8);
        return func_node;
    } else {
        return declared_gvar(tok, ty);
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
        LVar *lvar = declared_lvar(INT, 8);
        lvar->next = locals;
        locals = lvar;
        expect(";");
        node = new_node_num(0);
        return node;
    } else if (consume_kind(TK_CHAR)) {
        LVar *lvar = declared_lvar(CHAR, 1);
        lvar->next = locals;
        locals = lvar;
        expect(";");
        node = new_node_num(0);
        return node;
    } else if (consume_kind(TK_BOOL)) {
        LVar *lvar = declared_lvar(BOOL, 1);
        lvar->next = locals;
        locals = lvar;
        expect(";");
        node = new_node_num(0);
        return node;
    } else if(consume_kind(TK_STRUCT)) {
        LVar *lvar = declare_structs();
        lvar->next = locals;
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
        else if (consume("%"))
            node = new_binary(ND_MOD, node, unary());
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
        Node *ret_node = calloc(1, sizeof(Node));
        if (arg_type != NULL && arg_type->ty == PTR){
            ret_node = new_node_num(8);
        } else if (arg_type != NULL && arg_type->ty == ARRAY) {
            ret_node = new_node_num(arg_type->array_size);
        } else if (arg_type != NULL && arg_type->ty == CHAR) {
            ret_node = new_node_num(1);
        } else if (node->kind == ND_STRUCTDEF) {
            ret_node = new_node_num(node->offset);
        } else {
            ret_node = new_node_num(4);
        }
        arg_type = NULL;
        return ret_node;
    }
    return primary();
}

Node *struct_ref(Node *defined_struct_node, Token *left_token) {
    bool is_arrow = consume("->");
    bool is_point = consume(".");
    if (is_arrow || is_point) {
        Token *tok = consume_indent();
        Token *next_struct_tok = calloc(1, sizeof(Token));
        int offset = 0;
        for (Node *struct_node_var = defined_struct_node->lhs; struct_node_var; struct_node_var = struct_node_var->child) {
            if (struct_node_var->namelen == tok->len && !memcmp(struct_node_var->name, tok->str, tok->len))
                offset += struct_node_var->offset;
                next_struct_tok->str = struct_node_var->type->type_name;
                next_struct_tok->len = struct_node_var->type->type_name_len;
                break;
        }
        Node *struct_ref_node = calloc(1, sizeof(Node));
        struct_ref_node->offset = offset;
        struct_ref_node->lhs = struct_ref(find_defined_structs(next_struct_tok), tok);
        if (is_arrow) {
            struct_ref_node->kind = ND_STRUCTREF_PTR;
        } else if (is_point) {
            struct_ref_node->kind = ND_STRUCTREF;
        }
        return struct_ref_node;
    }
    return NULL;
}

Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_indent();
    if (tok) {

        if(startswith(tok->next->str, "(")) {
            char t[64];
            mysubstr(t, tok->str, 0, tok->len);
            Node *node = calloc(1, sizeof(Node));
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
        int enums_index = find_defined_enum(tok);
        if(lvar) {
            Node *node = calloc(1, sizeof(Node));
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
            if (consume("->")){
                Token *struct_type_token = calloc(1, sizeof(Token));
                struct_type_token->str = lvar->type->ptr_to->type_name;
                struct_type_token->len = lvar->type->ptr_to->type_name_len;
                Node *defined_struct_node = find_defined_structs(struct_type_token);
                Token *tok = consume_indent();
                int offset = get_struct_node_offset(defined_struct_node, tok);
                Node *struct_ref_node = calloc(1, sizeof(Node));
                struct_ref_node->offset = offset;
                struct_ref_node->kind = ND_STRUCTREF_PTR;
                struct_ref_node->lhs = struct_ref(defined_struct_node, tok);
                node->lhs = struct_ref_node;
                node->type = NULL;
                return node;
            }
            if (consume(".")) {
                Token *struct_type_token = calloc(1, sizeof(Token));
                struct_type_token->str = lvar->type->type_name;
                struct_type_token->len = lvar->type->type_name_len;
                Node *defined_struct_node = find_defined_structs(struct_type_token);
                Token *tok = consume_indent();
                int offset = get_struct_node_offset(defined_struct_node, tok);
                Node *struct_ref_node = calloc(1, sizeof(Node));
                struct_ref_node->offset = offset;
                struct_ref_node->kind = ND_STRUCTREF;
                struct_ref_node->lhs = struct_ref(defined_struct_node, tok);
                node->lhs = struct_ref_node;
                node->type = NULL;
                return node;
            }
            return node;
        } else if(gvar) {
            Node *node = calloc(1, sizeof(Node));
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
            if (consume("->")) {
                Token *struct_type_token = calloc(1, sizeof(Token));
                struct_type_token->str = gvar->type->ptr_to->type_name;
                struct_type_token->len = gvar->type->ptr_to->type_name_len;
                Node *defined_struct_node = find_defined_structs(struct_type_token);
                Token *tok = consume_indent();
                int offset = get_struct_node_offset(defined_struct_node, tok);
                Node *struct_ref_node = calloc(1, sizeof(Node));
                struct_ref_node->offset = offset;
                struct_ref_node->kind = ND_STRUCTREF_PTR;
                struct_ref_node->lhs = struct_ref(defined_struct_node, tok);
                node->lhs = struct_ref_node;
                node->type = NULL;
                return node;
            }
            if (consume(".")) {
                Token *struct_type_token = calloc(1, sizeof(Token));
                struct_type_token->str = gvar->type->type_name;
                struct_type_token->len = gvar->type->type_name_len;
                Node *defined_struct_node = find_defined_structs(struct_type_token);
                Token *tok = consume_indent();
                int offset = get_struct_node_offset(defined_struct_node, tok);
                Node *struct_ref_node = calloc(1, sizeof(Node));
                struct_ref_node->offset = offset;
                struct_ref_node->kind = ND_STRUCTREF;
                struct_ref_node->lhs = struct_ref(defined_struct_node, tok);
                node->lhs = struct_ref_node;
                node->type = NULL;
                return node;
            }
            return node;
        } else if(enums_index >= 0) {
            return new_node_num(enums_index);
        } else {
            char t[64];
            mysubstr(t, tok->str, 0, tok->len);
            error("%s is not defined.", t);
        }
        return NULL;
    }
    if (token->kind == TK_STR) {
        Node *node = calloc(1, sizeof(Node));
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
    if (token->kind == TK_STRUCT) {
        if(consume_kind(TK_STRUCT)){
            Token *struct_type_token = consume_indent();
            Node *defined_struct_node = find_defined_structs(struct_type_token);
            return defined_struct_node;
        }
    }
    if (token->kind == TK_TRUE) {
        token = token->next;
        return new_node_num(1);
    }
    if (token->kind == TK_FALSE) {
        token = token->next;
        return new_node_num(0);
    }

    return new_node_num(expect_number());
}

