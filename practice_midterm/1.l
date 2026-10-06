%{

#include<stdio.h>

int lines = 0;
int words = 0;
int chars = 0;
%}

%%

\n {
    lines++;
    chars++;
}

[ \t]+ {
    chars += yyleng;
}

[^ \t\n]+ {
    words++;
    chars += yyleng;
}

%%

int main(){
    yylex();

    printf("\nLines: %d\n", lines);
    printf("words: %d\n", words);
    printf("characters: %d\n", chars);

    return 0;
}

int yywrap()
{
    return 1;
}