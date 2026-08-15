#ifndef EXPRTREE_H
#define EXPRTREE_H
#include <stdio.h>

typedef struct tnode{
    int val;
    char *op;
    struct tnode *left,*right;
} tnode;

extern FILE *targetFile;

tnode* makeLeafNode(int n);
tnode* makeOperatorNode(char c, tnode *l, tnode *r);
int evaluate(tnode *t);

int getReg();
void freeReg();
int codeGen(tnode *t);

#endif