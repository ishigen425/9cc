struct test {
    int x;
    int y;
    int z;
};

struct test *a;
struct test b;

int main() {
    a = calloc(1, sizeof(struct test));
    a->x = 1; a->y = 2; a->z = 3;
    b.x = 11; b.y = 12; b.z = 13;
    return a->x + a->y + a->z + b.x + b.y + b.z;
}
