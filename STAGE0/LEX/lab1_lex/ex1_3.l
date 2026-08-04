%{
#include<stdio.h>

%}

%%

A[a-zA-Z]+    { if (yyleng == 3) printf("%s\n",yytext);}

.|\n              { }

%%

int main()
{
    printf("Enter the words:\n");

    yylex();

    return 0;
}

int yywrap(){
    return 1;
}