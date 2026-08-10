%{
#include <stdio.h>
#include <stdlib.h>
#include "exprtree.h"
#include "codegen.h"

int yylex();
void yyerror(const char* s);

FILE* targetFile;
%}

%union{
    tnode* node;
}

%token <node> NUM;
%token PLUS MINUS MUL DIV END;

%type <node> E program;

%left PLUS MINUS
%left MUL DIV

%%

program : E END {
    int resultReg = codeGen($1);
    writeResult(resultReg);
    freeReg();
};

E : E PLUS E {
        $$ = makeOperatorNode('+', $1, $3);
    }
    | E MINUS E {
        $$ = makeOperatorNode('-', $1, $3);
    }
    | E MUL E {
        $$ = makeOperatorNode('*', $1, $3);
    }
    | E DIV E {
        $$ = makeOperatorNode('/', $1, $3);
    }
    | '(' E ')' {
        $$ = $2;
    }
    | NUM {
        $$ = $1;
    };

%%

extern FILE* yyin; // it is file pointer of lexer. defaulted to "stdin"
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Cannot open input file\n");
        return 1;
    }

    targetFile = fopen("target.xsm", "w");
    if (!targetFile) {
        printf("Could not open target file\n");
        exit(1);
    }

    fprintf(targetFile, "0\n");
    fprintf(targetFile, "2056\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");

    yyparse();

    fclose(targetFile);
    return 0;
}

void yyerror(const char* s) {
    printf("%s\n", s);
}