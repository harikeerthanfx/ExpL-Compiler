#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE* targetFile;
int reg = -1;

int getReg() {
    if (reg >= 19) {
        printf("Out of registers\n");
        exit(1);
    }

    reg++;
    return reg;
}

void freeReg() {
    reg--;
}

void writeResult(int resultReg) {
    fprintf(targetFile, "MOV [4096], R%d\n", resultReg);

    fprintf(targetFile, "MOV SP, 4095\n");

    // Write
    fprintf(targetFile, "MOV R2, \"Write\"\n");
    fprintf(targetFile, "PUSH R2\n");

    fprintf(targetFile, "MOV R2, -2\n");
    fprintf(targetFile, "PUSH R2\n");

    fprintf(targetFile, "PUSH R%d\n", resultReg);

    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");

    fprintf(targetFile, "CALL 0\n");

    fprintf(targetFile, "POP R0\n");
    fprintf(targetFile, "POP R1\n");
    fprintf(targetFile, "POP R1\n");
    fprintf(targetFile, "POP R1\n");
    fprintf(targetFile, "POP R1\n");

    // Exit
    fprintf(targetFile, "MOV R2, \"Exit\"\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");

    fprintf(targetFile, "CALL 0\n");
}

int codeGen(tnode* t) {
    if (t == NULL)
        return -1;

    // if it is a number
    if (t->op == '\0') {
        int r = getReg();
        fprintf(targetFile, "MOV R%d, %d\n", r, t->val);
        return r;
    }

    // else it is an operator
    int p = codeGen(t->left);
    int q = codeGen(t->right);

    char* instr;
    switch (t->op) {
        case '+': instr = "ADD"; break;
        case '-': instr = "SUB"; break;
        case '*': instr = "MUL"; break;
        case '/': instr = "DIV"; break;
        default:
            printf("Unknown operator '%c'\n", t->op);
            exit(1);
    }

    fprintf(targetFile, "%s R%d, R%d\n", instr, p, q);

    freeReg();     // release the (right) register (q)
    return p;      // store result in  lower (left) register
}