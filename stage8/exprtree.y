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
Classtable *currentClass = NULL;
Memberfunclist *currentMethod = NULL;
static int currentClassHasOwnMethodDecl = 0;

static void InstallGlobalVarList(VarList *variables, Typetable *type,
                                 Classtable *classType) {
    VarList *variable;

    if (classType != NULL) {
        for (variable = variables; variable != NULL; variable = variable->next) {
            if (variable->isArray || variable->isPointer || variable->isFunction) {
                fprintf(stderr, "Error: class variables must be scalar global variables\n");
                exit(1);
            }
        }
    }

    for (variable = variables; variable != NULL; variable = variable->next) {
        if (classType != NULL) {
            InstallClassVariable(variable->name, classType);
        } else {
            Typetable *variableType = type;
            if (variable->isPointer) {
                variableType = PointerType(type);
                if (variableType == NULL) {
                    fprintf(stderr, "Error: pointer declarations are supported only for int and str\n");
                    exit(1);
                }
            }

            if (variable->isFunction) {
                Install(variable->name, variableType, 0, 0, 0, 0,
                        variable->paramlist, nextFunctionLabel++);
            } else {
                Install(variable->name, variableType, variable->size,
                        variable->rows, variable->cols, variable->isArray,
                        NULL, -1);
            }
        }
    }
}

static void CheckClassFieldOrder(void) {
    if (currentClass != NULL && currentClassHasOwnMethodDecl) {
        fprintf(stderr, "Error: class fields must be declared before methods in class '%s'\n",
                currentClass->name);
        exit(1);
    }
}

static void InstallDeclaredClassField(char *typeName, char *fieldName) {
    if (currentClass != NULL && currentClass->parentptr != NULL &&
        Class_Flookup(currentClass->parentptr, fieldName) != NULL) {
        fprintf(stderr, "Error: field '%s' redeclares an inherited field in class '%s'\n",
                fieldName, currentClass->name);
        exit(1);
    }

    if (Class_Finstall(currentClass, typeName, fieldName) == NULL) {
        fprintf(stderr, "Error: field '%s %s' is invalid, duplicated, or exceeds the 8-field/8-word limit in class '%s'\n",
                typeName, fieldName, currentClass == NULL ? "?" : currentClass->name);
        exit(1);
    }
}
%}

