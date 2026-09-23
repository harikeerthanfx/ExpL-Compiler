%{
#include <stdio.h>
#include <stdlib.h>
#include "exprtree.h"
#include "codegen.h"

int yylex();
void yyerror(const char* s);

tnode* root;

FILE* targetFile;
extern int nextBinding;
%}

%union{
    tnode* node;
    char* str;
    int num;
    struct VarList *varlist;   // TASK1: stores the variable list — because VarList needs to pass the list to Bison
    struct GDeclItem *gdecllist;
    struct Paramstruct *paramlist;
    struct ArgList *arglist;
}

%token <num> NUM 
%token <str> ID

%token <str> STRING //TASK1 forgotten croassroads(iykyk)

%token PLUS MINUS MUL DIV MOD //EX2 change for mod

%token ADDRESS // EX2: token for the address-of operator

%token LT GT LE GE EQ NE
%token AND OR NOT

%token T_BEGIN T_END 

%token DECL ENDDECL //TASK1
%token INT STR //TASK1
%token MAIN

%token ASSIGN SEMICOLON

%token READ WRITE
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL DOWHILE
%token BREAK CONTINUE
%token RETURN

%type <node> Program Slist Stmt InputStmt OutputStmt AsgStmt E Variable //TASK3 new nonterminal Variable added
%type <node> IfStmt WhileStmt
%type <node> BreakStmt ContinueStmt
%type <node> RepeatStmt DoWhileStmt

%type <num> Type // TASK1: Type gets INT or STR and returns TYPE_INT or TYPE_STR as an integer value
%type <varlist> VarList // TASK1: VarList gives the variable list — because we need all variable names later
%type <gdecllist> Gid GidList
%type <paramlist> Param ParamList ParamListOpt
%type <arglist> ArgList ArgListOpt

%left OR
%left AND
%left EQ NE 
%left LT GT LE GE 
%left PLUS MINUS
%left MUL DIV MOD
%right NOT

%start Program

%%
// Stage 5 Task 1: global declarations can contain variables or function
// signatures.  A signature is stored in the GST; its body is a later task.
Declarations : DECL DeclList ENDDECL
            | DECL ENDDECL
            ;

DeclList : DeclList Decl
         | Decl
         ;

Decl : Type GidList SEMICOLON {
        struct GDeclItem *temp = $2;
        while (temp != NULL) {
            int varType = $1;
            if (temp->isPointer) {
                if ($1 == TYPE_INT)
                    varType = TYPE_INT_PTR;
                else if ($1 == TYPE_STR)
                    varType = TYPE_STR_PTR;
            }
            if (temp->isFunction)
                InstallFunction(temp->name, varType, temp->paramlist);
            else
                Install(temp->name, varType, temp->size, temp->rows, temp->cols);
            temp = temp->next;
        }
    }
    ;

GidList : GidList ',' Gid {
            struct GDeclItem *tail = $1;
            while (tail->next != NULL) tail = tail->next;
            tail->next = $3;
            $$ = $1;
        }
        | Gid { $$ = $1; }
        ;

Gid : ID {
        $$ = calloc(1, sizeof(*$$)); $$->name = $1; $$->size = 1; $$->rows = 1; $$->cols = 1;
    }
    | ID '[' NUM ']' {
        $$ = calloc(1, sizeof(*$$)); $$->name = $1; $$->size = $3; $$->rows = 1; $$->cols = $3;
    }
    | ID '[' NUM ']' '[' NUM ']' {
        $$ = calloc(1, sizeof(*$$)); $$->name = $1; $$->size = $3 * $6; $$->rows = $3; $$->cols = $6;
    }
    | MUL ID {
        $$ = calloc(1, sizeof(*$$)); $$->name = $2; $$->size = 1; $$->rows = 1; $$->cols = 1; $$->isPointer = 1;
    }
    | ID '(' ParamListOpt ')' {
        $$ = calloc(1, sizeof(*$$)); $$->name = $1; $$->isFunction = 1; $$->paramlist = $3;
    }
    ;

