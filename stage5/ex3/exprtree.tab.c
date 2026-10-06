/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "exprtree.y"

#include <stdio.h>
#include <stdlib.h>
#include "exprtree.h"
#include "codegen.h"

int yylex();
void yyerror(const char* s);

tnode* root;

FILE* targetFile;
extern int nextBinding;

#line 86 "exprtree.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "exprtree.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUM = 3,                        /* NUM  */
  YYSYMBOL_ID = 4,                         /* ID  */
  YYSYMBOL_STRING = 5,                     /* STRING  */
  YYSYMBOL_PLUS = 6,                       /* PLUS  */
  YYSYMBOL_MINUS = 7,                      /* MINUS  */
  YYSYMBOL_MUL = 8,                        /* MUL  */
  YYSYMBOL_DIV = 9,                        /* DIV  */
  YYSYMBOL_MOD = 10,                       /* MOD  */
  YYSYMBOL_ADDRESS = 11,                   /* ADDRESS  */
  YYSYMBOL_LT = 12,                        /* LT  */
  YYSYMBOL_GT = 13,                        /* GT  */
  YYSYMBOL_LE = 14,                        /* LE  */
  YYSYMBOL_GE = 15,                        /* GE  */
  YYSYMBOL_EQ = 16,                        /* EQ  */
  YYSYMBOL_NE = 17,                        /* NE  */
  YYSYMBOL_AND = 18,                       /* AND  */
  YYSYMBOL_OR = 19,                        /* OR  */
  YYSYMBOL_NOT = 20,                       /* NOT  */
  YYSYMBOL_T_BEGIN = 21,                   /* T_BEGIN  */
  YYSYMBOL_T_END = 22,                     /* T_END  */
  YYSYMBOL_DECL = 23,                      /* DECL  */
  YYSYMBOL_ENDDECL = 24,                   /* ENDDECL  */
  YYSYMBOL_INT = 25,                       /* INT  */
  YYSYMBOL_STR = 26,                       /* STR  */
  YYSYMBOL_MAIN = 27,                      /* MAIN  */
  YYSYMBOL_TUPLE = 28,                     /* TUPLE  */
  YYSYMBOL_DOT = 29,                       /* DOT  */
  YYSYMBOL_ASSIGN = 30,                    /* ASSIGN  */
  YYSYMBOL_SEMICOLON = 31,                 /* SEMICOLON  */
  YYSYMBOL_READ = 32,                      /* READ  */
  YYSYMBOL_WRITE = 33,                     /* WRITE  */
  YYSYMBOL_IF = 34,                        /* IF  */
  YYSYMBOL_THEN = 35,                      /* THEN  */
  YYSYMBOL_ELSE = 36,                      /* ELSE  */
  YYSYMBOL_ENDIF = 37,                     /* ENDIF  */
  YYSYMBOL_WHILE = 38,                     /* WHILE  */
  YYSYMBOL_DO = 39,                        /* DO  */
  YYSYMBOL_ENDWHILE = 40,                  /* ENDWHILE  */
  YYSYMBOL_REPEAT = 41,                    /* REPEAT  */
  YYSYMBOL_UNTIL = 42,                     /* UNTIL  */
  YYSYMBOL_DOWHILE = 43,                   /* DOWHILE  */
  YYSYMBOL_BREAK = 44,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 45,                  /* CONTINUE  */
  YYSYMBOL_RETURN = 46,                    /* RETURN  */
  YYSYMBOL_47_ = 47,                       /* '('  */
  YYSYMBOL_48_ = 48,                       /* ')'  */
  YYSYMBOL_49_ = 49,                       /* ','  */
  YYSYMBOL_50_ = 50,                       /* '['  */
  YYSYMBOL_51_ = 51,                       /* ']'  */
  YYSYMBOL_52_ = 52,                       /* '{'  */
  YYSYMBOL_53_ = 53,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 54,                  /* $accept  */
  YYSYMBOL_Declarations = 55,              /* Declarations  */
  YYSYMBOL_DeclList = 56,                  /* DeclList  */
  YYSYMBOL_Decl = 57,                      /* Decl  */
  YYSYMBOL_FieldList = 58,                 /* FieldList  */
  YYSYMBOL_Field = 59,                     /* Field  */
  YYSYMBOL_TupleVarList = 60,              /* TupleVarList  */
  YYSYMBOL_TupleVar = 61,                  /* TupleVar  */
  YYSYMBOL_GidList = 62,                   /* GidList  */
  YYSYMBOL_Gid = 63,                       /* Gid  */
  YYSYMBOL_ParamListOpt = 64,              /* ParamListOpt  */
  YYSYMBOL_ParamList = 65,                 /* ParamList  */
  YYSYMBOL_Param = 66,                     /* Param  */
  YYSYMBOL_Type = 67,                      /* Type  */
  YYSYMBOL_ReturnType = 68,                /* ReturnType  */
  YYSYMBOL_Program = 69,                   /* Program  */
  YYSYMBOL_FdefBlock = 70,                 /* FdefBlock  */
  YYSYMBOL_Fdef = 71,                      /* Fdef  */
  YYSYMBOL_72_1 = 72,                      /* $@1  */
  YYSYMBOL_MainBlock = 73,                 /* MainBlock  */
  YYSYMBOL_74_2 = 74,                      /* $@2  */
  YYSYMBOL_FunctionBody = 75,              /* FunctionBody  */
  YYSYMBOL_LocalDeclBlock = 76,            /* LocalDeclBlock  */
  YYSYMBOL_LocalDeclBlockOpt = 77,         /* LocalDeclBlockOpt  */
  YYSYMBOL_LocalDeclList = 78,             /* LocalDeclList  */
  YYSYMBOL_LocalDecl = 79,                 /* LocalDecl  */
  YYSYMBOL_Slist = 80,                     /* Slist  */
  YYSYMBOL_Stmt = 81,                      /* Stmt  */
  YYSYMBOL_Variable = 82,                  /* Variable  */
  YYSYMBOL_InputStmt = 83,                 /* InputStmt  */
  YYSYMBOL_OutputStmt = 84,                /* OutputStmt  */
  YYSYMBOL_AsgStmt = 85,                   /* AsgStmt  */
  YYSYMBOL_IfStmt = 86,                    /* IfStmt  */
  YYSYMBOL_WhileStmt = 87,                 /* WhileStmt  */
  YYSYMBOL_BreakStmt = 88,                 /* BreakStmt  */
  YYSYMBOL_ContinueStmt = 89,              /* ContinueStmt  */
  YYSYMBOL_RepeatStmt = 90,                /* RepeatStmt  */
  YYSYMBOL_DoWhileStmt = 91,               /* DoWhileStmt  */
  YYSYMBOL_E = 92,                         /* E  */
  YYSYMBOL_ArgListOpt = 93,                /* ArgListOpt  */
  YYSYMBOL_ArgList = 94                    /* ArgList  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  23
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   594

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  107
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  243

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   301


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      47,    48,     2,     2,    49,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    50,     2,    51,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,     2,    53,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    83,    83,    84,    87,    88,    91,   108,   114,   115,
     118,   119,   122,   123,   126,   127,   130,   136,   139,   142,
     145,   148,   151,   155,   160,   161,   164,   170,   173,   176,
     182,   185,   190,   193,   302,   303,   308,   311,   314,   317,
     320,   323,   328,   329,   332,   332,   337,   337,   342,   345,
     350,   351,   354,   355,   358,   359,   362,   365,   370,   373,
     377,   380,   383,   386,   389,   392,   395,   398,   401,   408,
     412,   416,   420,   424,   430,   434,   439,   443,   446,   450,
     454,   458,   462,   466,   471,   474,   477,   480,   483,   486,
     489,   492,   495,   498,   501,   504,   507,   510,   513,   516,
     519,   522,   525,   528,   532,   533,   536,   537
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUM", "ID", "STRING",
  "PLUS", "MINUS", "MUL", "DIV", "MOD", "ADDRESS", "LT", "GT", "LE", "GE",
  "EQ", "NE", "AND", "OR", "NOT", "T_BEGIN", "T_END", "DECL", "ENDDECL",
  "INT", "STR", "MAIN", "TUPLE", "DOT", "ASSIGN", "SEMICOLON", "READ",
  "WRITE", "IF", "THEN", "ELSE", "ENDIF", "WHILE", "DO", "ENDWHILE",
  "REPEAT", "UNTIL", "DOWHILE", "BREAK", "CONTINUE", "RETURN", "'('",
  "')'", "','", "'['", "']'", "'{'", "'}'", "$accept", "Declarations",
  "DeclList", "Decl", "FieldList", "Field", "TupleVarList", "TupleVar",
  "GidList", "Gid", "ParamListOpt", "ParamList", "Param", "Type",
  "ReturnType", "Program", "FdefBlock", "Fdef", "$@1", "MainBlock", "$@2",
  "FunctionBody", "LocalDeclBlock", "LocalDeclBlockOpt", "LocalDeclList",
  "LocalDecl", "Slist", "Stmt", "Variable", "InputStmt", "OutputStmt",
  "AsgStmt", "IfStmt", "WhileStmt", "BreakStmt", "ContinueStmt",
  "RepeatStmt", "DoWhileStmt", "E", "ArgListOpt", "ArgList", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-62)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     170,    83,   -22,   -62,    20,     7,    27,    42,    56,   -62,
     -62,   -62,   -62,    51,    90,   -62,    43,    19,   350,    56,
     -62,   -62,    50,   -62,   -62,   -62,    65,   -62,   -62,   133,
     123,   -15,   -62,    91,   -20,    45,   107,    97,   114,   119,
     137,   447,   447,   174,   175,   377,   -62,   167,   -62,   -62,
     -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,   226,    64,
     226,   204,   180,   -62,    43,   -62,   237,    17,   -62,   -62,
      45,    17,    17,    17,   462,   392,   -62,   -62,   219,   -62,
      17,   251,   208,   209,   -62,    79,    71,   -62,    80,   244,
     206,   226,   -62,   252,   -62,   -62,   -21,   -62,    45,    17,
      17,   -62,    86,   255,   216,   230,   253,   258,   260,   -62,
     502,   141,   -62,   226,   -62,   305,   177,    64,   -62,   320,
     -62,   275,   278,   285,    17,   -62,   -62,   267,    17,    17,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    17,
      17,   277,   297,   302,   300,   301,    17,    17,   -62,   -62,
     337,   293,   -62,   -62,   -62,   342,   -13,   -62,   -62,   -62,
     344,   -62,   166,   -62,   327,   562,   303,   306,   -62,   144,
     144,   -62,   -62,   -62,   150,   150,   150,   150,   164,   164,
     116,   576,    17,   -62,   -62,   447,   447,   281,   304,   -62,
     285,   -62,   -62,   177,   299,   -62,   345,    43,   176,   -62,
     102,   307,   -62,    17,   202,   330,   409,     9,   322,   327,
     -62,   -62,   177,    12,   -62,   -62,    17,   298,   -62,   562,
     -62,   447,   325,   326,   -62,   -62,   308,    13,   -62,   522,
      17,   424,   -62,   -62,   -62,   -62,   343,   542,   328,   -62,
     348,   -62,   -62
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,    32,    33,     0,    34,     0,     0,     0,    43,
      41,     3,    32,     0,     0,     5,     0,     0,     0,     0,
      39,    35,     0,     1,    42,    40,     0,     2,     4,    18,
       0,     0,    17,     0,    69,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,     0,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    38,    25,     0,
      25,     0,    21,     6,     0,    46,     0,     0,    73,    37,
       0,     0,     0,     0,     0,     0,    80,    81,     0,    58,
       0,     0,     0,    24,    27,     0,     0,     9,     0,     0,
       0,    25,    16,     0,    72,    99,    69,   100,     0,     0,
       0,   103,     0,     0,     0,     0,     0,     0,     0,    36,
       0,     0,    44,     0,    28,     0,     0,     0,    10,     0,
      23,    19,     0,    53,   105,   101,    98,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    70,     0,     0,     0,     0,     0,     0,    76,    30,
       0,     0,    26,    29,    14,     0,     0,    13,     8,    11,
       0,    22,     0,    52,     0,   107,     0,   104,    89,    84,
      85,    86,    87,    88,    90,    91,    92,    93,    94,    95,
      96,    97,     0,    74,    75,     0,     0,     0,     0,    31,
      53,    15,     7,     0,     0,    51,     0,     0,     0,    55,
       0,     0,   102,     0,     0,     0,     0,     0,     0,     0,
      12,    20,     0,     0,    50,    54,     0,     0,    47,   106,
      71,     0,     0,     0,    83,    82,     0,     0,    56,     0,
       0,     0,    78,    79,    45,    57,     0,     0,     0,    49,
       0,    77,    48
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -62,   -62,   -62,   359,   -62,   259,   165,   185,   182,   316,
     -56,   -62,   273,     0,   -62,   -62,   383,    -6,   -62,    46,
     -62,   181,   -62,   203,   -62,   194,   -35,   -42,   -18,   -62,
     -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,   -61,   -62,
     -62
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     4,    14,    15,    86,    87,   156,   157,    31,    32,
      82,    83,    84,     5,     6,     7,     8,     9,   151,    10,
      93,   201,   163,   164,   198,   199,    45,    46,   101,    48,
      49,    50,    51,    52,    53,    54,    55,    56,   102,   166,
     167
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      47,    16,    24,    79,    89,    17,    74,    75,    66,    66,
     104,   105,   106,    24,    16,    21,    63,    68,   192,   110,
      95,    96,    97,    47,    47,    35,   124,    47,    98,    67,
      67,    22,    79,    79,    64,   122,   193,    99,   126,   127,
     224,    18,    23,   228,   235,     2,     3,    29,   186,    34,
      20,    30,   103,    35,    25,    26,    47,    47,    85,    88,
      85,    64,   193,   165,   100,    57,    33,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     125,     2,     3,   114,   118,   187,   188,   115,   119,    12,
       3,    85,   128,   129,   130,   131,   132,    58,   133,   134,
     135,   136,   137,   138,   139,   140,    34,    11,    12,     3,
      35,    13,    59,    85,    27,    12,     3,    88,    13,   116,
     117,   204,   128,   129,   130,   131,   132,    62,   133,   134,
     135,   136,   137,   138,    37,    38,    39,   141,    69,    65,
      40,    41,   219,    42,    70,   149,    43,    44,   216,   150,
     205,   206,   130,   131,   132,   229,   128,   129,   130,   131,
     132,    71,   197,    79,    79,   217,    72,    47,    47,   237,
     128,   129,   130,   131,   132,    79,   133,   134,   135,   136,
      60,   154,    47,    61,    73,   155,   231,    47,    47,    79,
     195,    12,     3,     1,   196,     2,     3,    80,   197,    47,
     214,    12,     3,    47,   196,    76,    77,    90,   128,   129,
     130,   131,   132,    47,   133,   134,   135,   136,   137,   138,
     139,   140,   128,   129,   130,   131,   132,    91,   133,   134,
     135,   136,   137,   138,   139,   140,   128,   129,   130,   131,
     132,    94,   133,   134,   135,   136,   137,   138,   139,   140,
     109,    12,     3,   220,    81,   111,   112,   121,   113,   128,
     129,   130,   131,   132,   143,   133,   134,   135,   136,   137,
     138,   139,   140,   128,   129,   130,   131,   132,   144,   133,
     134,   135,   136,   137,   138,   139,   140,   128,   129,   130,
     131,   132,   120,   133,   134,   135,   136,   137,   138,   139,
     140,   145,    34,   142,   123,   146,    35,   147,   162,   153,
     128,   129,   130,   131,   132,   168,   133,   134,   135,   136,
     137,   138,   139,   140,   159,   160,   161,   182,   183,   207,
      37,    38,    39,   184,    34,   185,    40,    41,    35,    42,
     186,   189,    43,    44,   230,   190,   191,   194,   200,   212,
     211,   202,   208,   225,    34,   203,   232,   233,    35,   241,
     218,   234,    37,    38,    39,   239,   221,   222,    40,    41,
     242,    42,    36,    28,    43,    44,   158,   227,   210,   213,
      92,    34,    37,    38,    39,    35,   152,    19,    40,    41,
     226,    42,   215,   209,    43,    44,    34,     0,     0,    78,
      35,     0,     0,     0,     0,     0,     0,     0,     0,    37,
      38,    39,     0,    34,     0,    40,    41,    35,    42,     0,
       0,    43,    44,     0,    37,    38,    39,     0,    34,     0,
      40,    41,    35,    42,   108,     0,    43,    44,     0,     0,
       0,    37,    38,    39,     0,     0,     0,    40,    41,   223,
      42,    34,     0,    43,    44,    35,    37,    38,    39,     0,
       0,   238,    40,    41,     0,    42,    34,     0,    43,    44,
      35,     0,     0,     0,     0,     0,     0,     0,     0,    37,
      38,    39,     0,     0,     0,    40,    41,     0,    42,     0,
       0,    43,    44,     0,    37,    38,    39,     0,     0,     0,
     107,    41,     0,    42,     0,     0,    43,    44,   128,   129,
     130,   131,   132,     0,   133,   134,   135,   136,   137,   138,
     139,   140,     0,     0,     0,     0,     0,     0,   128,   129,
     130,   131,   132,   148,   133,   134,   135,   136,   137,   138,
     139,   140,     0,     0,     0,     0,     0,     0,   128,   129,
     130,   131,   132,   236,   133,   134,   135,   136,   137,   138,
     139,   140,     0,     0,     0,     0,     0,     0,   128,   129,
     130,   131,   132,   240,   133,   134,   135,   136,   137,   138,
     139,   140,   128,   129,   130,   131,   132,     0,   133,   134,
     135,   136,   137,   138,   139
};

static const yytype_int16 yycheck[] =
{
      18,     1,     8,    45,    60,    27,    41,    42,    29,    29,
      71,    72,    73,    19,    14,     8,    31,    35,    31,    80,
       3,     4,     5,    41,    42,     8,    47,    45,    11,    50,
      50,     4,    74,    75,    49,    91,    49,    20,    99,   100,
      31,    21,     0,    31,    31,    25,    26,     4,    39,     4,
       4,     8,    70,     8,     8,     4,    74,    75,    58,    59,
      60,    49,    49,   124,    47,    19,    47,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
      98,    25,    26,     4,     4,   146,   147,     8,     8,    25,
      26,    91,     6,     7,     8,     9,    10,    47,    12,    13,
      14,    15,    16,    17,    18,    19,     4,    24,    25,    26,
       8,    28,    47,   113,    24,    25,    26,   117,    28,    48,
      49,   182,     6,     7,     8,     9,    10,     4,    12,    13,
      14,    15,    16,    17,    32,    33,    34,    51,    31,    48,
      38,    39,   203,    41,    47,     4,    44,    45,    46,     8,
     185,   186,     8,     9,    10,   216,     6,     7,     8,     9,
      10,    47,   162,   205,   206,   200,    47,   185,   186,   230,
       6,     7,     8,     9,    10,   217,    12,    13,    14,    15,
      47,     4,   200,    50,    47,     8,   221,   205,   206,   231,
      24,    25,    26,    23,    28,    25,    26,    30,   198,   217,
      24,    25,    26,   221,    28,    31,    31,     3,     6,     7,
       8,     9,    10,   231,    12,    13,    14,    15,    16,    17,
      18,    19,     6,     7,     8,     9,    10,    47,    12,    13,
      14,    15,    16,    17,    18,    19,     6,     7,     8,     9,
      10,     4,    12,    13,    14,    15,    16,    17,    18,    19,
      31,    25,    26,    51,    28,     4,    48,    51,    49,     6,
       7,     8,     9,    10,    48,    12,    13,    14,    15,    16,
      17,    18,    19,     6,     7,     8,     9,    10,    48,    12,
      13,    14,    15,    16,    17,    18,    19,     6,     7,     8,
       9,    10,    48,    12,    13,    14,    15,    16,    17,    18,
      19,    48,     4,    48,    52,    47,     8,    47,    23,     4,
       6,     7,     8,     9,    10,    48,    12,    13,    14,    15,
      16,    17,    18,    19,     4,    50,    48,    50,    31,    48,
      32,    33,    34,    31,     4,    35,    38,    39,     8,    41,
      39,     4,    44,    45,    46,    52,     4,     3,    21,     4,
      51,    48,    48,    31,     4,    49,    31,    31,     8,    31,
      53,    53,    32,    33,    34,    22,    36,    37,    38,    39,
      22,    41,    22,    14,    44,    45,   117,   212,   193,   197,
      64,     4,    32,    33,    34,     8,   113,     4,    38,    39,
     209,    41,   198,   190,    44,    45,     4,    -1,    -1,    22,
       8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      33,    34,    -1,     4,    -1,    38,    39,     8,    41,    -1,
      -1,    44,    45,    -1,    32,    33,    34,    -1,     4,    -1,
      38,    39,     8,    41,    42,    -1,    44,    45,    -1,    -1,
      -1,    32,    33,    34,    -1,    -1,    -1,    38,    39,    40,
      41,     4,    -1,    44,    45,     8,    32,    33,    34,    -1,
      -1,    37,    38,    39,    -1,    41,     4,    -1,    44,    45,
       8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      33,    34,    -1,    -1,    -1,    38,    39,    -1,    41,    -1,
      -1,    44,    45,    -1,    32,    33,    34,    -1,    -1,    -1,
      38,    39,    -1,    41,    -1,    -1,    44,    45,     6,     7,
       8,     9,    10,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,     6,     7,
       8,     9,    10,    31,    12,    13,    14,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,     6,     7,
       8,     9,    10,    31,    12,    13,    14,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,     6,     7,
       8,     9,    10,    31,    12,    13,    14,    15,    16,    17,
      18,    19,     6,     7,     8,     9,    10,    -1,    12,    13,
      14,    15,    16,    17,    18
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    23,    25,    26,    55,    67,    68,    69,    70,    71,
      73,    24,    25,    28,    56,    57,    67,    27,    21,    70,
      73,     8,     4,     0,    71,    73,     4,    24,    57,     4,
       8,    62,    63,    47,     4,     8,    22,    32,    33,    34,
      38,    39,    41,    44,    45,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    73,    47,    47,
      47,    50,     4,    31,    49,    48,    29,    50,    82,    31,
      47,    47,    47,    47,    80,    80,    31,    31,    22,    81,
      30,    28,    64,    65,    66,    67,    58,    59,    67,    64,
       3,    47,    63,    74,     4,     3,     4,     5,    11,    20,
      47,    82,    92,    82,    92,    92,    92,    38,    42,    31,
      92,     4,    48,    49,     4,     8,    48,    49,     4,     8,
      48,    51,    64,    52,    47,    82,    92,    92,     6,     7,
       8,     9,    10,    12,    13,    14,    15,    16,    17,    18,
      19,    51,    48,    48,    48,    48,    47,    47,    31,     4,
       8,    72,    66,     4,     4,     8,    60,    61,    59,     4,
      50,    48,    23,    76,    77,    92,    93,    94,    48,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    50,    31,    31,    35,    39,    92,    92,     4,
      52,     4,    31,    49,     3,    24,    28,    67,    78,    79,
      21,    75,    48,    49,    92,    80,    80,    48,    48,    77,
      61,    51,     4,    62,    24,    79,    46,    80,    53,    92,
      51,    36,    37,    40,    31,    31,    75,    60,    31,    92,
      46,    80,    31,    31,    53,    31,    31,    92,    37,    22,
      31,    31,    22
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    54,    55,    55,    56,    56,    57,    57,    58,    58,
      59,    59,    60,    60,    61,    61,    62,    62,    63,    63,
      63,    63,    63,    63,    64,    64,    65,    65,    66,    66,
      66,    66,    67,    67,    68,    68,    69,    69,    69,    69,
      69,    69,    70,    70,    72,    71,    74,    73,    75,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    80,    80,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    82,
      82,    82,    82,    82,    83,    84,    85,    86,    86,    87,
      88,    89,    90,    91,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    93,    93,    94,    94
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     2,     2,     1,     3,     7,     3,     1,
       2,     3,     3,     1,     1,     2,     3,     1,     1,     4,
       7,     2,     5,     4,     1,     0,     3,     1,     2,     3,
       3,     4,     1,     1,     1,     2,     5,     4,     3,     2,
       2,     1,     2,     1,     0,    10,     0,     9,     6,     5,
       3,     2,     1,     0,     2,     1,     3,     4,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     7,     3,     2,     5,     5,     4,    10,     8,     8,
       2,     2,     7,     7,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     1,
       1,     2,     4,     1,     1,     0,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 6: /* Decl: Type GidList SEMICOLON  */
