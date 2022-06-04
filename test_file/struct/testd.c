struct test {
    int x;
};
int main() {
    struct test *t;
    t->x = 1;
    struct test *tt;
    tt->x = 2;
    int a;
    return t->x + tt->x;
}
