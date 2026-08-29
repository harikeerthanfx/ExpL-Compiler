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
    struct VarList *varlist;   // TASK1: stores the variable list — because VarList needs to pass the list to Bison
}

%token <num> NUM 
%token <str> ID

%token PLUS MINUS MUL DIV
%token LT GT LE GE EQ NE

%token T_BEGIN T_END 

%token DECL ENDDECL //TASK1
%token INT STR //TASK1

%token ASSIGN SEMICOLON

%token READ WRITE
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL DOWHILE
%token BREAK CONTINUE

%type <node> Program Slist Stmt InputStmt OutputStmt AsgStmt E
%type <node> IfStmt WhileStmt
%type <node> BreakStmt ContinueStmt
%type <node> RepeatStmt DoWhileStmt

%type <num> Type // TASK1: Type gets INT or STR and returns TYPE_INT or TYPE_STR as an integer value
%type <varlist> VarList // TASK1: VarList gives the variable list — because we need all variable names later

%left EQ NE 
%left LT GT LE GE 
%left PLUS MINUS
%left MUL DIV

%start Program

%%
//Task1 added things are Declarations, DeclList, Decl, Type, VarList
Declarations : DECL DeclList ENDDECL
            | DECL ENDDECL
            ;

DeclList : DeclList Decl
         | Decl
         ;

Decl : Type VarList SEMICOLON { // TASK1: goes through all variable names and installs each one with the declared type
        struct VarList *temp = $2;

        while (temp != NULL) {
            Install(temp->name, $1, 1);
            temp = temp->next;
        }
     } 
     ;

Type : INT {
        $$ = TYPE_INT;
     }
     | STR {
        $$ = TYPE_STR;
     }
     ;

VarList : VarList ',' ID { // TASK1: creates and adds each variable name to the variable list
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $3;
            newVar->next = NULL;

            struct VarList *temp = $1;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newVar;

            $$ = $1;
        }

        | ID {
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $1;
            newVar->next = NULL;

            $$ = newVar;
        }
        ;

Program : Declarations T_BEGIN Slist T_END SEMICOLON {
            root = $3;
        }
        | Declarations T_BEGIN T_END SEMICOLON {
            root = NULL;
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
    } 
    | IfStmt {
        $$ = $1;
    } 
    | WhileStmt {
        $$ = $1;
    } 
    | BreakStmt {
        $$ = $1;
    }
    | ContinueStmt {
        $$ = $1;
    }
    | RepeatStmt {
        $$ = $1;
    }
    | DoWhileStmt {
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

IfStmt : IF '(' E ')' THEN Slist ELSE Slist ENDIF SEMICOLON {
        $$ = makeIfNode($3, $6, $8);
    }
    | IF '(' E ')' THEN Slist ENDIF SEMICOLON {
        $$ = makeIfNode($3, $6, NULL);
    };

WhileStmt : WHILE '(' E ')' DO Slist ENDWHILE SEMICOLON {
        $$ = makeWhileNode($3, $6);        
    };

BreakStmt : BREAK SEMICOLON {
        $$ = makeBreakNode();
    };

ContinueStmt : CONTINUE SEMICOLON {
        $$ = makeContinueNode();
    };

RepeatStmt : REPEAT Slist UNTIL '(' E ')' SEMICOLON {
        $$ = makeRepeatNode($2, $5);
    };

DoWhileStmt : DO Slist WHILE '(' E ')' SEMICOLON {
        $$ = makeDoWhileNode($2, $5);
    };

    
E : E PLUS E {
        $$ = makeOperatorNode("+", $1, $3);
    }
    | E MINUS E {
        $$ = makeOperatorNode("-", $1, $3);
    }
    | E MUL E {
        $$ = makeOperatorNode("*", $1, $3);
    }
    | E DIV E {
        $$ = makeOperatorNode("/", $1, $3);
    }
    | '(' E ')' {
        $$ = $2;
    }
    | E LT E {
        $$ = makeOperatorNode("<", $1, $3);
    }
    | E GT E {
        $$ = makeOperatorNode(">", $1, $3);
    }
    | E LE E {
        $$ = makeOperatorNode("<=", $1, $3);
    }
    | E GE E {
        $$ = makeOperatorNode(">=", $1, $3);
    }
    | E EQ E {
        $$ = makeOperatorNode("==", $1, $3);
    }
    | E NE E {
        $$ = makeOperatorNode("!=", $1, $3);
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
    fprintf(targetFile, "MOV SP, 4200\n"); // the first mem addr from which we gon store stuff
    // usually we initialize it to 4095 (so push increments it to 4096)

    yyparse(); 
    
    PrintSymbolTable(); // TASK1: prints the symbol table to check the installed variables

    codeGen(root); //TASK2

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