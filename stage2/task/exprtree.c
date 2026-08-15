#include "exprtree.h"
#include <stdio.h>
#include <stdlib.h>


tnode* createTree(int val, int type, int nodetype, char* varname, tnode* l, tnode* r) {
    tnode* temp = (tnode*)malloc(sizeof(tnode));

    temp->val = val;
    temp->type = type;
    temp->nodetype = nodetype;
    temp->varname = varname;
    temp->left = l;
    temp->right = r;

    return temp;
}


tnode* makeNumNode(int n) {
    return createTree(n, TYPE_INT, NODE_NUM, NULL, NULL, NULL);
}


tnode* makeOperatorNode(char op, tnode* l, tnode* r) {
    int nodetype;

    switch(op)
    {
        case '+':
            nodetype = NODE_PLUS;
            break;

        case '-':
            nodetype = NODE_MINUS;
            break;

        case '*':
            nodetype = NODE_MUL;
            break;

        case '/':
            nodetype = NODE_DIV;
            break;

        default:
            fprintf(stderr, "Invalid operator %c\n", op);
            exit(1);
    }

    return createTree(0, TYPE_INT, nodetype, NULL, l, r);
}


tnode* makeIdNode(char* name) {
    return createTree(0, TYPE_INT, NODE_ID, name, NULL, NULL);
}


tnode* makeAssignNode(tnode* id, tnode* expr) {
    return createTree(0, TYPE_INT, NODE_ASSIGN, NULL, id, expr);
}


tnode* makeReadNode(tnode* id) {
    return createTree(0, TYPE_INT, NODE_READ, NULL, id, NULL);
}


tnode* makeWriteNode(tnode* expr) {
    return createTree(0, TYPE_INT, NODE_WRITE, NULL, expr, NULL);
}


tnode* makeConnectorNode(tnode* left, tnode* right) {
    return createTree(0, TYPE_INT, NODE_CONNECTOR, NULL, left, right);
}

/*
if wanted to clarify the AST structure, use this funtion "printTree"
(by simple edit you can change it to post order or pre order traversals)

modified : exprtree.c , exprtree.h and exprtree.y(call happens here)
*/
void printTree(tnode *t)
{
    if (t == NULL)
        return;

    switch (t->nodetype)
    {
        case NODE_NUM:
            printf("%d ", t->val);
            break;

        case NODE_ID:
            printf("%s ", t->varname);
            break;

        case NODE_PLUS:      printf("+ "); break;
        case NODE_MINUS:     printf("- "); break;
        case NODE_MUL:       printf("* "); break;
        case NODE_DIV:       printf("/ "); break;
        case NODE_READ:      printf("READ "); break;
        case NODE_WRITE:     printf("WRITE "); break;
        case NODE_ASSIGN:    printf("= "); break;
        case NODE_CONNECTOR: printf("CONNECTOR "); break;
    }

    printTree(t->left);
    printTree(t->right);
}