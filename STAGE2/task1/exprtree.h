#ifndef EXPRTREE_H
#define EXPRTREE_H

enum {
    NODE_NUM,
    NODE_ID,
    NODE_PLUS,
    NODE_MINUS,
    NODE_MUL,
    NODE_DIV,
    NODE_READ,
    NODE_WRITE,
    NODE_ASSIGN,
    NODE_CONNECTOR
};

enum {
	TYPE_INT
};

typedef struct tnode { 
	int type;	// type of variable - INT, BOOL etc, for now, only INT
	int val;	// value of a number for NUM nodes - 5, 10
	char* varname;	// name of a variable for ID nodes - a,b,...,z
	int nodetype;  // information about non-leaf nodes - read/write/connector/+/* etc
	struct tnode *left,*right;	// left and right branches
} tnode;

tnode* createTree(int val, int type, int nodetype, char* varname, tnode* l, tnode* r);

tnode* makeNumNode(int n);
tnode* makeOperatorNode(char op, tnode* l, tnode* r);
tnode* makeIdNode(char* name);
tnode* makeAssignNode(tnode* id, tnode* expr);
tnode* makeReadNode(tnode* id);
tnode* makeWriteNode(tnode* expr);
tnode* makeConnectorNode(tnode* left, tnode* right);

#endif