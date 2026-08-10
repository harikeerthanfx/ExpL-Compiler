#include "exprtree.h"
#include <stdio.h>
#include <stdlib.h>

tnode* makeLeafNode(int n) {
    tnode* temp = (tnode*)malloc(sizeof(tnode));
    temp->val = n;
    temp->op = '\0';
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

tnode* makeOperatorNode(char op, tnode* l, tnode* r) {
    tnode* temp = (tnode*)malloc(sizeof(tnode));
    temp->val = 0;
    temp->op = op;
    temp->left = l;
    temp->right = r;
    return temp;
}

void prefix(tnode* root) {
    if (root == NULL)
        return;

    if (root->op == '\0')
        printf("%d ", root->val);
    else
        printf("%c ", root->op);

    prefix(root->left);
    prefix(root->right);
}

void postfix(tnode* root) {
    if (root == NULL)
        return;

    postfix(root->left);
    postfix(root->right);

    if (root->op == '\0')
        printf("%d ", root->val);
    else
        printf("%c ", root->op);
}