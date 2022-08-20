
struct test {
    bool result;
    int val;
};

int main() {
    struct test *t;
    t = calloc(1, sizeof(struct test));
    t->result = true;
    return t->result;
}
