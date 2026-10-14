#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *targetFile;

int reg = -1;
int label = 0;

#define MAX_LOOP_DEPTH 100

static int loopTop = -1;
static int loopBreak[MAX_LOOP_DEPTH];
static int loopContinue[MAX_LOOP_DEPTH];
static int generatingMain;
static int currentLocalCount;

/* R19 is reserved for library-call scratch values. */
int getReg(void) {
    if (reg >= 18) {
        fprintf(stderr, "Out of registers\n");
        exit(1);
    }
    return ++reg;
}

void freeReg(void) {
    if (reg >= 0) {
        --reg;
    }
}

int getLabel(void) {
    return label++;
}

static void pushLoop(int breakLabel, int continueLabel) {
    loopBreak[++loopTop] = breakLabel;
    loopContinue[loopTop] = continueLabel;
}

static void popLoop(void) {
    --loopTop;
}

/* An Lentry uses BP-relative storage; otherwise the variable is global. */
//stage6: User-defined variables occupy one pointer word; fields use heap record offsets.
static int idAddress(tnode *t) {
    int addressReg = getReg();

    if (t->Lentry != NULL) {
        fprintf(targetFile, "MOV R%d, BP\n", addressReg);
        fprintf(targetFile, "ADD R%d, %d\n", addressReg, t->Lentry->binding);
    } else {
        fprintf(targetFile, "MOV R%d, %d\n", addressReg, t->Gentry->binding);
    }
    return addressReg;
}

static int arrayAddress(tnode *t) {
    int indexReg = codeGen(t->left);
    int baseReg = getReg();

    if (t->Lentry != NULL) {
        fprintf(targetFile, "MOV R%d, BP\n", baseReg);
        fprintf(targetFile, "ADD R%d, %d\n", baseReg, t->Lentry->binding);
    } else {
        fprintf(targetFile, "MOV R%d, %d\n", baseReg, t->Gentry->binding);
    }
    fprintf(targetFile, "ADD R%d, R%d\n", indexReg, baseReg);
    freeReg();
    return indexReg;
}

static int array2Address(tnode *t) {
    int rowReg = codeGen(t->left);
    int colReg = codeGen(t->middle);
    int baseReg = getReg();
    int colsReg = getReg();

    if (t->Lentry != NULL) {
        fprintf(targetFile, "MOV R%d, BP\n", baseReg);
        fprintf(targetFile, "ADD R%d, %d\n", baseReg, t->Lentry->binding);
        fprintf(targetFile, "MOV R%d, %d\n", colsReg, t->Lentry->cols);
    } else {
        fprintf(targetFile, "MOV R%d, %d\n", baseReg, t->Gentry->binding);
        fprintf(targetFile, "MOV R%d, %d\n", colsReg, t->Gentry->cols);
    }
    fprintf(targetFile, "MUL R%d, R%d\n", rowReg, colsReg);
    fprintf(targetFile, "ADD R%d, R%d\n", rowReg, colReg);
    fprintf(targetFile, "ADD R%d, R%d\n", rowReg, baseReg);

    freeReg();
    freeReg();
    freeReg();
    return rowReg;
}

//stage6: Compute lvalue addresses for dereferences and nested record fields.
/* Return the address of a variable, array element, dereference, or field. */
static int lvalueAddress(tnode *t) {
    int addressReg;

    switch (t->nodetype) {
    case NODE_ID:
        return idAddress(t);
    case NODE_ARRAY:
        return arrayAddress(t);
    case NODE_ARRAY2D:
        return array2Address(t);
    case NODE_DEREFERENCE:
        return codeGen(t->left);
    case NODE_FIELD:
        addressReg = lvalueAddress(t->left);
        /* A user-defined value is represented by a pointer in its storage slot. */
        fprintf(targetFile, "MOV R%d, [R%d]\n", addressReg, addressReg);
        fprintf(targetFile, "ADD R%d, %d\n", addressReg, t->Fentry->fieldIndex);
        return addressReg;
    default:
        fprintf(stderr, "Error: expression is not an assignable location\n");
        exit(1);
    }
}

