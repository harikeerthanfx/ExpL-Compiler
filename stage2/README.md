# Stage 2: Expression Trees, Interpretation, and Code Generation

This stage is easiest to understand as one compiler front end with two different
back ends:

```text
source program
    ↓
Flex scanner (exprtree.l): characters → tokens
    ↓
Bison parser (exprtree.y): tokens → syntax tree (AST)
    ↓
                    ┌─ ex1: evaluate the tree in C
                    └─ task: generate XSM assembly from the tree
```

The shared idea is the AST. The scanner and parser describe the language; the
last step decides what the AST *does*.

## 1. Read these files in this order

| File | What to look for |
|---|---|
| [`ex1/exprtree.h`](./ex1/exprtree.h) | Node kinds and the `tnode` data structure |
| [`ex1/exprtree.c`](./ex1/exprtree.c) | Constructors that build each kind of AST node |
| [`ex1/exprtree.l`](./ex1/exprtree.l) | How source text becomes tokens |
| [`ex1/exprtree.y`](./ex1/exprtree.y) | Grammar rules and AST construction actions |
| [`ex1/eval.c`](./ex1/eval.c) | How the interpreter executes AST nodes |
| [`task/codegen.c`](./task/codegen.c) | How the code generator emits XSM instructions |
| [`task/exprtree.y`](./task/exprtree.y) | Same parser idea, but calls code generation instead of evaluation |
| [`ex1/Makefile`](./ex1/Makefile) / [`task/Makefile`](./task/Makefile) | How Flex, Bison, and GCC are combined |

`ex1` is the interpreter version. `task` keeps the same language and AST
construction, but emits assembly into `target.xsm`.

## 2. The AST: the central data structure

In `tnode`, the fields have these jobs:

- `nodetype`: what this node represents (`NODE_NUM`, `NODE_ID`, `NODE_PLUS`,
  `NODE_ASSIGN`, `NODE_READ`, etc.).
- `val`: integer value for a number node.
- `varname`: variable name for an identifier node.
- `left`, `right`: child nodes. Leaves such as numbers and identifiers have no
  children.
- `type`: the value type. Stage 2 currently uses only `TYPE_INT`.

The `make...Node` functions in `exprtree.c` hide the details of filling these
fields. For example:

```text
makeOperatorNode('+', left, right) → NODE_PLUS(left, right)
makeAssignNode(id, expr)          → NODE_ASSIGN(id, expr)
makeReadNode(id)                  → NODE_READ(id, NULL)
makeWriteNode(expr)               → NODE_WRITE(expr, NULL)
makeConnectorNode(a, b)           → NODE_CONNECTOR(a, b)
```

A connector joins statements in source order. For example, two statements
become `CONNECTOR(statement1, statement2)`. An expression such as `a - 3`
becomes `MINUS(ID(a), NUM(3))`.

## 3. Scanner syntax: `exprtree.l`

Flex checks the source text and returns token kinds to Bison:

- `[0-9]+`: converts the matched text to an integer, stores it in `yylval.num`,
  and returns `NUM`.
- `[a-z]`: duplicates the one-character variable name into `yylval.str` and
  returns `ID`. The current language therefore accepts single lowercase
  variables, such as `a` or `x`.
- Operators and punctuation return tokens such as `PLUS`, `ASSIGN`,
  `SEMICOLON`, or the literal `'('` and `')'`.
- `begin`, `end`, `read`, and `write` are reserved-word tokens.
- Whitespace is ignored; any other character reports an error and exits.

Remember the distinction: the scanner recognizes *tokens*; it does not decide
whether a sequence of tokens is a valid program.

## 4. Grammar and Bison actions: `exprtree.y`

The grammar's shape is:

```text
Program    → begin Slist end | begin end
Slist      → Slist Stmt | Stmt
Stmt       → InputStmt | OutputStmt | AsgStmt
InputStmt  → read ( ID ) ;
OutputStmt → write ( E ) ;
AsgStmt    → ID = E ;
E          → E + E | E - E | E * E | E / E
           | ( E ) | NUM | ID
```

The punctuation above is just a readable rendering; the Bison file uses token
names for most tokens (`T_BEGIN`, `READ`, `SEMICOLON`, and so on).

Important Bison syntax:

- `%token <num> NUM` and `%token <str> ID` say which `yylval` field carries
  each token's value.
- `%type <node> ...` says those grammar symbols carry `tnode *` values.
- In an action, `$1`, `$2`, etc. are the values of symbols on the right-hand
  side; `$$` is the value produced by the rule.
- Example: `E : E PLUS E { $$ = makeOperatorNode('+', $1, $3); }`
  builds a plus node from the left and right expression trees.
