#pragma once

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum
{
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;
struct Token
{
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQL, // ==
    ND_NEQ, // !=
    ND_LSS, // <
    ND_LEQ, // <=
    ND_GRT, // >
    ND_GEQ, // >=
    ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;
struct Node
{
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // kindがND_NUMの場合のみ使う
};

extern Token *token;
extern char *user_input;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
// トークナイズ
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);
// ノード生成
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
// 生成規則
Node *primary();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *expr();
// asm生成
void gen();
