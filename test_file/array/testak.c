
int main() {
    int a[4][2][4];
    int i; int j; int k;
    for (i = 0; i < 4; i = i + 1) {
        for (j = 0; j < 2; j = j + 1) {
            for (k = 0; k < 4; k = k + 1){
                a[i][j][k] = i + j + k;
            }
        }
    }
    int ans; ans = 0;
    for (i = 0; i < 4; i = i + 1) {
        for (j = 0; j < 2; j = j + 1) {
            for (k = 0; k < 4; k = k + 1){
                ans = ans + a[i][j][k];
            }
        }
    }
    return ans;
}
