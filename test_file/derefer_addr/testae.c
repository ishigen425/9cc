char *f(){
    char *ret;
    ret = "Hello World!\n";
    return ret;
}

int *ff(){
    int *ret;
    ret = malloc(8);
    *ret = 10;
    return ret;
}

int main() {
    printf(f());
    return *ff();
}

