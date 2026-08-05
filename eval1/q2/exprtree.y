%{
    #include <stdlib.h>
    #include <stdio.h>
    #include "exprtree.h"

    int yylex(void);
    int yyerror(const char *s);

    extern FILE *yyin;
%}

%union{
    tnode *no;
}

%token <no> NUM
%token PLUS MINUS MUL DIV END

%type <no> expr program

%left PLUS MINUS
%left MUL DIV

%%

program : expr END
{
    $$ = $1;

    printf("Prefix  : ");
    prefix($1);
    printf("\n");

    printf("Postfix : ");
    postfix($1);
    printf("\n");

    //printf("Answer  : %d\n", evaluate($1));

    exit(0);
}
;

expr : expr PLUS expr   { $$ = makeOperatorNode('+', $1, $3); }
     | expr MINUS expr  { $$ = makeOperatorNode('-', $1, $3); }
     | expr MUL expr    { $$ = makeOperatorNode('*', $1, $3); }
     | expr DIV expr    { $$ = makeOperatorNode('/', $1, $3); }
     | '(' expr ')'     { $$ = $2; }
     | NUM              { $$ = $1; }
     ;

%%

int yyerror(char const *s)
{
    printf("yyerror %s\n", s);
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        yyin = fopen(argv[1], "r");

        if(yyin == NULL)
        {
            printf("Cannot open input file\n");
            return 1;
        }
    }

    yyparse();

    return 0;
}