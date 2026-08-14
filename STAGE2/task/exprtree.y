%{
#include <stdio.h>
#include <stdlib.h>
#include "exprtree.h"
#include "codegen.h"

int yylex();
void yyerror(const char* s);

tnode* root;

FILE* targetFile;
%}

%union{
    tnode* node;
    char* str;
    int num;
}

%token <num> NUM
%token PLUS MINUS MUL DIV
%token <str> ID
%token T_BEGIN T_END READ WRITE
%token ASSIGN SEMICOLON

%type <node> Program Slist Stmt InputStmt OutputStmt AsgStmt E

%left PLUS MINUS
%left MUL DIV

%%

Program : T_BEGIN Slist T_END {
        root = $2;
    } 
    | T_BEGIN T_END {
        root = NULL; // no syntax tree as T_BEGIN & T_END are non-terminals
    };

Slist : Slist Stmt {
        $$ = makeConnectorNode($1, $2);
    }
    | Stmt {
        $$ = $1;
    };

Stmt : InputStmt {
        $$ = $1;
    }
    | OutputStmt {
        $$ = $1;
    }
    | AsgStmt {
        $$ = $1;
    };

InputStmt : READ '(' ID ')' SEMICOLON { // u read into a var, like read(b);
        $$ = makeReadNode(makeIdNode($3));
    };

OutputStmt : WRITE '(' E ')' SEMICOLON { // u can write an expression like write(5+8);
        $$ = makeWriteNode($3);
    };

AsgStmt : ID ASSIGN E SEMICOLON {
        $$ = makeAssignNode(makeIdNode($1), $3);
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
        $$ = makeNumNode($1);
    }
    | ID {
        $$ = makeIdNode($1);
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
    fprintf(targetFile, "MOV SP, 4200\n");

    yyparse();

    codeGen(root);

    fprintf(targetFile, "MOV R2, \"Exit\"\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "CALL 0\n");


    fclose(targetFile);
    return 0;
}

void yyerror(const char* s) {
    printf("%s\n", s);
}