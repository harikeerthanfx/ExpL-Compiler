#ifndef EXPRTREE_H
#define EXPRTREE_H

typedef struct tnode {
    int val;
    char op;
    struct tnode* left;
    struct tnode* right;
} tnode;

tnode* makeLeafNode(int n);
tnode* makeOperatorNode(char op, tnode* l, tnode* r);

void prefix(tnode* root);
void postfix(tnode* root);

#endif