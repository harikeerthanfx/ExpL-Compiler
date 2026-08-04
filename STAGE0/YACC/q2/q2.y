%{
#include <stdio.h>

int yylex();
void yyerror(const char *s);
%}

%token LETTER DIGIT

%%

variable
    : LETTER rest
      {
          printf("Valid Variable\n");
      }
    ;

rest
    : LETTER rest
    | DIGIT rest
    |
    ;

%%

int main()
{
    yyparse();
    return 0;
}

void yyerror(const char *s)
{
    printf("Invalid Variable\n");
}