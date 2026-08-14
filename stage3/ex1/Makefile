all: exprtree

exprtree: exprtree.tab.c lex.yy.c exprtree.c eval.c
	gcc exprtree.tab.c lex.yy.c exprtree.c eval.c -o exprtree

exprtree.tab.c exprtree.tab.h: exprtree.y
	bison -d exprtree.y

lex.yy.c: exprtree.l exprtree.tab.h
	flex exprtree.l

clean:
	rm -f exprtree exprtree.tab.c exprtree.tab.h lex.yy.c