#line 91 "exprtree.y"
                              {
        struct GDeclItem *temp = (yyvsp[-1].gdecllist);
        while (temp != NULL) {
            int varType = (yyvsp[-2].num);
            if (temp->isPointer) {
                if ((yyvsp[-2].num) == TYPE_INT)
                    varType = TYPE_INT_PTR;
                else if ((yyvsp[-2].num) == TYPE_STR)
                    varType = TYPE_STR_PTR;
            }
            if (temp->isFunction)
                InstallFunction(temp->name, varType, temp->paramlist);
            else
                Install(temp->name, varType, temp->size, temp->rows, temp->cols);
            temp = temp->next;
        }
    }
#line 1414 "exprtree.tab.c"
    break;

  case 7: /* Decl: TUPLE ID '(' FieldList ')' TupleVarList SEMICOLON  */
#line 108 "exprtree.y"
                                                        {
        InstallTupleType((yyvsp[-5].str), (yyvsp[-3].fieldlist));
        InstallTupleVariables((yyvsp[-5].str), (yyvsp[-1].tuplevarlist), 0);
    }
#line 1423 "exprtree.tab.c"
    break;

  case 8: /* FieldList: FieldList ',' Field  */
#line 114 "exprtree.y"
                                { (yyval.fieldlist) = appendField((yyvsp[-2].fieldlist), (yyvsp[0].fieldlist)); }
