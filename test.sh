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
    cc -static -no-pie -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 'test_file/four_arithmetic_operations/testaa.c'
assert 42 'test_file/four_arithmetic_operations/testab.c'
assert 21 'test_file/four_arithmetic_operations/testac.c'
assert 41 'test_file/four_arithmetic_operations/testad.c'
assert 47 'test_file/four_arithmetic_operations/testae.c'
assert 15 'test_file/four_arithmetic_operations/testaf.c'
assert 4 'test_file/four_arithmetic_operations/testag.c'
assert 37 'test_file/four_arithmetic_operations/testah.c'
assert 6 'test_file/four_arithmetic_operations/testai.c'
assert 6 'test_file/four_arithmetic_operations/testaj.c'
assert 20 'test_file/four_arithmetic_operations/testak.c'
assert 20 'test_file/four_arithmetic_operations/testal.c'
assert 10 'test_file/four_arithmetic_operations/testam.c'
assert 1 'test_file/four_arithmetic_operations/testan.c'
assert 1 'test_file/four_arithmetic_operations/testao.c'
assert 0 'test_file/four_arithmetic_operations/testap.c'

assert 0 'test_file/relational_operator/testaa.c'
assert 1 'test_file/relational_operator/testab.c'
assert 1 'test_file/relational_operator/testac.c'
assert 0 'test_file/relational_operator/testad.c'
assert 1 'test_file/relational_operator/testae.c'
assert 0 'test_file/relational_operator/testaf.c'
assert 0 'test_file/relational_operator/testag.c'
assert 1 'test_file/relational_operator/testah.c'
assert 1 'test_file/relational_operator/testai.c'
assert 0 'test_file/relational_operator/testaj.c'
assert 1 'test_file/relational_operator/testak.c'
assert 0 'test_file/relational_operator/testal.c'
assert 0 'test_file/relational_operator/testam.c'
assert 1 'test_file/relational_operator/testan.c'
assert 1 'test_file/relational_operator/testao.c'
assert 0 'test_file/relational_operator/testap.c'

assert 2 'test_file/variables/testaa.c'
assert 4 'test_file/variables/testab.c'
assert 2 'test_file/variables/testac.c'
assert 2 'test_file/variables/testad.c'
assert 2 'test_file/variables/testae.c'
assert 3 'test_file/variables/testaf.c'
assert 20 'test_file/variables/testag.c'
assert 6 'test_file/variables/testah.c'
assert 6 'test_file/variables/testai.c'
assert 6 'test_file/variables/testaj.c'
assert 2 'test_file/variables/testak.c'
assert 2 'test_file/variables/testal.c'

assert 1 'test_file/while_statement/testaa.c'
assert 11 'test_file/while_statement/testab.c'
assert 18 'test_file/while_statement/testac.c'
assert 1 'test_file/while_statement/testad.c'
assert 1 'test_file/while_statement/testae.c'

assert 2 'test_file/if_statement/testaa.c'
assert 2 'test_file/if_statement/testab.c'
assert 1 'test_file/if_statement/testac.c'
assert 1 'test_file/if_statement/testad.c'
assert 2 'test_file/if_statement/testae.c'
assert 3 'test_file/if_statement/testaf.c'
assert 2 'test_file/if_statement/testag.c'
assert 3 'test_file/if_statement/testah.c'

assert 10 'test_file/if_statement/testai.c'
assert 5 'test_file/if_statement/testaj.c'

assert 10 'test_file/for_statement/testaa.c'
assert 1 'test_file/for_statement/testab.c'
assert 0 'test_file/for_statement/testac.c'
assert 0 'test_file/for_statement/testad.c'
assert 1 'test_file/for_statement/testae.c'

assert 10 'test_file/block/testaa.c'
assert 16 'test_file/block/testab.c'
assert 32 'test_file/block/testac.c'

assert 3 'test_file/call_func/testaa.c'
assert 5 'test_file/call_func/testab.c'
assert 3 'test_file/call_func/testac.c'
assert 5 'test_file/call_func/testad.c'
assert 15 'test_file/call_func/testae.c'
assert 3 'test_file/call_func/testaf.c'
assert 2 'test_file/call_func/testag.c'
assert 3 'test_file/call_func/testah.c'
assert 3 'test_file/call_func/testai.c'
assert 0 'test_file/call_func/testaj.c'
assert 4 'test_file/call_func/testak.c'
assert 8 'test_file/call_func/testal.c'
assert 24 'test_file/call_func/testam.c'
assert 21 'test_file/call_func/testan.c'
assert 21 'test_file/call_func/testao.c'
assert 24 'test_file/call_func/testap.c'
assert 1 'test_file/call_func/testaq.c'

