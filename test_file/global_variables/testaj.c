int memo[30]; int fib(int x){ if(x == 0){ return 0; } if(x == 1){ memo[1] = 1; return 1; } memo[x] = fib(x-1) + fib(x-2); return memo[x]; } int main() { return fib(9); } 
