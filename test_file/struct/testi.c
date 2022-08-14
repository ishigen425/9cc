#include <stdlib.h>
#include <stdio.h>

struct test0 {
    int x;
};

struct test1 {
    int x;
    struct test0 *w;
};

struct test2 {
    int x;
    struct test1 *w;
};

struct test3 {
    int x;
    struct test2 *w;
};

struct test3 *t;

int main() {
    t = calloc(1, sizeof(struct test3));
    t->w = calloc(1, sizeof(struct test2));
    t->w->w = calloc(1, sizeof(struct test1));
    t->w->w->w = calloc(1, sizeof(struct test0));
    t->x = 1;
    t->w->x = 4;
    t->w->w->x = 5;
    t->w->w->w->x = 2;
    return t->x + t->w->x + t->w->w->x + t->w->w->w->x;
}
