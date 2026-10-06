%{

#include<stdio.h>

int count_int = 0;
int count_float = 0;

%}

%%

[0-9]+\.[0-9]+ {
    count_float += 1;
}

[0-9]+ {
    count_int += 1;
}

[ \t\n]+ {

}

. {

}

%%

int main(){
    yylex();

    printf("\nintegers = %d\n", count_int);
    printf("Floats = %d\n", count_float);

    return 0;
    
}

int yywrap(){
    return 1;
}