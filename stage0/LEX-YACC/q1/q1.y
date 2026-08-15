%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);
%}

%union
{
    char c;
}

%token <c> ID
%left '+'
%left '*'

%%

start
    : expr '\n'
    ;

expr
    : expr '+' expr
      {
          printf("+");
      }
    | expr '*' expr
      {
          printf("*");
      }
    | '(' expr ')'
    | ID
      {
          printf("%c", $1);
      }
    ;

%%

int main()
{
    printf("Enter Expression: ");
    yyparse();
    return 0;
}

void yyerror(const char *s)
{
    printf("\nInvalid Expression\n");
}