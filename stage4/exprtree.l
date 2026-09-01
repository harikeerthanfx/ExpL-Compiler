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

"&" { return ADDRESS; } // EX2: recognizes the address-of operator

"(" { return '('; }
")" { return ')'; }

"[" { return '['; } //TASK3 for arrays
"]" { return ']'; }

"," { return ','; }
 
"<=" { return LE; } // flex dont rly care about ordering, it checks for longest match first
"<" { return LT; } // if length same only it checks for order and returns whatever matched before
">=" { return GE; }
">" { return GT; }
"==" { return EQ; }
"!=" { return NE; }

[ \t\r\n] { } // a space before them to say space is also ignored

"begin" { return T_BEGIN; }
"end" { return T_END; }

"decl"       { return DECL; }
"enddecl"    { return ENDDECL; }
"int"        { return INT; }
"str"        { return STR; }


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

\"[^\"]*\" { //TASK1 forgotten crossroads(iykyk)
    yytext[yyleng - 1] = '\0';
    yylval.str = strdup(yytext + 1);
    return STRING;
}

[a-zA-Z][a-zA-Z0-9]* {
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