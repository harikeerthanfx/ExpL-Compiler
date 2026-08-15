#include "eval.h"
#include <stdio.h>
#include <stdlib.h>

int memory[26];

int evaluate(tnode *t)
{
    if (t == NULL)
        return 0;

    switch (t->nodetype)
    {
        case NODE_NUM:
            return t->val;

        case NODE_ID:
        {
            int index = t->varname[0] - 'a';
            return memory[index];
        }

        case NODE_PLUS:
            return evaluate(t->left) + evaluate(t->right);

        case NODE_MINUS:
            return evaluate(t->left) - evaluate(t->right);

        case NODE_MUL:
            return evaluate(t->left) * evaluate(t->right);

        case NODE_DIV:
            return evaluate(t->left) / evaluate(t->right);

        case NODE_ASSIGN:
        {
            int value = evaluate(t->right);
            int index = t->left->varname[0] - 'a';

            memory[index] = value;

            return value;
        }

        case NODE_READ:
        {
            int index = t->left->varname[0] - 'a';

            scanf("%d", &memory[index]);

            return 0;
        }

        case NODE_WRITE:
        {
            int value = evaluate(t->left);

            printf("%d\n", value);

            return value;
        }

        case NODE_CONNECTOR:
        {
            evaluate(t->left);
            evaluate(t->right);

            return 0;
        }

        default:
            printf("Unknown node type\n");
            exit(1);
    }
}