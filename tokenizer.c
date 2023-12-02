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

void expect_type(char *op) {
    if (strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        error_at(token->str, user_input, "expected \"%s\"", op); 
    }
    token = token->next;
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
        error_at(token->str, user_input, "Not Number", "");
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

char *exclude_double_quote(char *p, int include_len, int *exclude_len) {
    char *str = malloc(sizeof(char) * include_len);
    int str_idx = 0;
    for(int i = 0; i < include_len; i++) {
        if(memcmp(p-include_len-1+i, "\\\"", 2) == 0){
            continue;
        }
        memcpy(&(str[str_idx]), p-include_len-1+i, 1);
        str_idx++;
    }
    *exclude_len = str_idx;
    for(;str_idx < include_len; str_idx++) str[str_idx] = '\0';
    return str;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
 }

Token head;
Token *cur = &head;

char *tokenize_until_end_char(char *p, char *end_char) {
    while (memcmp(p, end_char, 1) != 0) {
        if(isspace(*p)) {
            p++;
            continue;
        }

        // ignore include
        if (startswith(p, "#include \"")) {
            p += 10;
            while (!startswith(p, "\"")) p++;
            p++;
            continue;
        }

        if (startswith(p, "#include <")) {
            p += 10;
            while (!startswith(p, ">")) p++;
            p++;
            continue;
        }

        if (startswith(p, "#define ")) {
            cur = new_token(TK_DEFINE, cur, p, 8);
            p += 8;
            p = tokenize_until_end_char(p, "\n");
            cur = new_token(TK_DEFINE_END, cur, "", 0);
            continue;
        }

        if (startswith(p, "typedef ")) {
            cur = new_token(TK_TYPEDEF, cur, p, 7);
            p += 7;
            continue;
        }

        if (startswith(p, "enum ")) {
            cur = new_token(TK_ENUM, cur, p, 4);
            p += 4;
            continue;
        }

        if (startswith(p, "true;") || startswith(p, "true ")) {
            cur = new_token(TK_TRUE, cur, p, 4);
            p += 4;
            continue;
        }

        if (startswith(p, "false;") || startswith(p, "false ")) {
            cur = new_token(TK_FALSE, cur, p, 5);
            p += 5;
            continue;
        }

        // Multi-letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (startswith(p, "&&")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if(startswith(p, "||")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if(startswith(p, "!")) {
            cur = new_token(TK_RESERVED, cur, p, 1);
            p++;
            continue;
        }

        if (startswith(p, ".")){
            cur = new_token(TK_RESERVED, cur, p, 1);
            p++;
            continue;
        }

        if (startswith(p, "->")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (startswith(p, "struct ") || startswith(p, "struct{")) {
            cur = new_token(TK_STRUCT, cur, p, 6);
            p += 6;
            continue;
        }

        if (startswith(p, "sizeof ") || startswith(p, "sizeof(")) {
            cur = new_token(TK_SIZEOF, cur, p, 6);
            p += 6;
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

        if (startswith(p, "int ")) {
            cur = new_token(TK_INT, cur, p, 3);
            p += 3;
            continue;
        }

        if (startswith(p, "char ")) {
            cur = new_token(TK_CHAR, cur, p, 4);
            p += 4;
            continue;
        }

        if (startswith(p, "bool ")) {
            cur = new_token(TK_BOOL, cur, p, 4);
            p += 4;
            continue;
        }

        if (startswith(p, "//")) {
            p += 2;
            while (*p != '\n'){
                p++;
            }
            continue;
        }

        if (startswith(p, "/*")) {
            p += 2;
            while (!startswith(p, "*/") && *p != '\0'){
                p++;
            }
            if (*p == '\0') {
                error_at(p, user_input,"Comment not closed", "");
            }
            p += 2;
            continue;
        }

        if (startswith(p, "\"")){
            int len = 0;
            p++;
            while(!startswith(p, "\"")){
                if(startswith(p, "\\\"")) {
                    p += 2;
                    len += 2;
                }
                p++;
                len++;
            }
            p++;
            int exclude_len;
            char *str = exclude_double_quote(p, len, &exclude_len);
            cur = new_token(TK_STR, cur, str, exclude_len);
            continue;
        }

        if (startswith(p, "'")){
            p++;
            cur = new_token(TK_LT_CHAR, cur, p, 1);
            p++;
            if (!startswith(p, "'")) {
                error_at(p, user_input, "expected '\%c'", "");
            }
            p++;
            continue;
        }

        if (strchr("+-*/)(<>=;{},&[]%", *p)) {
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

        error_at(p, user_input, "Can not tokenized", "");
    }
    return p;
}

void tokenize(char *p) {
    head.next = NULL;
    user_input = p;
    
    p = tokenize_until_end_char(p, "\0");

    new_token(TK_EOF, cur, p, 0);
    token = head.next;
    return;
}

