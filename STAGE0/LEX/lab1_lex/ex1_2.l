%{
    #include<stdio.h>

    int integers = 0;
    int floats = 0;
%}

%%

[0-9]+\.[0-9]+      { floats++; }
[0-9]+              { integers++; }

%%

int main()
{
    printf("Enter input (Ctrl+D to finish):\n");

    yylex();

    printf("Integers : %d\n", integers);
    printf("Floats   : %d\n", floats);

    return 0;
}

int yywrap()
{
    return 1;
}