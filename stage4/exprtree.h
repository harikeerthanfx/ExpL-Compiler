#ifndef EXPRTREE_H
#define EXPRTREE_H

enum { //its main fucntion is when we generate codegen (that switch case) and other function is When creating AST nodes
    NODE_NUM,
    
    NODE_STR, //TASK1 forgotten croassroads(iykyk)

    NODE_ID,
    NODE_ADDRESS, // EX2: represents the address-of operator
    NODE_DEREFERENCE, // EX2: represents dereferencing a pointer
    NODE_ARRAY, //TASK3 for accessing array elemets or reading elements
    NODE_ARRAY2D, //EX1
    NODE_PLUS,
    NODE_MINUS,
    NODE_MUL,
    NODE_DIV,
    NODE_MOD, // EX2: modulo operator
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
    NODE_DOWHILE
};

enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_STR,
    TYPE_INT_PTR,   // EX2: pointer to int
    TYPE_STR_PTR    // EX2: pointer to string
};

struct VarList { //Since VarList contains multiple IDs, we need a way to remember all of them before installing them -
    char *name;  // So we’ll create a small linked list for declared variable names.
    int size; //TASK3 for arrays
    int rows; // EX1: number of rows for 2D array
    int cols; // EX1: number of columns for 2D array
    int isPointer; // EX2: 1 if variable is a pointer, 0 otherwise
    struct VarList *next;
};

struct Gsymbol { //for TASK1 (This represents one entry in the symbol table.)
    char *name;
    int type;
    int size;
	int rows; // EX1: number of rows
	int cols; // EX1: number of columns
    int binding;
    struct Gsymbol *next;
};

typedef struct tnode { 
	int type; // type of variable - INT, BOOL for now
	int val; // value of a number for NUM nodes - 5, 10
	char* varname; // name of a variable for ID nodes - a,b,...,z
	int nodetype; // information about non-leaf nodes - read/write/connector/+/* etc

    struct Gsymbol *Gentry;   // TASK2 : pointer to corresponding GST entry 

	struct tnode *left, *middle, *right; 
} tnode;

tnode* createTree(int val, int type, int nodetype, char* varname, tnode* l, tnode* m, tnode* r);

tnode* makeNumNode(int n);

tnode* makeStrNode(char *s); //TASK1 forgotten crossroads(iykyk)

tnode* makeOperatorNode(char* op, tnode* l, tnode* r);
tnode* makeIdNode(char* name);

tnode* makeArrayNode(char *name, tnode *index); //TASK3 for accessing or reading array elements(This function will later build: arr[i])
tnode* makeArray2DNode(char *name, tnode *rowIndex, tnode *colIndex); // EX1: creates AST node for 2D array access

tnode* makeAddressNode(tnode *var); // EX2: creates an AST node for the address-of operator
tnode* makeDereferenceNode(tnode *ptr); // EX2: creates an AST node for dereferencing a pointer

tnode* makeAssignNode(tnode* id, tnode* expr);
tnode* makeReadNode(tnode* id);
tnode* makeWriteNode(tnode* expr);
tnode* makeConnectorNode(tnode* l, tnode* r);

tnode* makeIfNode(tnode* cond, tnode* thenStmt, tnode* elseStmt);
tnode* makeWhileNode(tnode* cond, tnode* body);

tnode* makeBreakNode();
tnode* makeContinueNode();

tnode* makeRepeatNode(tnode* body, tnode* cond);
tnode* makeDoWhileNode(tnode* body, tnode* cond);

struct Gsymbol *Lookup(char *name); //for TASK1  (Search for a variable in the symbol table.)
void Install(char *name, int type, int size, int rows, int cols); // EX1: installs variable with 2D dimension information //for TASK1 (Add a new variable to the symbol table.)
void PrintSymbolTable(); //for TASK1 (This is mainly for Task 1 verification.)

#endif