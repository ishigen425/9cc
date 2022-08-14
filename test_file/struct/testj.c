#include <stdlib.h>
#include <stdio.h>

struct test0 {
    int x;
    int y;
};

struct test1 {
    int x;
    struct test0 w;
};

struct test2 {
    int x;
    struct test1 *w;
};

struct test2 *t_global;

int main() {
    struct test2 *t;
    t = calloc(1, sizeof(struct test2));
    t->w = calloc(1, sizeof(struct test1));
    t->w->w.x = 10;
    t_global = calloc(1, sizeof(struct test2));
    t_global->w = calloc(1, sizeof(struct test1));
    t_global->w->w.x = 11;
    return t->w->w.x + t_global->w->w.x;
}