static void emitExit(void) {
    fprintf(targetFile, "MOV R19, \"Exit\"\n");
    fprintf(targetFile, "PUSH R19\nPUSH R19\nPUSH R19\nPUSH R19\nPUSH R19\n");
    fprintf(targetFile, "CALL 0\n");
}

/* Keep allocator code in the generated program.  R19 is outside the
 * expression register allocator; save the live expression registers before
 * using R1-R5 as allocator temporaries. */
static void saveRegsThrough(int lastReg) {
    int i;
    for (i = 0; i <= lastReg; ++i) fprintf(targetFile, "PUSH R%d\n", i);
}

static void restoreRegsThrough(int lastReg) {
    int i;
    for (i = lastReg; i >= 0; --i) fprintf(targetFile, "POP R%d\n", i);
}

//stage6: Generate initialization and fixed 8-word free-list heap allocator logic.
static void emitHeapInit(void) {
    int loop = getLabel();
    int done = getLabel();

    /* Address 1024 stores the head.  Blocks are eight words each, starting
     * at 1032; the first word of each free block links to the next one. */
    fprintf(targetFile, "MOV R1, 1032\nMOV R2, 1040\nMOV R3, 2040\n");
    fprintf(targetFile, "L%d:\nMOV R4, R1\nEQ R4, R3\nJNZ R4, L%d\n", loop, done);
    fprintf(targetFile, "MOV [R1], R2\nADD R1, 8\nADD R2, 8\nJMP L%d\n", loop);
    fprintf(targetFile, "L%d:\nMOV R4, -1\nMOV [R1], R4\n", done);
    fprintf(targetFile, "MOV R1, 1024\nMOV R2, 1032\nMOV [R1], R2\n");
}

//stage6: Generate fixed-block allocation and preserve live registers across allocator work.
static void emitAlloc(int resultReg, int savedTop, int size) {
    int bad = getLabel();
    int haveBlock = getLabel();
    int done = getLabel();

    saveRegsThrough(savedTop);
    fprintf(targetFile, "MOV R1, %d\nMOV R2, 0\nMOV R3, R1\nLE R3, R2\nJNZ R3, L%d\n", size, bad);
    fprintf(targetFile, "MOV R2, 8\nMOV R3, R1\nGT R3, R2\nJNZ R3, L%d\n", bad);
    fprintf(targetFile, "MOV R1, [1024]\nMOV R2, -1\nMOV R3, R1\nEQ R3, R2\nJZ R3, L%d\n", haveBlock);
    fprintf(targetFile, "L%d:\nMOV R1, -1\nMOV R%d, R1\nJMP L%d\n", bad, resultReg, done);
    fprintf(targetFile, "L%d:\nMOV R2, [R1]\nMOV [1024], R2\n", haveBlock);
    fprintf(targetFile, "MOV R%d, R1\nL%d:\n", resultReg, done);
    restoreRegsThrough(savedTop);
}

//stage6: Validate, clear, and return a released block to the heap free list.
static void emitFree(int pointerReg, int savedTop) {
    int bad = getLabel();
    int scan = getLabel();
    int clear = getLabel();
    int clearLoop = getLabel();
    int done = getLabel();

    saveRegsThrough(savedTop);
    fprintf(targetFile, "MOV R1, R%d\nMOV R2, 1032\nMOV R3, R1\nLT R3, R2\nJNZ R3, L%d\n", pointerReg, bad);
    fprintf(targetFile, "MOV R2, 2040\nMOV R3, R1\nGT R3, R2\nJNZ R3, L%d\n", bad);
    fprintf(targetFile, "MOV R2, R1\nSUB R2, 1032\nMOV R3, 8\nMOD R2, R3\nJNZ R2, L%d\n", bad);
    fprintf(targetFile, "MOV R2, [1024]\nL%d:\nMOV R3, -1\nMOV R4, R2\nEQ R4, R3\nJNZ R4, L%d\n", scan, clear);
    fprintf(targetFile, "MOV R4, R2\nEQ R4, R1\nJNZ R4, L%d\nMOV R2, [R2]\nJMP L%d\n", bad, scan);
    fprintf(targetFile, "L%d:\nMOV R3, R1\nMOV R4, 0\nMOV R5, 8\n", clear);
    fprintf(targetFile, "L%d:\nMOV [R3], R4\nADD R3, 1\nSUB R5, 1\nJNZ R5, L%d\n", clearLoop, clearLoop);
    fprintf(targetFile, "MOV R2, [1024]\nMOV [R1], R2\nMOV [1024], R1\nJMP L%d\n", done);
    fprintf(targetFile, "L%d:\nL%d:\n", bad, done);
    restoreRegsThrough(savedTop);
}

