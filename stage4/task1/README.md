TASK 1 — HOW TO CHECK

(to check the flags first do make and give them the inouts i have saved using thosenames.txt
and you can see the flags in the terminal)

1. Put this in input.txt:

decl
int num, sum;
str mesg;
enddecl

begin
end;

2. Compile:

make

3. Run:

./exprtree input.txt

4. Expected output:

Name Type Size Binding
num INT 1 4096
sum INT 1 4097
mesg STR 1 4098

5. If this output is correct → Task 1 completed.

---

TASK 1 — GLOBAL SYMBOL TABLE

WHAT WAS THE PROBLEM BEFORE TASK 1?

Previously, our compiler could parse and use variables like:

    a = 10;
    b = a + 5;

But the compiler had no proper record of the variables.

It did not know:

- Which variables are declared
- What type each variable has
- How much memory each variable needs
- Where each variable is stored in memory

---

WHAT DID WE ADD?

We added a Global Symbol Table.

The symbol table stores information about every declared variable.

Example:

    Name    Type    Size    Binding
    num     INT     1       4096
    sum     INT     1       4097
    mesg    STR     1       4098

For each variable, we store:

- Name → Variable name
- Type → INT or STR
- Size → Memory locations needed
- Binding → Starting memory address

---

WHY DO WE NEED DECLARATIONS?

The variables are declared before the main program:

    decl
        int num, sum;
        str mesg;
    enddecl

This tells the compiler which variables exist and their types.

---

WHAT HAPPENS?

Input:

    int num, sum;

Flow:

    Lexer
      ↓
    Recognizes: int, num, comma, sum
      ↓
    YACC Parser
      ↓
    Matches:

        Type VarList SEMICOLON

      ↓
    Gets the type and variable names
      ↓
    Calls Install()
      ↓
    Variables are added to the Global Symbol Table

---

WHAT DOES LOOKUP() DO?

Lookup() searches the symbol table to check whether a variable already exists.

Example:

    Lookup("num")

If "num" already exists, the compiler should not add it again.

This prevents duplicate declarations like:

    decl
        int num;
        int num;
    enddecl

---

WHAT DOES INSTALL() DO?

Install() creates a new entry in the symbol table.

It:

1. Creates a new symbol table entry
2. Stores the variable name
3. Stores the type
4. Stores the size
5. Assigns a memory binding
6. Adds the entry to the symbol table

---

HOW ARE MEMORY BINDINGS GIVEN?

We start with:

    nextBinding = 4096

When a variable is added:

    num  → 4096
    sum  → 4097
    mesg → 4098

After assigning an address, nextBinding is increased by the variable size.

---

FINAL IDEA

Before Task 1:

    num

was just a name in the program.

After Task 1:

    num
      ↓
    Global Symbol Table
      ↓
    Type: INT
    Binding: 4096

Now the compiler knows what the variable is and where it is stored in memory.

ONE-LINE SUMMARY:

Task 1 creates a Global Symbol Table to store information about every declared variable and assign each variable a memory location.
