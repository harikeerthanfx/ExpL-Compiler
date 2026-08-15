#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *targetFile;

int reg = -1;
int label = 0;

/* Stack for nested while loops */
#define MAX_LOOP_DEPTH 100

int loopStart[MAX_LOOP_DEPTH];
int loopEnd[MAX_LOOP_DEPTH];
int loopTop = -1;


int getReg()
{
    if (reg >= 19) {
        printf("Out of registers\n");
        exit(1);
    }

    reg++;
    return reg;
}


void freeReg()
{
    if (reg >= 0)
        reg--;
}


int getLabel()
{
    return label++;
}


/* Enter a while loop */
void pushLoop(int start, int end)
{
    if (loopTop >= MAX_LOOP_DEPTH - 1) {
        printf("Too many nested loops\n");
        exit(1);
    }

    loopTop++;

    loopStart[loopTop] = start;
    loopEnd[loopTop] = end;
}


/* Exit a while loop */
void popLoop()
{
    if (loopTop >= 0)
        loopTop--;
}


int codeGen(tnode *t)
{
    if (t == NULL)
        return -1;

    switch (t->nodetype)
    {
        case NODE_NUM:
        {
            int r = getReg();

            fprintf(targetFile, "MOV R%d, %d\n",
                    r, t->val);

            return r;
        }


        case NODE_ID:
        {
            int r = getReg();
            int addr = 4096 + (t->varname[0] - 'a');

            fprintf(targetFile, "MOV R%d, [%d]\n",
                    r, addr);

            return r;
        }


        case NODE_PLUS:
        case NODE_MINUS:
        case NODE_MUL:
        case NODE_DIV:
        {
            int leftReg = codeGen(t->left);
            int rightReg = codeGen(t->right);

            switch (t->nodetype)
            {
                case NODE_PLUS:
                    fprintf(targetFile,
                            "ADD R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_MINUS:
                    fprintf(targetFile,
                            "SUB R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_MUL:
                    fprintf(targetFile,
                            "MUL R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_DIV:
                    fprintf(targetFile,
                            "DIV R%d, R%d\n",
                            leftReg, rightReg);
                    break;
            }

            freeReg();

            return leftReg;
        }


        case NODE_LT:
        case NODE_GT:
        case NODE_LE:
        case NODE_GE:
        case NODE_EQ:
        case NODE_NE:
        {
            int leftReg = codeGen(t->left);
            int rightReg = codeGen(t->right);

            switch (t->nodetype)
            {
                case NODE_LT:
                    fprintf(targetFile,
                            "LT R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_GT:
                    fprintf(targetFile,
                            "GT R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_LE:
                    fprintf(targetFile,
                            "LE R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_GE:
                    fprintf(targetFile,
                            "GE R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_EQ:
                    fprintf(targetFile,
                            "EQ R%d, R%d\n",
                            leftReg, rightReg);
                    break;

                case NODE_NE:
                    fprintf(targetFile,
                            "NE R%d, R%d\n",
                            leftReg, rightReg);
                    break;
            }

            freeReg();

            return leftReg;
        }


        case NODE_ASSIGN:
        {
            int r = codeGen(t->right);

            int addr =
                4096 + (t->left->varname[0] - 'a');

            fprintf(targetFile,
                    "MOV [%d], R%d\n",
                    addr, r);

            freeReg();

            return -1;
        }


        case NODE_CONNECTOR:
        {
            codeGen(t->left);
            codeGen(t->right);

            return -1;
        }


        case NODE_READ:
        {
            int addr =
                4096 + (t->left->varname[0] - 'a');

            fprintf(targetFile,
                    "MOV R2, \"Read\"\n");

            fprintf(targetFile,
                    "PUSH R2\n");

            fprintf(targetFile,
                    "MOV R2, -1\n");

            fprintf(targetFile,
                    "PUSH R2\n");

            fprintf(targetFile,
                    "MOV R2, %d\n",
                    addr);

            fprintf(targetFile,
                    "PUSH R2\n");

            fprintf(targetFile,
                    "MOV R2, 0\n");

            fprintf(targetFile,
                    "PUSH R2\n");

            fprintf(targetFile,
                    "PUSH R0\n");

            fprintf(targetFile,
                    "CALL 0\n");

            fprintf(targetFile,
                    "POP R0\n");

            fprintf(targetFile,
                    "POP R1\n");

            fprintf(targetFile,
                    "POP R1\n");

            fprintf(targetFile,
                    "POP R1\n");

            fprintf(targetFile,
                    "POP R1\n");

            return -1;
        }


        case NODE_WRITE:
        {
            int r = codeGen(t->left);

            fprintf(targetFile,
                    "MOV R2, \"Write\"\n");

            fprintf(targetFile,
                    "PUSH R2\n");

            fprintf(targetFile,
                    "MOV R2, -2\n");

            fprintf(targetFile,
                    "PUSH R2\n");

            fprintf(targetFile,
                    "PUSH R%d\n",
                    r);

            fprintf(targetFile,
                    "PUSH R2\n");

            fprintf(targetFile,
                    "PUSH R0\n");

            fprintf(targetFile,
                    "CALL 0\n");

            fprintf(targetFile,
                    "POP R0\n");

            fprintf(targetFile,
                    "POP R1\n");

            fprintf(targetFile,
                    "POP R1\n");

            fprintf(targetFile,
                    "POP R1\n");

            fprintf(targetFile,
                    "POP R1\n");

            freeReg();

            return -1;
        }


        case NODE_IF:
        {
            int labelElse = getLabel();
            int labelEnd = getLabel();

            int condReg = codeGen(t->left);

            fprintf(targetFile,
                    "JZ R%d, L%d\n",
                    condReg, labelElse);

            freeReg();

            codeGen(t->middle);

            fprintf(targetFile,
                    "JMP L%d\n",
                    labelEnd);

            fprintf(targetFile,
                    "L%d:\n",
                    labelElse);

            if (t->right != NULL)
                codeGen(t->right);

            fprintf(targetFile,
                    "L%d:\n",
                    labelEnd);

            return -1;
        }


        case NODE_WHILE:
        {
            int labelStart = getLabel();
            int labelEnd = getLabel();

            /*
             * Save the labels of this while loop.
             * break    -> labelEnd
             * continue -> labelStart
             */
            pushLoop(labelStart, labelEnd);

            fprintf(targetFile,
                    "L%d:\n",
                    labelStart);

            int condReg = codeGen(t->left);

            fprintf(targetFile,
                    "JZ R%d, L%d\n",
                    condReg, labelEnd);

            freeReg();

            codeGen(t->right);

            fprintf(targetFile,
                    "JMP L%d\n",
                    labelStart);

            fprintf(targetFile,
                    "L%d:\n",
                    labelEnd);

            /*
             * Remove this loop from the stack.
             */
            popLoop();

            return -1;
        }


        case NODE_BREAK:
        {
            /*
             * Only generate code if we are
             * currently inside a while loop.
             */
            if (loopTop >= 0)
            {
                fprintf(targetFile,
                        "JMP L%d\n",
                        loopEnd[loopTop]);
            }

            return -1;
        }


        case NODE_CONTINUE:
        {
            /*
             * Only generate code if we are
             * currently inside a while loop.
             */
            if (loopTop >= 0)
            {
                fprintf(targetFile,
                        "JMP L%d\n",
                        loopStart[loopTop]);
            }

            return -1;
        }
    }

    return -1;
}