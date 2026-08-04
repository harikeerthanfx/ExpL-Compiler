%{
#include <stdio.h>

int level = 0;

int yylex();
void yyerror(const char *s);
%}

%token IF ID

%%

stmt
    : IF '(' ID ')' 
      {
          level++;
          printf("IF at level %d\n", level);
      }
      '{' stmt_list '}'
      {
          level--;
      }
    | ID ';'
    ;

stmt_list
    : stmt_list stmt
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
    printf("Syntax Error\n");
}