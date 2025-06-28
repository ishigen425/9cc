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
    for (StructAlias *al = struct_aliases; al; al = al->next) {
        if ((al->tag_len == tok->len && !memcmp(tok->str, al->tag, al->tag_len)) ||
            (al->alias_len == tok->len && !memcmp(tok->str, al->alias, al->alias_len))) {
            for (Node *var = defined_structs; var; var = var->child) {
                if (var->namelen == al->tag_len && !memcmp(al->tag, var->name, al->tag_len))
                    return var;
            }
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_STRUCTDEF;
            node->name = al->tag;
            node->namelen = al->tag_len;
            return node;
        }
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

TypedefName *find_typedef(Token *tok) {
    for (TypedefName *td = typedefs; td; td = td->next) {
        if (td->len == tok->len && !memcmp(tok->str, td->name, td->len))
            return td;
    }
    return NULL;
}