static void emitRead(tnode *variable) {
    int addressReg = lvalueAddress(variable);

    fprintf(targetFile, "MOV R19, \"Read\"\nPUSH R19\n");
    fprintf(targetFile, "MOV R19, -1\nPUSH R19\n");
    fprintf(targetFile, "PUSH R%d\nPUSH R19\nPUSH R19\nCALL 0\n", addressReg);
    fprintf(targetFile, "POP R19\nPOP R19\nPOP R19\nPOP R19\nPOP R19\n");
    freeReg();
}

static void pushArgs(tnode *arguments) {
    int valueReg;

    if (arguments == NULL) {
        return;
    }

    pushArgs(arguments->right);
    valueReg = codeGen(arguments->left);
    fprintf(targetFile, "PUSH R%d\n", valueReg);
    freeReg();
}

static int argCount(tnode *arguments) {
    int count = 0;

    while (arguments != NULL) {
        ++count;
        arguments = arguments->right;
    }
    return count;
}

static int functionCall(tnode *t) {
    int savedTop = reg;
    int arguments = argCount(t->arglist);
    int i;
    int resultReg;

    for (i = 0; i <= savedTop; ++i) {
        fprintf(targetFile, "PUSH R%d\n", i);
    }

    pushArgs(t->arglist);
    fprintf(targetFile, "PUSH R19\n");
    fprintf(targetFile, "CALL F%d\n", t->Gentry->flabel);

    resultReg = getReg();
    fprintf(targetFile, "POP R%d\n", resultReg);
    while (arguments-- > 0) {
        /* Do not overwrite the returned value (which may be in R0). */
        fprintf(targetFile, "POP R19\n");
    }
    for (i = savedTop; i >= 0; --i) {
        fprintf(targetFile, "POP R%d\n", i);
    }
    return resultReg;
}

static void binary(tnode *t, const char *instruction) {
    int leftReg = codeGen(t->left);
    int rightReg = codeGen(t->right);

    fprintf(targetFile, "%s R%d, R%d\n", instruction, leftReg, rightReg);
    freeReg();
}

