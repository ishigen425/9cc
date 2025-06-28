struct test0 { int x; int y; };
typedef struct test0 Test0;
struct test1 {
    Test0 t;
    int z;
};
int main() {
    struct test1 v;
    v.t.x = 5;
    v.t.y = 7;
    v.z = 3;
    return v.t.x + v.t.y + v.z;
}
