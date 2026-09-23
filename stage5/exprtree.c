#include "exprtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Gsymbol *Ghead = NULL; //for TASK1 (Ghead is the starting pointer of our whole symbol table)
int nextBinding = 4096;
int nextFlabel = 0;

/* Only one function is being parsed at a time, so one current local table is
 * sufficient.  Its bindings are relative to BP for the next stage. */
static struct Lsymbol *Lhead = NULL;
static struct TupleType *TupleTypes = NULL;
static struct Gsymbol *currentFunction = NULL;
static int currentReturnType = TYPE_INT;
static int nextLocalBinding = 1;
static int nextParamBinding = -3;

static struct FunctionAST *functionASTs = NULL;

static void semanticError(const char *message, const char *name) {
    fprintf(stderr, "Semantic error: %s%s%s\n", message, name ? " " : "", name ? name : "");
    exit(1);
}

static struct Lsymbol *LLookup(char *name) {
    struct Lsymbol *entry = Lhead;
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) return entry;
        entry = entry->next;
    }
    return NULL;
}

static void LInstall(char *name, int type, int binding, int size, struct TupleType *tupleType) {
    if (LLookup(name) != NULL) semanticError("local name already declared:", name);
    struct Lsymbol *entry = calloc(1, sizeof(*entry));
    entry->name = strdup(name);
    entry->type = type;
    entry->binding = binding;
    entry->size = size;
    entry->tupleType = tupleType;
    entry->next = Lhead;
    Lhead = entry;
}

static struct TupleType *LookupTupleType(char *name) {
    for (struct TupleType *type = TupleTypes; type != NULL; type = type->next)
        if (strcmp(type->name, name) == 0) return type;
    return NULL;
}

static struct Field *LookupField(struct TupleType *type, char *name) {
    for (struct Field *field = type->fields; field != NULL; field = field->next)
        if (strcmp(field->name, name) == 0) return field;
    return NULL;
}

static void clearLocalTable(void) {
    struct Lsymbol *entry = Lhead;
    while (entry != NULL) {
        struct Lsymbol *next = entry->next;
        free(entry->name);
        free(entry);
        entry = next;
    }
    Lhead = NULL;
}

static void requireSameParameters(struct Paramstruct *declared, struct Paramstruct *defined) {
    while (declared != NULL && defined != NULL) {
        if (declared->type != defined->type || strcmp(declared->name, defined->name) != 0)
            semanticError("function definition parameters do not match declaration", NULL);
        declared = declared->next;
        defined = defined->next;
    }
    if (declared != NULL || defined != NULL)
        semanticError("function definition has a different number of parameters", NULL);
}

struct Gsymbol *Lookup(char *name) { //for TASK1 (Lookup() searches the symbol table and returns the address of the matching symbol-table entry.)
    struct Gsymbol *temp = Ghead;

    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}
//for TASK1 (creates a new symbol-table entry and adds it to the linked list)
void Install(char *name, int type, int size, int rows, int cols) { // EX1: installs variable with its dimensions in the GST
	if (Lookup(name) != NULL) {
		printf("Error: Variable %s already declared\n", name);
		exit(1);
	}

	struct Gsymbol *newEntry = (struct Gsymbol *)malloc(sizeof(struct Gsymbol));

	newEntry->name = strdup(name);
	newEntry->type = type;
	newEntry->size = size;
	newEntry->rows = rows;
	newEntry->cols = cols;
	newEntry->binding = nextBinding;
	newEntry->paramlist = NULL;
	newEntry->flabel = -1;
	newEntry->defined = 0;
	newEntry->tupleType = NULL;

	nextBinding = nextBinding + size;
	newEntry->next = NULL;

	if (Ghead == NULL) {
		Ghead = newEntry;
	}
	else {
		struct Gsymbol *temp = Ghead;
		while (temp->next != NULL) temp = temp->next;
		temp->next = newEntry;
	}
}

