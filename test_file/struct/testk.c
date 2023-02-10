struct Token {
    struct Token *tok;
    int val;
    char *str;
    int len;
};

int main() {
    struct Token *token; token = malloc(1000);
    token->tok = malloc(1000);
    token->tok->len = 10;
    return token->tok->len;
}
