
int main() {
    int a[3][2];
    a[0][0] = 1;
    a[0][1] = 2;
    a[1][0] = 3;
    a[1][1] = 4;
    a[2][0] = 5;
    a[2][1] = 6;
    int ans; ans = 0;
    int i; int j;
    for (i = 0; i < 3; i = i + 1) {
        for (j = 0; j < 2; j = j + 1) {
            ans = ans + a[i][j];
        }
    }
    return ans;
}