ParamListOpt : ParamList { $$ = $1; }
             | /* empty */ { $$ = NULL; }
             ;

ParamList : ParamList ',' Param {
            struct Paramstruct *tail = $1;
            while (tail->next != NULL) tail = tail->next;
            tail->next = $3;
            $$ = $1;
          }
          | Param { $$ = $1; }
          ;

Param : Type ID {
        $$ = calloc(1, sizeof(*$$)); $$->type = $1; $$->name = $2;
    }
    ;

Type : INT {
        $$ = TYPE_INT;
     }
     | STR {
        $$ = TYPE_STR;
     }
     ;

//varlist updated for TASK3
/*
This supports all of these:
int a;
int a[10];
int a, b[5];
int a[10], b, c[20];
*/
VarList : VarList ',' ID '[' NUM ']' '[' NUM ']' { // EX1: VarList updated to support 2D arrays by storing rows, columns, and total allocated size
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $3;
            newVar->rows = $5;
            newVar->cols = $8;
            newVar->size = $5 * $8;
            newVar->next = NULL;

            struct VarList *temp = $1;
            while (temp->next != NULL) temp = temp->next;
            temp->next = newVar;
            $$ = $1;
        }
        | VarList ',' ID '[' NUM ']' { 
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $3;
            newVar->rows = 1;
            newVar->cols = $5;
            newVar->size = $5;
            newVar->next = NULL;

            struct VarList *temp = $1;
            while (temp->next != NULL) temp = temp->next;
            temp->next = newVar;
            $$ = $1;
        }
        | VarList ',' ID {
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $3;
            newVar->rows = 1;
            newVar->cols = 1;
            newVar->size = 1;
            newVar->next = NULL;

            struct VarList *temp = $1;
            while (temp->next != NULL) temp = temp->next;
            temp->next = newVar;
            $$ = $1;
        }
        | ID '[' NUM ']' '[' NUM ']' {  // EX1: VarList updated to support 2D arrays by storing rows, columns, and total allocated size
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $1;
            newVar->isPointer = 0; // EX2: marks this as a normal variable, not a pointer
            newVar->rows = $3;
            newVar->cols = $6;
            newVar->size = $3 * $6;
            newVar->next = NULL;
            $$ = newVar;
        }
        | ID '[' NUM ']' {
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $1;
            newVar->isPointer = 0; // EX2: marks this as a normal variable, not a pointer
            newVar->rows = 1;
            newVar->cols = $3;
            newVar->size = $3;
            newVar->next = NULL;
            $$ = newVar;
        }
        | ID {
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $1;
            newVar->isPointer = 0; // EX2: marks this as a normal variable, not a pointer
            newVar->rows = 1;
            newVar->cols = 1;
            newVar->size = 1;
            newVar->next = NULL;
            $$ = newVar;
        }
        | VarList ',' MUL ID { // EX2: adds a pointer variable to the declaration list
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $4;
            newVar->isPointer = 1;
            newVar->rows = 1;
            newVar->cols = 1;
            newVar->size = 1;
            newVar->next = NULL;
            struct VarList *temp = $1;
            while (temp->next != NULL) temp = temp->next;
            temp->next = newVar;
            $$ = $1;
        }
        | MUL ID { // EX2: creates the first pointer variable in the declaration list
            struct VarList *newVar = malloc(sizeof(struct VarList));
            newVar->name = $2;
            newVar->isPointer = 1;
            newVar->rows = 1;
            newVar->cols = 1;
            newVar->size = 1;
            newVar->next = NULL;
            $$ = newVar;
        };

/* The first two alternatives are the previous Stage 4 program forms.  The
 * Stage 5 alternatives now construct a separate AST for every function. */
Program : Declarations T_BEGIN Slist T_END SEMICOLON {
            root = $3;
        }
        | Declarations T_BEGIN T_END SEMICOLON {
            root = NULL;
        }
        | Declarations FdefBlock MainBlock {
            root = NULL;
        }
        | FdefBlock MainBlock {
            root = NULL;
        }
        | MainBlock {
            root = NULL;
        }
        ;

