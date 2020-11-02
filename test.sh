#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 37 '-5+6*7;'
assert 6 '+10-+4;'
assert 6 '+10+-4;'
assert 20 '-20+40;'
assert 20 '- -20;'
assert 10 '----10;'

assert 0 '0==1;'
assert 1 '10==10;'
assert 1 '3!=2;'
assert 0 '4!=4;'
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'
assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 2 'a=1;1+a;'
assert 4 'b=2;b+2;'
assert 2 'x=1;1+x;'
assert 2 'y=1;1+y;'
assert 2 'z=1;1+z;'

assert 3 'abc=3;abc+0;'
assert 20 'test=10;test+test;'
assert 6 'foo=1;bar=2+3;foo+bar;'

assert 6 'foo=1;bar=2+3;return foo+bar;'
assert 6 'foo=0;bar=2*3;return foo+bar;'
assert 2 'returnx=1;returnx*2;'
assert 2 'aa=1;aa=aa+1;aa;'

assert 1 'a=0;while(a==0)a=a+1;a;'
assert 11 'a=0;while(a<=10)a=a+1;a+0;'
assert 18 'a=-3;while(a<16)a=a+7;a;'
assert 1 'a=5;while ( a > 1 ) a = a-1;a;'
assert 1 'a=1;while(a<1)a=2;a;'

assert 2 'a=1;if(a==1)a=2;a;'
assert 2 'a=1;if(a<=1)a=2;a;'
assert 1 'a=1;if(10<1)a=2;a;'
assert 1 'a=1;if(a>1)a=2;a;'

assert 2 'a=1;if(a==1)a=2;else a=3;a;'
assert 3 'a=1;if(a!=1)a=2;else a=3;a;'
assert 10 'b=3;while(b<6)b=b+1;if(b>=5)b=10;b;'

echo OK

