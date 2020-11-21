#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
#include <stdlib.h>

int ret3() { return 3; }
int ret5() { return 5; }
int add2(int x) { return x + 2; }
int mul2(int x) { return x * 2; }
int sum6(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; }
int mul3vardiv3var(int a, int b, int c, int d, int e, int f) { return a * b * c / d / e / f; }
int mul2var(int a, int b) { return a * b; }
int *alloc4(int x1, int x2, int x3, int x4){
  int *start = (int *)malloc(4);
  start[1] = x1;
  start[2] = x2;
  start[3] = x3;
  start[4] = x4;
  return start;
}
EOF

assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 'int main(){ 0;}'
assert 42 'int main(){ 42;}'
assert 21 'int main(){ 5+20-4;}'
assert 41 'int main(){  12 + 34 - 5;}'
assert 47 'int main(){ 5+6*7;}'
assert 15 'int main(){ 5*(9-6);}'
assert 4 'int main(){ (3+5)/2;}'
assert 37 'int main(){ -5+6*7;}'
assert 6 'int main(){ +10-+4;}'
assert 6 'int main(){ +10+-4;}'
assert 20 'int main(){ -20+40;}'
assert 20 'int main(){ - -20;}'
assert 10 'int main(){ ----10;}'

assert 0 'int main(){ 0==1;}'
assert 1 'int main(){ 10==10;}'
assert 1 'int main(){ 3!=2;}'
assert 0 'int main(){ 4!=4;}'
assert 1 'int main(){ 0<1;}'
assert 0 'int main(){ 1<1;}'
assert 0 'int main(){ 2<1;}'
assert 1 'int main(){ 0<=1;}'
assert 1 'int main(){ 1<=1;}'
assert 0 'int main(){ 2<=1;}'
assert 1 'int main(){ 1>0;}'
assert 0 'int main(){ 1>1;}'
assert 0 'int main(){ 1>2;}'
assert 1 'int main(){ 1>=0;}'
assert 1 'int main(){ 1>=1;}'
assert 0 'int main(){ 1>=2;}'

assert 2 'int main(){ int a;a=1;1+a;}'
assert 4 'int main(){ int b;b=2;b+2;}'
assert 2 'int main(){ int x;x=1;1+x;}'
assert 2 'int main(){ int y;y=1;1+y;}'
assert 2 'int main(){ int z;z=1;1+z;}'

assert 3 'int main(){ int abc;abc=3;abc+0;}'
assert 20 'int main(){ int test;test=10;test+test;}'
assert 6 'int main(){ int foo;int bar;foo=1;bar=2+3;foo+bar;}'

assert 6 'int main(){ int foo;int bar;foo=1;bar=2+3;return foo+bar;}'
assert 6 'int main(){ int foo;int bar;foo=0;bar=2*3;return foo+bar;}'
assert 2 'int main(){ int returnx;returnx=1;returnx*2;}'
assert 2 'int main(){ int aa;aa=1;aa=aa+1;aa;}'

assert 1 'int main(){ int a;a=0;while(a==0)a=a+1;a;}'
assert 11 'int main(){ int a;a=0;while(a<=10)a=a+1;a+0;}'
assert 18 'int main(){ int a;a=-3;while(a<16)a=a+7;a;}'
assert 1 'int main(){ int a;a=5;while ( a > 1 ) a = a-1;a;}'
assert 1 'int main(){ int a;a=1;while(a<1)a=2;a;}'

assert 2 'int main(){ int a;a=1;if(a==1)a=2;a;}'
assert 2 'int main(){ int a;a=1;if(a<=1)a=2;a;}'
assert 1 'int main(){ int a;a=1;if(10<1)a=2;a;}'
assert 1 'int main(){ int a;a=1;if(a>1)a=2;a;}'

assert 2 'int main(){ int a;a=1;if(a==1)a=2;else a=3;a;}'
assert 3 'int main(){ int a;a=1;if(a!=1)a=2;else a=3;a;}'
assert 2 'int main(){ int a;a=1;if(a==1)return 2;else return 3;}'
assert 3 'int main(){ int a;a=1;if(a!=1)return 2;else return 3;}'

assert 10 'int main(){ int b;b=3;while(b<6)b=b+1;if(b>=5)b=10;b;}'
assert 5 'int main(){ int b;b=3;while(b<6)if(b>=1)return 5;b;}'

assert 10 'int main(){ int c;int d;c=0;for(c=0;c<10;c=c+1)d=1;c;}'
assert 1 'int main(){ int c;int d;c=0;for(c=0;c<10;c=c+1)d=1;d;}'
assert 0 'int main(){ int d;d=10;for(;d>0;d=d-1)d;}'
assert 0 'int main(){ int d;int e;d=10;for(e=1;d>0;d=d-1)d;}'
assert 1 'int main(){ int cda;cda=1;for(cda=10;cda<20;cda=cda+1)if(cda==15)return 1;}'

