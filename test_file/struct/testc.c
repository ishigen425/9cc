struct test {
    int x;
};
int main() {
    struct test *t;
    t->x = 1;
    return t->x;
}
