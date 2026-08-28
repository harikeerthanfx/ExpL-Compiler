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

// we do SYNTAX ANALYSIS here - checking if the syntax is right like ID ASSIGN E SEMICOLON
// SEMANTIC ANALYSIS = is the meaning valid?
// a = b < c; can pass the above grammar SYNTACTICALLY, but fails SEMANTICALLY

%token <num> NUM 
%token <str> ID
// bison terminology: the SEMANTIC VALUE associated with NUM is <num>, 
// i.e bisons way of saying this is the C value associated with the grammar symbol

// these guys dont return a semantic value
// oru if kanda u know its an if, ok, but oru num kanda u need the value of the num, hence <num> exists
%token PLUS MINUS MUL DIV
%token LT GT LE GE EQ NE

%token T_BEGIN T_END 
%token ASSIGN SEMICOLON

%token READ WRITE
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL DOWHILE
%token BREAK CONTINUE
// 1. terminal -> %TOKEN (with or without semantic value, declaration required)
// 2. non terminal -> %TYPE (not required if no semantic value)

%type <node> Program Slist Stmt InputStmt OutputStmt AsgStmt E
%type <node> IfStmt WhileStmt
%type <node> BreakStmt ContinueStmt
%type <node> RepeatStmt DoWhileStmt

%left EQ NE // a + 5 < b * 2 -> (a + 5) < (b * 2) => arithmetic before comparison
%left LT GT LE GE // conventionally equality operators lower precedence than relation operators
%left PLUS MINUS
%left MUL DIV

%%
// the parser internally has one big automaton of ALL the grammar rules combined
// so when it asks for next yylex() call, it gets one lookahead token
// The current automaton state determines which tokens are valid next - 
// and which transition/action to take out of all the branches
// it decides between shift and reduce by matching the token
// if it is partially matched it shifts to next state, expecting another token
// if it matches a complete grammar rule, it reduces it into the non terminal on the left
// runs the yacc code block, and continues on with the reduced state

// matte missinte bottom up parser varacha manasilavum (stack, i/p, action)
// u will see stack being [begin, Slist, Stmt] after reducing second read to stmt
// so Slist Stmt, i.e stack top 2 elements have a reduction rule of Slist -> Slist Stmt
// so stack is [begin slist] again now

// so Slist is not storing the source text
// it just stores the pointer. the root = $2 (remember, %type <node> slist?)
// i.e Whenever I have an Slist, its semantic value is a tnode*
Program : T_BEGIN Slist T_END SEMICOLON {
        root = $2;
    } 
    | T_BEGIN T_END SEMICOLON {
        root = NULL; // no syntax tree as T_BEGIN & T_END are non-terminals
    };


// we play pointer passing/propagation not pointer traversal
// pointer traversal happens when we walk the AST in codeGen()
// we pass the same pointer upward .. Slist : Slist Stmtile $$ would be $2 of Program : Tbegin Slist Tend
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

    yyparse(); // yyparse() says "i need next token" -> calls yylex() reads chars from input
    // yylex() reads things char by char -> tries to match to LEX rules for every char 
    // when it finds a complete match -> it sets yytext to that value
    // then yylval takes the yytext, returns the token required and sends it to parser
    // i.e E : NUM { $$ : makeNode($1) } -> $1 is the value LEX put into yylval
    // athayith, YACC recieves NUM, to match and $1 contains the value
    // return tells YACC WHAT it received. yylval tells YACC the VALUE attached to it.
    // bison writes yylval as a global variable so when it calls yylex, it can access it

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