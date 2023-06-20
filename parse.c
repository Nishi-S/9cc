#include "9cc.h"

extern Token *token;
extern char *user_input;

// エラーを報告するための関数
// printfと同じ引数をとる
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号の時はトークンを1つ読み進めて
// 真を返す。それ以外のときは偽を返す
bool consume(char *op)
{
    if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    {
        return false;
    }
    token = token->next;
    return true;
}

// 次のトークンが記号の時はトークンを1つ読み進める
// それ以外の場合にはエラーを報告する。
void expect(char *op)
{
    if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    {
        error_at(token->str, "'%c'ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを１つ読み進めてその数値を返す
// それ以外の場合はエラーを報告する
int expect_number()
{
    if (token->kind != TK_NUM)
    {
        error_at(token->str, "数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

// 次のトークンがeofであるかを判定する。
bool at_eof()
{
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        // 空白文字をスキップ
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (!strncmp(p, "==", 2) || !strncmp(p, "!=", 2) || !strncmp(p, "<=", 2) || !strncmp(p, ">=", 2))
        {
            cur = new_token(TK_RESERVED, cur, p);
            cur->len = 2;
            p += 2;
            continue;
        }

        // 2項演算子
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '>' || *p == '<')
        {
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        // 数字
        if (isdigit(*p))
        {
            char *p_next;
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p_next, 10);
            cur->len = p_next - p;
            p = p_next;
            continue;
        }

        error("トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}
