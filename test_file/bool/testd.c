bool is_mod2(int val) {
    if (val % 2 == 0) {
        return true;
    }
    return false;
}

int main() {
    if(is_mod2(4)){
        return 2;
    }
    return 5;
}