FdefBlock : FdefBlock Fdef
          | Fdef
          ;

Fdef : Type ID '(' ParamListOpt ')' {
            BeginFunctionDefinition($1, $2, $4);
       } '{' LocalDeclBlock FunctionBody '}'
     ;

MainBlock : INT MAIN '(' ')' {
                BeginMainDefinition("main");
            } '{' LocalDeclBlock FunctionBody '}'
          ;

FunctionBody : T_BEGIN Slist RETURN E SEMICOLON T_END {
                    FinishFunctionDefinition($2, $4);
                }
             | T_BEGIN RETURN E SEMICOLON T_END {
                    FinishFunctionDefinition(NULL, $3);
                }
             ;

LocalDeclBlock : DECL LocalDeclList ENDDECL
               | DECL ENDDECL
               ;

LocalDeclList : LocalDeclList LocalDecl
              | LocalDecl
              ;

LocalDecl : Type GidList SEMICOLON {
                InstallLocalDeclarations($1, $2);
            }
          ;

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

//TASK3: new nonterminal production added
//n       → makeIdNode("n")
//arr[i]  → makeArrayNode("arr", AST of i)
Variable : ID {
            $$ = makeIdNode($1);
        }

        | ID '[' E ']' {
            $$ = makeArrayNode($1, $3);
        }

        | ID '[' E ']' '[' E ']' { // EX1: supports 2D array access like a[i][j]
            $$ = makeArray2DNode($1, $3, $6);
        }

        | MUL Variable { // EX2: allows a dereferenced pointer as an assignment target
            $$ = makeDereferenceNode($2);
        }
        ;

//TASK3 changes
InputStmt : READ '(' Variable ')' SEMICOLON {
        $$ = makeReadNode($3);
    };

OutputStmt : WRITE '(' E ')' SEMICOLON { // u can write an expression like write(5+8);
        $$ = makeWriteNode($3);
    };

//TASK3 changes
AsgStmt : Variable ASSIGN E SEMICOLON {
        $$ = makeAssignNode($1, $3);
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
    | E MOD E {
        $$ = makeOperatorNode("%", $1, $3); // EX2: creates the modulo operation node               
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
    | E AND E {
        $$ = makeOperatorNode("&&", $1, $3);
    }
    | E OR E {
        $$ = makeOperatorNode("||", $1, $3);
    }
    | NOT E {
        $$ = makeOperatorNode("!", $2, NULL);
    }
    | NUM {
        $$ = makeNumNode($1);
    }
    | STRING { //TASK1 forgotten crossroads(iykyk)
        $$ = makeStrNode($1);
    }
    | ADDRESS Variable { // EX2: address-of operator returns the address of a variable
        $$ = makeAddressNode($2);
    }
    | ID '(' ArgListOpt ')' {
        $$ = makeFuncCallNode($1, $3);
    }
    | Variable { //TASK3 change here
        $$ = $1;
    };

ArgListOpt : ArgList { $$ = $1; }
           | /* empty */ { $$ = NULL; }
           ;

ArgList : ArgList ',' E { $$ = appendArg($1, $3); }
        | E { $$ = makeArgList($1); }
        ;

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

    /* Global declarations determine the first safe stack location. */
    fprintf(targetFile, "MOV SP, %d\n", nextBinding - 1);
    
    PrintSymbolTable(); // TASK1: prints the symbol table to check the installed variables

    if (GetFunctionASTs() != NULL) {
        generateStage5Code();
    } else {
        /* Preserve the old Stage 4 top-level program path. */
        codeGen(root);
        fprintf(targetFile, "MOV R2, \"Exit\"\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "CALL 0\n");
    }


    fclose(targetFile);
    return 0;
}

void yyerror(const char* s) {
    printf("%s\n", s);
}
