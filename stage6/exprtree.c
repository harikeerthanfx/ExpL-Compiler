#include "exprtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Gsymbol* Ghead = NULL;
Lsymbol* Lhead = NULL;
int nextBinding = 4096;
int nextLocalBinding = 1;

//stage6: Type table, field lookup, and user-defined type identity.
static Typetable *Thead = NULL;
static Typetable *Ttail = NULL;
static Typetable *TcurrentDefinition = NULL;

static Typetable *CreateTypeEntry(const char *name, int size) {
    Typetable *entry = calloc(1, sizeof(Typetable));
    if (entry == NULL) return NULL;

    entry->name = strdup(name);
    if (entry->name == NULL) {
        free(entry);
        return NULL;
    }
    entry->size = size;

    if (Thead == NULL) Thead = entry;
    else Ttail->next = entry;
    Ttail = entry;
    return entry;
}

void TypeTableCreate(void) {
    /* Safe to call more than once; preserve user entries if already initialized. */
    if (Thead != NULL) return;

    if (CreateTypeEntry("int", 1) == NULL ||
        CreateTypeEntry("str", 1) == NULL ||
        CreateTypeEntry("bool", 1) == NULL ||
        CreateTypeEntry("void", 0) == NULL ||
        CreateTypeEntry("null", 0) == NULL ||
        CreateTypeEntry("int_ptr", 1) == NULL ||
        CreateTypeEntry("str_ptr", 1) == NULL) {
        fprintf(stderr, "Error: could not initialize type table\n");
        exit(1);
    }
}

Typetable *PointerType(Typetable *type) {
    if (type == NULL) return NULL;
    if (strcmp(type->name, "int") == 0) return TLookup("int_ptr");
    if (strcmp(type->name, "str") == 0) return TLookup("str_ptr");
    return NULL;
}

int TypeCode(Typetable *type) {
    if (type == NULL) return -1;
    if (strcmp(type->name, "int") == 0) return TYPE_INT;
    if (strcmp(type->name, "str") == 0) return TYPE_STR;
    if (strcmp(type->name, "bool") == 0) return TYPE_BOOL;
    if (strcmp(type->name, "int_ptr") == 0) return TYPE_INT_PTR;
    if (strcmp(type->name, "str_ptr") == 0) return TYPE_STR_PTR;
    return -1; /* AST user-type identity is handled in a later step. */
}

static Typetable *TypeEntryForCode(int type) {
    switch (type) {
        case TYPE_INT: return TLookup("int");
        case TYPE_STR: return TLookup("str");
        case TYPE_BOOL: return TLookup("bool");
        case TYPE_INT_PTR: return TLookup("int_ptr");
        case TYPE_STR_PTR: return TLookup("str_ptr");
        default: return NULL;
    }
}

//stage6: Install and find named aggregate types and their field metadata.
Typetable *TLookup(char *name) {
    Typetable *entry;
    if (name == NULL) return NULL;

    for (entry = Thead; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) return entry;
    }
    return NULL;
}

Typetable *TInstall(char *name, int size, Fieldlist *fields) {
    Typetable *entry;
    Fieldlist *field;
    int index = 0;
    if (name == NULL || TLookup(name) != NULL) return NULL;

    for (field = fields; field != NULL; field = field->next) {
        Fieldlist *previous;
        Typetable *knownType;
        for (knownType = Thead; knownType != NULL; knownType = knownType->next) {
            if (knownType == field->type) break;
        }
        if (field->name == NULL || field->type == NULL || knownType == NULL) return NULL;
        for (previous = fields; previous != field; previous = previous->next) {
            if (strcmp(previous->name, field->name) == 0) return NULL;
        }
        field->fieldIndex = index++;
    }

    entry = CreateTypeEntry(name, size);
    if (entry == NULL) return NULL;
    entry->fields = fields;
    return entry;
}

Typetable *TBeginDefinition(char *name) {
    Typetable *entry;
    if (name == NULL || TLookup(name) != NULL || TcurrentDefinition != NULL) return NULL;

    /* Install the incomplete entry first so fields may refer to this type recursively. */
    entry = TInstall(name, 0, NULL);
    if (entry == NULL) return NULL;
    TcurrentDefinition = entry;
    return entry;
}

