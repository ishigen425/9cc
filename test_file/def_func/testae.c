int fib(int x){ if(x==1){ return 1; }if(x==0){ return 0; } return fib(x-2) + fib(x-1); }int main(){ return fib(8); }