int codeGen(tnode *t) {
    int resultReg;
    int addressReg;
    int labelOne;
    int labelTwo;

    if (t == NULL) {
        return -1;
    }

    switch (t->nodetype) {
    case NODE_NUM:
        resultReg = getReg();
        fprintf(targetFile, "MOV R%d, %d\n", resultReg, t->val);
        return resultReg;

    case NODE_STR:
        resultReg = getReg();
        fprintf(targetFile, "MOV R%d, \"%s\"\n", resultReg, t->varname);
        return resultReg;

    //stage6: NULL is represented at runtime by the null pointer value 0.
    case NODE_NULL:
        resultReg = getReg();
        fprintf(targetFile, "MOV R%d, 0\n", resultReg);
        return resultReg;

    case NODE_ID:
        resultReg = idAddress(t);
        fprintf(targetFile, "MOV R%d, [R%d]\n", resultReg, resultReg);
        return resultReg;

    case NODE_ADDRESS:
        return idAddress(t->left);

    case NODE_DEREFERENCE:
        resultReg = codeGen(t->left);
        fprintf(targetFile, "MOV R%d, [R%d]\n", resultReg, resultReg);
        return resultReg;

    //stage6: Load and store user-defined fields through their allocated record pointer.
    case NODE_FIELD:
        resultReg = lvalueAddress(t);
        fprintf(targetFile, "MOV R%d, [R%d]\n", resultReg, resultReg);
        return resultReg;

    case NODE_ARRAY:
        resultReg = arrayAddress(t);
        fprintf(targetFile, "MOV R%d, [R%d]\n", resultReg, resultReg);
        return resultReg;

    case NODE_ARRAY2D:
        resultReg = array2Address(t);
        fprintf(targetFile, "MOV R%d, [R%d]\n", resultReg, resultReg);
        return resultReg;

    case NODE_PLUS: binary(t, "ADD"); return reg;
    case NODE_MINUS: binary(t, "SUB"); return reg;
    case NODE_MUL:
    case NODE_AND: binary(t, "MUL"); return reg;
    case NODE_DIV: binary(t, "DIV"); return reg;
    case NODE_MOD: binary(t, "MOD"); return reg;
    case NODE_LT: binary(t, "LT"); return reg;
    case NODE_GT: binary(t, "GT"); return reg;
    case NODE_LE: binary(t, "LE"); return reg;
    case NODE_GE: binary(t, "GE"); return reg;
    case NODE_EQ: binary(t, "EQ"); return reg;
    case NODE_NE: binary(t, "NE"); return reg;

    case NODE_OR:
        labelOne = codeGen(t->left);
        labelTwo = codeGen(t->right);
        fprintf(targetFile, "ADD R%d, R%d\n", labelOne, labelTwo);
        fprintf(targetFile, "MOV R%d, 0\n", labelTwo);
        fprintf(targetFile, "NE R%d, R%d\n", labelOne, labelTwo);
        freeReg();
        return labelOne;

    case NODE_FUNCTION:
        return functionCall(t);

    case NODE_ASSIGN:
        resultReg = codeGen(t->right);
        addressReg = lvalueAddress(t->left);
        fprintf(targetFile, "MOV [R%d], R%d\n", addressReg, resultReg);
        freeReg();
        freeReg();
        return -1;

    //stage6: Emit runtime allocation/free operations for user-defined values.
    case NODE_ALLOC:
        resultReg = getReg();
        emitAlloc(resultReg, resultReg - 1, GetSize(t->left->typeEntry));
        addressReg = lvalueAddress(t->left);
        fprintf(targetFile, "MOV [R%d], R%d\n", addressReg, resultReg);
        freeReg();
        freeReg();
        return -1;

    case NODE_FREE:
        resultReg = codeGen(t->left);
        emitFree(resultReg, reg);
        freeReg();
        return -1;

    case NODE_CONNECTOR:
        codeGen(t->left);
        codeGen(t->right);
        return -1;

    case NODE_READ:
        emitRead(t->left);
        return -1;

    case NODE_WRITE:
        resultReg = codeGen(t->left);
        fprintf(targetFile, "MOV R19, \"Write\"\nPUSH R19\n");
        fprintf(targetFile, "MOV R19, -2\nPUSH R19\n");
        fprintf(targetFile, "PUSH R%d\nPUSH R19\nPUSH R19\nCALL 0\n", resultReg);
        fprintf(targetFile, "POP R19\nPOP R19\nPOP R19\nPOP R19\nPOP R19\n");
        freeReg();
        return -1;

    case NODE_IF:
        resultReg = codeGen(t->left);
        labelOne = getLabel();
        labelTwo = getLabel();
        fprintf(targetFile, "JZ R%d, L%d\n", resultReg, labelOne);
        freeReg();
        codeGen(t->middle);
        fprintf(targetFile, "JMP L%d\nL%d:\n", labelTwo, labelOne);
        codeGen(t->right);
        fprintf(targetFile, "L%d:\n", labelTwo);
        return -1;

    case NODE_WHILE:
        labelOne = getLabel();
        labelTwo = getLabel();
        pushLoop(labelTwo, labelOne);
        fprintf(targetFile, "L%d:\n", labelOne);
        resultReg = codeGen(t->left);
        fprintf(targetFile, "JZ R%d, L%d\n", resultReg, labelTwo);
        freeReg();
        codeGen(t->right);
        fprintf(targetFile, "JMP L%d\nL%d:\n", labelOne, labelTwo);
        popLoop();
        return -1;

    case NODE_BREAK:
        if (loopTop >= 0) fprintf(targetFile, "JMP L%d\n", loopBreak[loopTop]);
        return -1;

    case NODE_CONTINUE:
        if (loopTop >= 0) fprintf(targetFile, "JMP L%d\n", loopContinue[loopTop]);
        return -1;

    case NODE_REPEAT:
        labelOne = getLabel();
        labelTwo = getLabel();
        pushLoop(labelTwo, labelTwo);
        fprintf(targetFile, "L%d:\n", labelOne);
        codeGen(t->right);
        fprintf(targetFile, "L%d:\n", labelTwo);
        resultReg = codeGen(t->left);
        fprintf(targetFile, "JZ R%d, L%d\n", resultReg, labelOne);
        freeReg();
        popLoop();
        return -1;

    case NODE_DOWHILE:
        labelOne = getLabel();
        labelTwo = getLabel();
        pushLoop(labelTwo, labelTwo);
        fprintf(targetFile, "L%d:\n", labelOne);
        codeGen(t->right);
        fprintf(targetFile, "L%d:\n", labelTwo);
        resultReg = codeGen(t->left);
        fprintf(targetFile, "JNZ R%d, L%d\n", resultReg, labelOne);
        freeReg();
        popLoop();
        return -1;

    case NODE_BODY:
        codeGen(t->left);
        return codeGen(t->right);

    case NODE_RETURN:
        resultReg = codeGen(t->left);
        if (generatingMain) {
            freeReg();
            emitExit();
            return -1;
        }
        addressReg = getReg();
        fprintf(targetFile, "MOV R%d, BP\n", addressReg);
        fprintf(targetFile, "SUB R%d, 2\n", addressReg);
        fprintf(targetFile, "MOV [R%d], R%d\n", addressReg, resultReg);
        freeReg();
        freeReg();
        while (currentLocalCount-- > 0) fprintf(targetFile, "POP R0\n");
        fprintf(targetFile, "POP BP\nRET\n");
        return -1;
    }
    return -1;
}

