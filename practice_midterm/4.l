%{
#include <stdio.h>
%}

%%

"//".* {
    printf("%s\n", yytext);
}

"/*"[^*]*"*/" {
    printf("%s\n", yytext);
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