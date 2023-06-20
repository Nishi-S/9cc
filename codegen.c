#include "9cc.h"

Node *expr()
{
    Node *node = equality();
    return node;
}

Node *equality()
{
    Node *node = relational();

    while (1)
    {
        if (consume("=="))
        {
            node = new_node(ND_EQL, node, relational());
        }
        else if (consume("!="))
        {
            node = new_node(ND_NEQ, node, relational());
        }
        else
        {
            return node;
        }
    }
}

Node *relational()
{
    Node *node = add();

    while (1)
    {
        if (consume("<"))
        {
            node = new_node(ND_LSS, node, add());
        }
        else if (consume("<="))
        {
            node = new_node(ND_LEQ, node, add());
        }
        else if (consume(">"))
        {
            node = new_node(ND_GRT, node, add());
        }
        else if (consume(">="))
        {
            node = new_node(ND_GEQ, node, add());
        }
        else
        {
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();

    while (1)
    {
        if (consume("+"))
        {
            node = new_node(ND_ADD, node, mul());
        }
        else if (consume("-"))
        {
            node = new_node(ND_SUB, node, mul());
        }
        else
        {
            return node;
        }
    }
}

Node *mul()
{
    Node *node = unary();

    while (1)
    {

        if (consume("*"))
        {
            node = new_node(ND_MUL, node, unary());
        }
        else if (consume("/"))
        {
            node = new_node(ND_DIV, node, unary());
        }
        else
        {
            return node;
        }
    }
}

Node *unary()
{
    Node *node;

    if (consume("+"))
    {
        node = new_node(ND_ADD, new_node_num(0), primary()); // +x = 0 + x と解釈
    }
    else if (consume("-"))
    {
        node = new_node(ND_SUB, new_node_num(0), primary()); // -x = 0 - x と解釈
    }
    else
    {
        node = primary();
    }

    return node;
}

Node *primary()
{
    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    // そうでなければ、数値のはず
    return new_node_num(expect_number());
}

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;

    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;

    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;

    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;

    case ND_EQL:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;

    case ND_NEQ:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;

    case ND_LSS:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;

    case ND_LEQ:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;

    case ND_GRT:
        printf("  cmp rdi, rax\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;

    case ND_GEQ:
        printf("  cmp rdi, rax\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}
