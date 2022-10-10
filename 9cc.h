// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
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
    ND_BLOCK,
    ND_FUNCALL, // 関数呼び出し
    ND_FUNCDEF, // 関数定義
    ND_ADDR,    // 単項&
    ND_DEREF,   // 単項*
    ND_GVARDEF, // グローバル変数の定義
    ND_GVARREF, // グローバル変数の参照
    ND_LITERAL, // リテラル文字
    ND_LITERALREF, // リテラル文字の参照
    ND_STRUCTDEF, // define struct
    ND_STRUCTREF, // reference struct
    ND_STRUCTREF_PTR, // reference struct pointer
    ND_ARRAYREF, // array reference
} NodeKind;

bool is_struct_ref(NodeKind kind);

typedef struct Type Type;

typedef enum {
    INT,
    PTR,
    ARRAY,
    CHAR,
    BOOL,
    STRUCT,
} TypeKind;

struct Type {
    TypeKind ty;
    char *type_name;
    int type_name_len;
    struct Type *ptr_to;
    size_t array_size;
};

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
    Type *type;     // 変数の型を保持する
};

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *elsehs;   // else文での実行式
    Node *initstmt; // for文での初期化式
    Node *testexpr; // for文での条件式
    Node *updstmt;  // for文での変化式
    Node *child;    // ブロックで使う
    Node *arg[6];   // 引数の参照を持つ
    int val;        // kindがND_NUMの場合のみ使う
    int offset;     // kindがND_LVAR,ND_STRUCTDEFの場合のみ使う
    char *name;     // kindがND_DEF_FUNCTIONの場合のみ使う
    int namelen;    // kindがND_DEF_FUNCTIONの場合のみ使う
    int argnum;     // 引数の個数
    Type *type;      // kindがND_LVARの場合のみ使う
    int localsnum;  // ローカル変数の個数
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
    TK_INT,      // int
    TK_SIZEOF,   // sizeof
    TK_CHAR,     // char
    TK_STR,      // string
    TK_LT_CHAR,      // literal char
    TK_STRUCT,   // struct
    TK_TYPEDEF,  // typedef
    TK_ENUM,     // enum
    TK_TRUE,     // true
    TK_FALSE,    // false
    TK_BOOL,     // bool
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

typedef struct GVar GVar;

struct GVar {
    GVar *next;
    char *name;
    int len;
    Type *type;     // 変数の型を保持する
    int all_ele;
};

typedef struct EnumDef EnumDef;

struct EnumDef {
    char *str;
    int len;
    int index;
    EnumDef *next;
};

void tokenize(char *p);
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *define_function_gvar();
Node *mul_ptr();

void error(char *fmt, ... );
void error_at(char *loc, char *user_input, char *msg, ...);
void debug_print(char *fmt, ... );
int mysubstr( char *t, char *s, int pos, int len );
bool startswith(char *p, char *q);

void gen(Node *node);
void program();

Node *code[100];
Token *token;
char *user_input;
Node *literals_def[100];

bool consume(char *op);
bool consume_kind(TokenKind kind);
Token *consume_indent();
int expect_number();
void expect(char *op);
void expect_type(char *op);

char filename[100];

LVar *locals;
GVar *globals;
GVar *literals;
Node *defined_structs;
EnumDef *defined_enums;

LVar *find_lvar(Token *tok);
GVar *find_gvar(Token *tok);
GVar *find_gvar_literals(Token *tok);
Node *find_defined_structs(Token *tok);
int get_struct_node_offset(Node *defined_struct_node, Token *tok);
int find_defined_enum(Token *tok);