#line 1429 "exprtree.tab.c"
    break;

  case 9: /* FieldList: Field  */
#line 115 "exprtree.y"
                  { (yyval.fieldlist) = (yyvsp[0].fieldlist); }
#line 1435 "exprtree.tab.c"
    break;

  case 10: /* Field: Type ID  */
#line 118 "exprtree.y"
                { (yyval.fieldlist) = makeField((yyvsp[0].str), (yyvsp[-1].num)); }
#line 1441 "exprtree.tab.c"
    break;

  case 11: /* Field: Type MUL ID  */
#line 119 "exprtree.y"
                    { (yyval.fieldlist) = makeField((yyvsp[0].str), (yyvsp[-2].num) == TYPE_INT ? TYPE_INT_PTR : TYPE_STR_PTR); }
#line 1447 "exprtree.tab.c"
    break;

  case 12: /* TupleVarList: TupleVarList ',' TupleVar  */
#line 122 "exprtree.y"
                                         { (yyval.tuplevarlist) = appendTupleVar((yyvsp[-2].tuplevarlist), (yyvsp[0].tuplevarlist)); }
#line 1453 "exprtree.tab.c"
    break;

  case 13: /* TupleVarList: TupleVar  */
#line 123 "exprtree.y"
                        { (yyval.tuplevarlist) = (yyvsp[0].tuplevarlist); }
