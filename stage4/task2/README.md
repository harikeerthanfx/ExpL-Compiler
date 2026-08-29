# Compiler Lab Stage 4 – Task 2

```bash
Run the test programs:

make
./exprtree 1repeat.txt
./labelTranslator target.xsm target2.xsm
./xsm -e target2.xsm

in this specific 21input.txt
input:
3
2
1
0
output:
6
machine halting

```

## Aim

Task 1 created the **Global Symbol Table (GST)** containing:

- Variable name
- Type
- Size
- Binding address

Example:

| Name | Type | Size | Binding |
| ---- | ---- | ---: | ------: |
| num  | INT  |    1 |    4096 |
| sum  | INT  |    1 |    4097 |

Task 2 connects this GST with the **AST and code generation**.

---

## Main Idea

When a variable is encountered while constructing the AST, its corresponding GST entry is found using `Lookup()`.

The AST ID node then stores a pointer to that GST entry.

```text
Variable name
     ↓
Lookup(name)
     ↓
GST entry
     ↓
Store pointer in AST node
     ↓
node->Gentry


```