/* Functions share the global namespace with variables.  They do not consume
 * static data memory, so binding is unused until code generation (Task 3). */
void InstallFunction(char *name, int type, struct Paramstruct *paramlist) {
    if (Lookup(name) != NULL) {
        printf("Error: %s already declared as a variable or function\n", name);
        exit(1);
    }

    struct Gsymbol *newEntry = malloc(sizeof(*newEntry));
    newEntry->name = strdup(name);
    newEntry->type = type;
    newEntry->size = 0;
    newEntry->rows = 0;
    newEntry->cols = 0;
    newEntry->binding = -1;
    newEntry->paramlist = paramlist;
    newEntry->flabel = nextFlabel++;
    newEntry->defined = 0;
    newEntry->tupleType = NULL;
    newEntry->next = NULL;

    if (Ghead == NULL)
        Ghead = newEntry;
    else {
        struct Gsymbol *temp = Ghead;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newEntry;
    }
}

struct Field *makeField(char *name, int type) {
    struct Field *field = calloc(1, sizeof(*field));
    field->name = name;
    field->type = type;
    return field;
}

struct Field *appendField(struct Field *fields, struct Field *field) {
    struct Field *tail = fields;
    while (tail->next != NULL) tail = tail->next;
    tail->next = field;
    return fields;
}

struct TupleVar *makeTupleVar(char *name, int isPointer) {
    struct TupleVar *variable = calloc(1, sizeof(*variable));
    variable->name = name;
    variable->isPointer = isPointer;
    return variable;
}

struct TupleVar *appendTupleVar(struct TupleVar *variables, struct TupleVar *variable) {
    struct TupleVar *tail = variables;
    while (tail->next != NULL) tail = tail->next;
    tail->next = variable;
    return variables;
}

void InstallTupleType(char *name, struct Field *fields) {
    if (LookupTupleType(name) != NULL)
        semanticError("tuple type already declared:", name);
    if (fields == NULL)
        semanticError("tuple type must contain at least one field:", name);

    struct TupleType *type = calloc(1, sizeof(*type));
    type->name = strdup(name);
    type->fields = fields;
    int offset = 0;
    for (struct Field *field = fields; field != NULL; field = field->next) {
        for (struct Field *previous = fields; previous != field; previous = previous->next)
            if (strcmp(previous->name, field->name) == 0)
                semanticError("duplicate tuple field:", field->name);
        field->offset = offset++;
    }
    type->size = offset;
    type->next = TupleTypes;
    TupleTypes = type;
}

void InstallTupleVariables(char *typeName, struct TupleVar *variables, int local) {
    struct TupleType *tupleType = LookupTupleType(typeName);
    if (tupleType == NULL)
        semanticError("unknown tuple type:", typeName);

    for (struct TupleVar *variable = variables; variable != NULL; variable = variable->next) {
        if (local) {
            int type = variable->isPointer ? TYPE_TUPLE_PTR : TYPE_TUPLE;
            int size = variable->isPointer ? 1 : tupleType->size;
            LInstall(variable->name, type, nextLocalBinding, size, tupleType);
            nextLocalBinding += size;
        } else {
            if (Lookup(variable->name) != NULL)
                semanticError("global name already declared:", variable->name);
            struct Gsymbol *entry = calloc(1, sizeof(*entry));
            entry->name = strdup(variable->name);
            entry->type = variable->isPointer ? TYPE_TUPLE_PTR : TYPE_TUPLE;
            entry->size = variable->isPointer ? 1 : tupleType->size;
            entry->rows = entry->cols = 1;
            entry->binding = nextBinding;
            entry->flabel = -1;
            entry->tupleType = tupleType;
            nextBinding += entry->size;
            if (Ghead == NULL) Ghead = entry;
            else {
                struct Gsymbol *tail = Ghead;
                while (tail->next != NULL) tail = tail->next;
                tail->next = entry;
            }
        }
    }
}

