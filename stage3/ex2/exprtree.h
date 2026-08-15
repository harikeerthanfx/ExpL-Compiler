#ifndef EXPRTREE_H
#define EXPRTREE_H

enum {
    NODE_NUM,
    NODE_ID,
    NODE_PLUS,
    NODE_MINUS,
    NODE_MUL,
    NODE_DIV,
    NODE_LT,
    NODE_GT,
    NODE_LE,
    NODE_GE,
    NODE_EQ,
    NODE_NE,
    NODE_READ,
    NODE_WRITE,
    NODE_ASSIGN,
    NODE_CONNECTOR,
    NODE_IF,
    NODE_WHILE,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_REPEAT,
    NODE_DOWHILE
};

enum {
    TYPE_BOOL,
    TYPE_INT
};

typedef struct tnode {
    int type;
    int val;
    char *varname;
    int nodetype;

    struct tnode *left, *middle, *right;
} tnode;

tnode* createTree(int val, int type, int nodetype, char *varname,
                  tnode *l, tnode *m, tnode *r);

tnode* makeNumNode(int n);
tnode* makeOperatorNode(char op, tnode *l, tnode *r);
tnode* makeIdNode(char *name);
tnode* makeAssignNode(tnode *id, tnode *expr);
tnode* makeReadNode(tnode *id);
tnode* makeWriteNode(tnode *expr);
tnode* makeConnectorNode(tnode *left, tnode *right);

tnode* makeRelationalNode(char *op, tnode *l, tnode *r);
tnode* makeIfNode(tnode *cond, tnode *thenStmt, tnode *elseStmt);
tnode* makeWhileNode(tnode *cond, tnode *body);

tnode* makeBreakNode(void);
tnode* makeContinueNode(void);

tnode* makeRepeatNode(tnode *body, tnode *cond);
tnode* makeDoWhileNode(tnode *body, tnode *cond);

void printTree(tnode *t);

#endif