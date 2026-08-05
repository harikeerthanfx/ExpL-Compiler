%{
    #include <stdlib.h>
    #include <stdio.h>
    #include "exprtree.h"

    int yylex(void);
    int yyerror(const char *s);
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

    codeGen($1);

    fprintf(targetFile,"MOV SP, 4095\n");

    fprintf(targetFile,"MOV R2, 5\n");
    fprintf(targetFile,"PUSH R2\n");

    fprintf(targetFile,"MOV R2, -2\n");
    fprintf(targetFile,"PUSH R2\n");

    fprintf(targetFile,"PUSH R0\n");

    fprintf(targetFile,"PUSH R2\n");
    fprintf(targetFile,"PUSH R2\n");

    fprintf(targetFile,"INT 7\n");

    fprintf(targetFile,"POP R0\n");
    fprintf(targetFile,"POP R1\n");
    fprintf(targetFile,"POP R1\n");
    fprintf(targetFile,"POP R1\n");
    fprintf(targetFile,"POP R1\n");
    fprintf(targetFile,"INT 10\n");

    exit(1);
}

expr : PLUS expr  expr   {$$ = makeOperatorNode('+',$2,$3);}
     | MINUS expr  expr  {$$ = makeOperatorNode('-',$2,$3);}
     | MUL expr  expr    {$$ = makeOperatorNode('*',$2,$3);}
     | DIV expr expr    {$$ = makeOperatorNode('/',$2,$3);}
     | '(' expr ')'     {$$ = $2;}
     | NUM              {$$ = $1;}
     ;

%%

int yyerror(char const *s)
{
    printf("yyerror %s",s);
    return 0;
}

int main(void)
{
    targetFile = fopen("target_file.xsm","w");

    fprintf(targetFile,"0\n");
    fprintf(targetFile,"2056\n");
    fprintf(targetFile,"0\n");
    fprintf(targetFile,"0\n");
    fprintf(targetFile,"0\n");
    fprintf(targetFile,"0\n");
    fprintf(targetFile,"0\n");
    fprintf(targetFile,"0\n");

    yyparse();

    fclose(targetFile);

    return 0;
}