void BeginFunctionDefinition(int returnType, char *name, struct Paramstruct *params) {
    struct Gsymbol *entry = Lookup(name);
    if (entry == NULL || entry->flabel < 0)
        semanticError("function definition has no global declaration:", name);
    if (entry->type != returnType)
        semanticError("function definition return type does not match declaration:", name);
    if (entry->defined)
        semanticError("function is defined more than once:", name);

    requireSameParameters(entry->paramlist, params);
    clearLocalTable();
    currentFunction = entry;
    currentReturnType = returnType;
    nextParamBinding = -3;
    nextLocalBinding = 1;
    for (struct Paramstruct *param = params; param != NULL; param = param->next)
        LInstall(param->name, param->type, nextParamBinding--, 1, NULL);
}

void BeginMainDefinition(char *name) {
    if (strcmp(name, "main") != 0)
        semanticError("main block must be named", "main");
    clearLocalTable();
    currentFunction = NULL;
    currentReturnType = TYPE_INT;
    nextLocalBinding = 1;
    nextParamBinding = -3;
}

void InstallLocalDeclarations(int type, struct GDeclItem *items) {
    for (struct GDeclItem *item = items; item != NULL; item = item->next) {
        if (item->isFunction || item->size != 1)
            semanticError("functions and arrays are not permitted in local declarations", item->name);
        int localType = type;
        if (item->isPointer)
            localType = type == TYPE_INT ? TYPE_INT_PTR : TYPE_STR_PTR;
        LInstall(item->name, localType, nextLocalBinding++, 1, NULL);
    }
}

void FinishFunctionDefinition(tnode *body, tnode *returnExpr) {
    if (returnExpr->type != currentReturnType)
        semanticError("return expression type does not match function return type", NULL);

    struct FunctionAST *record = calloc(1, sizeof(*record));
    record->name = strdup(currentFunction ? currentFunction->name : "main");
    record->entry = currentFunction;
    record->body = body;
    record->returnExpr = returnExpr;
    record->localCount = nextLocalBinding - 1;
    record->localSymbols = Lhead;
    record->next = functionASTs;
    functionASTs = record;
    if (currentFunction != NULL) currentFunction->defined = 1;
    /* The AST's ID nodes point into this local table.  Retain it until the
     * corresponding function has been emitted in Task 3. */
    Lhead = NULL;
    currentFunction = NULL;
}

struct FunctionAST *GetFunctionASTs(void) {
    return functionASTs;
}

static const char *typeToString(int type) {
    switch (type) {
        case TYPE_INT: return "INT";
        case TYPE_STR: return "STR";
        case TYPE_INT_PTR: return "INT_PTR";
        case TYPE_STR_PTR: return "STR_PTR";
        case TYPE_TUPLE: return "TUPLE";
        case TYPE_TUPLE_PTR: return "TUPLE_PTR";
        default: return "UNKNOWN";
    }
}

void PrintSymbolTable() { // TASK1: prints the symbol table to check whether variables were installed correctly
    struct Gsymbol *temp = Ghead;
    printf("%-16s %-10s %-10s %-6s %-8s %s\n",
           "Name", "Kind", "Type", "Size", "Binding", "Parameters / Label");
    printf("%-16s %-10s %-10s %-6s %-8s %s\n",
           "----------------", "----------", "----------", "------", "--------", "------------------");

    while (temp != NULL) {
        printf("%-16s %-10s ", temp->name,
               temp->flabel >= 0 ? "FUNCTION" : "VARIABLE");

        if (temp->flabel < 0) {
            printf("%-10s %-6d %-8d -\n", typeToString(temp->type), temp->size, temp->binding);
        } else {
            struct Paramstruct *param = temp->paramlist;
            printf("%-10s %-6s %-8s ", typeToString(temp->type), "-", "-");
            if (param == NULL) printf("() ");
            else {
                printf("(");
                while (param != NULL) {
                    printf("%s %s", typeToString(param->type), param->name);
                    if (param->next != NULL) printf(", ");
                    param = param->next;
                }
                printf(")");
            }
            printf(" / F%d\n", temp->flabel);
        }
        temp = temp->next;
    }
}