%union{
    tnode* node;
    char* str;
    int num;
    VarList* varlist;
    Paramstruct* paramlist;
    Typetable* type;
    ParamTypeInfo paramtype;
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
%token CLASS ENDCLASS EXTENDS SELF NEW DELETE
%token T_NULL

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

%type <node> Program MainBlock Body ReturnStmt ArgList Slist Stmt InputStmt OutputStmt AsgStmt E Variable Field MemberAccess FieldFunction
%type <str> ClassParentOpt
%type <node> IfStmt WhileStmt
%type <node> BreakStmt ContinueStmt
%type <node> RepeatStmt DoWhileStmt

%type <type> TypeName FType FieldType
%type <paramtype> ParamType LocalType
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
Program : TypeDefBlock ClassDefBlock GDeclBlock FDefBlock MainBlock { $$ = $5; }
        | TypeDefBlock ClassDefBlock GDeclBlock MainBlock           { $$ = $4; }
        | TypeDefBlock ClassDefBlock MainBlock                      { $$ = $3; };

/* Type-table actions from the reference grammar need a type table in this project. */
TypeDefBlock : TYPE TypeDefList ENDTYPE
             | /* empty */
             ;

/* Class syntax, including inheritance from an earlier class definition. */
ClassDefBlock : CLASS ClassDefList ENDCLASS { PrintClassTable(); }
              | /* empty */
              ;

ClassDefList : ClassDefList ClassDef
             | ClassDef
             ;

ClassDef : ID ClassParentOpt {
              currentClass = CInstall($1, $2);
              if (currentClass == NULL) {
                  fprintf(stderr, "Error: class '%s' is already defined, has an invalid parent, or cannot be installed\n", $1);
                  exit(1);
              }
              currentClassHasOwnMethodDecl = 0;
          } '{' DECL ClassMemberDecls ENDDECL ClassMethodDefList '}' {
              if (!Class_CheckMethodDefinitions(currentClass)) {
                  exit(1);
              }
              currentClass = NULL;
              currentMethod = NULL;
          }
         ;

/* One shared list avoids adjacent nullable field/method lists. The field
   actions below enforce the required fields-before-methods order. */
ClassMemberDecls : ClassMemberDecls ClassField
                 | ClassMemberDecls ClassMethodDecl { currentClassHasOwnMethodDecl = 1; }
                 | /* empty */
                 ;

ClassField : INT ID SEMICOLON {
               CheckClassFieldOrder();
               InstallDeclaredClassField("int", $2);
           }
           | STR ID SEMICOLON {
               CheckClassFieldOrder();
               InstallDeclaredClassField("str", $2);
           }
           | ID ID SEMICOLON {
               CheckClassFieldOrder();
               if (TLookup($1) == NULL && CLookup($1) == NULL) {
                   fprintf(stderr, "Error: field type '%s' is not defined before this class\n", $1);
                   exit(1);
               }
               if (currentClass != NULL && CLookup($1) == currentClass) {
                   fprintf(stderr, "Error: class fields may use only previously defined classes\n");
                   exit(1);
               }
               InstallDeclaredClassField($1, $2);
           }
           ;

ClassMethodDecl : INT ID '(' ParamList ')' SEMICOLON {
                    if (Class_Minstall(currentClass, $2, TLookup("int"), $4) == NULL) {
                        fprintf(stderr, "Error: method '%s' is invalid, duplicated, has an incompatible override signature, or exceeds the 8-method limit in class '%s'\n",
                                $2, currentClass == NULL ? "?" : currentClass->name);
                        exit(1);
                    }
                }
                | STR ID '(' ParamList ')' SEMICOLON {
                    if (Class_Minstall(currentClass, $2, TLookup("str"), $4) == NULL) {
                        fprintf(stderr, "Error: method '%s' is invalid, duplicated, has an incompatible override signature, or exceeds the 8-method limit in class '%s'\n",
                                $2, currentClass == NULL ? "?" : currentClass->name);
                        exit(1);
                    }
                }
                | ID ID '(' ParamList ')' SEMICOLON {
                    Typetable *returnType = TLookup($1);
                    if (returnType == NULL || Class_Minstall(currentClass, $2, returnType, $4) == NULL) {
                        fprintf(stderr, "Error: method '%s' has an invalid return type, duplicate name, incompatible override signature, or exceeds the 8-method limit in class '%s'\n",
                                $2, currentClass == NULL ? "?" : currentClass->name);
                        exit(1);
                    }
                }
                | INT MUL ID '(' ParamList ')' SEMICOLON {
                    if (Class_Minstall(currentClass, $3, PointerType(TLookup("int")), $5) == NULL) {
                        fprintf(stderr, "Error: method '%s' is invalid, duplicated, has an incompatible override signature, or exceeds the 8-method limit in class '%s'\n",
                                $3, currentClass == NULL ? "?" : currentClass->name);
                        exit(1);
                    }
                }
                | STR MUL ID '(' ParamList ')' SEMICOLON {
                    if (Class_Minstall(currentClass, $3, PointerType(TLookup("str")), $5) == NULL) {
                        fprintf(stderr, "Error: method '%s' is invalid, duplicated, has an incompatible override signature, or exceeds the 8-method limit in class '%s'\n",
                                $3, currentClass == NULL ? "?" : currentClass->name);
                        exit(1);
                    }
                }
                | ID MUL ID '(' ParamList ')' SEMICOLON {
                    Typetable *returnType = TLookup($1);
                    if (returnType == NULL || !IsUserDefinedType(returnType) ||
                        Class_Minstall(currentClass, $3, PointerType(returnType), $5) == NULL) {
                        fprintf(stderr, "Error: method '%s' has an invalid pointer return type, duplicate name, incompatible override signature, or exceeds the 8-method limit in class '%s'\n",
                                $3, currentClass == NULL ? "?" : currentClass->name);
                        exit(1);
                    }
                }
                ;

ClassMethodDefList : ClassMethodDef ClassMethodDefList
                   | /* empty */
                   ;
/* Method definitions are checked against their class declaration before parsing the body. */
ClassMethodDef : FType ID '(' ParamList ')' {
                    currentMethod = Class_MatchMethodDefinition(currentClass, $2, $1, $4);
                    if (currentMethod == NULL) exit(1);
                    BeginMethodScope($4, currentClass);
                } '{' LdeclBlock Body '}' {
                    char qualifiedName[256];
                    snprintf(qualifiedName, sizeof(qualifiedName), "%s.%s",
                             currentClass->name, $2);
                    SaveMethodAST(currentClass, currentMethod,
                                  finalizeBodyNode($9, $1, qualifiedName));
                    PrintLocalSymbolTable(qualifiedName);
                    EndFunctionScope();
                    currentMethod = NULL;
                }

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

GDecl : INT VarList SEMICOLON {
           InstallGlobalVarList($2, TLookup("int"), NULL);
       }
     | STR VarList SEMICOLON {
           InstallGlobalVarList($2, TLookup("str"), NULL);
       }
     | ID VarList SEMICOLON {
           Typetable *type = TLookup($1);
           Classtable *classType = CLookup($1);

           if (type != NULL && IsUserDefinedType(type)) {
               InstallGlobalVarList($2, type, NULL);
           } else if (classType != NULL) {
               InstallGlobalVarList($2, NULL, classType);
           } else {
               fprintf(stderr, "Error: type or class '%s' is not defined\n", $1);
               exit(1);
           }
       }
     ;

TypeName : INT { $$ = TLookup("int"); }
         | STR { $$ = TLookup("str"); }
         | ID  {
             Typetable *resolvedType = TLookup($1);
             if (resolvedType == NULL || !IsUserDefinedType(resolvedType)) {
                 fprintf(stderr, "Error: type '%s' is not defined yet\n", $1);
                 exit(1);
             }
             $$ = resolvedType;
         }
         ;

ClassParentOpt : EXTENDS ID { $$ = $2; }
               | /* empty */ { $$ = NULL; }
               ;

FType : TypeName { $$ = $1; }
      | TypeName MUL {
          $$ = PointerType($1);
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

/* Share the member prefix so fields and method calls resolve through one path. */
/* Keep a member access unresolved while it may still be a method-call prefix.
   Validate that it is a field only at the point where it is used as a value
   or an lvalue. */
Field : MemberAccess { $$ = $1; };

MemberAccess : SELF '.' ID {
                 $$ = makeMemberAccessNode(makeIdNode("self"), $3);
             }
            | Variable '.' ID {
                 $$ = makeMemberAccessNode($1, $3);
             }
            | Field '.' ID {
                 $$ = makeMemberAccessNode($1, $3);
             }
            ;

FDefBlock : FDefBlock FDef | FDef;

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

Param : ParamType ID {
        Paramstruct *p = calloc(1, sizeof(Paramstruct));

        if ($1.type == NULL && $1.classType == NULL) {
            fprintf(stderr, "Error: parameter type is not defined\n");
            exit(1);
        }

        p->name = $2;
        p->type = $1.type;
        p->classType = $1.classType;
        p->next = NULL;

        $$ = p;
    }
    | ParamType MUL ID {
        Paramstruct *p = calloc(1, sizeof(Paramstruct));

        p->name = $3;
        p->type = PointerType($1.type);
        if (p->type == NULL) {
            fprintf(stderr, "Error: pointer parameters are supported only for int and str\n");
            exit(1);
        }
        p->next = NULL;
        $$ = p;
    };

ParamType : INT {
              $$.type = TLookup("int");
              $$.classType = NULL;
          }
          | STR {
              $$.type = TLookup("str");
              $$.classType = NULL;
          }
          | ID {
              Typetable *type = TLookup($1);
              Classtable *classType = CLookup($1);

              if (type != NULL && IsUserDefinedType(type)) {
                  $$.type = type;
                  $$.classType = NULL;
              } else {
                  $$.type = NULL;
                  $$.classType = classType;
              }
          }
          ;

LdeclBlock : DECL LDecList ENDDECL | DECL ENDDECL;

LDecList : LDecList LDecl | LDecl;

LDecl : LocalType IdList SEMICOLON {
        if ($1.classType != NULL) {
            InstallLocalClassVariables($2, $1.classType);
        } else if ($1.type == NULL) {
            fprintf(stderr, "Error: local variable type is not defined\n");
            exit(1);
        } else {
            InstallLocalVariables($2, $1.type);
        }
    };

LocalType : ParamType { $$ = $1; };

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
    | Variable ASSIGN ALLOC '(' ')' SEMICOLON {
        $$ = makeAllocNode($1);
    }
    | Field ASSIGN ALLOC '(' ')' SEMICOLON {
        $$ = makeAllocNode(makeFieldValueNode($1));
    }
    | Variable ASSIGN NEW '(' ID ')' SEMICOLON {
        $$ = makeClassAllocNode($1, $5);
    }
    | Field ASSIGN NEW '(' ID ')' SEMICOLON {
        $$ = makeClassAllocNode(makeFieldValueNode($1), $5);
    }
    | FREE '(' ID ')' SEMICOLON {
        $$ = makeFreeNode(makeIdNode($3));
    }
    | FREE '(' Field ')' SEMICOLON {
        $$ = makeFreeNode(makeFieldValueNode($3));
    }
    | DELETE '(' Variable ')' SEMICOLON {
        $$ = makeClassDeleteNode($3);
    }
    | DELETE '(' Field ')' SEMICOLON {
        $$ = makeClassDeleteNode(makeFieldValueNode($3));
    };
    
InputStmt : READ '(' Variable ')' SEMICOLON { // u read into a var, like read(b);
        $$ = makeReadNode($3);
    }
    | READ '(' Field ')' SEMICOLON {
        $$ = makeReadNode(makeFieldValueNode($3));
    }
    ;

OutputStmt : WRITE '(' E ')' SEMICOLON { // u can write an expression like write(5+8);
        $$ = makeWriteNode($3);
    };

AsgStmt : Variable ASSIGN E SEMICOLON {
        $$ = makeAssignNode($1, $3);
    }
    | Field ASSIGN E SEMICOLON {
        $$ = makeAssignNode(makeFieldValueNode($1), $3);
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
    | T_NULL {
        $$ = makeNullNode();
    }
    | ADDRESS Variable { // address-of operator returns the address of a variable
        $$ = makeAddressNode($2);
    }
    | Variable {
        $$ = $1;
    }
    | Field {
        $$ = makeFieldValueNode($1);
    }
    | FieldFunction {
        $$ = $1;
    }
    | ID'('')' {
        $$ = makeFunctionCallNode($1, NULL);
    }
    | ID'('ArgList')' {
        $$ = makeFunctionCallNode($1, $3);
    };

FieldFunction : MemberAccess '(' ArgList ')' {
                  $$ = makeMethodCallNode($1, $3);
              }
              | MemberAccess '(' ')' {
                  $$ = makeMethodCallNode($1, NULL);
              }
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

    TypeTableCreate();
    if (yyparse() != 0) {
        fclose(yyin);
        return 1;
    }

    PrintTypeTable();
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
    printf("%s\n", s);
}
