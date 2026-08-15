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

"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return MUL; }
"/"     { return DIV; }

"<="    { return LE; }
">="    { return GE; }
"=="    { return EQ; }
"!="    { return NE; }
"<"     { return LT; }
">"     { return GT; }

"("     { return '('; }
")"     { return ')'; }

"begin"     { return T_BEGIN; }
"end"       { return T_END; }

"read"      { return READ; }
"write"     { return WRITE; }

"if"        { return IF; }
"then"      { return THEN; }
"else"      { return ELSE; }
"endif"     { return ENDIF; }

"while"     { return WHILE; }
"do"        { return DO; }
"endwhile"  { return ENDWHILE; }

"break"     { return BREAK; }
"continue"  { return CONTINUE; }

"="         { return ASSIGN; }
";"         { return SEMICOLON; }

[a-z] {
    yylval.str = strdup(yytext);
    return ID;
}

[ \t\r\n] { }

. {
    yyerror("Unknown character");
    exit(1);
}

%%

int yywrap()
{
    return 1;
}