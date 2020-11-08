#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

bool consume_kind(TokenKind kind) {
    if (token->kind == kind) {
        token = token->next;
        return true;
    }
    return false;
}

Token *consume_indent() {
    if (token->kind == TK_INDENT){
        Token *now = token;
        token = token->next;
        return now;
    }
    return NULL;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        error_at(token->str, user_input, "expected \"%s\"", op); 
    }
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, user_input, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            (c == '_');
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
 }

 void tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;
    user_input = p;

    while (*p) {
        if(isspace(*p)) {
            p++;
            continue;
        }

        // Multi-letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (startswith(p, "for ") || startswith(p, "for(")) {
            cur = new_token(TK_FOR, cur, p, 3);
            p += 3;
            continue;
        }

        if (startswith(p, "if ") || startswith(p, "if(")) {
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }

        if (startswith(p, "else ")) {
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }

        if (startswith(p, "while ") || startswith(p, "while(")){
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }

        if (startswith(p, "return ")) {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        if (strchr("+-*/)(<>=;{},&", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        } else {
            int len = 0;
            while(is_alnum(*(p+len))) {
                len++;
            }
            cur = new_token(TK_INDENT, cur, p, len);
            p += len;
            continue;
        }

        error_at(p, user_input, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    token = head.next;
    return;
}
