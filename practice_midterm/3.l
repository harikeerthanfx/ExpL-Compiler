%{
#include <stdio.h>
%}

%%

[Aa][A-Za-z]{2}([^A-Za-z]) {
    yytext[0] = 'A';
    yytext[3] = '\0';
    printf("%s\n", yytext);
}

[A-Za-z]+ {
}

[ \t\n]+ {
}

. {
}

%%

int main()
{
    yylex();
    return 0;
}

int yywrap()
{
    return 1;
}