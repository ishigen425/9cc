struct test {
    int x;
    int y;
    int z;
};
int main() {
    struct test t;
    t.x = 1;
    t.y = 2;
    t.z = 3;
    return t.x + t.y + t.z;
}
