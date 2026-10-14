#ifndef EXPRTREE_H
#define EXPRTREE_H

enum {
    NODE_NUM,
    NODE_STR,
    NODE_ID,
    NODE_ADDRESS,
    NODE_DEREFERENCE,
    NODE_ARRAY,
    NODE_ARRAY2D,
    NODE_PLUS,
    NODE_MINUS,
    NODE_MUL,
    NODE_DIV,
    NODE_MOD,
    NODE_READ,
    NODE_WRITE,
    NODE_ASSIGN,
    NODE_CONNECTOR,
    NODE_LT,
    NODE_GT,
    NODE_LE,
    NODE_GE,
    NODE_NE,
    NODE_EQ,
    NODE_IF,
    NODE_WHILE,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_REPEAT,
    NODE_DOWHILE,
    NODE_RETURN,
    NODE_BODY,
    NODE_FUNCTION,
    NODE_MAIN,
    NODE_ARG_LIST,
    NODE_OR,
    NODE_AND,
    NODE_FIELD,
    NODE_NULL,
    NODE_ALLOC,
    NODE_FREE
};

enum {
	TYPE_INT,
    TYPE_BOOL,
    TYPE_STR,
    TYPE_INT_PTR,
    TYPE_STR_PTR,
    TYPE_VOID,
    TYPE_NULL
};

//stage6: Type table and field metadata for user-defined types.
typedef struct Typetable {
    char *name;                 // type name
    int size;                   // number of words in a value of this type
    struct Fieldlist *fields;   // fields for user-defined aggregate types
    struct Typetable *next;
} Typetable;

typedef struct Fieldlist {
    char *name;                 // field name
    int fieldIndex;             // position in the containing type
    Typetable *type;             // declared type of this field
    struct Fieldlist *next;
} Fieldlist;

//stage6: Function parameters carry type-table references for type checking.
typedef struct Paramstruct {
    char* name;
    Typetable *type;
    struct Paramstruct* next;
} Paramstruct;

typedef struct VarList { // int a, b, c indel, oronninum oru node aayit linked list aakan
    // install() vilikumbo ivarellarum gsymbol tableilek povum
    char* name;
    int size; // arrays aanel we declare value for this size
    int rows; // number of rows for 2D array
    int cols; // number of columns for 2D array
    int isArray;
    int isPointer; // 1 if pointer, else 0
    int isFunction; // distinguishes a function declaration from a variable declaration
    struct Paramstruct* paramlist;
    struct VarList* next;
} VarList;

//stage6: Global/local symbol entries retain declared type identity.
typedef struct Gsymbol {
    char *name;
    Typetable *type;
    int size; // default 1, we take the value of varlist and copy it here, gtable is the collection of all varlists
    int rows;
	int cols;
    int isArray;
    int binding; // memory address of the variable (4099)
    struct Paramstruct* paramlist;
    int flabel;
    struct Gsymbol* next;
} Gsymbol;

typedef struct Lsymbol {
    char *name;
    Typetable *type;
    int binding;
    int size;
    int rows;
    int cols;
    int isArray;
    struct Lsymbol *next;
} Lsymbol;

typedef struct tnode {
	int type; // legacy primitive type code used by existing AST/codegen logic
	//stage6: Preserve exact type identity and field metadata through the AST.
    Typetable *typeEntry; // exact type identity, including user-defined types
	int val; // value of a number for NUM nodes - 5, 10
	char* varname; // name of a variable for ID nodes - a,b,...,z
    int nodetype; // information about non-leaf nodes - read/write/connector/+/* etc
    Gsymbol* Gentry; // pointer to corresponding GST entry
    Lsymbol* Lentry; // pointer to corresponding local-symbol-table entry
    Fieldlist *Fentry; // field-table entry for a NODE_FIELD
    struct tnode *arglist; // expressions passed to a function call
    struct tnode *left, *middle, *right;
} tnode;

typedef struct FunctionAST {
    char *name;
    tnode *tree;
    Lsymbol *localTable;
    int localCount;
    struct FunctionAST *next;
} FunctionAST;

extern FunctionAST *FunctionASTHead;
extern int nextBinding;

//stage6: Type-table and field-table APIs used by the Stage 6 front end.
void TypeTableCreate(void);
Typetable *TLookup(char *name);
Typetable *TInstall(char *name, int size, Fieldlist *fields);
Fieldlist *FLookup(Typetable *type, char *name);
int GetSize(Typetable *type);
int IsUserDefinedType(Typetable *type);
void PrintTypeTable(void);
Typetable *PointerType(Typetable *type);
int TypeCode(Typetable *type);
Typetable *TBeginDefinition(char *name);
Typetable *TCurrentDefinition(void);
Fieldlist *FInstall(Typetable *owner, char *name, Typetable *fieldType, int fieldIndex);
void TFinishDefinition(Typetable *type);

tnode* createTree(int val, int type, int nodetype, char* varname, tnode* l, tnode* m, tnode* r);

tnode* makeNumNode(int n);
tnode* makeStrNode(char *s);
tnode* makeNullNode(void);

tnode* makeOperatorNode(char* op, tnode* l, tnode* r);
tnode* makeIdNode(char* name);
tnode* makeAssignNode(tnode* id, tnode* expr);
tnode* makeAllocNode(tnode *target);
tnode* makeFreeNode(tnode *target);
tnode* makeReadNode(tnode* id);
tnode* makeWriteNode(tnode* expr);
tnode* makeConnectorNode(tnode* l, tnode* r);

tnode* makeIfNode(tnode* cond, tnode* thenStmt, tnode* elseStmt);
tnode* makeWhileNode(tnode* cond, tnode* body);

tnode* makeBreakNode();
tnode* makeContinueNode();

tnode* makeRepeatNode(tnode* body, tnode* cond);
tnode* makeDoWhileNode(tnode* body, tnode* cond);
tnode* makeReturnNode(tnode *expr);
tnode* makeBodyNode(tnode *statements, tnode *returnNode);
tnode* finalizeBodyNode(tnode *body, Typetable *returnType, char *functionName);
tnode* makeFunctionCallNode(char *name, tnode *arglist);
tnode* makeArgListNode(tnode *expr);
tnode* appendArgNode(tnode *arglist, tnode *expr);
void SaveFunctionAST(char *name, tnode *body, int isMain);

tnode* makeArrayNode(char* name, tnode* index); // for accessing array elements
tnode* makeArray2DNode(char *name, tnode *rowIndex, tnode *colIndex);
tnode* makeFieldNode(tnode *base, char *fieldName);

tnode* makeAddressNode(tnode *var); // creates an AST node for the address-of operator
tnode* makeDereferenceNode(tnode *ptr); // creates an AST node for dereferencing a pointer

struct Gsymbol* Lookup(char* name);
void Install(char* name, Typetable *type, int size, int rows, int cols, int isArray, Paramstruct *paramlist, int flabel);
void CheckFunctionDefinition(char *name, Typetable *returnType, Paramstruct *paramlist);
Lsymbol *LLookup(char *name);
void BeginFunctionScope(Paramstruct *paramlist);
void InstallLocalVariables(VarList *varlist, Typetable *type);
void EndFunctionScope(void);
void PrintLocalSymbolTable(char *functionName);
void PrintSymbolTable();

#endif