assert 10 'int main(){ int a;int d;a=1;d=0;if(a==1){a=2;d=10;}d;}'
assert 16 'int main(){ int a;int d;a=0;d=1;while(a<4){a=a+1;d=d*2;}d;}'
assert 32 'int main(){ int a;int d;a=0;d=1;for(a=0;a<5;a=a+0){a=a+1;d=d*2;}d;}'

assert 3 'int main(){ ret3();}'
assert 5 'int main(){ ret5();}'

assert 3 'int main(){ return ret3();}'
assert 5 'int main(){ return ret5();}'

assert 15 'int main(){ return ret3() * ret5();}'

assert 3 'int main(){ add2(1);}'
assert 2 'int main(){ add2(0);}'
assert 3 'int main(){ int a;a=1;add2(a);}'
assert 3 'int main(){ int a;a=1;return add2(a);}'

assert 0 'int main(){ mul2(0);}'
assert 4 'int main(){ mul2(2);}'
assert 8 'int main(){ add2(mul2(3));}'
assert 24 'int main(){ mul2(add2(mul2(add2(ret3()))));}'

assert 21 'int main(){ sum6(1,2,3,4,5,6);}'
assert 21 'int main(){ return sum6(1,2,3,4,5,6); }'
assert 24 'int main(){ return mul2var(3, 8); }'
assert 1 'int main(){ return mul3vardiv3var(1,2,3,1,2,3); }'

assert 7 'int ret7(){ return 7; }int main(){ return ret7(); }'
assert 10 'int no(int x){ return 10; }int main() { return no(10); }'
assert 2 'int sub(int x){ return x - 2; }int main(){ return sub(4); }'
assert 13 'int fib(int x){ if(x==1){ return 1; }if(x==0){ return 0; } return fib(x-2) + fib(x-1); }int main(){ return fib(7); }'
assert 21 'int fib(int x){ if(x==1){ return 1; }if(x==0){ return 0; } return fib(x-2) + fib(x-1); }int main(){ return fib(8); }'
assert 14 'int add2var(int x,int y){ return x + y; }int main(){ return add2var(6, 8); }'
assert 15 'int add5var(int a,int b,int c,int d,int e){ return a + b + c + d + e; }int main(){ return add5var(1, 2, 3, 4, 5); }'
assert 22 'int add5var(int a,int b,int c,int d,int e,int f){ return a + b + c + d + e + f; }int main(){ return add5var(1, 2, 3, 4, 5, 7); }'

assert 3 'int main() { int x;int y;x = 3; y = &x; return *y; }'
assert 3 'int main() { int x;int y;int z;x = 3; y = 0; z = &y + 8; return *z;}'

assert 3 'int main() { int x; int *y; y = &x; *y = 3; return x; }'
assert 3 'int main() { int x; int **y; int z; y = &x; *y = &z; **y = 3; return z; }'

assert 1 'int main() { int *p; p = alloc4(1,32,36,42); p = p + 1; return *p; }'
assert 32 'int main() { int *p; p = alloc4(1,32,36,42); p = p + 2; return *p; }'
assert 36 'int main() { int *p; p = alloc4(1,32,36,42); p = p + 3; return *p; }'
assert 42 'int main() { int *p; p = alloc4(1,32,36,42); p = p + 4; return *p; }'

assert 7 'int main() { int *p; p = alloc4(7,11,23,34); p = p + 4; p = p - 3; return *p; }'
assert 11 'int main() { int *p; p = alloc4(7,11,23,34); p = p + 4; p = p - 2; return *p; }'
assert 23 'int main() { int *p; p = alloc4(7,11,23,34); p = p + 4; p = p - 1; return *p; }'
assert 34 'int main() { int *p; p = alloc4(7,11,23,34); p = p + 4; p = p - 0; return *p; }'

assert 4 'int main() { return sizeof(1); }'
assert 4 'int main() { return sizeof(sizeof(10)); }'
assert 4 'int main() { int *x; return sizeof(sizeof(x)); }'
assert 8 'int main() { int *x; return sizeof(x); }'
assert 8 'int main() { int *x; return sizeof(x + 3); }'

assert 2 'int main() { int a[2]; *(a + 1) = 2; return *(a + 1); }'
assert 3 'int main() { int a[2]; *(a + 0) = 1; *(a + 1) = 2; return *(a + 0) + *(a + 1); }'
assert 3 'int main() { int a[2]; *a = 1; *(a + 1) = 2; return *a + *(a + 1); }'
assert 3 'int main() { int a[2]; *(a + 0) = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1); }'
assert 3 'int main() { int a[3]; return sizeof(a); }'

echo OK