#line 1459 "exprtree.tab.c"
    break;

  case 14: /* TupleVar: ID  */
#line 126 "exprtree.y"
              { (yyval.tuplevarlist) = makeTupleVar((yyvsp[0].str), 0); }
#line 1465 "exprtree.tab.c"
    break;

  case 15: /* TupleVar: MUL ID  */
#line 127 "exprtree.y"
                  { (yyval.tuplevarlist) = makeTupleVar((yyvsp[0].str), 1); }
#line 1471 "exprtree.tab.c"
    break;

  case 16: /* GidList: GidList ',' Gid  */
#line 130 "exprtree.y"
                          {
            struct GDeclItem *tail = (yyvsp[-2].gdecllist);
            while (tail->next != NULL) tail = tail->next;
            tail->next = (yyvsp[0].gdecllist);
            (yyval.gdecllist) = (yyvsp[-2].gdecllist);
        }
#line 1482 "exprtree.tab.c"
    break;

  case 17: /* GidList: Gid  */
#line 136 "exprtree.y"
              { (yyval.gdecllist) = (yyvsp[0].gdecllist); }
#line 1488 "exprtree.tab.c"
    break;

  case 18: /* Gid: ID  */
#line 139 "exprtree.y"
         {
        (yyval.gdecllist) = calloc(1, sizeof(*(yyval.gdecllist))); (yyval.gdecllist)->name = (yyvsp[0].str); (yyval.gdecllist)->size = 1; (yyval.gdecllist)->rows = 1; (yyval.gdecllist)->cols = 1;
    }
