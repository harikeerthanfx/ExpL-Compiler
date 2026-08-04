%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();
void yyerror(const char *s);

char *makePrefix(char *op, char *left, char *right)
{
    char *res = (char *)malloc(strlen(op) + strlen(left) + strlen(right) + 4);
    sprintf(res, "%s %s %s", op, left, right);
    return res;
}
%}

%union
{
    char *c;
}

%token <c> ID

%type <c> expr

%left '+'
%left '*'

%%

start
    : expr '\n'
      {
          printf("%s\n", $1);
      }
    ;

expr
    : expr '+' expr
      {
          $$ = makePrefix("+", $1, $3);
      }
    | expr '*' expr
      {
          $$ = makePrefix("*", $1, $3);
      }
    | '(' expr ')'
      {
          $$ = $2;
      }
    | ID
      {
          $$ = $1;
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