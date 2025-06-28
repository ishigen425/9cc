void void_param(int x) { 
    if (x > 10) return; 
}
int main() { void_param(5); void_param(15); return 77; }