#line 1496 "exprtree.tab.c"
    break;

  case 19: /* Gid: ID '[' NUM ']'  */
#line 142 "exprtree.y"
                     {
        (yyval.gdecllist) = calloc(1, sizeof(*(yyval.gdecllist))); (yyval.gdecllist)->name = (yyvsp[-3].str); (yyval.gdecllist)->size = (yyvsp[-1].num); (yyval.gdecllist)->rows = 1; (yyval.gdecllist)->cols = (yyvsp[-1].num);
    }
#line 1504 "exprtree.tab.c"
    break;

  case 20: /* Gid: ID '[' NUM ']' '[' NUM ']'  */
#line 145 "exprtree.y"
                                 {
        (yyval.gdecllist) = calloc(1, sizeof(*(yyval.gdecllist))); (yyval.gdecllist)->name = (yyvsp[-6].str); (yyval.gdecllist)->size = (yyvsp[-4].num) * (yyvsp[-1].num); (yyval.gdecllist)->rows = (yyvsp[-4].num); (yyval.gdecllist)->cols = (yyvsp[-1].num);
    }
#line 1512 "exprtree.tab.c"
    break;

  case 21: /* Gid: MUL ID  */
#line 148 "exprtree.y"
             {
        (yyval.gdecllist) = calloc(1, sizeof(*(yyval.gdecllist))); (yyval.gdecllist)->name = (yyvsp[0].str); (yyval.gdecllist)->size = 1; (yyval.gdecllist)->rows = 1; (yyval.gdecllist)->cols = 1; (yyval.gdecllist)->isPointer = 1;
    }
#line 1520 "exprtree.tab.c"
    break;

  case 22: /* Gid: MUL ID '(' ParamListOpt ')'  */
