#ifndef EXPRTREE_H
#define EXPRTREE_H

typedef struct tnode{
    int val;
    char *op;
    struct tnode *left,*right;
} tnode;

tnode* makeLeafNode(int n);
tnode* makeOperatorNode(char c, tnode *l, tnode *r);
int evaluate(tnode *t);

void prefix(tnode *t);
void postfix(tnode *t);

#endif