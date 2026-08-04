%{
#include <stdio.h>
%}

%x COMMENT

%%

"//".*              { printf("%s\n", yytext); }

"/*"                {
                        printf("%s", yytext);
                        BEGIN(COMMENT);
                    }

<COMMENT>"*/"       {
                        printf("%s\n", yytext);
                        BEGIN(INITIAL);
                    }

<COMMENT>.|\n       {
                        printf("%s", yytext);
                    }

.|\n                { }

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