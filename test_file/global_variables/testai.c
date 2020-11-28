int fib[30]; int main() { int i; fib[0] = 0; fib[1] = 1; for(i = 2; i < 30; i = i+1) { fib[i] = fib[i-1] + fib[i-2]; } return fib[8]; } 
