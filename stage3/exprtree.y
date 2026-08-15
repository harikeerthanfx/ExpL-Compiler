%{
#include <stdio.h>
#include <stdlib.h>
#include "exprtree.h"
#include "codegen.h"

int yylex();
void yyerror(const char *s);

tnode *root;

FILE *targetFile;
%}

%union {
    tnode *node;
    char *str;
    int num;
}

%token <num> NUM
%token <str> ID

%token PLUS MINUS MUL DIV
%token LT GT LE GE EQ NE

%token T_BEGIN T_END
%token READ WRITE
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token BREAK CONTINUE
%token ASSIGN SEMICOLON

%type <node> Program Slist Stmt
%type <node> InputStmt OutputStmt AsgStmt
%type <node> IfStmt WhileStmt
%type <node> BreakStmt ContinueStmt
%type <node> E

%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MUL DIV

%%

Program
    : T_BEGIN Slist T_END
    {
        root = $2;
    }
    | T_BEGIN T_END
    {
        root = NULL;
    }
    ;

Slist
    : Slist Stmt
    {
        $$ = makeConnectorNode($1, $2);
    }
    | Stmt
    {
        $$ = $1;
    }
    ;

Stmt
    : InputStmt
    {
        $$ = $1;
    }
    | OutputStmt
    {
        $$ = $1;
    }
    | AsgStmt
    {
        $$ = $1;
    }
    | IfStmt
    {
        $$ = $1;
    }
    | WhileStmt
    {
        $$ = $1;
    }
    | BreakStmt
    {
        $$ = $1;
    }
    | ContinueStmt
    {
        $$ = $1;
    }
    ;

InputStmt
    : READ '(' ID ')' SEMICOLON
    {
        $$ = makeReadNode(makeIdNode($3));
    }
    ;

OutputStmt
    : WRITE '(' E ')' SEMICOLON
    {
        $$ = makeWriteNode($3);
    }
    ;

AsgStmt
    : ID ASSIGN E SEMICOLON
    {
        $$ = makeAssignNode(makeIdNode($1), $3);
    }
    ;

IfStmt
    : IF '(' E ')' THEN Slist ELSE Slist ENDIF SEMICOLON
    {
        $$ = makeIfNode($3, $6, $8);
    }
    | IF '(' E ')' THEN Slist ENDIF SEMICOLON
    {
        $$ = makeIfNode($3, $6, NULL);
    }
    ;

WhileStmt
    : WHILE '(' E ')' DO Slist ENDWHILE SEMICOLON
    {
        $$ = makeWhileNode($3, $6);
    }
    ;

BreakStmt
    : BREAK SEMICOLON
    {
        $$ = makeBreakNode();
    }
    ;

ContinueStmt
    : CONTINUE SEMICOLON
    {
        $$ = makeContinueNode();
    }
    ;

E
    : E PLUS E
    {
        $$ = makeOperatorNode('+', $1, $3);
    }
    | E MINUS E
    {
        $$ = makeOperatorNode('-', $1, $3);
    }
    | E MUL E
    {
        $$ = makeOperatorNode('*', $1, $3);
    }
    | E DIV E
    {
        $$ = makeOperatorNode('/', $1, $3);
    }

    | E LT E
    {
        $$ = makeRelationalNode("<", $1, $3);
    }
    | E GT E
    {
        $$ = makeRelationalNode(">", $1, $3);
    }
    | E LE E
    {
        $$ = makeRelationalNode("<=", $1, $3);
    }
    | E GE E
    {
        $$ = makeRelationalNode(">=", $1, $3);
    }
    | E EQ E
    {
        $$ = makeRelationalNode("==", $1, $3);
    }
    | E NE E
    {
        $$ = makeRelationalNode("!=", $1, $3);
    }

    | '(' E ')'
    {
        $$ = $2;
    }
    | NUM
    {
        $$ = makeNumNode($1);
    }
    | ID
    {
        $$ = makeIdNode($1);
    }
    ;

%%

int main(int argc, char *argv[])
{
    extern FILE *yyin;

    if (argc < 2)
    {
        printf("Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");

    if (yyin == NULL)
    {
        printf("Cannot open input file\n");
        return 1;
    }

    targetFile = fopen("target.xsm", "w");

    if (targetFile == NULL)
    {
        printf("Could not open target.xsm\n");
        fclose(yyin);
        return 1;
    }

    /* XSM header */
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

    /* Generate target code */
    if (root != NULL)
        codeGen(root);

    /* Exit */
    fprintf(targetFile, "MOV R2, \"Exit\"\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "PUSH R2\n");
    fprintf(targetFile, "CALL 0\n");

    fclose(targetFile);
    fclose(yyin);

    return 0;
}

void yyerror(const char *s)
{
    printf("%s\n", s);
}