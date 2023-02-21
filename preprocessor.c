#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "9cc.h"

#define HASHMAP_SIZE 1009
typedef struct{
    char *key;
    void *value;
    struct HashMapStruct *next;
}HashMapStruct;

HashMapStruct *hashmap[HASHMAP_SIZE];

void hashmap_init() {
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        hashmap[i] = NULL;
    }
}

int hashmap_hash(char *key) {
    int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];
        hash = hash % HASHMAP_SIZE;
    }
    return hash % HASHMAP_SIZE;
}

void hashmap_put(char *key, void *value) {
    int hash = hashmap_hash(key);
    HashMapStruct *new = calloc(1, sizeof(HashMapStruct));
    new->key = key;
    new->value = value;
    new->next = hashmap[hash];
    hashmap[hash] = new;
}

void *hashmap_get(char *key) {
    int hash = hashmap_hash(key);
    HashMapStruct *cur = hashmap[hash];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            return cur->value;
        }
        cur = cur->next;
    }
    return NULL;
}

char *memcpy_char(char *src, int len) {
    char *dest = calloc(len+1, sizeof(char));
    for (int i = 0; i < len; i++) {
        dest[i] = src[i];
    }
    return dest;
}

void deep_copy(Token *src_token, Token *dest_token) {
    dest_token->kind = src_token->kind;
    dest_token->str = memcpy_char(src_token->str, src_token->len);
    dest_token->len = src_token->len;
    dest_token->next = src_token->next;
    dest_token->val = src_token->val;
    return;
}

void preprocess() {
    Token *head = calloc(1, sizeof(Token));
    Token *new_token = calloc(1, sizeof(Token));
    head->next = new_token;
    Token *cur = token;
    while(cur->kind != TK_EOF) {
        if (cur->kind == TK_DEFINE) {
            cur = cur->next;
            Token *left_tok = cur;
            char *key = memcpy_char(left_tok->str, left_tok->len);
            cur = cur->next;
            Token *right_tok = cur;
            cur = cur->next;
            hashmap_put(key, right_tok);
            continue;
        }

        char *key = memcpy_char(cur->next->str, cur->next->len);
        Token *value = hashmap_get(key);
        if (value != NULL) {
            value->next = cur->next->next;
            cur->next = value;
        }
        deep_copy(cur, new_token);
        new_token->next = calloc(1, sizeof(Token));
        new_token = new_token->next;
        cur = cur->next;
    }
    new_token->kind = TK_EOF;
    token = head->next;
    return;
}