tnode* makeAddressNode(tnode *var) { // EX2: creates an AST node for the address-of operator
    if (var->nodetype != NODE_ID) {
        fprintf(stderr, "Address-of operator can only be used with a variable\n");
        exit(1);
    }

    int pointerType;

    if (var->type == TYPE_INT)
        pointerType = TYPE_INT_PTR;
    else if (var->type == TYPE_STR)
        pointerType = TYPE_STR_PTR;
    else if (var->type == TYPE_TUPLE) {
        pointerType = TYPE_TUPLE_PTR;
    }
    else {
        fprintf(stderr, "Cannot take address of a pointer\n");
        exit(1);
    }

    tnode *node = createTree(0, pointerType, NODE_ADDRESS, NULL, var, NULL, NULL);
    node->tupleType = var->tupleType;
    return node;
}

tnode* makeDereferenceNode(tnode *ptr) { // EX2: creates an AST node for dereferencing a pointer
    int valueType;

    if (ptr->type == TYPE_INT_PTR)
        valueType = TYPE_INT;
    else if (ptr->type == TYPE_STR_PTR)
        valueType = TYPE_STR;
    else if (ptr->type == TYPE_TUPLE_PTR)
        valueType = TYPE_TUPLE;
    else {
        fprintf(stderr, "Cannot dereference a non-pointer\n");
        exit(1);
    }

    tnode *node = createTree(0, valueType, NODE_DEREFERENCE, NULL, ptr, NULL, NULL);
    node->tupleType = ptr->tupleType;
    return node;
}

tnode* createTree(int val, int type, int nodetype, char* varname, tnode* l, tnode* m, tnode* r) {
    tnode* temp = (tnode*)malloc(sizeof(tnode));
    temp->val = val;
    temp->type = type;
    temp->nodetype = nodetype;
    temp->varname = varname;
    temp->Gentry = NULL; //TASK2
    temp->Lentry = NULL;
    temp->arglist = NULL;
    temp->tupleType = NULL;
    temp->field = NULL;
    temp->left = l;
    temp->middle = m;
    temp->right = r;
    return temp;
}

tnode* makeNumNode(int n) {
    return createTree(n, TYPE_INT, NODE_NUM, NULL, NULL, NULL, NULL);
}

tnode* makeStrNode(char *s) { //TASK1 forgotten crossroads(iykyk)
    return createTree(0, TYPE_STR, NODE_STR, s, NULL, NULL, NULL);
}

tnode* makeOperatorNode(char* op, tnode* l, tnode* r) {
    int nodetype;
    int type;

    if (strcmp(op, "!") == 0) {
        if (l->type != TYPE_BOOL) {
            fprintf(stderr, "Type mismatch\n");
            exit(1);
        }
        return createTree(0, TYPE_BOOL, NODE_NOT, NULL, l, NULL, NULL);
    }
    if (strcmp(op, "+") == 0) {
        nodetype = NODE_PLUS;
        type = TYPE_INT;
    }
    else if (strcmp(op, "-") == 0) {
        nodetype = NODE_MINUS;
        type = TYPE_INT;
    }
    else if (strcmp(op, "*") == 0) {
        nodetype = NODE_MUL;
        type = TYPE_INT;
    }
    else if (strcmp(op, "/") == 0) {
        nodetype = NODE_DIV;
        type = TYPE_INT;
    }
    else if (strcmp(op, "%") == 0){
        nodetype = NODE_MOD; // EX2: identifies the modulo operation
        type = TYPE_INT; // EX2: modulo produces an integer
    }
    else if (strcmp(op, "<") == 0) {
        nodetype = NODE_LT;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, ">") == 0) {
        nodetype = NODE_GT;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, "<=") == 0) {
        nodetype = NODE_LE;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, ">=") == 0) {
        nodetype = NODE_GE;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, "!=") == 0) {
        nodetype = NODE_NE;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, "==") == 0) {
        nodetype = NODE_EQ;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, "&&") == 0) {
        nodetype = NODE_AND;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, "||") == 0) {
        nodetype = NODE_OR;
        type = TYPE_BOOL;
    }
    else {
        fprintf(stderr, "Invalid operator %s\n", op);
        exit(1);
    }

    if ((nodetype == NODE_AND || nodetype == NODE_OR) &&
        (l->type != TYPE_BOOL || r->type != TYPE_BOOL)) {
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }
    if (nodetype != NODE_AND && nodetype != NODE_OR &&
        (l->type != TYPE_INT || r->type != TYPE_INT)) { // left and right operands should both be strictly int 
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }

    return createTree(0, type, nodetype, NULL, l, NULL, r);
}

