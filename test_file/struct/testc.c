struct test {
    int x;
};
int main() {
    struct test *t;
    t = calloc(1, 4);
    t->x = 1;
    return t->x;
}
