#include "exprtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Gsymbol* Ghead = NULL;
Lsymbol* Lhead = NULL;
int nextBinding = 4096;
int nextLocalBinding = 1;

static Typetable *Thead = NULL;
static Typetable *Ttail = NULL;
static Typetable *TcurrentDefinition = NULL;
static Classtable *Chead = NULL;
static Classtable *Ctail = NULL;
static int nextClassIndex = 0;
static Classtable *activeMethodClass = NULL;
MethodAST *MethodASTHead = NULL;
static MethodAST *methodASTTail = NULL;

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

Classtable *CLookup(char *name) {
    Classtable *entry;
    if (name == NULL) return NULL;
    for (entry = Chead; entry != NULL; entry = entry->next) {
        if (strcmp(entry->name, name) == 0) return entry;
    }
    return NULL;
}

Classtable *CGetHead(void) {
    return Chead;
}

static int IsSameOrDerivedClass(Classtable *candidate, Classtable *expected) {
    while (candidate != NULL) {
        if (candidate == expected) return 1;
        candidate = candidate->parentptr;
    }
    return 0;
}

static Paramstruct *CloneClassParams(Paramstruct *source) {
    Paramstruct *head = NULL;
    Paramstruct *tail = NULL;

    while (source != NULL) {
        Paramstruct *copy = calloc(1, sizeof(Paramstruct));
        if (copy == NULL) goto fail;
        copy->name = strdup(source->name);
        if (copy->name == NULL) {
            free(copy);
            goto fail;
        }
        copy->type = source->type;
        copy->classType = source->classType;
        if (tail == NULL) head = copy;
        else tail->next = copy;
        tail = copy;
        source = source->next;
    }
    return head;

fail:
    while (head != NULL) {
        Paramstruct *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
    return NULL;
}

static void FreeClassParams(Paramstruct *param) {
    while (param != NULL) {
        Paramstruct *next = param->next;
        free(param->name);
        free(param);
        param = next;
    }
}

static int SameMethodSignature(Typetable *returnType, Paramstruct *params,
                               Memberfunclist *inherited) {
    Paramstruct *expected;

    if (inherited == NULL || inherited->type != returnType) return 0;
    expected = inherited->paramlist;
    while (expected != NULL && params != NULL) {
        if (expected->type != params->type ||
            expected->classType != params->classType) return 0;
        expected = expected->next;
        params = params->next;
    }
    return expected == NULL && params == NULL;
}

static void FreeClassMembers(Classtable *classEntry) {
    Fieldlist *field = classEntry->memberfield;
    Memberfunclist *method = classEntry->vfuncptr;

    while (field != NULL) {
        Fieldlist *next = field->next;
        free(field->name);
        free(field);
        field = next;
    }
    while (method != NULL) {
        Memberfunclist *nextMethod = method->next;
        FreeClassParams(method->paramlist);
        free(method->name);
        free(method);
        method = nextMethod;
    }
}

static int CopyInheritedMembers(Classtable *child, Classtable *parent) {
    Fieldlist *sourceField;
    Fieldlist **fieldTail = &child->memberfield;
    Memberfunclist *sourceMethod;
    Memberfunclist **methodTail = &child->vfuncptr;

    for (sourceField = parent->memberfield; sourceField != NULL;
         sourceField = sourceField->next) {
        Fieldlist *copy = calloc(1, sizeof(Fieldlist));
        if (copy == NULL) return 0;
        copy->name = strdup(sourceField->name);
        if (copy->name == NULL) {
            free(copy);
            return 0;
        }
        copy->fieldIndex = sourceField->fieldIndex;
        copy->type = sourceField->type;
        copy->classType = sourceField->classType;
        *fieldTail = copy;
        fieldTail = &copy->next;
        child->fieldCount++;
        child->fieldWords += sourceField->classType != NULL ? 2 : 1;
    }

    for (sourceMethod = parent->vfuncptr; sourceMethod != NULL;
         sourceMethod = sourceMethod->next) {
        Memberfunclist *copy = calloc(1, sizeof(Memberfunclist));
        if (copy == NULL) return 0;
        copy->name = strdup(sourceMethod->name);
        if (copy->name == NULL) {
            free(copy);
            return 0;
        }
        copy->paramlist = CloneClassParams(sourceMethod->paramlist);
        if (sourceMethod->paramlist != NULL && copy->paramlist == NULL) {
            free(copy->name);
            free(copy);
            return 0;
        }
        copy->type = sourceMethod->type;
        copy->ownerClass = sourceMethod->ownerClass;
        copy->funcposition = sourceMethod->funcposition;
        copy->flabel = sourceMethod->flabel;
        copy->isDefined = sourceMethod->isDefined;
        *methodTail = copy;
        methodTail = &copy->next;
        child->methodCount++;
    }
    return 1;
}

Classtable *CInstall(char *name, char *parentClassName) {
    Classtable *entry;
    Classtable *parent = NULL;

    if (name == NULL || TLookup(name) != NULL || CLookup(name) != NULL)
        return NULL;
    if (parentClassName != NULL) {
        parent = CLookup(parentClassName);
        if (parent == NULL) return NULL;
    }

    entry = calloc(1, sizeof(Classtable));
    if (entry == NULL) return NULL;
    entry->name = strdup(name);
    if (entry->name == NULL) {
        free(entry);
        return NULL;
    }
    entry->parentptr = parent;
    entry->classIndex = nextClassIndex;

    if (parent != NULL && !CopyInheritedMembers(entry, parent)) {
        FreeClassMembers(entry);
        free(entry->name);
        free(entry);
        return NULL;
    }

    /* Reserve eight stack words for this class's virtual function table.
       Classes are installed before global declarations, so globals are
       allocated immediately after all class tables. */
    nextBinding += 8;
    nextClassIndex++;

    if (Chead == NULL) Chead = entry;
    else Ctail->next = entry;
    Ctail = entry;
    return entry;
}

Fieldlist *Class_Flookup(Classtable *classEntry, char *name) {
    Fieldlist *field;
    if (classEntry == NULL || name == NULL) return NULL;
    for (field = classEntry->memberfield; field != NULL; field = field->next) {
        if (strcmp(field->name, name) == 0) return field;
    }
    return NULL;
}

Fieldlist *Class_Finstall(Classtable *classEntry, char *typeName, char *fieldName) {
    Fieldlist *field, *tail;
    Typetable *fieldType;
    Classtable *fieldClass;
    int fieldWords;

    if (classEntry == NULL || typeName == NULL || fieldName == NULL ||
        classEntry->fieldCount >= 8 || Class_Flookup(classEntry, fieldName) != NULL ||
        (classEntry->parentptr != NULL &&
         Class_Flookup(classEntry->parentptr, fieldName) != NULL))
        return NULL;

    fieldType = TLookup(typeName);
    fieldClass = fieldType == NULL ? CLookup(typeName) : NULL;
    if (fieldType == NULL && fieldClass == NULL) return NULL;
    fieldWords = fieldClass != NULL ? 2 : 1;
    if (classEntry->fieldWords + fieldWords > 8) return NULL;

    field = calloc(1, sizeof(Fieldlist));
    if (field == NULL) return NULL;
    field->name = strdup(fieldName);
    if (field->name == NULL) {
        free(field);
        return NULL;
    }
    field->fieldIndex = classEntry->fieldWords;
    classEntry->fieldCount++;
    classEntry->fieldWords += fieldWords;
    field->type = fieldType;
    field->classType = fieldClass;

    if (classEntry->memberfield == NULL) classEntry->memberfield = field;
    else {
        tail = classEntry->memberfield;
        while (tail->next != NULL) tail = tail->next;
        tail->next = field;
    }
    return field;
}

Memberfunclist *Class_Mlookup(Classtable *classEntry, char *name) {
    Memberfunclist *method;
    if (classEntry == NULL || name == NULL) return NULL;
    for (method = classEntry->vfuncptr; method != NULL; method = method->next) {
        if (strcmp(method->name, name) == 0) return method;
    }
    return NULL;
}

Memberfunclist *Class_Minstall(Classtable *classEntry, char *name,
                               Typetable *returnType, Paramstruct *paramlist) {
    Memberfunclist *method, *tail;
    if (classEntry == NULL || name == NULL || returnType == NULL) return NULL;

    method = Class_Mlookup(classEntry, name);
    if (method != NULL) {
        if (method->ownerClass == classEntry ||
            !SameMethodSignature(returnType, paramlist, method))
            return NULL;

        FreeClassParams(method->paramlist);
        method->paramlist = paramlist;
        method->type = returnType;
        method->ownerClass = classEntry;
        method->flabel = nextFunctionLabel++;
        method->isDefined = 0;
        return method;
    }

    if (classEntry->methodCount >= 8) return NULL;

    method = calloc(1, sizeof(Memberfunclist));
    if (method == NULL) return NULL;
    method->name = strdup(name);
    if (method->name == NULL) {
        free(method);
        return NULL;
    }
    method->ownerClass = classEntry;
    method->type = returnType;
    method->paramlist = paramlist;
    method->funcposition = classEntry->methodCount++;
    method->flabel = nextFunctionLabel++;

    if (classEntry->vfuncptr == NULL) classEntry->vfuncptr = method;
    else {
        tail = classEntry->vfuncptr;
        while (tail->next != NULL) tail = tail->next;
        tail->next = method;
    }
    return method;
}

Memberfunclist *Class_MatchMethodDefinition(Classtable *classEntry, char *name,
                                            Typetable *returnType, Paramstruct *paramlist) {
    Memberfunclist *method = Class_Mlookup(classEntry, name);
    Paramstruct *declared;
    Paramstruct *defined;
    int position = 1;

    if (method == NULL) {
        fprintf(stderr, "Error: method '%s' is defined but not declared in class '%s'\n",
                name == NULL ? "?" : name,
                classEntry == NULL ? "?" : classEntry->name);
        return NULL;
    }
    if (method->isDefined) {
        fprintf(stderr, "Error: method '%s' is defined more than once in class '%s'\n",
                name, classEntry->name);
        return NULL;
    }
    if (method->type != returnType) {
        fprintf(stderr, "Error: return type of method '%s' does not match its declaration in class '%s'\n",
                name, classEntry->name);
        return NULL;
    }

    declared = method->paramlist;
    defined = paramlist;
    while (declared != NULL && defined != NULL) {
        if (declared->type != defined->type ||
            declared->classType != defined->classType ||
            strcmp(declared->name, defined->name) != 0) {
            fprintf(stderr,
                    "Error: parameter %d of method '%s' does not match its declaration in class '%s'\n",
                    position, name, classEntry->name);
            return NULL;
        }
        declared = declared->next;
        defined = defined->next;
        position++;
    }
    if (declared != NULL || defined != NULL) {
        fprintf(stderr, "Error: parameter count of method '%s' does not match its declaration in class '%s'\n",
                name, classEntry->name);
        return NULL;
    }

    method->isDefined = 1;
    return method;
}

int Class_CheckMethodDefinitions(Classtable *classEntry) {
    Memberfunclist *method;
    int allDefined = 1;

    if (classEntry == NULL) return 0;
    for (method = classEntry->vfuncptr; method != NULL; method = method->next) {
        if (!method->isDefined) {
            fprintf(stderr, "Error: method '%s' is declared but not defined in class '%s'\n",
                    method->name, classEntry->name);
            allDefined = 0;
        }
    }
    return allDefined;
}

void PrintClassTable(void) {
    Classtable *classEntry;

    printf("\nClass Table\n");
    printf("%-20s %-20s %-12s %-10s %-10s\n",
           "Class name", "Parent class", "Class index", "Fields", "Methods");
    printf("%-20s %-20s %-12s %-10s %-10s\n",
           "--------------------", "--------------------", "------------", "----------", "----------");
    for (classEntry = Chead; classEntry != NULL; classEntry = classEntry->next) {
        printf("%-20s %-20s %-12d %-10d %-10d\n", classEntry->name,
               classEntry->parentptr == NULL ? "NULL" : classEntry->parentptr->name,
               classEntry->classIndex, classEntry->fieldCount, classEntry->methodCount);
    }

    printf("\nClass Field Table\n");
    printf("%-20s %-12s %-24s %s\n",
           "Class", "Word offset", "Field name", "Field type");
    printf("%-20s %-12s %-24s %s\n",
           "--------------------", "------------", "------------------------", "----------");
    for (classEntry = Chead; classEntry != NULL; classEntry = classEntry->next) {
        Fieldlist *field;
        for (field = classEntry->memberfield; field != NULL; field = field->next) {
            const char *typeName = field->classType != NULL ? field->classType->name :
                                   field->type != NULL ? field->type->name : "?";
            printf("%-20s %-12d %-24s %s\n",
                   classEntry->name, field->fieldIndex, field->name, typeName);
        }
    }

    printf("\nClass Method Table\n");
    printf("%-20s %-12s %-24s %-16s %-32s %s\n",
           "Class", "Method index", "Method name", "Return type", "Parameters", "Label");
    printf("%-20s %-12s %-24s %-16s %-32s %s\n",
           "--------------------", "------------", "------------------------", "----------------",
           "--------------------------------", "-----");
    for (classEntry = Chead; classEntry != NULL; classEntry = classEntry->next) {
        Memberfunclist *method;
        for (method = classEntry->vfuncptr; method != NULL; method = method->next) {
            Paramstruct *param;
            char parameters[256] = "";

            for (param = method->paramlist; param != NULL; param = param->next) {
                size_t used = strlen(parameters);
                snprintf(parameters + used, sizeof(parameters) - used, "%s%s %s",
                         used == 0 ? "" : ", ",
                         param->classType != NULL ? param->classType->name :
                         param->type != NULL ? param->type->name : "?",
                         param->name == NULL ? "?" : param->name);
            }
            if (method->paramlist == NULL) {
                strcpy(parameters, "(none)");
            }
            printf("%-20s %-12d %-24s %-16s %-32s F%d\n",
                   classEntry->name, method->funcposition, method->name,
                   method->type == NULL ? "?" : method->type->name,
                   parameters, method->flabel);
        }
    }
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
    newEntry->classType = NULL;
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

void InstallClassVariable(char *name, Classtable *classType) {
    Gsymbol *entry;
    Gsymbol *tail;

    if (name == NULL || classType == NULL) {
        fprintf(stderr, "Error: class variable has an undefined class type\n");
        exit(1);
    }
    if (Lookup(name) != NULL) {
        fprintf(stderr, "Error: symbol '%s' is already declared\n", name);
        exit(1);
    }

    entry = calloc(1, sizeof(Gsymbol));
    if (entry == NULL) {
        fprintf(stderr, "Error: could not install class variable '%s'\n", name);
        exit(1);
    }
    entry->name = strdup(name);
    if (entry->name == NULL) {
        free(entry);
        fprintf(stderr, "Error: could not install class variable '%s'\n", name);
        exit(1);
    }
    entry->classType = classType;
    entry->size = 2;
    entry->binding = nextBinding;
    nextBinding += 2;
    entry->flabel = -1;

    if (Ghead == NULL) Ghead = entry;
    else {
        tail = Ghead;
        while (tail->next != NULL) tail = tail->next;
        tail->next = entry;
    }
}

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
            declared->classType != defined->classType ||
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
                     int size, int rows, int cols, int isArray,
                     Classtable *classType) {
    Lsymbol *entry;
    Lsymbol *temp;

    if (type == NULL && classType == NULL) {
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
    entry->classType = classType;
    entry->binding = binding;
    entry->vtableBinding = classType == NULL ? 0 :
                          (binding > 0 ? binding + 1 : binding - 1);
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
        int parameterSize = paramlist->classType != NULL ? 2 : 1;
        LInstall(paramlist->name, paramlist->type, parameterBinding--,
                 parameterSize, 0, 0, 0, paramlist->classType);
        if (parameterSize == 2) parameterBinding--;
        paramlist = paramlist->next;
    }
}

/* The hidden table pointer and self pointer precede declared parameters.
   The table pointer is pushed first, so it occupies self's binding minus one. */
void BeginMethodScope(Paramstruct *paramlist, Classtable *classEntry) {
    int parameterBinding = -3;
    int parameterCount = 0;
    Paramstruct *param;

    if (classEntry == NULL) {
        fprintf(stderr, "Error: cannot begin a method scope without a class\n");
        exit(1);
    }
    for (param = paramlist; param != NULL; param = param->next)
        parameterCount += param->classType != NULL ? 2 : 1;
    Lhead = NULL;
    nextLocalBinding = 1;
    LInstall("self", NULL, -(parameterCount + 3), 2, 0, 0, 0, classEntry);
    while (paramlist != NULL) {
        int parameterSize = paramlist->classType != NULL ? 2 : 1;
        LInstall(paramlist->name, paramlist->type, parameterBinding--,
                 parameterSize, 0, 0, 0, paramlist->classType);
        if (parameterSize == 2) parameterBinding--;
        paramlist = paramlist->next;
    }
    activeMethodClass = classEntry;
}

void InstallLocalVariables(VarList *varlist, Typetable *type) {
    while (varlist != NULL) {
        int size = varlist->isArray ? varlist->size : 1;
        LInstall(varlist->name, type, nextLocalBinding,
                 size, varlist->rows, varlist->cols, varlist->isArray, NULL);
        nextLocalBinding += size;
        varlist = varlist->next;
    }
}

void InstallLocalClassVariables(VarList *varlist, Classtable *classType) {
    if (classType == NULL) {
        fprintf(stderr, "Error: local variable has an undefined class type\n");
        exit(1);
    }

    while (varlist != NULL) {
        if (varlist->isArray || varlist->isPointer || varlist->isFunction) {
            fprintf(stderr,
                    "Error: local class variables must be scalar values\n");
            exit(1);
        }
        LInstall(varlist->name, NULL, nextLocalBinding,
                 2, 0, 0, 0, classType);
        nextLocalBinding += 2;
        varlist = varlist->next;
    }
}

void EndFunctionScope(void) {
    /* The AST nodes retain their Lsymbol pointers; later stages free them after codegen. */
    Lhead = NULL;
    activeMethodClass = NULL;
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

void SaveMethodAST(Classtable *classEntry, Memberfunclist *methodEntry, tnode *body) {
    MethodAST *entry = calloc(1, sizeof(MethodAST));
    Lsymbol *local;

    if (entry == NULL || classEntry == NULL || methodEntry == NULL || body == NULL) {
        fprintf(stderr, "Error: could not save class method AST\n");
        exit(1);
    }
    entry->classEntry = classEntry;
    entry->methodEntry = methodEntry;
    entry->tree = body;
    entry->localTable = Lhead;
    for (local = Lhead; local != NULL; local = local->next) {
        if (local->binding > 0) entry->localCount += local->size;
    }

    if (MethodASTHead == NULL) MethodASTHead = entry;
    else methodASTTail->next = entry;
    methodASTTail = entry;
}

static const char *SymbolTypeName(Typetable *type) {
    return type == NULL ? "?" : type->name;
}

static const char *GlobalSymbolTypeName(Gsymbol *symbol) {
    if (symbol->classType != NULL) return symbol->classType->name;
    return SymbolTypeName(symbol->type);
}

static const char *LocalSymbolTypeName(Lsymbol *symbol) {
    if (symbol->classType != NULL) return symbol->classType->name;
    return SymbolTypeName(symbol->type);
}

static const char *ParameterTypeName(Paramstruct *parameter) {
    if (parameter->classType != NULL) return parameter->classType->name;
    return SymbolTypeName(parameter->type);
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
               temp->name, LocalSymbolTypeName(temp), temp->size, temp->binding, "-", "-");
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
                   temp->name, GlobalSymbolTypeName(temp), temp->size, temp->binding, "-", "-");
        } else {
            Paramstruct *param = temp->paramlist;
            char parameters[256] = "";
            if (param == NULL) {
                strcpy(parameters, "(none)");
            } else {
                while (param != NULL) {
                    size_t used = strlen(parameters);
                    snprintf(parameters + used, sizeof(parameters) - used, "%s%s %s",
                             used == 0 ? "" : ", ", ParameterTypeName(param), param->name);
                    param = param->next;
                }
            }
            char label[16];
            snprintf(label, sizeof(label), "F%d", temp->flabel);
            printf("%-16s %-10s %-8s %-10s %-24s %-14s\n",
                   temp->name, GlobalSymbolTypeName(temp), "-", "-", parameters, label);
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
    temp->classType = NULL;
    temp->classOwner = NULL;
    temp->nodetype = nodetype;
    temp->varname = varname;
    temp->Gentry = NULL;
    temp->Lentry = NULL;
    temp->Fentry = NULL;
    temp->Mentry = NULL;
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

tnode* makeNullNode(void) {
    tnode *node = createTree(0, TYPE_NULL, NODE_NULL, NULL, NULL, NULL, NULL);
    node->typeEntry = TLookup("null");
    return node;
}

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

tnode* makeIdNode(char* name) {
    Lsymbol *localEntry = LLookup(name);
    Gsymbol *globalEntry;
    tnode *node;

    /* Local scope takes precedence over a global declaration of the same name. */
    if (localEntry != NULL) {
        node = createTree(0, TypeCode(localEntry->type), NODE_ID, name, NULL, NULL, NULL);
        node->typeEntry = localEntry->type;
        node->classType = localEntry->classType;
        node->Lentry = localEntry;
        return node;
    }

    if (activeMethodClass != NULL) {
        fprintf(stderr, "Error: variable '%s' is not declared in method '%s'\n",
                name, activeMethodClass->name);
        exit(1);
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
    node->classType = globalEntry->classType;
    node->Gentry = globalEntry;
    return node;
}

tnode* makeAssignNode(tnode* id, tnode* expr) {
    int compatible;

    if (id == NULL || expr == NULL) {
        fprintf(stderr, "Error: invalid assignment\n");
        exit(1);
    }

    if (id->classType != NULL) {
        compatible = IsSameOrDerivedClass(expr->classType, id->classType) ||
                     (expr->classType == NULL && expr->typeEntry == TLookup("null"));
    } else {
        int nullToUserType = expr->typeEntry == TLookup("null") &&
                             IsUserDefinedType(id->typeEntry);
        compatible = expr->classType == NULL &&
                     (id->typeEntry == expr->typeEntry || nullToUserType);
    }

    if (!compatible) {
        // eg: a = 5 > 3
        // i.e id type and rhs type same avanam
        fprintf(stderr, "Type mismatch\n");
        exit(1);
    }

    return createTree(0, TYPE_INT, NODE_ASSIGN, NULL, id, NULL, expr);
}

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

tnode* makeClassAllocNode(tnode *target, char *className) {
    Classtable *requestedClass = CLookup(className);
    tnode *node;

    if (requestedClass == NULL) {
        fprintf(stderr, "Error: class '%s' is not defined\n",
                className == NULL ? "?" : className);
        exit(1);
    }
    if (target == NULL || target->classType == NULL) {
        fprintf(stderr, "Error: new(%s) target must be a class variable\n", className);
        exit(1);
    }
    if (!IsSameOrDerivedClass(requestedClass, target->classType)) {
        fprintf(stderr, "Error: new(%s) is not the same as or derived from target class '%s'\n",
                className, target->classType->name);
        exit(1);
    }

    node = createTree(0, TYPE_INT, NODE_ALLOC, NULL, target, NULL, NULL);
    node->classType = requestedClass;
    return node;
}

tnode* makeClassDeleteNode(tnode *target) {
    tnode *node;

    if (target == NULL || target->classType == NULL) {
        fprintf(stderr, "Error: delete() argument must be a class variable\n");
        exit(1);
    }

    node = createTree(0, TYPE_INT, NODE_FREE, NULL, target, NULL, NULL);
    node->classType = target->classType;
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

static int ArgumentMatchesParameter(Paramstruct *parameter, tnode *argument) {
    if (parameter == NULL || argument == NULL) return 0;

    if (parameter->classType != NULL) {
        return argument->classType == parameter->classType ||
               (argument->classType == NULL &&
                argument->typeEntry == TLookup("null"));
    }

    return argument->classType == NULL &&
           parameter->type == argument->typeEntry;
}

tnode* makeFunctionCallNode(char *name, tnode *arglist) {
    if (activeMethodClass != NULL) {
        fprintf(stderr, "Error: method '%s' cannot call a global function from class '%s'\n",
                name, activeMethodClass->name);
        exit(1);
    }
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
        if (!ArgumentMatchesParameter(formal, actual->left)) {
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

    if (activeMethodClass != NULL && local == NULL) {
        fprintf(stderr, "Error: global array '%s' is not accessible in method of class '%s'\n",
                name, activeMethodClass->name);
        exit(1);
    }
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

    if (activeMethodClass != NULL && local == NULL) {
        fprintf(stderr, "Error: global array '%s' is not accessible in method of class '%s'\n",
                name, activeMethodClass->name);
        exit(1);
    }
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

static int IsSelfNode(tnode *node) {
    return node != NULL && node->nodetype == NODE_ID &&
           node->varname != NULL && strcmp(node->varname, "self") == 0 &&
           node->Lentry != NULL && node->Lentry->classType != NULL;
}

static int IsDirectSelfField(tnode *node) {
    return node != NULL && node->nodetype == NODE_FIELD &&
           node->classOwner != NULL && node->Fentry != NULL &&
           IsSelfNode(node->left);
}

tnode* makeMemberAccessNode(tnode *base, char *memberName) {
    Fieldlist *field = NULL;
    Memberfunclist *method = NULL;
    tnode *node;

    if (base == NULL || memberName == NULL) {
        fprintf(stderr, "Error: invalid member access\n");
        exit(1);
    }

    if (base->classType != NULL) {
        field = Class_Flookup(base->classType, memberName);
        method = Class_Mlookup(base->classType, memberName);
        if (field == NULL && method == NULL) {
            fprintf(stderr, "Error: class '%s' has no field or method named '%s'\n",
                    base->classType->name, memberName);
            exit(1);
        }
        node = createTree(0, field == NULL ? -1 : TypeCode(field->type),
                          NODE_FIELD, memberName, base, NULL, NULL);
        node->classOwner = base->classType;
        node->Fentry = field;
        node->Mentry = method;
        if (field != NULL) {
            node->typeEntry = field->type;
            node->classType = field->classType;
        }
        return node;
    }

    if (base->typeEntry != NULL) {
        field = FLookup(base->typeEntry, memberName);
        if (field == NULL) {
            fprintf(stderr, "Error: type '%s' has no field named '%s'\n",
                    base->typeEntry->name, memberName);
            exit(1);
        }
        node = createTree(0, TypeCode(field->type), NODE_FIELD,
                          memberName, base, NULL, NULL);
        node->typeEntry = field->type;
        node->classType = field->classType;
        node->Fentry = field;
        return node;
    }

    fprintf(stderr, "Error: member access requires a value with fields or methods\n");
    exit(1);
}

tnode* makeFieldValueNode(tnode *memberAccess) {
    if (memberAccess == NULL || memberAccess->nodetype != NODE_FIELD ||
        memberAccess->Fentry == NULL) {
        fprintf(stderr, "Error: member name is not a field\n");
        exit(1);
    }

    if (memberAccess->classOwner != NULL &&
        (activeMethodClass != memberAccess->classOwner ||
         !IsSelfNode(memberAccess->left))) {
        fprintf(stderr, "Error: class fields are accessible only as self.field inside their class methods\n");
        exit(1);
    }
    return memberAccess;
}

tnode* makeMethodCallNode(tnode *memberAccess, tnode *arglist) {
    Memberfunclist *method;
    tnode *receiver;
    tnode *actual;
    Paramstruct *formal;
    int position = 1;
    tnode *node;
    int validReceiver = 0;

    if (memberAccess == NULL || memberAccess->nodetype != NODE_FIELD ||
        memberAccess->Mentry == NULL) {
        fprintf(stderr, "Error: member name is not a declared method\n");
        exit(1);
    }
    method = memberAccess->Mentry;
    receiver = memberAccess->left;

    if (IsSelfNode(receiver)) {
        validReceiver = activeMethodClass != NULL &&
                        receiver->classType == activeMethodClass &&
                        memberAccess->classOwner == activeMethodClass;
    } else if (IsDirectSelfField(receiver)) {
        validReceiver = activeMethodClass != NULL &&
                        receiver->left->classType == activeMethodClass;
    } else if (activeMethodClass == NULL && receiver != NULL &&
               receiver->nodetype == NODE_ID && receiver->classType != NULL &&
               (receiver->Gentry != NULL || receiver->Lentry != NULL)) {
        validReceiver = 1;
    }
    if (!validReceiver) {
        fprintf(stderr, "Error: method call is not permitted through this receiver\n");
        exit(1);
    }

    formal = method->paramlist;
    actual = arglist;
    while (formal != NULL && actual != NULL) {
        tnode *argument = actual->left;
        if (!ArgumentMatchesParameter(formal, argument)) {
            fprintf(stderr, "Error: argument %d of method '%s' has the wrong type\n",
                    position, method->name);
            exit(1);
        }
        formal = formal->next;
        actual = actual->right;
        position++;
    }
    if (formal != NULL || actual != NULL) {
        fprintf(stderr, "Error: argument count of method '%s' does not match its declaration\n",
                method->name);
        exit(1);
    }

    node = createTree(0, TypeCode(method->type), NODE_METHOD_CALL,
                      method->name, receiver, NULL, NULL);
    node->typeEntry = method->type;
    node->Mentry = method;
    node->arglist = arglist;
    return node;
}

tnode* makeFieldNode(tnode *base, char *fieldName) {
    return makeFieldValueNode(makeMemberAccessNode(base, fieldName));
}
