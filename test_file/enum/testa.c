typedef enum {
    INT,
    PTR,
    ARRAY,
    CHAR,
    STRUCT,
} TypeKind;

typedef enum {
    INT1,
    PTR1,
    ARRAY1,
    CHAR1,
    STRUCT1,
} TypeKind2;

int main() {
    return INT + PTR + ARRAY + CHAR + STRUCT +
        INT1 + PTR1 + ARRAY1 + CHAR1 + STRUCT1;
}
