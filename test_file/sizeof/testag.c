struct test {
    int x;
    int y;
    int z;
};
int main() {
    struct test *t;
    t = calloc(sizeof(struct test));
    t->x = 1;
    return t->x;
}
