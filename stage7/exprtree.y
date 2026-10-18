%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exprtree.h"
#include "codegen.h"

int yylex();
void yyerror(const char* s);

tnode* root;

FILE* targetFile;
int nextFunctionLabel = 0;
%}

%union{
    tnode* node;
    char* str;
    int num;
    VarList* varlist;
    Paramstruct* paramlist;
    Typetable* type;
    DeclType* declType; //stage7: "Type-name reductions must distinguish record types from classes for legal declaration contexts."
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
%token PLUS MINUS MUL DIV MOD
%token LT GT LE GE EQ NE OR AND

%token ADDRESS

/* ExpL user-defined types and heap operations (semantic support is pending). */
%token TYPE ENDTYPE ALLOC FREE
%token T_NULL
%token CLASS ENDCLASS SELF NEW DELETE //stage7: "The front end recognizes class delimiters, self, and the OExpL class allocation keywords."

%token T_BEGIN T_END 
%token DECL ENDDECL
%token INT STR
%token ASSIGN SEMICOLON
%token MAIN RETURN

%token READ WRITE
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL
%token BREAK CONTINUE
%token <str> STRING

// 1. terminal -> %TOKEN (with or without semantic value, declaration required)
// 2. non terminal -> %TYPE (not required if no semantic value)

%type <node> Program MainBlock Body ReturnStmt ArgList Slist Stmt InputStmt OutputStmt AsgStmt E Variable Field MethodCall
%type <node> IfStmt WhileStmt
%type <node> BreakStmt ContinueStmt
%type <node> RepeatStmt DoWhileStmt

%type <declType> TypeName //stage7: "A global declaration name can resolve to either a type-table or class-table entry."
%type <type> FType FieldType
%type <str> ClassMemberType //stage7: "Class member declarations share an identifier token for both type names and class names."
%type <varlist> VarList
%type <varlist> IdList
%type <paramlist> ParamList
%type <paramlist> Param

%left OR
%left AND
%left EQ NE // a + 5 < b * 2 -> (a + 5) < (b * 2) => arithmetic before comparison
%left LT GT LE GE // conventionally equality operators lower precedence than relation operators
%left PLUS MINUS
%left MUL DIV MOD
%right UMINUS

%start Program

%%
// the parser internally has one big automaton of ALL the grammar rules combined
// so when it asks for next yylex() call, it gets one lookahead token
// The current automaton state determines which tokens are valid next - 
// and which transition/action to take out of all the branches
// it decides between shift and reduce by matching the token
// if it is partially matched it shifts to next state, expecting another token
// if it matches a complete grammar rule, it reduces it into the non terminal on the left
// runs the yacc code block, and continues on with the reduced state
Program : TypeDefBlock ClassDefBlock GDeclBlock FDefBlock MainBlock { $$ = $5; } //stage7: "Classes are declared after record types and before globals as specified by OExpL."
        | TypeDefBlock ClassDefBlock GDeclBlock MainBlock           { $$ = $4; }
        | TypeDefBlock ClassDefBlock MainBlock                      { $$ = $3; };

//stage7: "The class section is optional, but when present contains all class declarations before globals."
ClassDefBlock : CLASS ClassDefList ENDCLASS
              | /* empty */
              ;

//stage7: "Install each class before its members so a class can contain a field of its own class type."
ClassDefList : ClassDefList ClassDef
             | ClassDef
             ;

//stage7: "Fields and method signatures are declared first; method bodies follow, matching the OExpL class syntax."
ClassDef : ID '{' {
             if (CInstall($1, NULL) == NULL) {
                 fprintf(stderr, "Error: class '%s' is duplicated or cannot be started\n", $1);
                 exit(1);
             }
         } DECL ClassMemberDeclList ENDDECL MethodDefList '}' {
             Class_FinishDefinition(Class_CurrentDefinition());
         }
         ;

//stage7: "Member declarations are kept separate so field-before-method order can be checked by the class table."
ClassMemberDeclList : ClassMemberDeclList ClassMemberDecl
                    | /* empty */
                    ;

ClassMemberDecl : ClassMemberType ID SEMICOLON {
                     if (Class_Finstall(Class_CurrentDefinition(), $1, $2) == NULL) {
                         fprintf(stderr, "Error: invalid, duplicate, or excess field '%s' in class\n", $2);
                         exit(1);
                     }
                 }
                 | ClassMemberType ID '(' ParamList ')' SEMICOLON {
                     Typetable *returnType = TLookup($1);
                     if (returnType == NULL || CLookup($1) != NULL ||
                         Class_Minstall(Class_CurrentDefinition(), $2, returnType, $4) == NULL) {
                         fprintf(stderr, "Error: invalid, duplicate, or excess method '%s' in class\n", $2);
                         exit(1);
                     }
                 }
                 ;

//stage7: "Class fields may name primitive types, user-defined types, or classes; methods cannot return class objects."
ClassMemberType : INT { $$ = "int"; }
                | STR { $$ = "str"; }
                | ID  { $$ = $1; }
                ;

//stage7: "Every method body must match a prior method signature and is checked with its own implicit self scope."
MethodDefList : MethodDefList MethodDef
              | /* empty */
              ;

MethodDef : FType ID '(' ParamList ')' {
               Classtable *owner = Class_CurrentDefinition();
               Memberfunclist *method = Class_Mlookup(owner, $2);
               if (method == NULL) {
                   fprintf(stderr, "Error: method '%s' has no declaration in its class\n", $2);
                   exit(1);
               }
               CheckMethodDefinition(owner, method, $1, $4);
               BeginMethodScope(owner, method);
           } '{' LdeclBlock Body '}' {
               Classtable *owner = Class_CurrentDefinition();
               Memberfunclist *method = Class_Mlookup(owner, $2);
               tnode *checkedBody = finalizeBodyNode($9, $1, $2);
               SaveMethodAST(owner, method, checkedBody); //stage7: part2 "Save the validated method tree for labelled object-code emission."
               EndMethodScope();
           }
           ;

/* //stage6: Parse type definitions and install their fields in the type table. */
/* Type-table actions from the reference grammar need a type table in this project. */
TypeDefBlock : TYPE TypeDefList ENDTYPE
             | /* empty */
             ;

TypeDefList : TypeDefList TypeDef
            | TypeDef
            ;

TypeDef : ID '{' {
            if (TBeginDefinition($1) == NULL) {
                fprintf(stderr, "Error: type '%s' is already defined or cannot be started\n", $1);
                exit(1);
            }
        } FieldDeclList '}' {
            TFinishDefinition(TCurrentDefinition());
        }
        ;

FieldDeclList : FieldDeclList FieldDecl
              | FieldDecl
              ;

FieldDecl : FieldType ID SEMICOLON {
              Typetable *owner = TCurrentDefinition();
              if (owner == NULL ||
                  FInstall(owner, $2, $1, owner->size) == NULL) {
                  fprintf(stderr, "Error: duplicate field '%s' or invalid field declaration\n", $2);
                  exit(1);
              }
              owner->size++;
          }
          ;

/* //stage6: Restrict aggregate fields to primitive or previously declared/self types. */
FieldType : INT { $$ = TLookup("int"); }
          | STR { $$ = TLookup("str"); }
          | ID {
              $$ = TLookup($1);
              if ($$ == NULL || !IsUserDefinedType($$)) {
                  fprintf(stderr, "Error: field type '%s' is undefined or not declarable here\n", $1);
                  exit(1);
              }
          }
          ;

GDeclBlock : DECL GDeclList ENDDECL | DECL ENDDECL;

GDeclList : GDeclList GDecl | GDecl;

/* //stage6: Resolve declared user types for globals, locals, parameters, and functions. */
GDecl : TypeName VarList SEMICOLON {
    // if i have like 
    // int a, b;
    // int c, d; (in two lines)
    // the varlist gon be like two diff linked lists, and each time reduction is done, we traverse thru
    VarList* temp = $2;

    while (temp != NULL) {
        Typetable *varType = $1->type;
        //stage7: "Class variables are global object references and cannot be arrays, pointers, or function declarations."
        if ($1->classType != NULL) {
            if (temp->isFunction || temp->isArray || temp->isPointer) {
                fprintf(stderr, "Error: class variables can only be simple global variables\n");
                exit(1);
            }
            InstallClassGlobal(temp->name, $1->classType);
            temp = temp->next;
            continue;
        }
        if (temp->isPointer) {
            varType = PointerType($1->type);
            if (varType == NULL) {
                fprintf(stderr, "Error: pointer declarations are supported only for int and str\n");
                exit(1);
            }
        }

        if (temp->isFunction) {
            Install(temp->name, varType, 0, 0, 0, 0, temp->paramlist,
                    nextFunctionLabel++);
        } else {
            Install(temp->name, varType, temp->size, temp->rows, temp->cols,
                    temp->isArray,
                    NULL, -1);
        }
        temp = temp->next;
    }
};

/* //stage6: Resolve user-defined types in declarations and function signatures. */
//stage7: "Preserve whether a name resolved to a record type or to a class for context-specific semantic checks."
TypeName : INT {
             $$ = calloc(1, sizeof(DeclType));
             $$->type = TLookup("int");
         }
         | STR {
             $$ = calloc(1, sizeof(DeclType));
             $$->type = TLookup("str");
         }
         | ID  {
             $$ = calloc(1, sizeof(DeclType));
             $$->type = TLookup($1);
             $$->classType = CLookup($1);
             if (($$->type == NULL || !IsUserDefinedType($$->type)) && $$->classType == NULL) {
                 fprintf(stderr, "Error: type or class '%s' is not defined yet\n", $1);
                 exit(1);
             }
         }
         ;

FType : TypeName {
          if ($1->classType != NULL) {
              fprintf(stderr, "Error: functions and methods cannot return class values\n");
              exit(1);
          }
          $$ = $1->type;
      }
      | TypeName MUL {
          if ($1->classType != NULL) {
              fprintf(stderr, "Error: pointers to class values are not supported\n");
              exit(1);
          }
          $$ = PointerType($1->type);
          if ($$ == NULL) {
              fprintf(stderr, "Error: pointer types are supported only for int and str\n");
              exit(1);
          }
      };

/*
This supports all of these:
    int a;
    int a[10];
    int a[10], b[10];
    int a, b[5];
    int a[10], b, c[20];
*/
VarList : VarList ',' ID '[' NUM ']' '[' NUM ']' {
        VarList* newVar = calloc(1, sizeof(VarList));
        newVar->name = $3;
        newVar->rows = $5;
        newVar->cols = $8;
        newVar->size = $5 * $8;
        newVar->isArray = 1;
        newVar->isPointer = 0;
        newVar->paramlist = NULL;
        newVar->next = NULL;

        VarList* temp = $1;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newVar;
        $$ = $1;
    }
    | ID '[' NUM ']' '[' NUM ']' { 
        VarList *newVar = calloc(1, sizeof(struct VarList));
        newVar->name = $1;
        newVar->rows = $3;
        newVar->cols = $6;
        newVar->size = $3 * $6;
        newVar->isArray = 1;
        newVar->isPointer = 0;
        newVar->paramlist = NULL;
        newVar->next = NULL;
        $$ = newVar;
    }
    | VarList ',' ID '[' NUM ']' { 
        VarList* newVar = calloc(1, sizeof(VarList));
        newVar->name = $3;
        newVar->size = $5; // array ayond size koduk
        newVar->rows = $5;
        newVar->isArray = 1;
        newVar->isPointer = 0;
        newVar->paramlist = NULL;
        newVar->next = NULL;

        VarList* temp = $1;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newVar;
        $$ = $1;
    }
    | ID '[' NUM ']' {
        VarList* newVar = calloc(1, sizeof(VarList));
        newVar->name = $1;
        newVar->size = $3;
        newVar->rows = $3;
        newVar->isArray = 1;
        newVar->isPointer = 0;
        newVar->paramlist = NULL;
        newVar->next = NULL;
        $$ = newVar;
    }
    | VarList ',' ID {
        VarList* newVar = calloc(1, sizeof(VarList));
        newVar->name = $3;
        newVar->size = 1;
        newVar->isPointer = 0;
        newVar->paramlist = NULL;
        newVar->next = NULL;

        VarList* temp = $1;
        while (temp->next) {
            temp = temp->next;
        }

        temp->next = newVar;
        $$ = $1;
    }
    | ID {
        VarList* newVar = calloc(1, sizeof(VarList));
        newVar->name = $1;
        newVar->size = 1;
        newVar->isPointer = 0;
        newVar->paramlist = NULL;
        newVar->next = NULL;

        $$ = newVar;
    } 
    | VarList ',' MUL ID { // adds a pointer variable to the declaration list
        VarList* newVar = calloc(1, sizeof(VarList));
        newVar->name = $4;
        newVar->isPointer = 1;
        newVar->size = 1;
        newVar->paramlist = NULL;
        newVar->next = NULL;
        
        VarList *temp = $1;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newVar;
        $$ = $1;
    } 
    | MUL ID {
        VarList *newVar = calloc(1, sizeof(VarList));
        newVar->name = $2;
        newVar->isPointer = 1;
        newVar->size = 1;
        newVar->paramlist = NULL;
        newVar->next = NULL;
        $$ = newVar;
    }
    |  VarList ',' ID'('ParamList')' {
        VarList *newVar = calloc(1, sizeof(VarList));

        newVar->name = $3;
        newVar->size = 1;
        newVar->rows = 0;
        newVar->cols = 0;
        newVar->isPointer = 0;
        newVar->isFunction = 1;
        newVar->paramlist = $5;
        newVar->next = NULL;

        VarList *temp = $1;
        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newVar;
        $$ = $1;
    }
    | ID'('ParamList')' {
        VarList *newVar = calloc(1, sizeof(VarList));

        newVar->name = $1;
        newVar->size = 1;
        newVar->rows = 0;
        newVar->cols = 0;
        newVar->isPointer = 0;
        newVar->isFunction = 1;
        newVar->paramlist = $3;
        newVar->next = NULL;

        $$ = newVar;
    }
    | VarList ',' MUL ID '(' ParamList ')' {
        VarList *newVar = calloc(1, sizeof(VarList));
        VarList *tail = $1;

        newVar->name = $4;
        newVar->isPointer = 1;
        newVar->isFunction = 1;
        newVar->paramlist = $6;

        while (tail->next != NULL) tail = tail->next;
        tail->next = newVar;
        $$ = $1;
    }
    | MUL ID '(' ParamList ')' {
        VarList *newVar = calloc(1, sizeof(VarList));

        newVar->name = $2;
        newVar->isPointer = 1;
        newVar->isFunction = 1;
        newVar->paramlist = $4;
        $$ = newVar;
    };

// n â†’ makeIdNode("n")
// arr[i] â†’ makeArrayNode("arr", AST of i)
Variable : ID {
        $$ = makeIdNode($1);
    }
    | SELF { $$ = makeSelfNode(); } //stage7: "self is an expression only in method scope, where semantic analysis resolves its implicit object argument."
    | ID '[' E ']' { // this is strictly for accessing array elements only, not declaration
        // also E aayond we can do like arr[i + 5 * 8] lol
        $$ = makeArrayNode($1, $3);
    }
    | ID '[' E ']' '[' E ']' {
        $$ = makeArray2DNode($1, $3, $6);
    }
    | MUL Variable { // allows a dereferenced pointer as an assignment target
        $$ = makeDereferenceNode($2);
    };

/* //stage6: Field grammar supports chained access; semantic actions validate each segment. */
/* Resolve every segment against the type of the preceding base expression. */
Field : Variable '.' ID {
            $$ = makeFieldNode($1, $3);
        }
     | Field '.' ID {
            $$ = makeFieldNode($1, $3);
        }
     ;

//stage7: "Method-call syntax supports class variables, self, and one class-valued self field as receivers."
MethodCall : Variable '.' ID '(' ')' {
                $$ = makeMethodCallNode($1, $3, NULL);
            }
          | Variable '.' ID '(' ArgList ')' {
                $$ = makeMethodCallNode($1, $3, $5);
            }
          | Field '.' ID '(' ')' {
                $$ = makeMethodCallNode($1, $3, NULL);
            }
          | Field '.' ID '(' ArgList ')' {
                $$ = makeMethodCallNode($1, $3, $5);
            }
          ;

FDefBlock : FDefBlock FDef | FDef;

/* //stage6: Function declarations/definitions carry user-defined types in signatures. */
FDef : FType ID'('ParamList')' {
        CheckFunctionDefinition($2, $1, $4);
        BeginFunctionScope($4);
    } '{' LdeclBlock Body '}' {
        SaveFunctionAST($2, finalizeBodyNode($9, $1, $2), 0);
        PrintLocalSymbolTable($2);
        EndFunctionScope();
    };

ParamList : ParamList ',' Param {
        Paramstruct *temp = $1;

        while (temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = $3;
        $$ = $1;
    }
    | Param {
        $$ = $1;
    }
    | { // param can be empty
        $$ = NULL;
    };

/* //stage6: Formal parameters preserve the type selected from the type table. */
//stage7: "The OExpL specification excludes class variables from function and method parameters."
Param : TypeName ID {
        if ($1->classType != NULL) {
            fprintf(stderr, "Error: class variables cannot be function or method arguments\n");
            exit(1);
        }
        Paramstruct *p = malloc(sizeof(Paramstruct));

        p->name = $2;
        p->type = $1->type;
        p->next = NULL;

        $$ = p;
    }
    | TypeName MUL ID {
        if ($1->classType != NULL) {
            fprintf(stderr, "Error: class variables cannot be function or method arguments\n");
            exit(1);
        }
        Paramstruct *p = malloc(sizeof(Paramstruct));

        p->name = $3;
        p->type = PointerType($1->type);
        if (p->type == NULL) {
            fprintf(stderr, "Error: pointer parameters are supported only for int and str\n");
            exit(1);
        }
        p->next = NULL;
        $$ = p;
    };

LdeclBlock : DECL LDecList ENDDECL
           | DECL ENDDECL
           | /* empty */ //stage7: "Allow methods without local variables to omit an empty decl/enddecl block, as the supplied examples do."
           ;

LDecList : LDecList LDecl | LDecl;

/* //stage6: Local declarations can use user-defined types. */
//stage7: "The OExpL specification also excludes class variables from local declarations."
LDecl : TypeName IdList SEMICOLON {
        if ($1->classType != NULL) {
            fprintf(stderr, "Error: class variables cannot be declared locally\n");
            exit(1);
        }
        InstallLocalVariables($2, $1->type);
    };

IdList : IdList ',' ID {
        VarList* newNode = calloc(1, sizeof(VarList));

        newNode->name = strdup($3);
        newNode->size = 1;
        newNode->rows = 0;
        newNode->cols = 0;
        newNode->isPointer = 0;
        newNode->next = NULL;

        VarList *temp = $1;

        while (temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = newNode;
        $$ = $1;
    }
    | IdList ',' ID '[' NUM ']' {
        VarList *newNode = calloc(1, sizeof(VarList));
        VarList *tail = $1;
        newNode->name = strdup($3);
        newNode->size = $5;
        newNode->rows = $5;
        newNode->isArray = 1;
        while (tail->next != NULL) tail = tail->next;
        tail->next = newNode;
        $$ = $1;
    }
    | IdList ',' ID '[' NUM ']' '[' NUM ']' {
        VarList *newNode = calloc(1, sizeof(VarList));
        VarList *tail = $1;
        newNode->name = strdup($3);
        newNode->rows = $5;
        newNode->cols = $8;
        newNode->size = $5 * $8;
        newNode->isArray = 1;
        while (tail->next != NULL) tail = tail->next;
        tail->next = newNode;
        $$ = $1;
    }
    | ID '[' NUM ']' {
        VarList *newNode = calloc(1, sizeof(VarList));
        newNode->name = strdup($1);
        newNode->size = $3;
        newNode->rows = $3;
        newNode->isArray = 1;
        $$ = newNode;
    }
    | ID '[' NUM ']' '[' NUM ']' {
        VarList *newNode = calloc(1, sizeof(VarList));
        newNode->name = strdup($1);
        newNode->rows = $3;
        newNode->cols = $6;
        newNode->size = $3 * $6;
        newNode->isArray = 1;
        $$ = newNode;
    }
    | ID {
        VarList* newNode = calloc(1, sizeof(VarList));

        newNode->name = strdup($1);
        newNode->size = 1;
        newNode->rows = 0;
        newNode->cols = 0;
        newNode->isPointer = 0;
        newNode->next = NULL;

        $$ = newNode;
    };

// Each argument-list node holds one expression in left and the next argument in right.
ArgList : ArgList ',' E { $$ = appendArgNode($1, $3); }
        | E             { $$ = makeArgListNode($1); };

/* A function body must end with exactly one return statement. */
Body : T_BEGIN Slist ReturnStmt T_END { $$ = makeBodyNode($2, $3); }
     | T_BEGIN ReturnStmt T_END       { $$ = makeBodyNode(NULL, $2); };

ReturnStmt : RETURN E SEMICOLON { $$ = makeReturnNode($2); };

/* main is a function with no parameters and an int return type. */
MainBlock : INT MAIN '(' ')' '{' {
        BeginFunctionScope(NULL);
    } LdeclBlock Body '}' {
        root = finalizeBodyNode($8, TLookup("int"), "main");
        SaveFunctionAST("main", root, 1);
        PrintLocalSymbolTable("main");
        EndFunctionScope();
        $$ = NULL;
    }

// so Slist is not storing the source text
// it just stores the pointer. the root = $2 (remember, %type <node> slist?)
// i.e Whenever I have an Slist, its semantic value is a tnode*

// we play pointer passing/propagation not pointer traversal
// pointer traversal happens when we walk the AST in codeGen()
// we pass the same pointer upward .. Slist : Slist Stmtile $$ would be $2 of Program : Tbegin Slist Tend

// matte missinte bottom up parser varacha manasilavum (stack, i/p, action)
// u will see stack being [begin, Slist, Stmt] after reducing second read to stmt
// so Slist Stmt, i.e stack top 2 elements have a reduction rule of Slist -> Slist Stmt
// so stack is [begin slist] again now
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
    }
    /* //stage6: Heap allocation and freeing of user-defined type values. */
    | Variable ASSIGN ALLOC '(' ')' SEMICOLON {
        $$ = makeAllocNode($1);
    }
    | Field ASSIGN ALLOC '(' ')' SEMICOLON {
        $$ = makeAllocNode($1);
    }
    | FREE '(' ID ')' SEMICOLON {
        $$ = makeFreeNode(makeIdNode($3));
    }
    | FREE '(' Field ')' SEMICOLON {
        $$ = makeFreeNode($3);
    }
    | DELETE '(' Variable ')' SEMICOLON {
        $$ = makeDeleteNode($3);
    } //stage7: "delete is parsed as a class-object operation and checked by the front end."
    ;
    
InputStmt : READ '(' Variable ')' SEMICOLON { // u read into a var, like read(b);
        $$ = makeReadNode($3);
    }
    | READ '(' Field ')' SEMICOLON {
        $$ = makeReadNode($3);
    }
    ;

OutputStmt : WRITE '(' E ')' SEMICOLON { // u can write an expression like write(5+8);
        $$ = makeWriteNode($3);
    };

AsgStmt : Variable ASSIGN E SEMICOLON {
        $$ = makeAssignNode($1, $3);
    }
    /* //stage6: Field expressions may be assigned, read, or written as locations. */
    | Field ASSIGN E SEMICOLON {
        $$ = makeAssignNode($1, $3);
    }
    ;

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
        $$ = makeOperatorNode("%", $1, $3);
    }
    | MINUS E %prec UMINUS {
        $$ = makeOperatorNode("-", makeNumNode(0), $2);
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
    | E OR E {
        $$ = makeOperatorNode("||", $1, $3);
    }
    | E AND E {
        $$ = makeOperatorNode("&&", $1, $3);
    }
    | NUM {
        $$ = makeNumNode($1);
    }
    | STRING {
        $$ = makeStrNode($1);
    }
    /* //stage6: NULL is represented as a distinct type for assignment/comparison checks. */
    | T_NULL {
        $$ = makeNullNode();
    }
    | NEW '(' ID ')' {
        $$ = makeNewNode($3);
    } //stage7: "new(ClassName) has the class identity needed for assignment and method-call checks."
    | ADDRESS Variable { // address-of operator returns the address of a variable
        $$ = makeAddressNode($2);
    }
    | Variable {
        $$ = $1;
    }
    | Field {
        $$ = $1;
    }
    | MethodCall { $$ = $1; } //stage7: "Resolved method calls participate in expressions just like function calls."
    | ID'('')' {
        $$ = makeFunctionCallNode($1, NULL);
    }
    | ID'('ArgList')' {
        $$ = makeFunctionCallNode($1, $3);
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

    TypeTableCreate();
    if (yyparse() != 0) {
        fclose(yyin);
        return 1;
    }

    PrintTypeTable();
    PrintClassTable(); //stage7: "Expose the parsed class table as part of front-end semantic analysis."
    PrintSymbolTable();
    targetFile = fopen("target.xsm", "w");
    if (targetFile == NULL) {
        fprintf(stderr, "Could not open target.xsm for writing\n");
        fclose(yyin);
        return 1;
    }
    generateProgram(FunctionASTHead);
    fclose(targetFile);
    fclose(yyin);
    return 0;
}

void yyerror(const char* s) {
    extern int yylineno; //stage7: "Read the line counter maintained by the Stage 7 lexer when reporting parse errors."
    printf("%s at line %d\n", s, yylineno); //stage7: "Point to the source line that caused a syntax error in a sample program."
}
