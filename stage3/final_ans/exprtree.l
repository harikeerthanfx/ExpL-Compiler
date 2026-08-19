%{
#include <stdlib.h>
#include <string.h>
#include "exprtree.h"
#include "exprtree.tab.h"

void yyerror(const char* s);
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
 
"<=" { return LE; } // flex dont rly care about ordering, it checks for longest match first
"<" { return LT; } // if length same only it checks for order and returns whatever matched before
">=" { return GE; }
">" { return GT; }
"==" { return EQ; }
"!=" { return NE; }

[ \t\r\n] { } // a space before them to say space is also ignored

"begin" { return T_BEGIN; }
"end" { return T_END; }
"read" { return READ; }
"write" { return WRITE; }

"if" { return IF; }
"then" { return THEN; }
"else" { return ELSE; }
"endif" { return ENDIF; }

"while" { return WHILE; }
"do" { return DO; }
"endwhile" { return ENDWHILE; }

"break" { return BREAK; }
"continue" { return CONTINUE; }

"repeat"    { return REPEAT; }
"until"     { return UNTIL; }
"dowhile"   { return DOWHILE; }

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