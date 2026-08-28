all: exprtree labelTranslator

exprtree: exprtree.tab.c lex.yy.c exprtree.c codegen.c
	gcc exprtree.tab.c lex.yy.c exprtree.c codegen.c -o exprtree

labelTranslator: labelTranslator.c
	gcc labelTranslator.c -o labelTranslator

exprtree.tab.c exprtree.tab.h: exprtree.y
	bison -d exprtree.y

lex.yy.c: exprtree.l exprtree.tab.h
	flex exprtree.l

clean:
	rm -f exprtree exprtree.tab.c exprtree.tab.h lex.yy.c labelTranslator target.xsm target2.xsm