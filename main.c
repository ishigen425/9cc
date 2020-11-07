#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズしてパースする
    tokenize(argv[1]);
    program();
    
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    
    // プロローグ
    // 変数26個分の領域を確保する
    for (int i = 0; code[i]; i++){
        gen(code[i]);
    }
    
    return 0;
}

