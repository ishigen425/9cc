struct test {
    int x;
    int y;
};
int main() {
    struct test *t;
    t = calloc(1, 8);
    t->x = 1;
    t->y = 2;
    struct test *tt;
    tt = calloc(1, 8);
    tt->x = 3;
    tt->y = 0;
    return t->x + tt->x + t->y + tt->y;
}
