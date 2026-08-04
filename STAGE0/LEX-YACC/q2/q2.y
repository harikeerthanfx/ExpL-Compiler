%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);
%}

%union
{
    char *str;
}

%token <str> ID

%left '+'
%left '*'

%%

start
    : expr '\n'
    ;

expr
    : expr '+' expr
      {
          printf("+ ");
      }
    | expr '*' expr
      {
          printf("* ");
      }
    | '(' expr ')'
    | ID
      {
          printf("%s ", $1);
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
    printf("Invalid Expression\n");
}