%{
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>
%}

%%

[a-zA-Z][a-zA-Z0-9]*    {
                            yylval.c = strdup(yytext);
                            return ID;
                        }

"+"     return '+';
"*"     return '*';
"("     return '(';
")"     return ')';

\n      return '\n';

[ \t]   ;

.       ;

%%

int yywrap()
{
    return 1;
}