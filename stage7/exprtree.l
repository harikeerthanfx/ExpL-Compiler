%{
#include <stdlib.h>
#include <string.h>
#include "exprtree.h"
#include "exprtree.tab.h"

void yyerror(const char* s);
/* //stage6: Lexer recognizes type-definition, NULL, and heap-operation keywords below. */
%}

/* //stage7: Track source lines so syntax failures in supplied examples can be located precisely. */
%option yylineno

%%

[0-9]+ {    
    yylval.num = atoi(yytext);
    return NUM;
}

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MUL; }
"/" { return DIV; }
"%" {return MOD; }

"&" { return ADDRESS; }

"[" { return '['; }
"]" { return ']'; }

"{" { return '{'; }
"}" { return '}'; }

"(" { return '('; }
")" { return ')'; }
"," { return ','; }
"." { return '.'; }
 
"<=" { return LE; } // flex dont rly care about ordering, it checks for longest match first
"<" { return LT; } // if length same only it checks for order and returns whatever matched before
">=" { return GE; }
">" { return GT; }
"==" { return EQ; }
"!=" { return NE; }
"||" { return OR; }
"&&" { return AND; }

[ \t\r\n] { } // a space before them to say space is also ignored

"begin" { return T_BEGIN; }
"end" { return T_END; }
"main" { return MAIN; }
"return" { return RETURN; }

"decl" { return DECL; }
"enddecl" { return ENDDECL; }
"int" { return INT; }
"str" { return STR; }
"type" { return TYPE; }
"endtype" { return ENDTYPE; }
"class" { return CLASS; } //stage7: "class begins the OExpL class-definition block."
"endclass" { return ENDCLASS; } //stage7: "endclass closes the class-definition block."
"self" { return SELF; } //stage7: "self is reserved so method bodies can refer to their implicit receiver."
"new" { return NEW; } //stage7: "new is reserved for class-instance creation expressions."
"delete" { return DELETE; } //stage7: "delete is reserved for class-instance release statements."
"NULL" { return T_NULL; }
"null" { return T_NULL; } //stage7: "Accept the lowercase null spelling used by the provided Stage 7 example programs."

"read" { return READ; }
"write" { return WRITE; }
"alloc" { return ALLOC; }
"free" { return FREE; }

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

"=" { return ASSIGN; }
";" { return SEMICOLON; }

\"[^\"]*\" {
    yytext[yyleng - 1] = '\0';
    yylval.str = strdup(yytext + 1);
    return STRING;
}

[a-zA-Z][a-zA-Z0-9_]* { //stage7: "Allow underscores in OExpL identifiers, as used by the BST example methods."
    yylval.str = strdup(yytext); // makes a copy of any string. (in the heap;)
    // strdup only allocs as much memory u need, wont reserve kore bytes that go to waste
    // ethra bytes aano size, athrakke allocate cheyyu [malloc(sizeof(string))]
    return ID;
}

. {
    printf("Unknown character: '%c' (ASCII %d)\n", yytext[0], yytext[0]);
    yyerror("Unknown character");
    exit(1);
}

%%

int yywrap() {
    return 1;
}
