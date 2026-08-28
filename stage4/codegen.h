#ifndef CODEGEN_H
#define CODEGEN_H

#include "exprtree.h"

int getReg();
void freeReg();
int getLabel();

int codeGen(tnode *root);

#endif