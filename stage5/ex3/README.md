# Exercise 3: tuple function arguments

Tuple types must be declared before they appear in a function signature. Use
`tuple TypeName parameter` for a by-value tuple parameter and
`tuple TypeName *parameter` for a tuple pointer parameter.

Tuple parameters are passed by value: the callee gets its own copy. Tuple
pointer parameters pass one address, so field assignments through that
parameter change the original tuple. Functions return the existing scalar
types; tuple return values are not part of this implementation.

Build and generate XSM code from this directory:

```sh
make
./exprtree ex3_tuple_args.txt
./labelTranslator target.xsm target2.xsm
```

The example uses hard-coded values, so it needs no runtime input. Its program
output is:

```text
7
12
```
