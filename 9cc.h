// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_NUM,
    ND_ASSIGN,
    ND_LVAR,
    ND_RETURN,
    ND_WHILE,
    ND_IF,
    ND_IF_ELSE,
    ND_ELSE,
    ND_FOR,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *elsehs;   // else文での実行式
    Node *initstmt; // for文での初期化式
    Node *testexpr; // for文での条件式
    Node *updstmt;  // for文での変化式
    int val;        // kindがND_NUMの場合のみ使う
    int offset;     // kindがND_LVARの場合のみ使う
};

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_INDENT,   // 識別子
    TK_NUM,      // 整数トークン
    TK_RETURN,   // return
    TK_WHILE,    // while
    TK_IF,       // if
    TK_ELSE,     // else
    TK_FOR,      // for
    TK_EOF,      // 入力の終わり
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

void tokenize(char *p);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void error(char *fmt, ... );
void error_at(char *loc, char *fmt, char *user_input, ...);
void debug_print(char *fmt, ... );

void gen(Node *node);
void program();

Node *code[100];
