# Stage 3 - Exercise 2

Added `repeat-until` and `do-while` statements.

## How to Run

```bash
Run the test programs:

make
./exprtree 1repeat.txt
./labelTranslator target.xsm target2.xsm
./xsm -e target2.xsm

./exprtree 2dowhile.txt
./labelTranslator target.xsm target2.xsm
./xsm -e target2.xsm

To test both together:

./exprtree 3input.txt
./labelTranslator target.xsm target2.xsm
./xsm -e target2.xsm
```

for 22forgotten.txt
input:
10
20
30
0

output
sum is:
60
good bye
Machine is halting.