assert 7 'test_file/def_func/testaa.c'
assert 10 'test_file/def_func/testab.c'
assert 2 'test_file/def_func/testac.c'
assert 13 'test_file/def_func/testad.c'
assert 21 'test_file/def_func/testae.c'
assert 14 'test_file/def_func/testaf.c'
assert 15 'test_file/def_func/testag.c'
assert 22 'test_file/def_func/testah.c'
assert 63 'test_file/def_func/testai.c'

assert 3 'test_file/derefer_addr/testaa.c'
assert 3 'test_file/derefer_addr/testab.c'
assert 3 'test_file/derefer_addr/testac.c'
assert 3 'test_file/derefer_addr/testad.c'
assert 10 'test_file/derefer_addr/testae.c'

# chibiccの実装でもint型の配列は8バイトずつの値として扱っていたので、一旦それに合わせるためにコメントアウトしておく。後から修正するはず。
# assert 1 'test_file/ptr_add/testaa.c'
# assert 32 'test_file/ptr_add/testab.c'
# assert 36 'test_file/ptr_add/testac.c'
# assert 42 'test_file/ptr_add/testad.c'
# assert 7 'test_file/ptr_add/testae.c'
# assert 11 'test_file/ptr_add/testaf.c'
# assert 23 'test_file/ptr_add/testag.c'
# assert 34 'test_file/ptr_add/testah.c'
assert 101 'test_file/ptr_add/testai.c'

assert 4 'test_file/sizeof/testaa.c'
assert 4 'test_file/sizeof/testab.c'
assert 4 'test_file/sizeof/testac.c'
assert 8 'test_file/sizeof/testad.c'
assert 8 'test_file/sizeof/testae.c'
assert 8 'test_file/sizeof/testaf.c'
assert 1 'test_file/sizeof/testag.c'

assert 2 'test_file/array/testaa.c'
assert 3 'test_file/array/testab.c'
assert 3 'test_file/array/testac.c'
assert 3 'test_file/array/testad.c'
assert 3 'test_file/array/testae.c'
assert 1 'test_file/array/testaf.c'
assert 6 'test_file/array/testag.c'
assert 9 'test_file/array/testah.c'
assert 3 'test_file/array/testai.c'
assert 21 'test_file/array/testaj.c'
assert 225 'test_file/array/testak.c'

assert 0 'test_file/global_variables/testaa.c'
assert 1 'test_file/global_variables/testab.c'
assert 3 'test_file/global_variables/testac.c'
assert 8 'test_file/global_variables/testad.c'
assert 10 'test_file/global_variables/testae.c'
assert 10 'test_file/global_variables/testaf.c'
assert 10 'test_file/global_variables/testag.c'
assert 10 'test_file/global_variables/testah.c'
assert 21 'test_file/global_variables/testai.c'
assert 34 'test_file/global_variables/testaj.c'
assert 0 'test_file/global_variables/testak.c'
assert 2 'test_file/global_variables/testal.c'
assert 10 'test_file/global_variables/testam.c'
assert 10 'test_file/global_variables/testan.c'
assert 10 'test_file/global_variables/testao.c'

assert 0 'test_file/char/testaa.c'
assert 3 'test_file/char/testab.c'
assert 3 'test_file/char/testac.c'
assert 0 'test_file/char/testad.c'
assert 0 'test_file/char/testae.c'
assert 65 'test_file/char/testaf.c'
assert 67 'test_file/char/testag.c'

assert 1 'test_file/comment/test.c'

assert 0 'test_file/struct/testa.c'
assert 0 'test_file/struct/testb.c'
assert 1 'test_file/struct/testc.c'
assert 6 'test_file/struct/testd.c'
assert 7 'test_file/struct/teste.c'
assert 6 'test_file/struct/testf.c'
assert 42 'test_file/struct/testg.c'
assert 12 'test_file/struct/testh.c'
assert 12 'test_file/struct/testi.c'
assert 26 'test_file/struct/testj.c'
assert 10 'test_file/struct/testk.c'

assert 20 'test_file/enum/testa.c'

assert 43 'test_file/preprocessor/testa.c'
assert 2 'test_file/preprocessor/testb.c'
assert 45 'test_file/preprocessor/testc.c'

echo OK

