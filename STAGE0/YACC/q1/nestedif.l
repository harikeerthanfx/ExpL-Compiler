%{
#include "y.tab.h"
%}

%%

"if"        return IF;
[a-zA-Z]+   return ID;

"("         return '(';
")"         return ')';
"{"         return '{';
"}"         return '}';
";"         return ';';

[ \t\n]+    ;

.           ;

%%

int yywrap()
{
    return 1;
}