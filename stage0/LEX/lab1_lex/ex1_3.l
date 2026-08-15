%{
    #include<stdio.h>

    int len = 0;
%}

%%

[0-9]+      {len ++; }

.|\n              { }

%%

int main()
{
    printf("Enter the number:\n");
    yylex();
    printf("length = %d",len);

    return 0;
}

int yywrap(){
    return 1;
}