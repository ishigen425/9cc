int main() { int x; int **y; int z; y = &x; *y = &z; **y = 3; return z; }
