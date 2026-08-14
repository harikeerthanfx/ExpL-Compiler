#include "eval.h"
#include <stdio.h>
#include <stdlib.h>

int vars[26];

int evaluate(tnode *t)
{
    if (t == NULL)
        return 0;

    switch (t->nodetype)
    {
        case NODE_NUM:
            return t->val;

        case NODE_ID:
            return vars[t->varname[0] - 'a'];

        case NODE_PLUS:
            return evaluate(t->left) + evaluate(t->right);

        case NODE_MINUS:
            return evaluate(t->left) - evaluate(t->right);

        case NODE_MUL:
            return evaluate(t->left) * evaluate(t->right);

        case NODE_DIV:
            return evaluate(t->left) / evaluate(t->right);

        case NODE_LT:
            return evaluate(t->left) < evaluate(t->right);

        case NODE_GT:
            return evaluate(t->left) > evaluate(t->right);

        case NODE_LE:
            return evaluate(t->left) <= evaluate(t->right);

        case NODE_GE:
            return evaluate(t->left) >= evaluate(t->right);

        case NODE_EQ:
            return evaluate(t->left) == evaluate(t->right);

        case NODE_NE:
            return evaluate(t->left) != evaluate(t->right);

        case NODE_ASSIGN:
        {
            int value = evaluate(t->right);
            int index = t->left->varname[0] - 'a';

            vars[index] = value;

            return value;
        }

        case NODE_CONNECTOR:
            evaluate(t->left);
            return evaluate(t->right);

        case NODE_READ:
        {
            int index = t->left->varname[0] - 'a';

            scanf("%d", &vars[index]);

            return vars[index];
        }

        case NODE_WRITE:
        {
            int value = evaluate(t->left);

            printf("%d\n", value);

            return value;
        }

        case NODE_IF:
        {
            int condition = evaluate(t->left);

            if (condition)
                return evaluate(t->middle);
            else if (t->right != NULL)
                return evaluate(t->right);

            return 0;
        }

        case NODE_WHILE:
        {
            while (evaluate(t->left))
                evaluate(t->right);

            return 0;
        }

        default:
            printf("Unknown node type\n");
            exit(1);
    }
}