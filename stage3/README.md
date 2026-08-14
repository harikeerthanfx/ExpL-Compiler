### How to Run

- Generate parser: `bison -d exprtree.y`
- Generate lexer: `flex exprtree.l`
- Compile: `gcc exprtree.tab.c lex.yy.c exprtree.c codegen.c -o exprtree`
- Generate labelled code: `./exprtree input.txt`
- Compile label translator: `gcc labelTranslator.c -o labelTranslator`
- Translate labels: `./labelTranslator target.xsm target2.xsm`
- Execute final XSM code: `./xsm -e workdir/stage3/target2.xsm`

### How to Run Using Make

- Build everything: `make`
- Generate labelled code: `./exprtree input.txt`
- Translate labels to addresses: `./labelTranslator target.xsm target2.xsm`
- Execute final XSM code: `./xsm -e workdir/stage3/target2.xsm`
- Clean generated files: `make clean`
