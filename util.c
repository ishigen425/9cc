#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"


void error(char *fmt, ... ) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *user_input, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input - 1;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, " ^");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void debug_print(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

int mysubstr( char *t, char *s, int pos, int len ) {
    if( pos < 0 || len < 0 || len > strlen(s) )
        return -1;
    for( s += pos; *s != '\0' && len > 0; len-- )
        *t++ = *s++;
    *t = '\0';
    return 0;
}


 bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
 }
