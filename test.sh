#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add2(int x) { return x + 2; }
int mul2(int x) { return x * 2; }
int sum6(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; }
int mul3vardiv3var(int a, int b, int c, int d, int e, int f) { return a * b * c / d / e / f; }
int mul2var(int a, int b) { return a * b; }
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

assert 0 'main(){ 0;}'
assert 42 'main(){ 42;}'
assert 21 'main(){ 5+20-4;}'
assert 41 'main(){  12 + 34 - 5;}'
assert 47 'main(){ 5+6*7;}'
assert 15 'main(){ 5*(9-6);}'
assert 4 'main(){ (3+5)/2;}'
assert 37 'main(){ -5+6*7;}'
assert 6 'main(){ +10-+4;}'
assert 6 'main(){ +10+-4;}'
assert 20 'main(){ -20+40;}'
assert 20 'main(){ - -20;}'
assert 10 'main(){ ----10;}'

assert 0 'main(){ 0==1;}'
assert 1 'main(){ 10==10;}'
assert 1 'main(){ 3!=2;}'
assert 0 'main(){ 4!=4;}'
assert 1 'main(){ 0<1;}'
assert 0 'main(){ 1<1;}'
assert 0 'main(){ 2<1;}'
assert 1 'main(){ 0<=1;}'
assert 1 'main(){ 1<=1;}'
assert 0 'main(){ 2<=1;}'
assert 1 'main(){ 1>0;}'
assert 0 'main(){ 1>1;}'
assert 0 'main(){ 1>2;}'
assert 1 'main(){ 1>=0;}'
assert 1 'main(){ 1>=1;}'
assert 0 'main(){ 1>=2;}'

assert 2 'main(){ a=1;1+a;}'
assert 4 'main(){ b=2;b+2;}'
assert 2 'main(){ x=1;1+x;}'
assert 2 'main(){ y=1;1+y;}'
assert 2 'main(){ z=1;1+z;}'

assert 3 'main(){ abc=3;abc+0;}'
assert 20 'main(){ test=10;test+test;}'
assert 6 'main(){ foo=1;bar=2+3;foo+bar;}'

assert 6 'main(){ foo=1;bar=2+3;return foo+bar;}'
assert 6 'main(){ foo=0;bar=2*3;return foo+bar;}'
assert 2 'main(){ returnx=1;returnx*2;}'
assert 2 'main(){ aa=1;aa=aa+1;aa;}'

assert 1 'main(){ a=0;while(a==0)a=a+1;a;}'
assert 11 'main(){ a=0;while(a<=10)a=a+1;a+0;}'
assert 18 'main(){ a=-3;while(a<16)a=a+7;a;}'
assert 1 'main(){ a=5;while ( a > 1 ) a = a-1;a;}'
assert 1 'main(){ a=1;while(a<1)a=2;a;}'

assert 2 'main(){ a=1;if(a==1)a=2;a;}'
assert 2 'main(){ a=1;if(a<=1)a=2;a;}'
assert 1 'main(){ a=1;if(10<1)a=2;a;}'
assert 1 'main(){ a=1;if(a>1)a=2;a;}'

assert 2 'main(){ a=1;if(a==1)a=2;else a=3;a;}'
assert 3 'main(){ a=1;if(a!=1)a=2;else a=3;a;}'
assert 2 'main(){ a=1;if(a==1)return 2;else return 3;}'
assert 3 'main(){ a=1;if(a!=1)return 2;else return 3;}'

assert 10 'main(){ b=3;while(b<6)b=b+1;if(b>=5)b=10;b;}'
assert 5 'main(){ b=3;while(b<6)if(b>=1)return 5;b;}'

assert 10 'main(){ c=0;for(c=0;c<10;c=c+1)d=1;c;}'
assert 1 'main(){ c=0;for(c=0;c<10;c=c+1)d=1;d;}'
assert 0 'main(){ d=10;for(;d>0;d=d-1)d;}'
assert 0 'main(){ d=10;for(e=1;d>0;d=d-1)d;}'
assert 1 'main(){ cda=1;for(cda=10;cda<20;cda=cda+1)if(cda==15)return 1;}'

assert 10 'main(){ a=1;d=0;if(a==1){a=2;d=10;}d;}'
assert 16 'main(){ a=0;d=1;while(a<4){a=a+1;d=d*2;}d;}'
assert 32 'main(){ a=0;d=1;for(a=0;a<5;a=a+0){a=a+1;d=d*2;}d;}'

assert 3 'main(){ ret3();}'
assert 5 'main(){ ret5();}'

assert 3 'main(){ return ret3();}'
assert 5 'main(){ return ret5();}'

assert 15 'main(){ return ret3() * ret5();}'

assert 3 'main(){ add2(1);}'
assert 2 'main(){ add2(0);}'
assert 3 'main(){ a=1;add2(a);}'
assert 3 'main(){ a=1;return add2(a);}'

assert 0 'main(){ mul2(0);}'
assert 4 'main(){ mul2(2);}'
assert 8 'main(){ add2(mul2(3));}'
assert 24 'main(){ mul2(add2(mul2(add2(ret3()))));}'

assert 21 'main(){ sum6(1,2,3,4,5,6);}'
assert 21 'main(){ return sum6(1,2,3,4,5,6); }'
assert 24 'main(){ return mul2var(3, 8); }'
assert 1 'main(){ return mul3vardiv3var(1,2,3,1,2,3); }'

assert 7 'ret7(){ return 7; }main(){ return ret7(); }'
assert 10 'no(x){ return 10; } main() { return no(10); }'
assert 2 'sub(x){ return x - 2; }main(){ return sub(4); }'
assert 13 'fib(x){ if(x==1){ return 1; }if(x==0){ return 0; } return fib(x-2) + fib(x-1); } main(){ return fib(7); }'
assert 21 'fib(x){ if(x==1){ return 1; }if(x==0){ return 0; } return fib(x-2) + fib(x-1); } main(){ return fib(8); }'
assert 14 'add2var(x, y){ return x + y; }main(){ return add2var(6, 8); }'
assert 15 'add5var(a,b,c,d,e){ return a + b + c + d + e; }main(){ return add5var(1, 2, 3, 4, 5); }'
assert 22 'add5var(a,b,c,d,e,f){ return a + b + c + d + e + f; }main(){ return add5var(1, 2, 3, 4, 5, 7); }'

echo OK

