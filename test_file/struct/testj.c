#include <stdlib.h>
#include <stdio.h>
typedef enum {
    TK_RESERVED, // 記号
    TK_INDENT,   // 識別子
    TK_NUM,      // 整数トークン
} TokenKind;

struct test0 {
    int x;
    int y;
};

struct test1 {
    int x;
    struct test0 w;
    TokenKind kind;
};

struct test2 {
    int x;
    struct test1 *w;
    struct test1 w1;
    TokenKind kind;
};

struct test2 *t_global;

int main() {
    struct test2 *t;
    t = calloc(1, sizeof(struct test2));
    t->w = calloc(1, sizeof(struct test1));
    t->w->w.x = 10;
    t->w1.w.x = 2;
    t->kind = TK_NUM;
    t_global = calloc(1, sizeof(struct test2));
    t_global->w = calloc(1, sizeof(struct test1));
    t_global->w->w.x = 11;
    t_global->w1.w.x = 3;
    t_global->kind = TK_INDENT;
    return t->w->w.x + t->w1.w.x + t_global->w->w.x + t_global->w1.w.x + t->kind - t_global->kind - t_global->kind;
}
