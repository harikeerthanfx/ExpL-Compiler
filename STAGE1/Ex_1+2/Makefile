all: exprtree

exprtree: y.tab.c lex.yy.c exprtree.c
	gcc y.tab.c lex.yy.c exprtree.c -o exprtree -lfl

y.tab.c y.tab.h: exprtree.y
	yacc -d exprtree.y

lex.yy.c: exprtree.l y.tab.h
	flex exprtree.l

clean:
	rm -f exprtree y.tab.c y.tab.h lex.yy.c target_file.xsm