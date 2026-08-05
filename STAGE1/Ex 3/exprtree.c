#include <stdlib.h>
#include <stdio.h>
#include "exprtree.h"

int reg = -1;

FILE *targetFile;

int getReg()
{
    return ++reg;
}

void freeReg()
{
    reg--;
}

int codeGen(tnode *t)
{
    int leftReg, rightReg;

    // Leaf Node
    if(t->op == NULL)
    {
        int r = getReg();

        fprintf(targetFile,"MOV R%d, %d\n",r,t->val);

        return r;
    }

    leftReg = codeGen(t->left);
    rightReg = codeGen(t->right);

    switch(*(t->op))
    {
        case '+':
            fprintf(targetFile,"ADD R%d, R%d\n",leftReg,rightReg);
            break;

        case '-':
            fprintf(targetFile,"SUB R%d, R%d\n",leftReg,rightReg);
            break;

        case '*':
            fprintf(targetFile,"MUL R%d, R%d\n",leftReg,rightReg);
            break;

        case '/':
            fprintf(targetFile,"DIV R%d, R%d\n",leftReg,rightReg);
            break;
    }

    freeReg();

    return leftReg;
}

tnode* makeLeafNode(int n)
{
    tnode *temp;
    temp = (tnode*)malloc(sizeof(tnode));
    temp->op = NULL;
    temp->val = n;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

tnode* makeOperatorNode(char c, tnode *l, tnode *r)
{
    tnode *temp;
    temp = (tnode*)malloc(sizeof(tnode));
    temp->op = malloc(sizeof(char));
    *(temp->op) = c;
    temp->left = l;
    temp->right = r;
    temp->val = 0;
    return temp;
}

int evaluate(tnode *t)
{
    if(t->op == NULL)
    {
        return t->val;
    }

    switch(*(t->op))
    {
        case '+': return evaluate(t->left) + evaluate(t->right);
        case '-': return evaluate(t->left) - evaluate(t->right);
        case '*': return evaluate(t->left) * evaluate(t->right);
        case '/': return evaluate(t->left) / evaluate(t->right);
    }

    return 0;
}