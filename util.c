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

void error_at(char *loc, char *user_input, char *msg, char *op) {
    char *line = loc;
    while(user_input < line && line[-1] != '\n'){
        line--;
    }
    char *end = loc;
    while(*end != '\n'){
        end++;
    }
    int line_num = 1;
    for (char *p = user_input; p < line; p++){
        if(*p == '\n'){
            line_num++;
        }
    }
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end- line), line);

    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    fprintf(stderr, msg, op);
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

 bool is_struct_ref(NodeKind kind) {
    return kind == ND_STRUCTREF || kind == ND_STRUCTREF_PTR;
}

int get_struct_node_offset(Node *defined_struct_node, Token *tok) {
    int offset = 0;
    for (Node *struct_node_var = defined_struct_node->lhs; struct_node_var; struct_node_var = struct_node_var->child) {
        if (struct_node_var->namelen == tok->len && !memcmp(struct_node_var->name, tok->str, tok->len))
            offset += struct_node_var->offset;
    }
    return offset;
}
