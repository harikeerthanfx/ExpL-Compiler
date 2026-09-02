#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE* targetFile;
int reg = -1;
int label = 0;

#define MAX_LOOP_DEPTH 100 // nested loops require stack
int loopTop = -1; // innermost loop
int loopBreak[MAX_LOOP_DEPTH];
int loopContinue[MAX_LOOP_DEPTH];

int getReg() {
    if (reg >= 19) {
        printf("Out of registers\n");
        exit(1);
    }

    reg++;
    return reg;
}

int getLabel() {
    return label++; // post-increment
}

void freeReg() {
    if (reg >= 0) reg--;
}

void pushLoop(int breakLabel, int continueLabel) {
    if (loopTop >= MAX_LOOP_DEPTH - 1) {
        printf("Too many nested loops\n");
        exit(1);
    }

    loopTop++;

    // push to stacks
    loopBreak[loopTop] = breakLabel;
    loopContinue[loopTop] = continueLabel;
}

void popLoop() {
    if (loopTop >= 0) loopTop--;
}

int getArrayAddress(tnode *t) { // TASK3: computes and returns the register containing the address of an array element

    int indexReg = codeGen(t->left); // evaluate index expression

    int baseReg = getReg();

    fprintf(targetFile, "MOV R%d, %d\n", baseReg, t->Gentry->binding);
    fprintf(targetFile, "ADD R%d, R%d\n", indexReg, baseReg);

    freeReg();

    return indexReg;
}

int getArray2DAddress(tnode *t) { // EX1: computes address of a 2D array element
	int rowReg = codeGen(t->left);
	int colReg = codeGen(t->middle);
	int baseReg = getReg();
	int colsReg = getReg();

	fprintf(targetFile, "MOV R%d, %d\n", baseReg, t->Gentry->binding);
	fprintf(targetFile, "MOV R%d, %d\n", colsReg, t->Gentry->cols);
	fprintf(targetFile, "MUL R%d, R%d\n", rowReg, colsReg);
	fprintf(targetFile, "ADD R%d, R%d\n", rowReg, colReg);
	fprintf(targetFile, "ADD R%d, R%d\n", rowReg, baseReg);

	freeReg();
	freeReg();
	freeReg();

	return rowReg;
}

