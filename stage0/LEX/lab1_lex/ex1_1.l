%{
#include <stdio.h>

int lines = 0;
int words = 0;
int chars = 0;
%}

%%

\n          { lines++; }

[ \t]+      { }

[^ \t\n]+   { words++; chars += yyleng; }

%%

int main()
{
    printf("Enter text (Press Ctrl+D when finished):\n");

    yylex();

    printf("\nLines      : %d\n", lines);
    printf("Words      : %d\n", words);
    printf("Characters : %d\n", chars);

    return 0;
}

int yywrap()
{
    return 1;
}