Typetable *TCurrentDefinition(void) {
    return TcurrentDefinition;
}

Fieldlist *FInstall(Typetable *owner, char *name, Typetable *fieldType, int fieldIndex) {
    Fieldlist *field;
    Fieldlist *tail;
    Typetable *knownType;
    if (owner == NULL || name == NULL || fieldType == NULL || owner != TcurrentDefinition)
        return NULL;
    for (knownType = Thead; knownType != NULL; knownType = knownType->next) {
        if (knownType == fieldType) break;
    }
    if (knownType == NULL) return NULL;
    if (FLookup(owner, name) != NULL) return NULL;

    field = calloc(1, sizeof(Fieldlist));
    if (field == NULL) return NULL;
    field->name = strdup(name);
    if (field->name == NULL) {
        free(field);
        return NULL;
    }
    field->fieldIndex = fieldIndex;
    field->type = fieldType;
    if (owner->fields == NULL) owner->fields = field;
    else {
        tail = owner->fields;
        while (tail->next != NULL) tail = tail->next;
        tail->next = field;
    }
    return field;
}

void TFinishDefinition(Typetable *type) {
    Fieldlist *field;
    int size = 0;
    if (type == NULL || type != TcurrentDefinition) {
        fprintf(stderr, "Error: invalid type definition state\n");
        exit(1);
    }
    for (field = type->fields; field != NULL; field = field->next) size++;
    type->size = size;
    TcurrentDefinition = NULL;
}

Fieldlist *FLookup(Typetable *type, char *name) {
    Fieldlist *field;
    if (type == NULL || name == NULL) return NULL;

    for (field = type->fields; field != NULL; field = field->next) {
        if (strcmp(field->name, name) == 0) return field;
    }
    return NULL;
}

int GetSize(Typetable *type) {
    return type == NULL ? 0 : type->size;
}

int IsUserDefinedType(Typetable *type) {
    return type != NULL &&
           type != TLookup("int") &&
           type != TLookup("str") &&
           type != TLookup("bool") &&
           type != TLookup("void") &&
           type != TLookup("null") &&
           type != TLookup("int_ptr") &&
           type != TLookup("str_ptr");
}

void PrintTypeTable(void) {
    Typetable *type;

    printf("\nType Table\n");
    printf("%-20s %s\n", "Type", "Size (words)");
    printf("%-20s %s\n", "--------------------", "------------");

    for (type = Thead; type != NULL; type = type->next) {
        printf("%-20s %d\n", type->name, type->size);
    }

    printf("\nField Table\n");
    printf("%-20s %-12s %-20s %s\n",
           "Type", "Field index", "Field name", "Field type");
    printf("%-20s %-12s %-20s %s\n",
           "--------------------", "------------", "--------------------", "----------");
    for (type = Thead; type != NULL; type = type->next) {
        Fieldlist *field;
        for (field = type->fields; field != NULL; field = field->next) {
            printf("%-20s %-12d %-20s %s\n",
                   type->name, field->fieldIndex, field->name,
                   field->type == NULL ? "?" : field->type->name);
        }
    }
}

//stage6: Symbol tables store type entries for variables, functions, and parameters.
FunctionAST *FunctionASTHead = NULL;
static FunctionAST *functionASTTail = NULL;

