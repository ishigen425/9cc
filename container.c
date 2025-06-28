#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "9cc.h"

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var ; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

GVar *find_gvar(Token *tok) {
    for (GVar *var = globals; var ; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

GVar *find_gvar_literals(Token *tok) {
    for (GVar *var = literals; var ; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

Node *find_defined_structs(Token *tok) {
    for (Node *var = defined_structs; var; var = var->child) {
        if (var->namelen == tok->len && !memcmp(tok->str, var->name, var->namelen))
            return var;
    }
    return NULL;
}

int find_defined_enum(Token *tok) {
    for (EnumDef *var = defined_enums; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->str, var->len)) {
            return var->index;
        }
    }
    return -1;
}

Typedef *find_typedef(Token *tok) {
    for (Typedef *td = defined_typedefs; td; td = td->next) {
        if (td->len == tok->len && !memcmp(tok->str, td->name, td->len))
            return td;
    }
    return NULL;
}