static void prologue(int localCount) {
    int i;

    fprintf(targetFile, "PUSH BP\nMOV BP, SP\n");
    for (i = 0; i < localCount; ++i) {
        fprintf(targetFile, "PUSH R0\n");
    }
}

void generateProgram(FunctionAST *functions) {
    FunctionAST *function;

    /* Header word 8 enables the runtime library used by Write/Read/Exit. */
    fprintf(targetFile, "0\n2056\n0\n0\n0\n0\n0\n1\n");
    fprintf(targetFile, "MOV SP, %d\n", nextBinding - 1);
    //stage6: Initialize the heap before main begins.
    emitHeapInit();
    fprintf(targetFile, "MOV R0, 0\n");
    fprintf(targetFile, "JMP MAIN\n");

    for (function = functions; function != NULL; function = function->next) {
        reg = -1;
        loopTop = -1;
        currentLocalCount = function->localCount;

        if (function->tree->nodetype == NODE_MAIN) {
            generatingMain = 1;
            fprintf(targetFile, "MAIN:\n");
            prologue(function->localCount);
            codeGen(function->tree->left);
        } else {
            generatingMain = 0;
            fprintf(targetFile, "F%d:\n", Lookup(function->name)->flabel);
            prologue(function->localCount);
            codeGen(function->tree);
        }
    }
}