int codeGen(tnode* t) {
    if (t == NULL)
        return -1;

    switch (t->nodetype) {
        case NODE_NUM: {
            int r = getReg();
            fprintf(targetFile, "MOV R%d, %d\n", r, t->val);
            return r;
        }

        case NODE_STR: { //TASK1 forgotten crossroads(iykyk)
            int r = getReg();
            fprintf(targetFile, "MOV R%d, \"%s\"\n", r, t->varname);
            return r;
        }
        
        case NODE_ID: { // whenever stuff like d = a * 3 + b comes -> so we fetch values of them and stores it in reg
            int r = getReg();
            int addr = t->Gentry->binding; //TASK2
            fprintf(targetFile, "MOV R%d, [%d]\n", r, addr); // [5000] means fetch value from addr 5000
            return r;
        }

        case NODE_ADDRESS: { // EX2: generates the address of a variable
            int r = getReg();
            fprintf(targetFile, "MOV R%d, %d\n", r, t->left->Gentry->binding);
            return r;
        }

        case NODE_DEREFERENCE: { // EX2: gets the value stored at the address held by a pointer
            int r = codeGen(t->left);
            fprintf(targetFile, "MOV R%d, [R%d]\n", r, r);
            return r;
        }

        
        case NODE_ARRAY: { // TASK3: get the value stored at an array element
            int addrReg = getArrayAddress(t);
            fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
            return addrReg;
        }

        case NODE_ARRAY2D: { // EX1: get value stored at a 2D array element
            int addrReg = getArray2DAddress(t);
            fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
            return addrReg;
        }

        // fall through - group all 5 ops together

        case NODE_PLUS:
        case NODE_MINUS:
        case NODE_MUL:
        case NODE_DIV:
        case NODE_MOD: { // EX2: includes modulo operation

            int leftReg = codeGen(t->left);
            int rightReg = codeGen(t->right);

            switch (t->nodetype) {

                case NODE_PLUS:
                    fprintf(targetFile, "ADD R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_MINUS:
                    fprintf(targetFile, "SUB R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_MUL:
                    fprintf(targetFile, "MUL R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_DIV:
                    fprintf(targetFile, "DIV R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_MOD:
                    fprintf(targetFile, "MOD R%d, R%d\n", leftReg, rightReg); // EX2: generates modulo instruction
                    break;
            }

            freeReg(); // free rightReg
            return leftReg; // assembly stores result from rightReg into leftReg
        }

        case NODE_LT:
        case NODE_GT:
        case NODE_LE:
        case NODE_GE:
        case NODE_EQ:
        case NODE_NE: {
            int leftReg = codeGen(t->left);
            int rightReg = codeGen(t->right);

            switch (t->nodetype) {
                case NODE_LT:
                    fprintf(targetFile, "LT R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_GT:
                    fprintf(targetFile, "GT R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_LE:
                    fprintf(targetFile, "LE R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_GE:
                    fprintf(targetFile, "GE R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_EQ:
                    fprintf(targetFile, "EQ R%d, R%d\n", leftReg, rightReg);
                    break;

                case NODE_NE:
                    fprintf(targetFile, "NE R%d, R%d\n", leftReg, rightReg);
                    break;
            }
            
            freeReg();
            return leftReg;
        }

        case NODE_ASSIGN: { // TASK3: updated to support assignment to array elements
            int r = codeGen(t->right);

            if (t->left->nodetype == NODE_ID) {
                int addr = t->left->Gentry->binding;
                fprintf(targetFile, "MOV [%d], R%d\n", addr, r);
            }
            else if (t->left->nodetype == NODE_ARRAY) { // TASK3: handle arr[i] = value
                int addrReg = getArrayAddress(t->left);
                fprintf(targetFile, "MOV [R%d], R%d\n", addrReg, r);
                freeReg();
            }
            else if (t->left->nodetype == NODE_ARRAY2D) { // EX1: handle arr[i][j] = value
                int addrReg = getArray2DAddress(t->left);
                fprintf(targetFile, "MOV [R%d], R%d\n", addrReg, r);
                freeReg();
            }
            else if (t->left->nodetype == NODE_DEREFERENCE) { // EX2: assigns a value through a pointer
                int addrReg = codeGen(t->left->left);
                fprintf(targetFile, "MOV [R%d], R%d\n", addrReg, r);
                freeReg();
            }

            freeReg();
            return -1;
        }

        case NODE_CONNECTOR: {
            codeGen(t->left); // Goes left, runs read(a) (or_anything_else), prints its XSM assembly to the file.
            codeGen(t->right); // u know
            return -1;
        }

        // five elements of stack
        // 1. function code
        // 2. arg1
        // 3. arg2
        // 4. arg3
        // 5. return value slot

        case NODE_WRITE: {
            int r = codeGen(t->left); 
            // we take t->left coz write only has one child, that child is the 
            // root of the entire subtree.. whether it be operator, node or id


            // For Write, the ABI contract says:
            // Arg1 = -2 (-2 = stdout)
            // Arg2 = Buffer (here buffer means the reg that contains the value u wanna print)
            // Arg3 = unused
            // i.e take value from the buffer (reg) and print in stdout

            fprintf(targetFile, "MOV R2, \"Write\"\n");
            fprintf(targetFile, "PUSH R2\n");
            fprintf(targetFile, "MOV R2, -2\n");
            fprintf(targetFile, "PUSH R2\n");
            fprintf(targetFile, "PUSH R%d\n", r);
            fprintf(targetFile, "PUSH R2\n");
            fprintf(targetFile, "PUSH R0\n");
            fprintf(targetFile, "CALL 0\n");

            // POP (return_value_reg + 3 arguments + function code)
            fprintf(targetFile, "POP R0\n");
            fprintf(targetFile, "POP R1\n");
            fprintf(targetFile, "POP R1\n");
            fprintf(targetFile, "POP R1\n");
            fprintf(targetFile, "POP R1\n");

            freeReg();

            return -1;
        }

        case NODE_READ: { // TASK3 + EX1: supports reading into variables and array elements
            if (t->left->nodetype == NODE_ID) {
                int addr = t->left->Gentry->binding;

                fprintf(targetFile, "MOV R2, \"Read\"\n");
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "MOV R2, -1\n");
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "MOV R2, %d\n", addr);
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "PUSH R0\n");
                fprintf(targetFile, "CALL 0\n");
            }
            else if (t->left->nodetype == NODE_ARRAY) { // TASK3: handle read(arr[i])
                int addrReg = getArrayAddress(t->left);

                fprintf(targetFile, "MOV R2, \"Read\"\n");
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "MOV R2, -1\n");
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "PUSH R%d\n", addrReg);
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "PUSH R0\n");
                fprintf(targetFile, "CALL 0\n");

                freeReg();
            }
            else if (t->left->nodetype == NODE_ARRAY2D) { // EX1: handle read(arr[i][j])
                int addrReg = getArray2DAddress(t->left);

                fprintf(targetFile, "MOV R2, \"Read\"\n");
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "MOV R2, -1\n");
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "PUSH R%d\n", addrReg);
                fprintf(targetFile, "PUSH R2\n");
                fprintf(targetFile, "PUSH R0\n");
                fprintf(targetFile, "CALL 0\n");

                freeReg();
            }

            fprintf(targetFile, "POP R0\n");
            fprintf(targetFile, "POP R1\n");
            fprintf(targetFile, "POP R1\n");
            fprintf(targetFile, "POP R1\n");
            fprintf(targetFile, "POP R1\n");

            return -1;
        }

        case NODE_IF: {
            int condReg = codeGen(t->left);
            int labelElse = getLabel();
            int labelEnd = getLabel();

            fprintf(targetFile, "JZ R%d, L%d\n", condReg, labelElse);
            freeReg();
            
            codeGen(t->middle); // if body
            // if execute aaya, jump to end by skipping else
            fprintf(targetFile, "JMP L%d\n", labelEnd);
            
            // same labelElse value 
            fprintf(targetFile, "L%d:\n", labelElse);
            if (t->right != NULL) codeGen(t->right); // else body

            fprintf(targetFile, "L%d:\n", labelEnd);
            return -1;
        }

        case NODE_WHILE: {
            int labelStart = getLabel();
            int labelEnd = getLabel();

            pushLoop(labelEnd, labelStart); 
            // break = leave and go to LabelEnd
            // continue = start again so labelStart

            fprintf(targetFile, "L%d:\n", labelStart);
            int condReg = codeGen(t->left); // check true
            fprintf(targetFile, "JZ R%d, L%d\n", condReg, labelEnd); // false aanel

            freeReg();
            codeGen(t->right); // body
            fprintf(targetFile, "JMP L%d\n", labelStart); // loop

            fprintf(targetFile, "L%d:\n", labelEnd); // next label heading
            popLoop();
            return -1;
        }

        case NODE_BREAK: {
            if (loopTop >= 0) { // loopil aanel breakine work cheyicha mathi
                fprintf(targetFile, "JMP L%d\n", loopBreak[loopTop]);
            }

            return -1;
        }

        case NODE_CONTINUE: {
            if (loopTop >= 0) {
                fprintf(targetFile, "JMP L%d\n", loopContinue[loopTop]);
            }

            return -1;
        }

        case NODE_REPEAT: {
            int labelStart = getLabel();
            int labelCondition = getLabel();
            int labelEnd = getLabel();

            // condition check is at the bottom not top
            pushLoop(labelEnd, labelCondition);
            fprintf(targetFile, "L%d:\n", labelStart);
            codeGen(t->right);

            // condition check
            fprintf(targetFile, "L%d:\n", labelCondition);
            int condReg = codeGen(t->left);

            // repeat till condition is FALSE
            fprintf(targetFile, "JZ R%d, L%d\n", condReg, labelStart);

            freeReg();
            fprintf(targetFile, "L%d:\n", labelEnd);
            popLoop();
            return -1;
        }

        case NODE_DOWHILE: {
            int labelStart = getLabel();
            int labelCondition = getLabel();
            int labelEnd = getLabel();

            pushLoop(labelEnd, labelCondition);
            fprintf(targetFile, "L%d:\n", labelStart);

            codeGen(t->right);

            fprintf(targetFile, "L%d:\n", labelCondition);
            int condReg = codeGen(t->left);

            fprintf(targetFile, "JNZ R%d, L%d\n", condReg, labelStart);
            freeReg();

            fprintf(targetFile, "L%d:\n", labelEnd);
            popLoop();
            return -1;
        }
    }

    return -1;
}