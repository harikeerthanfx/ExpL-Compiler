/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_EXPRTREE_TAB_H_INCLUDED
# define YY_YY_EXPRTREE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUM = 258,                     /* NUM  */
    ID = 259,                      /* ID  */
    STRING = 260,                  /* STRING  */
    PLUS = 261,                    /* PLUS  */
    MINUS = 262,                   /* MINUS  */
    MUL = 263,                     /* MUL  */
    DIV = 264,                     /* DIV  */
    MOD = 265,                     /* MOD  */
    ADDRESS = 266,                 /* ADDRESS  */
    LT = 267,                      /* LT  */
    GT = 268,                      /* GT  */
    LE = 269,                      /* LE  */
    GE = 270,                      /* GE  */
    EQ = 271,                      /* EQ  */
    NE = 272,                      /* NE  */
    AND = 273,                     /* AND  */
    OR = 274,                      /* OR  */
    NOT = 275,                     /* NOT  */
    T_BEGIN = 276,                 /* T_BEGIN  */
    T_END = 277,                   /* T_END  */
    DECL = 278,                    /* DECL  */
    ENDDECL = 279,                 /* ENDDECL  */
    INT = 280,                     /* INT  */
    STR = 281,                     /* STR  */
    MAIN = 282,                    /* MAIN  */
    TUPLE = 283,                   /* TUPLE  */
    DOT = 284,                     /* DOT  */
    ASSIGN = 285,                  /* ASSIGN  */
    SEMICOLON = 286,               /* SEMICOLON  */
    READ = 287,                    /* READ  */
    WRITE = 288,                   /* WRITE  */
    IF = 289,                      /* IF  */
    THEN = 290,                    /* THEN  */
    ELSE = 291,                    /* ELSE  */
    ENDIF = 292,                   /* ENDIF  */
    WHILE = 293,                   /* WHILE  */
    DO = 294,                      /* DO  */
    ENDWHILE = 295,                /* ENDWHILE  */
    REPEAT = 296,                  /* REPEAT  */
    UNTIL = 297,                   /* UNTIL  */
    DOWHILE = 298,                 /* DOWHILE  */
    BREAK = 299,                   /* BREAK  */
    CONTINUE = 300,                /* CONTINUE  */
    RETURN = 301                   /* RETURN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "exprtree.y"

    tnode* node;
    char* str;
    int num;
    struct VarList *varlist;   // TASK1: stores the variable list — because VarList needs to pass the list to Bison
    struct GDeclItem *gdecllist;
    struct Paramstruct *paramlist;
    struct ArgList *arglist;
    struct Field *fieldlist;
    struct TupleVar *tuplevarlist;

#line 122 "exprtree.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_EXPRTREE_TAB_H_INCLUDED  */
