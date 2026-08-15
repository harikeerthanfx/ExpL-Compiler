#ifndef CODEGEN_H
#define CODEGEN_H

#include "exprtree.h"

int getReg();
void freeReg();

int codeGen(tnode *root);
void writeResult(int resultReg);

#endif