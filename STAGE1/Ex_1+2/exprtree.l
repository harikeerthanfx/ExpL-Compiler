%{
#include <stdlib.h>
#include "exprtree.h"
#include "exprtree.tab.h"

void yyerror(const char* s);

int number;
%}

%%

[0-9]+ {    
    number = atoi(yytext);
    yylval.node = makeLeafNode(number);
    return NUM;
}

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MUL; }
"/" { return DIV; }

"(" { return '('; }
")" { return ')'; }

[ \t] { } // for space or tab both we add a trailing space upfront
[\r] { }
\n { return END; }

. {
    yyerror("Unknown character");
    exit(1);
}

%%

int yywrap() {
    return 1;
}