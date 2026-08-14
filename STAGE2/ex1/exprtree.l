%{
#include <stdlib.h>
#include <string.h>
#include "exprtree.h"
#include "exprtree.tab.h"

void yyerror(const char* s);

int number;
%}

%%

[0-9]+ {    
    yylval.num = atoi(yytext);
    return NUM;
}

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MUL; }
"/" { return DIV; }

"(" { return '('; }
")" { return ')'; }

[ \t\r\n] { } // a space before them to say space is also ignored

"begin" { return T_BEGIN; }
"end" { return T_END; }
"read" { return READ; }
"write" { return WRITE; }

"=" { return ASSIGN; }
";" { return SEMICOLON; }

[a-z] { // variables
    yylval.str = strdup(yytext);
    return ID;
}

. {
    yyerror("Unknown character");
    exit(1);
}

%%

int yywrap() {
    return 1;
}