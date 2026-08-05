%{
    #include<stdio.h>
%}

%%

[0-9]+              { printf("length = %d",yyleng); }

%%

int main()
{
    printf("Enter input (Ctrl+D to finish):\n");

    yylex();

    return 0;
}

int yywrap()
{
    return 1;
}