Gsymbol* Lookup(char *name) { // searches the symbol table and returns the address of the matching symbol-table entry
    Gsymbol* temp = Ghead;

    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void Install(char *name, Typetable *type, int size, int rows, int cols, int isArray, Paramstruct *paramlist, int flabel) {
    // entry to symbol table
    if (type == NULL) {
        fprintf(stderr, "Error: symbol '%s' has an undefined type\n", name);
        exit(1);
    }
    if (Lookup(name)) {
        printf("Error: symbol '%s' is already declared\n", name);
        exit(1);
    }

    Gsymbol* newEntry = (Gsymbol*)malloc(sizeof(Gsymbol));

    newEntry->name = strdup(name);
    newEntry->type = type;
    newEntry->size = size;
    newEntry->rows = rows;
	newEntry->cols = cols;
    newEntry->isArray = isArray;

    newEntry->paramlist = paramlist;
    newEntry->flabel = flabel;

    if (flabel == -1) { // flabel = -1 => no function
        newEntry->binding = nextBinding;
        nextBinding += size;
    }
    else {
        newEntry->binding = -1;
    }

    newEntry->next = NULL;

    if (Ghead == NULL) {
        Ghead = newEntry;
    } else {
        Gsymbol *temp = Ghead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        
        temp->next = newEntry;
    }
}

//stage6: Function signatures compare user-defined return and parameter types by identity.
/* Task 2(a): a definition must have exactly the signature declared globally. */
void CheckFunctionDefinition(char *name, Typetable *returnType, Paramstruct *paramlist) {
    Gsymbol *function = Lookup(name);
    Paramstruct *declared;
    Paramstruct *defined;
    int position = 1;

    if (function == NULL) {
        fprintf(stderr, "Error: function '%s' is defined without a global declaration\n", name);
        exit(1);
    }
    if (function->flabel == -1) {
        fprintf(stderr, "Error: '%s' is declared as a variable, not a function\n", name);
        exit(1);
    }
    if (function->type != returnType) {
        fprintf(stderr, "Error: return type of function '%s' does not match its declaration\n", name);
        exit(1);
    }

    declared = function->paramlist;
    defined = paramlist;
    while (declared != NULL && defined != NULL) {
        if (declared->type != defined->type ||
            strcmp(declared->name, defined->name) != 0) {
            fprintf(stderr,
                    "Error: parameter %d of function '%s' does not match its declaration\n",
                    position, name);
            exit(1);
        }
        declared = declared->next;
        defined = defined->next;
        position++;
    }

    if (declared != NULL || defined != NULL) {
        fprintf(stderr, "Error: parameter count of function '%s' does not match its declaration\n", name);
        exit(1);
    }
}

Lsymbol *LLookup(char *name) {
    Lsymbol *temp = Lhead;

    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

/* Lsymbol table

a      INT    -3    ← parameter
b      INT    -4    ← parameter
x      INT     1    ← local
msg    STR     2    ← local */

static void LInstall(char *name, Typetable *type, int binding,
                     int size, int rows, int cols, int isArray) {
    Lsymbol *entry;
    Lsymbol *temp;

    if (type == NULL) {
        fprintf(stderr, "Error: local symbol '%s' has an undefined type\n", name);
        exit(1);
    }
    if (LLookup(name) != NULL) {
        fprintf(stderr, "Error: local symbol '%s' is already declared in this function\n", name);
        exit(1);
    }

    entry = calloc(1, sizeof(Lsymbol));
    entry->name = strdup(name);
    entry->type = type;
    entry->binding = binding;
    entry->size = size;
    entry->rows = rows;
    entry->cols = cols;
    entry->isArray = isArray;

    if (Lhead == NULL) {
        Lhead = entry;
        return;
    }

    temp = Lhead;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = entry;
}

/* Parameters are below BP; locals are above BP in the activation record. */
void BeginFunctionScope(Paramstruct *paramlist) {
    int parameterBinding = -3;

    Lhead = NULL;
    nextLocalBinding = 1;
    while (paramlist != NULL) {
        LInstall(paramlist->name, paramlist->type, parameterBinding--,
                 1, 0, 0, 0);
        paramlist = paramlist->next;
    }
}

void InstallLocalVariables(VarList *varlist, Typetable *type) {
    while (varlist != NULL) {
        int size = varlist->isArray ? varlist->size : 1;
        LInstall(varlist->name, type, nextLocalBinding,
                 size, varlist->rows, varlist->cols, varlist->isArray);
        nextLocalBinding += size;
        varlist = varlist->next;
    }
}

void EndFunctionScope(void) {
    /* The AST nodes retain their Lsymbol pointers; later stages free them after codegen. */
    Lhead = NULL;
}

void SaveFunctionAST(char *name, tnode *body, int isMain) {
    FunctionAST *entry = calloc(1, sizeof(FunctionAST));
    Lsymbol *local = Lhead;
    tnode *tree = body;

    if (isMain) {
        tree = createTree(0, TYPE_INT, NODE_MAIN, name, body, NULL, NULL);
    }

    entry->name = strdup(name);
    entry->tree = tree;
    entry->localTable = local;
    while (local != NULL) {
        if (local->binding > 0) {
            entry->localCount += local->size;
        }
        local = local->next;
    }
    if (FunctionASTHead == NULL) {
        FunctionASTHead = entry;
    } else {
        functionASTTail->next = entry;
    }
    functionASTTail = entry;
}

static const char *SymbolTypeName(Typetable *type) {
    return type == NULL ? "?" : type->name;
}

static void PrintTableHeader(void) {
    printf("%-16s %-10s %-8s %-10s %-24s %-14s\n",
           "Name", "Type", "Size", "Binding", "Parameters", "Function label");
}

void PrintLocalSymbolTable(char *functionName) {
    Lsymbol *temp = Lhead;

    printf("\nLocal Symbol Table: %s\n", functionName);
    PrintTableHeader();
    while (temp != NULL) {
        printf("%-16s %-10s %-8d %-10d %-24s %-14s\n",
               temp->name, SymbolTypeName(temp->type), temp->size, temp->binding, "-", "-");
        temp = temp->next;
    }
}

void PrintSymbolTable() {
    Gsymbol *temp = Ghead;
    printf("\nGlobal Symbol Table\n");
    PrintTableHeader();

    while (temp != NULL) {
        if (temp->flabel == -1) {
            printf("%-16s %-10s %-8d %-10d %-24s %-14s\n",
                   temp->name, SymbolTypeName(temp->type), temp->size, temp->binding, "-", "-");
        } else {
            Paramstruct *param = temp->paramlist;
            char parameters[256] = "";
            if (param == NULL) {
                strcpy(parameters, "(none)");
            } else {
                while (param != NULL) {
                    size_t used = strlen(parameters);
                    snprintf(parameters + used, sizeof(parameters) - used, "%s%s %s",
                             used == 0 ? "" : ", ", SymbolTypeName(param->type), param->name);
                    param = param->next;
                }
            }
            char label[16];
            snprintf(label, sizeof(label), "F%d", temp->flabel);
            printf("%-16s %-10s %-8s %-10s %-24s %-14s\n",
                   temp->name, SymbolTypeName(temp->type), "-", "-", parameters, label);
        }

        temp = temp->next;
    }
}

tnode* makeAddressNode(tnode *var) { // creates an AST node for the address-of operator
    if (var->nodetype != NODE_ID) {
        fprintf(stderr, "Address-of operator can only be used with a variable\n");
        exit(1);
    }

    int pointerType;

    if (var->type == TYPE_INT)
        pointerType = TYPE_INT_PTR;
    else if (var->type == TYPE_STR)
        pointerType = TYPE_STR_PTR;
    else {
        fprintf(stderr, "Cannot take address of a pointer\n");
        exit(1);
    }

    return createTree(0, pointerType, NODE_ADDRESS, NULL, var, NULL, NULL);
}

tnode* makeDereferenceNode(tnode *ptr) { // EX2: creates an AST node for dereferencing a pointer
    int valueType;

    if (ptr->type == TYPE_INT_PTR)
        valueType = TYPE_INT;
    else if (ptr->type == TYPE_STR_PTR)
        valueType = TYPE_STR;
    else {
        fprintf(stderr, "Cannot dereference a non-pointer\n");
        exit(1);
    }

    return createTree(0, valueType, NODE_DEREFERENCE, NULL, ptr, NULL, NULL);
}

tnode* createTree(int val, int type, int nodetype, char* varname, tnode* l, tnode* m, tnode* r) {
    tnode* temp = (tnode*)malloc(sizeof(tnode));

    temp->val = val;
    temp->type = type;
    temp->typeEntry = TypeEntryForCode(type);
    temp->nodetype = nodetype;
    temp->varname = varname;
    temp->Gentry = NULL;
    temp->Lentry = NULL;
    temp->Fentry = NULL;
    temp->arglist = NULL;
    temp->left = l;
    temp->middle = m;
    temp->right = r;

    return temp;
}

tnode* makeNumNode(int n) {
    return createTree(n, TYPE_INT, NODE_NUM, NULL, NULL, NULL, NULL);
}

tnode* makeStrNode(char *s) {
    return createTree(0, TYPE_STR, NODE_STR, s, NULL, NULL, NULL); // we put s into varname field as first arg (int val) is int
}

//stage6: Build NULL AST nodes with the dedicated null type-table entry.
tnode* makeNullNode(void) {
    tnode *node = createTree(0, TYPE_NULL, NODE_NULL, NULL, NULL, NULL, NULL);
    node->typeEntry = TLookup("null");
    return node;
}

//stage6: Equality checks enforce name equivalence and permit NULL/user-type comparisons.
tnode* makeOperatorNode(char* op, tnode* l, tnode* r) {
    int nodetype;
    int type;

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
    else if (strcmp(op, "%") == 0) {
        nodetype = NODE_MOD;
        type = TYPE_INT;
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
    else if (strcmp(op, "||") == 0) {
        nodetype = NODE_OR;
        type = TYPE_BOOL;
    }
    else if (strcmp(op, "&&") == 0) {
        nodetype = NODE_AND;
        type = TYPE_BOOL;
    }
    else {
        fprintf(stderr, "Invalid operator %s\n", op);
        exit(1);
    }

    if ((nodetype == NODE_OR || nodetype == NODE_AND) &&
        (l->type != TYPE_BOOL || r->type != TYPE_BOOL)) {
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }
    if (nodetype == NODE_EQ || nodetype == NODE_NE) {
        int sameNamedType = l->typeEntry == r->typeEntry;
        int leftIsNull = l->typeEntry == TLookup("null");
        int rightIsNull = r->typeEntry == TLookup("null");
        int nullComparedWithUserType =
            (leftIsNull && IsUserDefinedType(r->typeEntry)) ||
            (rightIsNull && IsUserDefinedType(l->typeEntry));

        if (!sameNamedType && !nullComparedWithUserType) {
            fprintf(stderr, "Type mismatch\n");
            exit(1);
        }
    }
    if (nodetype != NODE_EQ && nodetype != NODE_NE &&
        nodetype != NODE_OR && nodetype != NODE_AND &&
        (l->type != TYPE_INT || r->type != TYPE_INT)) {
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }

    return createTree(0, type, nodetype, NULL, l, NULL, r);
}

//stage6: Resolve identifiers while preserving their exact user-defined type entry.
tnode* makeIdNode(char* name) {
    Lsymbol *localEntry = LLookup(name);
    Gsymbol *globalEntry;
    tnode *node;

    /* Local scope takes precedence over a global declaration of the same name. */
    if (localEntry != NULL) {
        node = createTree(0, TypeCode(localEntry->type), NODE_ID, name, NULL, NULL, NULL);
        node->typeEntry = localEntry->type;
        node->Lentry = localEntry;
        return node;
    }

    globalEntry = Lookup(name);
    if (globalEntry == NULL) {
        printf("Error: Variable %s not declared\n", name);
        exit(1);
    }
    if (globalEntry->flabel != -1) {
        fprintf(stderr, "Error: '%s' is a function; call it with parentheses\n", name);
        exit(1);
    }

    node = createTree(0, TypeCode(globalEntry->type), NODE_ID, name, NULL, NULL, NULL);
    node->typeEntry = globalEntry->type;
    node->Gentry = globalEntry;
    return node;
}

//stage6: Enforce name-based type compatibility, allowing NULL for user-defined values.
tnode* makeAssignNode(tnode* id, tnode* expr) {
    // rhs null avanam + lhs null avaruth + lhs field indel athu null avaruth (field = null anel athu default type aan not user-defined)
    int nullToUserType = expr->typeEntry == TLookup("null") &&
                         IsUserDefinedType(id->typeEntry);

    if (id->typeEntry != expr->typeEntry && !nullToUserType) {
        // eg: a = 5 > 3
        // i.e id type and rhs type same avanam
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }

    return createTree(0, TYPE_INT, NODE_ASSIGN, NULL, id, NULL, expr);
}

//stage6: Validate heap operations against user-defined types and the 8-word block limit.
tnode* makeAllocNode(tnode *target) {
    int size;
    tnode *node;

    if (target == NULL || !IsUserDefinedType(target->typeEntry)) {
        fprintf(stderr, "Error: alloc() target must have a user-defined type\n");
        exit(1);
    }

    size = GetSize(target->typeEntry);
    if (size <= 0 || size > 8) {
        fprintf(stderr,
                "Error: type '%s' needs %d words; alloc() supports types of 1 to 8 words\n",
                target->typeEntry->name, size);
        exit(1);
    }

    node = createTree(0, TYPE_INT, NODE_ALLOC, NULL, target, NULL, NULL);
    node->typeEntry = target->typeEntry;
    return node;
}

//stage6: Validate that free receives a user-defined type value.
tnode* makeFreeNode(tnode *target) {
    tnode *node;

    if (target == NULL || !IsUserDefinedType(target->typeEntry)) {
        fprintf(stderr, "Error: free() argument must have a user-defined type\n");
        exit(1);
    }

    node = createTree(0, TYPE_INT, NODE_FREE, NULL, target, NULL, NULL);
    node->typeEntry = target->typeEntry;
    return node;
}

tnode* makeReadNode(tnode* id) {
    return createTree(0, TYPE_INT, NODE_READ, NULL, id, NULL, NULL);
}

tnode* makeWriteNode(tnode* expr) {
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

//stage6: Check return expressions and function calls against declared user types.
tnode* makeReturnNode(tnode *expr) {
    if (expr->typeEntry == NULL || expr->typeEntry == TLookup("bool") ||
        expr->typeEntry == TLookup("void") || expr->typeEntry == TLookup("null")) {
        fprintf(stderr, "Error: invalid function return type\n");
        exit(1);
    }
    tnode *node = createTree(0, expr->type, NODE_RETURN, NULL, expr, NULL, NULL);
    node->typeEntry = expr->typeEntry;
    return node;
}

tnode* makeBodyNode(tnode *statements, tnode *returnNode) {
    tnode *node = createTree(0, returnNode->type, NODE_BODY, NULL,
                             statements, NULL, returnNode);
    node->typeEntry = returnNode->typeEntry;
    return node;
}

tnode* finalizeBodyNode(tnode *body, Typetable *returnType, char *functionName) {
    if (body->typeEntry != returnType) {
        fprintf(stderr, "Error: return type of function '%s' does not match its declaration\n",
                functionName);
        exit(1);
    }
    return body;
}

tnode* makeArgListNode(tnode *expr) {
    tnode *node = createTree(0, expr->type, NODE_ARG_LIST, NULL, expr, NULL, NULL);
    node->typeEntry = expr->typeEntry;
    return node;
}

tnode* appendArgNode(tnode *arglist, tnode *expr) {
    tnode *tail = arglist;
    tnode *newArg = makeArgListNode(expr);

    while (tail->right != NULL) {
        tail = tail->right;
    }
    tail->right = newArg;
    return arglist;
}

//stage6: Check actual argument types against the function's formal type entries.
tnode* makeFunctionCallNode(char *name, tnode *arglist) {
    Gsymbol *function = Lookup(name);
    Paramstruct *formal;
    tnode *actual;
    int position = 1;
    tnode *node;

    if (function == NULL) {
        fprintf(stderr, "Error: function '%s' is not declared\n", name);
        exit(1);
    }
    if (function->flabel == -1) {
        fprintf(stderr, "Error: '%s' is a variable, not a function\n", name);
        exit(1);
    }

    formal = function->paramlist;
    actual = arglist;
    while (formal != NULL && actual != NULL) {
        if (formal->type != actual->left->typeEntry) {
            fprintf(stderr, "Error: argument %d of '%s' has the wrong type\n",
                    position, name);
            exit(1);
        }
        formal = formal->next;
        actual = actual->right;
        position++;
    }
    if (formal != NULL || actual != NULL) {
        fprintf(stderr, "Error: argument count of '%s' does not match its declaration\n", name);
        exit(1);
    }

    node = createTree(0, TypeCode(function->type), NODE_FUNCTION, name, NULL, NULL, NULL);
    node->typeEntry = function->type;
    node->Gentry = function;
    node->arglist = arglist;
    return node;
}

// for an array with 3 elements, the ast looks like this
//                   CONNECTOR
//               /            \
//           CONNECTOR       ASSIGN
//           /       \        /    \
//       ASSIGN     ASSIGN   ARRAY  30
//       /   \       /  \    |
//    ARRAY  10   ARRAY 20  some-expression
//      |            |
//    NUM(0)       NUM(1)

tnode* makeArrayNode(char* name, tnode* index) { // index is an expression E 
    Lsymbol *local = LLookup(name);
    Gsymbol *global = local == NULL ? Lookup(name) : NULL;
    Typetable *elementType;
    int size, cols, isArray;

    if (local == NULL && global == NULL) {
        fprintf(stderr, "Error: array '%s' is not declared\n", name);
        exit(1);
    }
    elementType = local != NULL ? local->type : global->type;
    size = local != NULL ? local->size : global->size;
    cols = local != NULL ? local->cols : global->cols;
    isArray = local != NULL ? local->isArray : global->isArray;
    if (!isArray) {
        fprintf(stderr, "Error: '%s' is not an array\n", name);
        exit(1);
    }
    if (cols > 0) {
        fprintf(stderr, "Error: two-dimensional array '%s' requires two indices\n", name);
        exit(1);
    }

    if (index->type != TYPE_INT) {
        printf("Error: Array index must be an integer\n");
        exit(1);
    }

    if (index->nodetype == NODE_NUM) {
        if (index->val >= size) {
            fprintf(stderr, "Error: Array index %d out of bounds (array size %d)\n", 
                    index->val, size);
            exit(1);
        }
    }

    tnode* node = createTree(0, TypeCode(elementType), NODE_ARRAY, name, index, NULL, NULL);
    node->typeEntry = elementType;
    node->Gentry = global;
    node->Lentry = local;

    return node;
}

tnode* makeArray2DNode(char *name, tnode *rowIndex, tnode *colIndex) {
	Lsymbol *local = LLookup(name);
	Gsymbol *global = local == NULL ? Lookup(name) : NULL;
	Typetable *elementType;
	int rows, cols, isArray;

	if (local == NULL && global == NULL) {
		fprintf(stderr, "Error: array '%s' is not declared\n", name);
		exit(1);
	}
	elementType = local != NULL ? local->type : global->type;
	rows = local != NULL ? local->rows : global->rows;
	cols = local != NULL ? local->cols : global->cols;
	isArray = local != NULL ? local->isArray : global->isArray;
	if (!isArray || rows <= 0 || cols <= 0) {
		fprintf(stderr, "Error: '%s' is not a two-dimensional array\n", name);
		exit(1);
	}

	if (rowIndex->type != TYPE_INT || colIndex->type != TYPE_INT) {
		printf("Error: Array indices must be integers\n");
		exit(1);
	}

	if (rowIndex->nodetype == NODE_NUM && rowIndex->val >= rows) {
		fprintf(stderr, "Error: row index %d out of bounds (row count %d)\n", rowIndex->val, rows);
		exit(1);
	}
	if (colIndex->nodetype == NODE_NUM && colIndex->val >= cols) {
		fprintf(stderr, "Error: column index %d out of bounds (column count %d)\n", colIndex->val, cols);
		exit(1);
	}
	tnode *node = createTree(0, TypeCode(elementType), NODE_ARRAY2D, name, rowIndex, colIndex, NULL);
	node->typeEntry = elementType;
	node->Gentry = global;
	node->Lentry = local;

	return node;
}

//stage6: Resolve field names, field types, and offsets while constructing the AST.
tnode* makeFieldNode(tnode *base, char *fieldName) {
    Fieldlist *field;
    tnode *node;

    if (base == NULL || base->typeEntry == NULL || fieldName == NULL) {
        fprintf(stderr, "Error: invalid field access\n");
        exit(1);
    }

    if (base->typeEntry->fields == NULL) {
        fprintf(stderr, "Error: type '%s' has no fields\n", base->typeEntry->name);
        exit(1);
    }

    field = FLookup(base->typeEntry, fieldName);
    if (field == NULL) {
        fprintf(stderr, "Error: type '%s' has no field named '%s'\n",
                base->typeEntry->name, fieldName);
        exit(1);
    }

    node = createTree(0, TypeCode(field->type), NODE_FIELD, fieldName,
                      base, NULL, NULL);
    node->typeEntry = field->type;
    node->Fentry = field;
    return node;
}