#line 151 "exprtree.y"
                                  {
        (yyval.gdecllist) = calloc(1, sizeof(*(yyval.gdecllist))); (yyval.gdecllist)->name = (yyvsp[-3].str); (yyval.gdecllist)->isFunction = 1;
        (yyval.gdecllist)->isPointer = 1; (yyval.gdecllist)->paramlist = (yyvsp[-1].paramlist);
    }
#line 1529 "exprtree.tab.c"
    break;

  case 23: /* Gid: ID '(' ParamListOpt ')'  */
#line 155 "exprtree.y"
                              {
        (yyval.gdecllist) = calloc(1, sizeof(*(yyval.gdecllist))); (yyval.gdecllist)->name = (yyvsp[-3].str); (yyval.gdecllist)->isFunction = 1; (yyval.gdecllist)->paramlist = (yyvsp[-1].paramlist);
    }
#line 1537 "exprtree.tab.c"
    break;

  case 24: /* ParamListOpt: ParamList  */
#line 160 "exprtree.y"
                         { (yyval.paramlist) = (yyvsp[0].paramlist); }
#line 1543 "exprtree.tab.c"
    break;

  case 25: /* ParamListOpt: %empty  */
#line 161 "exprtree.y"
                           { (yyval.paramlist) = NULL; }
#line 1549 "exprtree.tab.c"
    break;

  case 26: /* ParamList: ParamList ',' Param  */
#line 164 "exprtree.y"
                                {
            struct Paramstruct *tail = (yyvsp[-2].paramlist);
            while (tail->next != NULL) tail = tail->next;
            tail->next = (yyvsp[0].paramlist);
            (yyval.paramlist) = (yyvsp[-2].paramlist);
          }
#line 1560 "exprtree.tab.c"
    break;

  case 27: /* ParamList: Param  */
#line 170 "exprtree.y"
                  { (yyval.paramlist) = (yyvsp[0].paramlist); }
#line 1566 "exprtree.tab.c"
    break;

  case 28: /* Param: Type ID  */
#line 173 "exprtree.y"
                {
        (yyval.paramlist) = calloc(1, sizeof(*(yyval.paramlist))); (yyval.paramlist)->type = (yyvsp[-1].num); (yyval.paramlist)->name = (yyvsp[0].str); (yyval.paramlist)->size = 1;
    }
#line 1574 "exprtree.tab.c"
    break;

  case 29: /* Param: Type MUL ID  */
#line 176 "exprtree.y"
                  {
        (yyval.paramlist) = calloc(1, sizeof(*(yyval.paramlist)));
        (yyval.paramlist)->type = (yyvsp[-2].num) == TYPE_INT ? TYPE_INT_PTR : TYPE_STR_PTR;
        (yyval.paramlist)->name = (yyvsp[0].str);
        (yyval.paramlist)->size = 1;
    }
#line 1585 "exprtree.tab.c"
    break;

  case 30: /* Param: TUPLE ID ID  */
#line 182 "exprtree.y"
                  {
        (yyval.paramlist) = makeTupleParam((yyvsp[-1].str), (yyvsp[0].str), 0);
    }
#line 1593 "exprtree.tab.c"
    break;

  case 31: /* Param: TUPLE ID MUL ID  */
#line 185 "exprtree.y"
                      {
        (yyval.paramlist) = makeTupleParam((yyvsp[-2].str), (yyvsp[0].str), 1);
    }
#line 1601 "exprtree.tab.c"
    break;

  case 32: /* Type: INT  */
#line 190 "exprtree.y"
           {
        (yyval.num) = TYPE_INT;
     }
#line 1609 "exprtree.tab.c"
    break;

  case 33: /* Type: STR  */
#line 193 "exprtree.y"
           {
        (yyval.num) = TYPE_STR;
     }
#line 1617 "exprtree.tab.c"
    break;

  case 34: /* ReturnType: Type  */
#line 302 "exprtree.y"
                  { (yyval.num) = (yyvsp[0].num); }
#line 1623 "exprtree.tab.c"
    break;

  case 35: /* ReturnType: Type MUL  */
#line 303 "exprtree.y"
                      { (yyval.num) = (yyvsp[-1].num) == TYPE_INT ? TYPE_INT_PTR : TYPE_STR_PTR; }
#line 1629 "exprtree.tab.c"
    break;

  case 36: /* Program: Declarations T_BEGIN Slist T_END SEMICOLON  */
#line 308 "exprtree.y"
                                                     {
            root = (yyvsp[-2].node);
        }
#line 1637 "exprtree.tab.c"
    break;

  case 37: /* Program: Declarations T_BEGIN T_END SEMICOLON  */
#line 311 "exprtree.y"
                                               {
            root = NULL;
        }
#line 1645 "exprtree.tab.c"
    break;

  case 38: /* Program: Declarations FdefBlock MainBlock  */
#line 314 "exprtree.y"
                                           {
            root = NULL;
        }
#line 1653 "exprtree.tab.c"
    break;

  case 39: /* Program: Declarations MainBlock  */
#line 317 "exprtree.y"
                                 {
            root = NULL;
        }
#line 1661 "exprtree.tab.c"
    break;

  case 40: /* Program: FdefBlock MainBlock  */
#line 320 "exprtree.y"
                              {
            root = NULL;
        }
#line 1669 "exprtree.tab.c"
    break;

  case 41: /* Program: MainBlock  */
#line 323 "exprtree.y"
                    {
            root = NULL;
        }
#line 1677 "exprtree.tab.c"
    break;

  case 44: /* $@1: %empty  */
#line 332 "exprtree.y"
                                          {
            BeginFunctionDefinition((yyvsp[-4].num), (yyvsp[-3].str), (yyvsp[-1].paramlist));
       }
#line 1685 "exprtree.tab.c"
    break;

  case 46: /* $@2: %empty  */
#line 337 "exprtree.y"
                             {
                BeginMainDefinition("main");
            }
#line 1693 "exprtree.tab.c"
    break;

  case 48: /* FunctionBody: T_BEGIN Slist RETURN E SEMICOLON T_END  */
#line 342 "exprtree.y"
                                                      {
                    FinishFunctionDefinition((yyvsp[-4].node), (yyvsp[-2].node));
                }
#line 1701 "exprtree.tab.c"
    break;

  case 49: /* FunctionBody: T_BEGIN RETURN E SEMICOLON T_END  */
#line 345 "exprtree.y"
                                                {
                    FinishFunctionDefinition(NULL, (yyvsp[-2].node));
                }
