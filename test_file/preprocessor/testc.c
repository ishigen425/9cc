#define ROOP10 int i; for ( i = 0;i < 10; i = i + 1)

int main() {
    int a; a = 0;
    ROOP10 {
        a = a + i;
    }
    return a;
}
