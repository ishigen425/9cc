int *x[2]; int main(){ int y; int z; x[0] = &y; x[1] = &z; y = 3; z = 7; return *(x[0]) + *(x[1]); }
