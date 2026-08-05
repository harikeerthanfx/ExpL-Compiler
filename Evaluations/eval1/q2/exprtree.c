#include <stdlib.h>
#include <stdio.h>
#include "exprtree.h"

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

    temp->op = (char*)malloc(sizeof(char));
    *(temp->op) = c;

    temp->left = l;
    temp->right = r;
    temp->val = 0;

    return temp;
}

/*

int evaluate(tnode *t)
{
  if(t->op == NULL)
  return t->val;
  
  switch(*(t->op))
  {
    case '+':
    return evaluate(t->left) + evaluate(t->right);
    
    case '-':
    return evaluate(t->left) - evaluate(t->right);
    
    case '*':
    return evaluate(t->left) * evaluate(t->right);
    
    case '/':
    return evaluate(t->left) / evaluate(t->right);
  }
  
  return 0;
}
*/

void prefix(tnode *t)
{
    if(t == NULL)
        return;

    if(t->op == NULL)
        printf("%d ", t->val);
    else
        printf("%c ", *(t->op));

    prefix(t->left);
    prefix(t->right);
}

void postfix(tnode *t)
{
    if(t == NULL)
        return;

    postfix(t->left);
    postfix(t->right);

    if(t->op == NULL)
        printf("%d ", t->val);
    else
        printf("%c ", *(t->op));
}