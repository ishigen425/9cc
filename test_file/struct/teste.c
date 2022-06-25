struct test {
    int x;
    int y;
    int z;
};
int main() {
    struct test *t;
    t = calloc(1, 12);
    t->x = 1;
    t->y = 2;
    t->z = 4;
    return t->x + t->y + t->z;
}
