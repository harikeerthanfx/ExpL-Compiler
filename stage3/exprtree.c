#include <string.h>
#include "exprtree.h"
#include <stdio.h>
#include <stdlib.h>

tnode* createTree(int val, int type, int nodetype, char *varname,
                  tnode *l, tnode *m, tnode *r)
{
    tnode* temp = (tnode*)malloc(sizeof(tnode));

    temp->val = val;
    temp->type = type;
    temp->nodetype = nodetype;
    temp->varname = varname;

    temp->left = l;
    temp->middle = m;
    temp->right = r;

    return temp;
}

tnode* makeNumNode(int n)
{
    return createTree(n, TYPE_INT, NODE_NUM, NULL, NULL, NULL, NULL);
}

tnode* makeOperatorNode(char op, tnode* l, tnode* r)
{
    int nodetype;

    switch(op)
    {
        case '+': nodetype = NODE_PLUS; break;
        case '-': nodetype = NODE_MINUS; break;
        case '*': nodetype = NODE_MUL; break;
        case '/': nodetype = NODE_DIV; break;

        default:
            fprintf(stderr, "Invalid operator %c\n", op);
            exit(1);
    }

    return createTree(0, TYPE_INT, nodetype, NULL, l, NULL, r);
}

tnode* makeRelationalNode(char *op, tnode *l, tnode *r)
{
    int nodetype;

    if (!strcmp(op, "<"))
        nodetype = NODE_LT;
    else if (!strcmp(op, ">"))
        nodetype = NODE_GT;
    else if (!strcmp(op, "<="))
        nodetype = NODE_LE;
    else if (!strcmp(op, ">="))
        nodetype = NODE_GE;
    else if (!strcmp(op, "=="))
        nodetype = NODE_EQ;
    else if (!strcmp(op, "!="))
        nodetype = NODE_NE;
    else
    {
        fprintf(stderr, "Invalid relational operator %s\n", op);
        exit(1);
    }

    return createTree(0, TYPE_BOOL, nodetype, NULL, l, NULL, r);
}

tnode* makeIdNode(char *name)
{
    return createTree(0, TYPE_INT, NODE_ID, name, NULL, NULL, NULL);
}

tnode* makeAssignNode(tnode* id, tnode* expr)
{
    return createTree(0, TYPE_INT, NODE_ASSIGN, NULL, id, NULL, expr);
}

tnode* makeReadNode(tnode* id)
{
    return createTree(0, TYPE_INT, NODE_READ, NULL, id, NULL, NULL);
}

tnode* makeWriteNode(tnode* expr)
{
    return createTree(0, TYPE_INT, NODE_WRITE, NULL, expr, NULL, NULL);
}

tnode* makeConnectorNode(tnode* left, tnode* right)
{
    return createTree(0, TYPE_INT, NODE_CONNECTOR, NULL, left, NULL, right);
}

tnode* makeIfNode(tnode *cond, tnode *thenStmt, tnode *elseStmt)
{
    return createTree(0, TYPE_BOOL, NODE_IF, NULL,
                      cond, thenStmt, elseStmt);
}

tnode* makeWhileNode(tnode *cond, tnode *body)
{
    return createTree(0, TYPE_BOOL, NODE_WHILE, NULL,
                      cond, NULL, body);
}

void printTree(tnode *t)
{
    if (t == NULL)
        return;

    switch (t->nodetype)
    {
        case NODE_NUM:       printf("%d ", t->val); break;
        case NODE_ID:        printf("%s ", t->varname); break;

        case NODE_PLUS:      printf("+ "); break;
        case NODE_MINUS:     printf("- "); break;
        case NODE_MUL:       printf("* "); break;
        case NODE_DIV:       printf("/ "); break;

        case NODE_LT:        printf("< "); break;
        case NODE_GT:        printf("> "); break;
        case NODE_LE:        printf("<= "); break;
        case NODE_GE:        printf(">= "); break;
        case NODE_EQ:        printf("== "); break;
        case NODE_NE:        printf("!= "); break;

        case NODE_READ:      printf("READ "); break;
        case NODE_WRITE:     printf("WRITE "); break;
        case NODE_ASSIGN:    printf("= "); break;
        case NODE_CONNECTOR: printf("CONNECTOR "); break;
        case NODE_IF:        printf("IF "); break;
        case NODE_WHILE:     printf("WHILE "); break;
    }

    printTree(t->left);
    printTree(t->middle);
    printTree(t->right);
}