tnode* makeIdNode(char *name) { //TASK2 : we storing the entry pointer in the Gentry pointer space we just introduced in the struct
    /* Task 2 scope rule: local declarations (including parameters) hide a
     * global variable with the same name. */
    struct Lsymbol *localEntry = LLookup(name);
    if (localEntry != NULL) {
        tnode *node = createTree(0, localEntry->type, NODE_ID, name, NULL, NULL, NULL);
        node->Lentry = localEntry;
        node->tupleType = localEntry->tupleType;
        return node;
    }

    struct Gsymbol *entry = Lookup(name);

    if (entry == NULL || entry->flabel >= 0) {
        printf("Error: Variable %s not declared\n", name);
        exit(1);
    }

    tnode *node = createTree(0, entry->type, NODE_ID, name, NULL, NULL, NULL);
    node->Gentry = entry;
    node->tupleType = entry->tupleType;
    return node;
}

tnode *makeFieldNode(char *tupleName, char *fieldName) {
    tnode *tuple = makeIdNode(tupleName);
    if (tuple->type != TYPE_TUPLE)
        semanticError("field access requires a tuple variable:", tupleName);
    struct Field *field = LookupField(tuple->tupleType, fieldName);
    if (field == NULL)
        semanticError("tuple has no such field:", fieldName);
    tnode *node = createTree(0, field->type, NODE_FIELD, fieldName, tuple, NULL, NULL);
    node->tupleType = tuple->tupleType;
    node->field = field;
    return node;
}

struct ArgList *makeArgList(tnode *expr) {
    struct ArgList *args = calloc(1, sizeof(*args));
    args->expr = expr;
    return args;
}

struct ArgList *appendArg(struct ArgList *args, tnode *expr) {
    struct ArgList *tail = args;
    while (tail->next != NULL) tail = tail->next;
    tail->next = makeArgList(expr);
    return args;
}

tnode *makeFuncCallNode(char *name, struct ArgList *args) {
    struct Gsymbol *function = Lookup(name);
    if (function == NULL || function->flabel < 0)
        semanticError("call to undeclared function:", name);

    struct Paramstruct *formal = function->paramlist;
    struct ArgList *actual = args;
    while (formal != NULL && actual != NULL) {
        if (actual->expr->type == TYPE_TUPLE || actual->expr->type == TYPE_TUPLE_PTR)
            semanticError("tuples cannot be passed as function arguments", name);
        if (formal->type != actual->expr->type)
            semanticError("argument type does not match function declaration:", name);
        formal = formal->next;
        actual = actual->next;
    }
    if (formal != NULL || actual != NULL)
        semanticError("argument count does not match function declaration:", name);

    tnode *node = createTree(0, function->type, NODE_FUNC_CALL, name, NULL, NULL, NULL);
    node->Gentry = function;
    node->arglist = args;
    return node;
}

