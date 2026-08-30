#ifndef EXPRTREE_H
#define EXPRTREE_H

enum {
    NODE_NUM,
    NODE_ID,
    NODE_ARRAY, //TASK3 for accessing array elemets or reading elements
    NODE_PLUS,
    NODE_MINUS,
    NODE_MUL,
    NODE_DIV,
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
    TYPE_STR //for TASK1
};

struct VarList { //Since VarList contains multiple IDs, we need a way to remember all of them before installing them -
    char *name;  // So we’ll create a small linked list for declared variable names.
    int size; //TASK3 for arrays
    struct VarList *next;
};

struct Gsymbol { //for TASK1 (This represents one entry in the symbol table.)
    char *name;
    int type;
    int size;
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
tnode* makeOperatorNode(char* op, tnode* l, tnode* r);
tnode* makeIdNode(char* name);

tnode* makeArrayNode(char *name, tnode *index); //TASK3 for accessing or reading array elements(This function will later build: arr[i])

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
void Install(char *name, int type, int size); //for TASK1 (Add a new variable to the symbol table.)
void PrintSymbolTable(); //for TASK1 (This is mainly for Task 1 verification.)

#endif