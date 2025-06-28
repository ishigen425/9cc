#define NULL ((void *)0)

int main() {
    int *p = NULL;
    if (p == NULL) {
        return 42;
    }
    return 0;
}