// TASK3: creates a NODE_ARRAY for an array access and stores its GST entry and index expression
tnode* makeArrayNode(char *name, tnode *index) {
    if (LLookup(name) != NULL) {
        printf("Error: %s is a local variable, not an array\n", name);
        exit(1);
    }
    struct Gsymbol *entry = Lookup(name);

    if (entry == NULL) {
        printf("Error: Variable %s not declared\n", name);
        exit(1);
    }

    if (entry->size == 1) {
        printf("Error: %s is not an array\n", name);
        exit(1);
    }

    if (index->type != TYPE_INT) {
        printf("Error: Array index must be an integer\n");
        exit(1);
    }

    tnode *node = createTree(0, entry->type, NODE_ARRAY, name, index, NULL, NULL);
    node->Gentry = entry;

    return node;
}

// EX1: creates a NODE_ARRAY2D for a 2D array access like a[i][j]
tnode* makeArray2DNode(char *name, tnode *rowIndex, tnode *colIndex) {
	if (LLookup(name) != NULL) {
		printf("Error: %s is a local variable, not an array\n", name);
		exit(1);
	}
	struct Gsymbol *entry = Lookup(name);

	if (entry == NULL) {
		printf("Error: Variable %s not declared\n", name);
		exit(1);
	}

	if (entry->rows == 1 || entry->cols == 1) {
		printf("Error: %s is not a 2D array\n", name);
		exit(1);
	}

	if (rowIndex->type != TYPE_INT || colIndex->type != TYPE_INT) {
		printf("Error: Array indices must be integers\n");
		exit(1);
	}

	tnode *node = createTree(0, entry->type, NODE_ARRAY2D, name, rowIndex, colIndex, NULL);
	node->Gentry = entry;

	return node;
}

tnode* makeAssignNode(tnode* id, tnode* expr) {
    if (id->type != expr->type) {
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }
    if (id->type == TYPE_TUPLE && id->tupleType != expr->tupleType) {
        fprintf(stderr, "Type mismatch: tuple types differ\n");
        exit(1);
    }
    return createTree(0, id->type, NODE_ASSIGN, NULL, id, NULL, expr);
}

tnode* makeReadNode(tnode* id) {
    if (id->type == TYPE_TUPLE)
        semanticError("read requires a tuple field, not a whole tuple", NULL);
    return createTree(0, TYPE_INT, NODE_READ, NULL, id, NULL, NULL);
}

tnode* makeWriteNode(tnode* expr) {
    if (expr->type == TYPE_TUPLE)
        semanticError("write requires a tuple field, not a whole tuple", NULL);
    return createTree(0, TYPE_INT, NODE_WRITE, NULL, expr, NULL, NULL);
}

tnode* makeConnectorNode(tnode* l, tnode* r) {
    return createTree(0, TYPE_INT, NODE_CONNECTOR, NULL, l, NULL, r);
}

tnode* makeIfNode(tnode* cond, tnode* thenStmt, tnode* elseStmt) {
    if (cond->type != TYPE_BOOL) {
        fprintf(stderr, "Type mismatch\n"); // if (a + b) ❌ --- if (a < b) ✅
        exit(1);
    }

    return createTree(0, TYPE_BOOL, NODE_IF, NULL, cond, thenStmt, elseStmt);
}

tnode* makeWhileNode(tnode* cond, tnode* body) {
    if (cond->type != TYPE_BOOL) {
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }

    return createTree(0, TYPE_BOOL, NODE_WHILE, NULL, cond, NULL, body);
}

tnode* makeBreakNode() {
    return createTree(0, TYPE_INT, NODE_BREAK, NULL, NULL, NULL, NULL);
}

tnode* makeContinueNode() {
    return createTree(0, TYPE_INT, NODE_CONTINUE, NULL, NULL, NULL, NULL);
}

tnode* makeRepeatNode(tnode* body, tnode* cond) {
    return createTree(0, TYPE_BOOL, NODE_REPEAT, NULL, cond, NULL, body);
}

tnode* makeDoWhileNode(tnode* body, tnode* cond) {
    return createTree(0, TYPE_BOOL, NODE_DOWHILE, NULL, cond, NULL, body);
}