#line 1709 "exprtree.tab.c"
    break;

  case 56: /* LocalDecl: Type GidList SEMICOLON  */
#line 362 "exprtree.y"
                                   {
                InstallLocalDeclarations((yyvsp[-2].num), (yyvsp[-1].gdecllist));
            }
#line 1717 "exprtree.tab.c"
    break;

  case 57: /* LocalDecl: TUPLE ID TupleVarList SEMICOLON  */
#line 365 "exprtree.y"
                                            {
                InstallTupleVariables((yyvsp[-2].str), (yyvsp[-1].tuplevarlist), 1);
            }
#line 1725 "exprtree.tab.c"
    break;

  case 58: /* Slist: Slist Stmt  */
#line 370 "exprtree.y"
                   {
        (yyval.node) = makeConnectorNode((yyvsp[-1].node), (yyvsp[0].node));
    }
#line 1733 "exprtree.tab.c"
    break;

  case 59: /* Slist: Stmt  */
#line 373 "exprtree.y"
           {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1741 "exprtree.tab.c"
    break;

  case 60: /* Stmt: InputStmt  */
#line 377 "exprtree.y"
                 {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1749 "exprtree.tab.c"
    break;

  case 61: /* Stmt: OutputStmt  */
#line 380 "exprtree.y"
                 {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1757 "exprtree.tab.c"
    break;

  case 62: /* Stmt: AsgStmt  */
#line 383 "exprtree.y"
              {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1765 "exprtree.tab.c"
    break;

  case 63: /* Stmt: IfStmt  */
#line 386 "exprtree.y"
             {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1773 "exprtree.tab.c"
    break;

  case 64: /* Stmt: WhileStmt  */
#line 389 "exprtree.y"
                {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1781 "exprtree.tab.c"
    break;

  case 65: /* Stmt: BreakStmt  */
#line 392 "exprtree.y"
                {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1789 "exprtree.tab.c"
    break;

  case 66: /* Stmt: ContinueStmt  */
#line 395 "exprtree.y"
                   {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1797 "exprtree.tab.c"
    break;

  case 67: /* Stmt: RepeatStmt  */
#line 398 "exprtree.y"
                 {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1805 "exprtree.tab.c"
    break;

  case 68: /* Stmt: DoWhileStmt  */
#line 401 "exprtree.y"
                  {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1813 "exprtree.tab.c"
    break;

  case 69: /* Variable: ID  */
#line 408 "exprtree.y"
              {
            (yyval.node) = makeIdNode((yyvsp[0].str));
        }
#line 1821 "exprtree.tab.c"
    break;

  case 70: /* Variable: ID '[' E ']'  */
#line 412 "exprtree.y"
                       {
            (yyval.node) = makeArrayNode((yyvsp[-3].str), (yyvsp[-1].node));
        }
#line 1829 "exprtree.tab.c"
    break;

  case 71: /* Variable: ID '[' E ']' '[' E ']'  */
#line 416 "exprtree.y"
                                 { // EX1: supports 2D array access like a[i][j]
            (yyval.node) = makeArray2DNode((yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node));
        }
#line 1837 "exprtree.tab.c"
    break;

  case 72: /* Variable: ID DOT ID  */
#line 420 "exprtree.y"
                    {
            (yyval.node) = makeFieldNode((yyvsp[-2].str), (yyvsp[0].str));
        }
#line 1845 "exprtree.tab.c"
    break;

  case 73: /* Variable: MUL Variable  */
#line 424 "exprtree.y"
                       { // EX2: allows a dereferenced pointer as an assignment target
            (yyval.node) = makeDereferenceNode((yyvsp[0].node));
        }
#line 1853 "exprtree.tab.c"
    break;

  case 74: /* InputStmt: READ '(' Variable ')' SEMICOLON  */
#line 430 "exprtree.y"
                                            {
        (yyval.node) = makeReadNode((yyvsp[-2].node));
    }
#line 1861 "exprtree.tab.c"
    break;

  case 75: /* OutputStmt: WRITE '(' E ')' SEMICOLON  */
#line 434 "exprtree.y"
                                       { // u can write an expression like write(5+8);
        (yyval.node) = makeWriteNode((yyvsp[-2].node));
    }
#line 1869 "exprtree.tab.c"
    break;

  case 76: /* AsgStmt: Variable ASSIGN E SEMICOLON  */
#line 439 "exprtree.y"
                                      {
        (yyval.node) = makeAssignNode((yyvsp[-3].node), (yyvsp[-1].node));
    }
#line 1877 "exprtree.tab.c"
    break;

  case 77: /* IfStmt: IF '(' E ')' THEN Slist ELSE Slist ENDIF SEMICOLON  */
#line 443 "exprtree.y"
                                                            {
        (yyval.node) = makeIfNode((yyvsp[-7].node), (yyvsp[-4].node), (yyvsp[-2].node));
    }
#line 1885 "exprtree.tab.c"
    break;

  case 78: /* IfStmt: IF '(' E ')' THEN Slist ENDIF SEMICOLON  */
#line 446 "exprtree.y"
                                              {
        (yyval.node) = makeIfNode((yyvsp[-5].node), (yyvsp[-2].node), NULL);
    }
#line 1893 "exprtree.tab.c"
    break;

  case 79: /* WhileStmt: WHILE '(' E ')' DO Slist ENDWHILE SEMICOLON  */
#line 450 "exprtree.y"
                                                        {
        (yyval.node) = makeWhileNode((yyvsp[-5].node), (yyvsp[-2].node));        
    }
#line 1901 "exprtree.tab.c"
    break;

  case 80: /* BreakStmt: BREAK SEMICOLON  */
#line 454 "exprtree.y"
                            {
        (yyval.node) = makeBreakNode();
    }
#line 1909 "exprtree.tab.c"
    break;

  case 81: /* ContinueStmt: CONTINUE SEMICOLON  */
#line 458 "exprtree.y"
                                  {
        (yyval.node) = makeContinueNode();
    }
#line 1917 "exprtree.tab.c"
    break;

  case 82: /* RepeatStmt: REPEAT Slist UNTIL '(' E ')' SEMICOLON  */
#line 462 "exprtree.y"
                                                    {
        (yyval.node) = makeRepeatNode((yyvsp[-5].node), (yyvsp[-2].node));
    }
#line 1925 "exprtree.tab.c"
    break;

  case 83: /* DoWhileStmt: DO Slist WHILE '(' E ')' SEMICOLON  */
#line 466 "exprtree.y"
                                                 {
        (yyval.node) = makeDoWhileNode((yyvsp[-5].node), (yyvsp[-2].node));
    }
#line 1933 "exprtree.tab.c"
    break;

  case 84: /* E: E PLUS E  */
#line 471 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode("+", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1941 "exprtree.tab.c"
    break;

  case 85: /* E: E MINUS E  */
#line 474 "exprtree.y"
                {
        (yyval.node) = makeOperatorNode("-", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1949 "exprtree.tab.c"
    break;

  case 86: /* E: E MUL E  */
#line 477 "exprtree.y"
              {
        (yyval.node) = makeOperatorNode("*", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1957 "exprtree.tab.c"
    break;

  case 87: /* E: E DIV E  */
#line 480 "exprtree.y"
              {
        (yyval.node) = makeOperatorNode("/", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1965 "exprtree.tab.c"
    break;

  case 88: /* E: E MOD E  */
#line 483 "exprtree.y"
              {
        (yyval.node) = makeOperatorNode("%", (yyvsp[-2].node), (yyvsp[0].node)); // EX2: creates the modulo operation node               
    }
#line 1973 "exprtree.tab.c"
    break;

  case 89: /* E: '(' E ')'  */
#line 486 "exprtree.y"
                {
        (yyval.node) = (yyvsp[-1].node);
    }
#line 1981 "exprtree.tab.c"
    break;

  case 90: /* E: E LT E  */
#line 489 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode("<", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1989 "exprtree.tab.c"
    break;

  case 91: /* E: E GT E  */
#line 492 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode(">", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1997 "exprtree.tab.c"
    break;

  case 92: /* E: E LE E  */
#line 495 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode("<=", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 2005 "exprtree.tab.c"
    break;

  case 93: /* E: E GE E  */
#line 498 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode(">=", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 2013 "exprtree.tab.c"
    break;

  case 94: /* E: E EQ E  */
#line 501 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode("==", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 2021 "exprtree.tab.c"
    break;

  case 95: /* E: E NE E  */
#line 504 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode("!=", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 2029 "exprtree.tab.c"
    break;

  case 96: /* E: E AND E  */
#line 507 "exprtree.y"
              {
        (yyval.node) = makeOperatorNode("&&", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 2037 "exprtree.tab.c"
    break;

  case 97: /* E: E OR E  */
#line 510 "exprtree.y"
             {
        (yyval.node) = makeOperatorNode("||", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 2045 "exprtree.tab.c"
    break;

  case 98: /* E: NOT E  */
#line 513 "exprtree.y"
            {
        (yyval.node) = makeOperatorNode("!", (yyvsp[0].node), NULL);
    }
#line 2053 "exprtree.tab.c"
    break;

  case 99: /* E: NUM  */
#line 516 "exprtree.y"
          {
        (yyval.node) = makeNumNode((yyvsp[0].num));
    }
#line 2061 "exprtree.tab.c"
    break;

  case 100: /* E: STRING  */
#line 519 "exprtree.y"
             { //TASK1 forgotten crossroads(iykyk)
        (yyval.node) = makeStrNode((yyvsp[0].str));
    }
#line 2069 "exprtree.tab.c"
    break;

  case 101: /* E: ADDRESS Variable  */
#line 522 "exprtree.y"
                       { // EX2: address-of operator returns the address of a variable
        (yyval.node) = makeAddressNode((yyvsp[0].node));
    }
#line 2077 "exprtree.tab.c"
    break;

  case 102: /* E: ID '(' ArgListOpt ')'  */
#line 525 "exprtree.y"
                            {
        (yyval.node) = makeFuncCallNode((yyvsp[-3].str), (yyvsp[-1].arglist));
    }
#line 2085 "exprtree.tab.c"
    break;

  case 103: /* E: Variable  */
#line 528 "exprtree.y"
               { //TASK3 change here
        (yyval.node) = (yyvsp[0].node);
    }
#line 2093 "exprtree.tab.c"
    break;

  case 104: /* ArgListOpt: ArgList  */
#line 532 "exprtree.y"
                     { (yyval.arglist) = (yyvsp[0].arglist); }
#line 2099 "exprtree.tab.c"
    break;

  case 105: /* ArgListOpt: %empty  */
#line 533 "exprtree.y"
                         { (yyval.arglist) = NULL; }
#line 2105 "exprtree.tab.c"
    break;

  case 106: /* ArgList: ArgList ',' E  */
#line 536 "exprtree.y"
                        { (yyval.arglist) = appendArg((yyvsp[-2].arglist), (yyvsp[0].node)); }
#line 2111 "exprtree.tab.c"
    break;

  case 107: /* ArgList: E  */
#line 537 "exprtree.y"
            { (yyval.arglist) = makeArgList((yyvsp[0].node)); }
#line 2117 "exprtree.tab.c"
    break;


#line 2121 "exprtree.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 540 "exprtree.y"


extern FILE* yyin; // it is file pointer of lexer. defaulted to "stdin"
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Cannot open input file\n");
        return 1;
    }

    targetFile = fopen("target.xsm", "w");
    if (!targetFile) {
        printf("Could not open target file\n");
        exit(1);
    }

    fprintf(targetFile, "0\n");
    fprintf(targetFile, "2056\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    fprintf(targetFile, "0\n");
    yyparse(); 

    /* Global declarations determine the first safe stack location. */
    fprintf(targetFile, "MOV SP, %d\n", nextBinding - 1);
    
    PrintSymbolTable(); // TASK1: prints the symbol table to check the installed variables

    if (GetFunctionASTs() != NULL) {
        generateStage5Code();
    } else {
        /* Preserve the old Stage 4 top-level program path. */
        codeGen(root);
        fprintf(targetFile, "MOV R2, \"Exit\"\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "PUSH R2\n");
        fprintf(targetFile, "CALL 0\n");
    }


    fclose(targetFile);
    return 0;
}

void yyerror(const char* s) {
    extern int yylineno;
    printf("%s at line %d\n", s, yylineno);
}