- Precedence declarations make `*` and `/` bind more tightly than `+` and `-`;
  both groups are left-associative. Parentheses can explicitly group an
  expression.
- The `Slist` action uses `makeConnectorNode($1, $2)` to preserve statement
  order in the AST.

In this grammar, `Program` stores the completed tree in the global `root`.
`main` calls `yyparse()` before passing `root` to the interpreter or code
generator.

## 5. The two back ends

### `ex1`: interpret the AST

`evaluate(t)` in `ex1/eval.c` recursively returns an integer for expressions:

- A number returns `val`; an identifier reads its slot from `memory[26]`.
- An arithmetic node evaluates both children and applies its operator.
- Assignment evaluates the right side, stores the result in the variable's
  slot, and returns the value.
- `READ` fills the variable slot using `scanf`; `WRITE` evaluates and prints
  its child.
- `CONNECTOR` evaluates the left statement, then the right statement.

Variable `a` maps to array index `0`, `b` to `1`, and so forth, using
`varname[0] - 'a'`.

### `task`: generate XSM assembly

`codeGen(t)` in `task/codegen.c` recursively emits instructions to the global
`targetFile`:

- A number gets a register and emits `MOV Rn, value`.
- An identifier gets a register and loads from its memory address:
  `4096 + (letter - 'a')`.
- An arithmetic node generates its left and right children, emits `ADD`, `SUB`,
  `MUL`, or `DIV`, and keeps the result in the left register. It then frees the
  right register.
- Assignment generates the expression and stores its result at the
  destination variable's memory address.
- `READ` and `WRITE` emit the XSM system-call setup (`PUSH` arguments, `CALL 0`,
  then stack cleanup).
- A connector generates the left statement and then the right.

`getReg()` allocates registers from `R0` upward; `freeReg()` releases the most
recent register. The recursion is what makes nested expressions work. For
`a - b`, code generation first loads `a`, then `b`, then subtracts the second
register from the first.

The `task/exprtree.y` `main` writes the XSM header and stack setup, parses the
input, calls `codeGen(root)`, then writes the exit system-call sequence. The
output is assembly in `target.xsm`; generating it is not the same as executing
it.

## 6. Walk through the sample

Both [`ex1/input.txt`](./ex1/input.txt) and [`task/input.txt`](./task/input.txt)
contain:

```text
begin
read(a);
read(b);
write(a - b);
end
```

Conceptually, the AST is:

```text
CONNECTOR
├── CONNECTOR
│   ├── READ ── ID(a)
│   └── READ ── ID(b)
└── WRITE
    └── MINUS
        ├── ID(a)
        └── ID(b)
```

Follow it end to end:

1. Flex turns `read`, `(`, `a`, `)`, `;` into tokens.
2. Bison matches `InputStmt` and builds `READ(ID(a))`.
3. The `Slist` rule connects that statement to the next one, and then to
   `WRITE(MINUS(ID(a), ID(b)))`.
4. `ex1` would execute those nodes using C memory and standard input/output.
   `task` instead emits assembly for the same operations.

## 7. Build and run

Run these commands from the relevant folder:

```sh
cd stage2/ex1
make
./exprtree input.txt
```

The interpreter reads the *program* from `input.txt`; its `read(a)` and
`read(b)` calls then request integer data from standard input.

For code generation:

```sh
cd stage2/task
make
./exprtree input.txt
```

This writes `target.xsm` in `task/`. The Makefiles run Bison (`bison -d`), Flex,
and GCC to build the executable.

## 8. Exam recall checklist

Be ready to explain these without looking:

1. **What does Flex do?** Text to tokens.
2. **What does Bison do?** Tokens to grammar structure and AST, using actions.
3. **Why build an AST?** It represents meaning/structure without keeping
   irrelevant punctuation; later passes can traverse it.
4. **What does `tnode` store?** Node kind, literal/name data, type, and child
   pointers.
5. **How are statements sequenced?** A left-to-right chain of connector nodes.
6. **How does precedence work?** Bison declarations: `*`/`/` higher than `+`/`-`,
   with left associativity.
7. **What differs between `ex1` and `task`?** Their AST consumers:
   `evaluate()` executes in C; `codeGen()` emits XSM.
8. **How are variables addressed?** `a` is base + 0, `b` is base + 1, etc.;
   the interpreter uses array index `letter - 'a'`, while codegen uses address
   `4096 + letter - 'a'`.
9. **How is an arithmetic expression generated?** Recursively generate both
   children, emit the operation using their registers, release the right
   register, return the left register.

If you remember only one architecture diagram, remember:

```text
exprtree.l → tokens → exprtree.y → AST → eval.c OR codegen.c
```
