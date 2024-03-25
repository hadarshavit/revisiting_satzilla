/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
   especially those whose name start with YY_ or lp_yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with lp_yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30704

/* Bison version string.  */
#define YYBISON_VERSION "3.7.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 9 "lp.y"

#include <string.h>
#include <ctype.h>

#include "lpkit.h"
#include "read.h"

static int HadVar0, HadVar1, HadVar2, do_add_row, Had_lineair_sum0, HadSign;
static char Last_var[NAMELEN], Last_var0[NAMELEN];
static REAL f, f0, f1;
static int x;
static int state, state0;
static int Sign;
static int isign, isign0;      /* internal_sign variable to make sure nothing goes wrong */
                /* with lookahead */
static int make_neg;   /* is true after the relational operator is seen in order */
                /* to remember if lin_term stands before or after re_op */
static int Within_int_decl = FALSE; /* TRUE when we are within an int declaration */
static int Within_sec_decl = FALSE; /* TRUE when we are within an sec declaration */
static int Within_sos_decl = FALSE; /* TRUE when we are within an sos declaration */
static int Within_sos_decl1;
static short SOStype0; /* SOS type */
static short SOStype; /* SOS type */
static int SOSNr;
static int weight; /* SOS weight */
static int SOSweight = 0; /* SOS weight */

static int HadConstraint;
static int HadVar;
static int Had_lineair_sum;

#define YY_FATAL_ERROR lex_fatal_error

/* let's please C++ users */
#ifdef __cplusplus
extern "C" {
#endif

static int wrap(void)
{
  return(1);
}

#ifdef __cplusplus
};
#endif

#define lp_yywrap wrap
#define lp_yyerror read_error

#include "lex.c"


#line 125 "y.tab.c"

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


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int lp_yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum lp_yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    VAR = 258,                     /* VAR  */
    CONS = 259,                    /* CONS  */
    INTCONS = 260,                 /* INTCONS  */
    VARIABLECOLON = 261,           /* VARIABLECOLON  */
    INF = 262,                     /* INF  */
    SEC_INT = 263,                 /* SEC_INT  */
    SEC_SEC = 264,                 /* SEC_SEC  */
    SEC_SOS = 265,                 /* SEC_SOS  */
    SOSDESCR = 266,                /* SOSDESCR  */
    SIGN = 267,                    /* SIGN  */
    AR_M_OP = 268,                 /* AR_M_OP  */
    RE_OPLE = 269,                 /* RE_OPLE  */
    RE_OPGE = 270,                 /* RE_OPGE  */
    END_C = 271,                   /* END_C  */
    COMMA = 272,                   /* COMMA  */
    COLON = 273,                   /* COLON  */
    MINIMISE = 274,                /* MINIMISE  */
    MAXIMISE = 275,                /* MAXIMISE  */
    UNDEFINED = 276                /* UNDEFINED  */
  };
  typedef enum lp_yytokentype lp_yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define VAR 258
#define CONS 259
#define INTCONS 260
#define VARIABLECOLON 261
#define INF 262
#define SEC_INT 263
#define SEC_SEC 264
#define SEC_SOS 265
#define SOSDESCR 266
#define SIGN 267
#define AR_M_OP 268
#define RE_OPLE 269
#define RE_OPGE 270
#define END_C 271
#define COMMA 272
#define COLON 273
#define MINIMISE 274
#define MAXIMISE 275
#define UNDEFINED 276

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE lp_yylval;

int lp_yyparse (void);


/* Symbol kind.  */
enum lp_yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_VAR = 3,                        /* VAR  */
  YYSYMBOL_CONS = 4,                       /* CONS  */
  YYSYMBOL_INTCONS = 5,                    /* INTCONS  */
  YYSYMBOL_VARIABLECOLON = 6,              /* VARIABLECOLON  */
  YYSYMBOL_INF = 7,                        /* INF  */
  YYSYMBOL_SEC_INT = 8,                    /* SEC_INT  */
  YYSYMBOL_SEC_SEC = 9,                    /* SEC_SEC  */
  YYSYMBOL_SEC_SOS = 10,                   /* SEC_SOS  */
  YYSYMBOL_SOSDESCR = 11,                  /* SOSDESCR  */
  YYSYMBOL_SIGN = 12,                      /* SIGN  */
  YYSYMBOL_AR_M_OP = 13,                   /* AR_M_OP  */
  YYSYMBOL_RE_OPLE = 14,                   /* RE_OPLE  */
  YYSYMBOL_RE_OPGE = 15,                   /* RE_OPGE  */
  YYSYMBOL_END_C = 16,                     /* END_C  */
  YYSYMBOL_COMMA = 17,                     /* COMMA  */
  YYSYMBOL_COLON = 18,                     /* COLON  */
  YYSYMBOL_MINIMISE = 19,                  /* MINIMISE  */
  YYSYMBOL_MAXIMISE = 20,                  /* MAXIMISE  */
  YYSYMBOL_UNDEFINED = 21,                 /* UNDEFINED  */
  YYSYMBOL_YYACCEPT = 22,                  /* $accept  */
  YYSYMBOL_inputfile = 23,                 /* inputfile  */
  YYSYMBOL_24_1 = 24,                      /* $@1  */
  YYSYMBOL_constraints = 25,               /* constraints  */
  YYSYMBOL_x_constraints = 26,             /* x_constraints  */
  YYSYMBOL_constraint = 27,                /* constraint  */
  YYSYMBOL_28_2 = 28,                      /* $@2  */
  YYSYMBOL_RE_OP = 29,                     /* RE_OP  */
  YYSYMBOL_real_constraint = 30,           /* real_constraint  */
  YYSYMBOL_31_3 = 31,                      /* $@3  */
  YYSYMBOL_32_4 = 32,                      /* $@4  */
  YYSYMBOL_33_5 = 33,                      /* $@5  */
  YYSYMBOL_optionalrange = 34,             /* optionalrange  */
  YYSYMBOL_35_6 = 35,                      /* $@6  */
  YYSYMBOL_36_7 = 36,                      /* $@7  */
  YYSYMBOL_xxx_lineair_sum = 37,           /* xxx_lineair_sum  */
  YYSYMBOL_38_8 = 38,                      /* $@8  */
  YYSYMBOL_cons_term = 39,                 /* cons_term  */
  YYSYMBOL_int_sec_sos_declarations = 40,  /* int_sec_sos_declarations  */
  YYSYMBOL_real_int_sec_sos_decls = 41,    /* real_int_sec_sos_decls  */
  YYSYMBOL_SEC_INT_SEC_SOS = 42,           /* SEC_INT_SEC_SOS  */
  YYSYMBOL_int_sec_sos_declaration = 43,   /* int_sec_sos_declaration  */
  YYSYMBOL_44_9 = 44,                      /* $@9  */
  YYSYMBOL_xx_int_sec_sos_declaration = 45, /* xx_int_sec_sos_declaration  */
  YYSYMBOL_46_10 = 46,                     /* $@10  */
  YYSYMBOL_x_int_sec_sos_declaration = 47, /* x_int_sec_sos_declaration  */
  YYSYMBOL_optionalsos = 48,               /* optionalsos  */
  YYSYMBOL_49_11 = 49,                     /* $@11  */
  YYSYMBOL_optionalsostype = 50,           /* optionalsostype  */
  YYSYMBOL_51_12 = 51,                     /* $@12  */
  YYSYMBOL_optionalSOSweight = 52,         /* optionalSOSweight  */
  YYSYMBOL_vars = 53,                      /* vars  */
  YYSYMBOL_x_vars = 54,                    /* x_vars  */
  YYSYMBOL_optionalcomma = 55,             /* optionalcomma  */
  YYSYMBOL_variable = 56,                  /* variable  */
  YYSYMBOL_variablecolon = 57,             /* variablecolon  */
  YYSYMBOL_intcons = 58,                   /* intcons  */
  YYSYMBOL_sosdescr = 59,                  /* sosdescr  */
  YYSYMBOL_onevarwithoptionalweight = 60,  /* onevarwithoptionalweight  */
  YYSYMBOL_INTCONSorVARIABLE = 61,         /* INTCONSorVARIABLE  */
  YYSYMBOL_x_onevarwithoptionalweight = 62, /* x_onevarwithoptionalweight  */
  YYSYMBOL_x_lineair_sum = 63,             /* x_lineair_sum  */
  YYSYMBOL_64_13 = 64,                     /* $@13  */
  YYSYMBOL_xxxx_lineair_sum = 65,          /* xxxx_lineair_sum  */
  YYSYMBOL_REALCONS = 66,                  /* REALCONS  */
  YYSYMBOL_xx_lineair_term = 67,           /* xx_lineair_term  */
  YYSYMBOL_x_lineair_term = 68,            /* x_lineair_term  */
  YYSYMBOL_lineair_sum = 69,               /* lineair_sum  */
  YYSYMBOL_x_SIGN = 70,                    /* x_SIGN  */
  YYSYMBOL_lineair_term = 71,              /* lineair_term  */
  YYSYMBOL_72_14 = 72,                     /* $@14  */
  YYSYMBOL_optional_AR_M_OP = 73,          /* optional_AR_M_OP  */
  YYSYMBOL_VAR_STORE = 74,                 /* VAR_STORE  */
  YYSYMBOL_VARIABLE = 75,                  /* VARIABLE  */
  YYSYMBOL_xx_lineair_sum = 76,            /* xx_lineair_sum  */
  YYSYMBOL_objective_function = 77,        /* objective_function  */
  YYSYMBOL_real_of = 78,                   /* real_of  */
  YYSYMBOL_optional_lineair_sum = 79,      /* optional_lineair_sum  */
  YYSYMBOL_RHS_STORE = 80                  /* RHS_STORE  */
};
typedef enum lp_yysymbol_kind_t lp_yysymbol_kind_t;




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
typedef __INT_LEAST8_TYPE__ lp_yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t lp_yytype_int8;
#else
typedef signed char lp_yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ lp_yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t lp_yytype_int16;
#else
typedef short lp_yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ lp_yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t lp_yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char lp_yytype_uint8;
#else
typedef short lp_yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ lp_yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t lp_yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short lp_yytype_uint16;
#else
typedef int lp_yytype_uint16;
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
typedef lp_yytype_int8 lp_yy_state_t;

/* State numbers in computations.  */
typedef int lp_yy_state_fast_t;

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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about lp_yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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

#if !defined lp_yyoverflow

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
#endif /* !defined lp_yyoverflow */

#if (! defined lp_yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union lp_yyalloc
{
  lp_yy_state_t lp_yyss_alloc;
  YYSTYPE lp_yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union lp_yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (lp_yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T lp_yynewbytes;                                         \
        YYCOPY (&lp_yyptr->Stack_alloc, Stack, lp_yysize);                    \
        Stack = &lp_yyptr->Stack_alloc;                                    \
        lp_yynewbytes = lp_yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        lp_yyptr += lp_yynewbytes / YYSIZEOF (*lp_yyptr);                        \
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
          YYPTRDIFF_T lp_yyi;                      \
          for (lp_yyi = 0; lp_yyi < (Count); lp_yyi++)   \
            (Dst)[lp_yyi] = (Src)[lp_yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   104

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  91
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  115

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   276


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by lp_yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (lp_yysymbol_kind_t, lp_yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by lp_yylex.  */
static const lp_yytype_int8 lp_yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const lp_yytype_int16 lp_yyrline[] =
{
       0,    67,    67,    67,    79,    80,    83,    84,    88,    90,
      89,    98,    98,   102,   107,   114,   101,   153,   159,   169,
     158,   194,   196,   195,   202,   204,   210,   211,   214,   215,
     218,   218,   218,   223,   222,   230,   230,   252,   253,   256,
     258,   257,   265,   273,   271,   288,   291,   298,   299,   302,
     303,   308,   309,   313,   340,   358,   382,   397,   399,   404,
     406,   411,   413,   420,   420,   438,   439,   443,   443,   446,
     450,   457,   494,   496,   502,   506,   514,   513,   518,   524,
     525,   529,   543,   547,   556,   562,   566,   570,   573,   584,
     585,   589
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (lp_yysymbol_kind_t, lp_yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *lp_yysymbol_name (lp_yysymbol_kind_t lp_yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const lp_yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "VAR", "CONS",
  "INTCONS", "VARIABLECOLON", "INF", "SEC_INT", "SEC_SEC", "SEC_SOS",
  "SOSDESCR", "SIGN", "AR_M_OP", "RE_OPLE", "RE_OPGE", "END_C", "COMMA",
  "COLON", "MINIMISE", "MAXIMISE", "UNDEFINED", "$accept", "inputfile",
  "$@1", "constraints", "x_constraints", "constraint", "$@2", "RE_OP",
  "real_constraint", "$@3", "$@4", "$@5", "optionalrange", "$@6", "$@7",
  "xxx_lineair_sum", "$@8", "cons_term", "int_sec_sos_declarations",
  "real_int_sec_sos_decls", "SEC_INT_SEC_SOS", "int_sec_sos_declaration",
  "$@9", "xx_int_sec_sos_declaration", "$@10", "x_int_sec_sos_declaration",
  "optionalsos", "$@11", "optionalsostype", "$@12", "optionalSOSweight",
  "vars", "x_vars", "optionalcomma", "variable", "variablecolon",
  "intcons", "sosdescr", "onevarwithoptionalweight", "INTCONSorVARIABLE",
  "x_onevarwithoptionalweight", "x_lineair_sum", "$@13",
  "xxxx_lineair_sum", "REALCONS", "xx_lineair_term", "x_lineair_term",
  "lineair_sum", "x_SIGN", "lineair_term", "$@14", "optional_AR_M_OP",
  "VAR_STORE", "VARIABLE", "xx_lineair_sum", "objective_function",
  "real_of", "optional_lineair_sum", "RHS_STORE", YY_NULLPTR
};

static const char *
lp_yysymbol_name (lp_yysymbol_kind_t lp_yysymbol)
{
  return lp_yytname[lp_yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const lp_yytype_int16 lp_yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276
};
#endif

#define YYPACT_NINF (-47)

#define lp_yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-91)

#define lp_yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const lp_yytype_int8 lp_yypact[] =
{
     -47,    49,    22,   -47,   -47,    34,    34,    44,    40,     3,
     -47,    17,   -47,   -47,    40,   -47,   -47,   -47,    48,   -47,
     -47,   -47,   -47,    55,    21,   -47,   -47,   -47,   -47,    60,
     -47,   -47,   -47,   -47,    70,   -47,    33,   -47,   -47,   -47,
     -47,   -47,    55,   -47,   -47,   -47,    39,   -47,    19,    52,
     -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,    70,
     -47,   -47,   -47,   -47,   -47,    63,     6,   -47,    68,   -47,
       1,   -47,   -47,   -47,   -47,    62,     2,   -47,   -47,    52,
     -47,    72,    73,    66,   -47,     1,   -47,   -47,    67,   -47,
      56,   -47,   -47,   -47,   -47,    13,   -47,   -47,   -47,   -47,
     -47,    61,   -47,   -47,    64,    75,   -47,    76,   -47,   -47,
     -47,   -47,   -47,   -47,   -47
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const lp_yytype_int8 lp_yydefact[] =
{
       2,     0,    74,     1,    75,    74,    74,    74,     0,    63,
      87,     0,    86,    85,     0,    82,    68,    67,    79,    72,
      76,     9,    22,    26,    63,     6,     8,    84,    21,    74,
      13,    88,    73,    80,     0,    81,    63,    91,    30,    31,
      32,     3,    27,    33,    28,     7,    74,    65,    79,     0,
      81,    77,    10,    23,    29,    35,    66,    69,    71,     0,
      11,    12,    14,    78,    37,    39,    35,    70,    63,    40,
      47,    38,    15,    56,    54,    42,    48,    49,    53,    17,
      41,    56,     0,     0,    52,     0,    57,    18,     0,    55,
       0,    58,    43,    36,    50,    74,    16,    59,    54,    60,
      53,    45,    25,    19,     0,     0,    61,     0,    44,    91,
      24,    55,    46,    20,    62
};

  /* YYPGOTO[NTERM-NUM].  */
static const lp_yytype_int8 lp_yypgoto[] =
{
     -47,   -47,   -47,   -47,   -47,    69,   -47,     5,    50,   -47,
     -47,   -47,   -47,   -47,   -47,    23,   -47,   -47,   -47,   -47,
     -47,    43,   -47,    24,   -47,   -47,   -47,   -47,   -47,   -47,
     -47,   -47,   -47,   -47,   -13,    -9,   -19,    14,     9,   -47,
     -47,   -47,   -47,   -47,   -46,   -47,    51,   -47,    -7,    82,
     -47,    53,    54,   -33,   -47,   -47,    65,   -47,   -11
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const lp_yytype_int8 lp_yydefgoto[] =
{
      -1,     1,     2,    23,    24,    25,    36,    62,    26,    49,
      68,    79,    88,    95,   109,    27,    37,   103,    41,    42,
      43,    44,    55,    64,    65,    66,    70,    73,    83,   101,
     108,    75,    76,    85,    86,    81,    97,    80,    77,    91,
      99,    28,    29,    46,    18,    58,    47,     7,     8,    19,
      35,    34,    51,    20,    30,     9,    10,    11,    53
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const lp_yytype_int8 lp_yytable[] =
{
      14,    50,    57,    -4,    15,   -51,   -34,    74,   -51,    21,
      22,    -4,    -4,    -4,   -34,   -34,   -34,   -83,   -83,    84,
     102,    -5,    48,    16,    17,     4,    67,    21,    22,    -5,
      -5,    -5,    33,    31,     4,   -83,   -83,    78,   -89,    48,
      22,     5,     6,    15,    16,    17,     4,   -83,   -83,     3,
     -89,     4,    78,   -64,   -64,   -64,     4,   100,   110,    15,
     -90,    33,    98,    38,    39,    40,    60,    61,    16,    17,
      12,    13,     4,    15,    69,    22,    82,    89,    92,   107,
     111,   112,    93,    96,    87,    54,    52,   106,   104,   105,
      71,    72,   114,    45,    94,    90,    32,    56,   113,     0,
       0,    59,     0,     0,    63
};

static const lp_yytype_int8 lp_yycheck[] =
{
       7,    34,    48,     0,     3,     3,     0,     6,     6,     6,
       7,     8,     9,    10,     8,     9,    10,    14,    15,    17,
       7,     0,    29,     4,     5,    12,    59,     6,     7,     8,
       9,    10,    13,    16,    12,    14,    15,    70,    16,    46,
       7,    19,    20,     3,     4,     5,    12,    14,    15,     0,
      16,    12,    85,    14,    15,    16,    12,    90,   104,     3,
      16,    13,     6,     8,     9,    10,    14,    15,     4,     5,
       5,     6,    12,     3,    11,     7,    14,     5,     5,    18,
       5,     5,    16,    16,    79,    42,    36,   100,    95,    98,
      66,    68,   111,    24,    85,    81,    14,    46,   109,    -1,
      -1,    48,    -1,    -1,    50
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const lp_yytype_int8 lp_yystos[] =
{
       0,    23,    24,     0,    12,    19,    20,    69,    70,    77,
      78,    79,    78,    78,    70,     3,     4,     5,    66,    71,
      75,     6,     7,    25,    26,    27,    30,    37,    63,    64,
      76,    16,    71,    13,    73,    72,    28,    38,     8,     9,
      10,    40,    41,    42,    43,    27,    65,    68,    70,    31,
      75,    74,    30,    80,    43,    44,    68,    66,    67,    73,
      14,    15,    29,    74,    45,    46,    47,    75,    32,    11,
      48,    45,    37,    49,     6,    53,    54,    60,    75,    33,
      59,    57,    14,    50,    17,    55,    56,    29,    34,     5,
      59,    61,     5,    16,    60,    35,    16,    58,     6,    62,
      75,    51,     7,    39,    70,    57,    56,    18,    52,    36,
      66,     5,     5,    80,    58
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const lp_yytype_int8 lp_yyr1[] =
{
       0,    22,    24,    23,    25,    25,    26,    26,    27,    28,
      27,    29,    29,    31,    32,    33,    30,    34,    35,    36,
      34,    37,    38,    37,    39,    39,    40,    40,    41,    41,
      42,    42,    42,    44,    43,    46,    45,    47,    47,    48,
      49,    48,    50,    51,    50,    52,    52,    53,    53,    54,
      54,    55,    55,    56,    57,    58,    59,    60,    60,    61,
      61,    62,    62,    64,    63,    65,    65,    66,    66,    67,
      67,    68,    69,    69,    70,    70,    72,    71,    71,    73,
      73,    74,    75,    76,    76,    77,    77,    77,    78,    79,
      79,    80
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const lp_yytype_int8 lp_yyr2[] =
{
       0,     2,     0,     4,     0,     1,     1,     2,     1,     0,
       3,     1,     1,     0,     0,     0,     8,     0,     0,     0,
       5,     1,     0,     3,     2,     1,     0,     1,     1,     2,
       1,     1,     1,     0,     3,     0,     5,     1,     2,     0,
       0,     3,     0,     0,     4,     0,     2,     0,     1,     1,
       3,     0,     1,     0,     0,     0,     0,     2,     3,     2,
       2,     2,     4,     0,     2,     1,     2,     1,     1,     1,
       2,     2,     2,     3,     0,     1,     0,     3,     4,     0,
       1,     0,     1,     0,     1,     2,     2,     1,     2,     0,
       1,     0
};


enum { YYENOMEM = -2 };

#define lp_yyerrok         (lp_yyerrstatus = 0)
#define lp_yyclearin       (lp_yychar = YYEMPTY)

#define YYACCEPT        goto lp_yyacceptlab
#define YYABORT         goto lp_yyabortlab
#define YYERROR         goto lp_yyerrorlab


#define YYRECOVERING()  (!!lp_yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (lp_yychar == YYEMPTY)                                        \
      {                                                           \
        lp_yychar = (Token);                                         \
        lp_yylval = (Value);                                         \
        YYPOPSTACK (lp_yylen);                                       \
        lp_yystate = *lp_yyssp;                                         \
        goto lp_yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        lp_yyerror (YY_("syntax error: cannot back up")); \
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
  if (lp_yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (lp_yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      lp_yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
lp_yy_symbol_value_print (FILE *lp_yyo,
                       lp_yysymbol_kind_t lp_yykind, YYSTYPE const * const lp_yyvaluep)
{
  FILE *lp_yyoutput = lp_yyo;
  YYUSE (lp_yyoutput);
  if (!lp_yyvaluep)
    return;
# ifdef YYPRINT
  if (lp_yykind < YYNTOKENS)
    YYPRINT (lp_yyo, lp_yytoknum[lp_yykind], *lp_yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (lp_yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
lp_yy_symbol_print (FILE *lp_yyo,
                 lp_yysymbol_kind_t lp_yykind, YYSTYPE const * const lp_yyvaluep)
{
  YYFPRINTF (lp_yyo, "%s %s (",
             lp_yykind < YYNTOKENS ? "token" : "nterm", lp_yysymbol_name (lp_yykind));

  lp_yy_symbol_value_print (lp_yyo, lp_yykind, lp_yyvaluep);
  YYFPRINTF (lp_yyo, ")");
}

/*------------------------------------------------------------------.
| lp_yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
lp_yy_stack_print (lp_yy_state_t *lp_yybottom, lp_yy_state_t *lp_yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; lp_yybottom <= lp_yytop; lp_yybottom++)
    {
      int lp_yybot = *lp_yybottom;
      YYFPRINTF (stderr, " %d", lp_yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (lp_yydebug)                                                  \
    lp_yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
lp_yy_reduce_print (lp_yy_state_t *lp_yyssp, YYSTYPE *lp_yyvsp,
                 int lp_yyrule)
{
  int lp_yylno = lp_yyrline[lp_yyrule];
  int lp_yynrhs = lp_yyr2[lp_yyrule];
  int lp_yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             lp_yyrule - 1, lp_yylno);
  /* The symbols being reduced.  */
  for (lp_yyi = 0; lp_yyi < lp_yynrhs; lp_yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", lp_yyi + 1);
      lp_yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+lp_yyssp[lp_yyi + 1 - lp_yynrhs]),
                       &lp_yyvsp[(lp_yyi + 1) - (lp_yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (lp_yydebug)                          \
    lp_yy_reduce_print (lp_yyssp, lp_yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int lp_yydebug;
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
lp_yydestruct (const char *lp_yymsg,
            lp_yysymbol_kind_t lp_yykind, YYSTYPE *lp_yyvaluep)
{
  YYUSE (lp_yyvaluep);
  if (!lp_yymsg)
    lp_yymsg = "Deleting";
  YY_SYMBOL_PRINT (lp_yymsg, lp_yykind, lp_yyvaluep, lp_yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (lp_yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int lp_yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE lp_yylval;
/* Number of syntax errors so far.  */
int lp_yynerrs;




/*----------.
| lp_yyparse.  |
`----------*/

int
lp_yyparse (void)
{
    lp_yy_state_fast_t lp_yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int lp_yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow lp_yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T lp_yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    lp_yy_state_t lp_yyssa[YYINITDEPTH];
    lp_yy_state_t *lp_yyss = lp_yyssa;
    lp_yy_state_t *lp_yyssp = lp_yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE lp_yyvsa[YYINITDEPTH];
    YYSTYPE *lp_yyvs = lp_yyvsa;
    YYSTYPE *lp_yyvsp = lp_yyvs;

  int lp_yyn;
  /* The return value of lp_yyparse.  */
  int lp_yyresult;
  /* Lookahead symbol kind.  */
  lp_yysymbol_kind_t lp_yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE lp_yyval;



#define YYPOPSTACK(N)   (lp_yyvsp -= (N), lp_yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int lp_yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  lp_yychar = YYEMPTY; /* Cause a token to be read.  */
  goto lp_yysetstate;


/*------------------------------------------------------------.
| lp_yynewstate -- push a new state, which is found in lp_yystate.  |
`------------------------------------------------------------*/
lp_yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  lp_yyssp++;


/*--------------------------------------------------------------------.
| lp_yysetstate -- set current state (the top of the stack) to lp_yystate.  |
`--------------------------------------------------------------------*/
lp_yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", lp_yystate));
  YY_ASSERT (0 <= lp_yystate && lp_yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *lp_yyssp = YY_CAST (lp_yy_state_t, lp_yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (lp_yyss, lp_yyssp);

  if (lp_yyss + lp_yystacksize - 1 <= lp_yyssp)
#if !defined lp_yyoverflow && !defined YYSTACK_RELOCATE
    goto lp_yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T lp_yysize = lp_yyssp - lp_yyss + 1;

# if defined lp_yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        lp_yy_state_t *lp_yyss1 = lp_yyss;
        YYSTYPE *lp_yyvs1 = lp_yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if lp_yyoverflow is a macro.  */
        lp_yyoverflow (YY_("memory exhausted"),
                    &lp_yyss1, lp_yysize * YYSIZEOF (*lp_yyssp),
                    &lp_yyvs1, lp_yysize * YYSIZEOF (*lp_yyvsp),
                    &lp_yystacksize);
        lp_yyss = lp_yyss1;
        lp_yyvs = lp_yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= lp_yystacksize)
        goto lp_yyexhaustedlab;
      lp_yystacksize *= 2;
      if (YYMAXDEPTH < lp_yystacksize)
        lp_yystacksize = YYMAXDEPTH;

      {
        lp_yy_state_t *lp_yyss1 = lp_yyss;
        union lp_yyalloc *lp_yyptr =
          YY_CAST (union lp_yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (lp_yystacksize))));
        if (! lp_yyptr)
          goto lp_yyexhaustedlab;
        YYSTACK_RELOCATE (lp_yyss_alloc, lp_yyss);
        YYSTACK_RELOCATE (lp_yyvs_alloc, lp_yyvs);
#  undef YYSTACK_RELOCATE
        if (lp_yyss1 != lp_yyssa)
          YYSTACK_FREE (lp_yyss1);
      }
# endif

      lp_yyssp = lp_yyss + lp_yysize - 1;
      lp_yyvsp = lp_yyvs + lp_yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, lp_yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (lp_yyss + lp_yystacksize - 1 <= lp_yyssp)
        YYABORT;
    }
#endif /* !defined lp_yyoverflow && !defined YYSTACK_RELOCATE */

  if (lp_yystate == YYFINAL)
    YYACCEPT;

  goto lp_yybackup;


/*-----------.
| lp_yybackup.  |
`-----------*/
lp_yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  lp_yyn = lp_yypact[lp_yystate];
  if (lp_yypact_value_is_default (lp_yyn))
    goto lp_yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (lp_yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      lp_yychar = lp_yylex ();
    }

  if (lp_yychar <= YYEOF)
    {
      lp_yychar = YYEOF;
      lp_yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (lp_yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      lp_yychar = YYUNDEF;
      lp_yytoken = YYSYMBOL_YYerror;
      goto lp_yyerrlab1;
    }
  else
    {
      lp_yytoken = YYTRANSLATE (lp_yychar);
      YY_SYMBOL_PRINT ("Next token is", lp_yytoken, &lp_yylval, &lp_yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  lp_yyn += lp_yytoken;
  if (lp_yyn < 0 || YYLAST < lp_yyn || lp_yycheck[lp_yyn] != lp_yytoken)
    goto lp_yydefault;
  lp_yyn = lp_yytable[lp_yyn];
  if (lp_yyn <= 0)
    {
      if (lp_yytable_value_is_error (lp_yyn))
        goto lp_yyerrlab;
      lp_yyn = -lp_yyn;
      goto lp_yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (lp_yyerrstatus)
    lp_yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", lp_yytoken, &lp_yylval, &lp_yylloc);
  lp_yystate = lp_yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++lp_yyvsp = lp_yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  lp_yychar = YYEMPTY;
  goto lp_yynewstate;


/*-----------------------------------------------------------.
| lp_yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
lp_yydefault:
  lp_yyn = lp_yydefact[lp_yystate];
  if (lp_yyn == 0)
    goto lp_yyerrlab;
  goto lp_yyreduce;


/*-----------------------------.
| lp_yyreduce -- do a reduction.  |
`-----------------------------*/
lp_yyreduce:
  /* lp_yyn is the number of a rule to reduce with.  */
  lp_yylen = lp_yyr2[lp_yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  lp_yyval = lp_yyvsp[1-lp_yylen];


  YY_REDUCE_PRINT (lp_yyn);
  switch (lp_yyn)
    {
  case 2: /* $@1: %empty  */
#line 67 "lp.y"
{
  isign = 0;
  make_neg = 0;
  Sign = 0;
  HadConstraint = FALSE;
  HadVar = HadVar0 = FALSE;
}
#line 1365 "y.tab.c"
    break;

  case 9: /* $@2: %empty  */
#line 90 "lp.y"
{
  if(!add_constraint_name(Last_var))
    YYABORT;
  HadConstraint = TRUE;
}
#line 1375 "y.tab.c"
    break;

  case 13: /* $@3: %empty  */
#line 102 "lp.y"
{
  HadVar1 = HadVar0;
  HadVar0 = FALSE;
}
#line 1384 "y.tab.c"
    break;

  case 14: /* $@4: %empty  */
#line 107 "lp.y"
{
  if(!store_re_op((char *) lp_yytext, HadConstraint, HadVar, Had_lineair_sum))
    YYABORT;
  make_neg = 1;
  f1 = 0;
}
#line 1395 "y.tab.c"
    break;

  case 15: /* $@5: %empty  */
#line 114 "lp.y"
{
  Had_lineair_sum0 = Had_lineair_sum;
  Had_lineair_sum = TRUE;
  HadVar2 = HadVar0;
  HadVar0 = FALSE;
  do_add_row = FALSE;
  if(HadConstraint && !HadVar ) {
    /* it is a range */
    /* already handled */
  }
  else if(!HadConstraint && HadVar) {
    /* it is a bound */

    if(!store_bounds(TRUE))
      YYABORT;
  }
  else {
    /* it is a row restriction */
    do_add_row = TRUE;
  }
}
#line 1421 "y.tab.c"
    break;

  case 16: /* real_constraint: xx_lineair_sum $@3 RE_OP $@4 xxx_lineair_sum $@5 optionalrange END_C  */
#line 137 "lp.y"
{
  if((!HadVar) && (!HadConstraint)) {
    lp_yyerror("parse error");
    YYABORT;
  }
  if(do_add_row)
    add_row();
  HadConstraint = FALSE;
  HadVar = HadVar0 = FALSE;
  isign = 0;
  make_neg = 0;
  null_tmp_store(TRUE);
}
#line 1439 "y.tab.c"
    break;

  case 17: /* optionalrange: %empty  */
#line 153 "lp.y"
{
  if((!HadVar1) && (Had_lineair_sum0))
    if(!negate_constraint())
      YYABORT;
}
#line 1449 "y.tab.c"
    break;

  case 18: /* $@6: %empty  */
#line 159 "lp.y"
{
  make_neg = 0;
  isign = 0;
  if(HadConstraint)
    HadVar = Had_lineair_sum = FALSE;
  HadVar0 = FALSE;
  if(!store_re_op((*lp_yytext == '<') ? ">" : (*lp_yytext == '>') ? "<" : (char *) lp_yytext, HadConstraint, HadVar, Had_lineair_sum))
    YYABORT;
}
#line 1463 "y.tab.c"
    break;

  case 19: /* $@7: %empty  */
#line 169 "lp.y"
{
  f -= f1;
}
#line 1471 "y.tab.c"
    break;

  case 20: /* optionalrange: RE_OP $@6 cons_term $@7 RHS_STORE  */
#line 173 "lp.y"
{
  if((HadVar1) || (!HadVar2) || (HadVar0)) {
    lp_yyerror("parse error");
    YYABORT;
  }

  if(HadConstraint && !HadVar ) {
    /* it is a range */
    /* already handled */
    if(!negate_constraint())
      YYABORT;
  }
  else if(!HadConstraint && HadVar) {
    /* it is a bound */

    if(!store_bounds(TRUE))
      YYABORT;
  }
}
#line 1495 "y.tab.c"
    break;

  case 22: /* $@8: %empty  */
#line 196 "lp.y"
{
  isign = Sign;
}
#line 1503 "y.tab.c"
    break;

  case 25: /* cons_term: INF  */
#line 205 "lp.y"
{
  isign = Sign;
}
#line 1511 "y.tab.c"
    break;

  case 33: /* $@9: %empty  */
#line 223 "lp.y"
{
  Within_sos_decl1 = Within_sos_decl;
}
#line 1519 "y.tab.c"
    break;

  case 35: /* $@10: %empty  */
#line 230 "lp.y"
{
  if((!Within_int_decl) && (!Within_sec_decl) && (!Within_sos_decl1)) {
    lp_yyerror("parse error");
    YYABORT;
  }
  SOStype = SOStype0;
  check_int_sec_sos_decl(Within_int_decl, Within_sec_decl, Within_sos_decl1 = (Within_sos_decl1 ? 1 : 0));
}
#line 1532 "y.tab.c"
    break;

  case 36: /* xx_int_sec_sos_declaration: $@10 optionalsos vars optionalsostype END_C  */
#line 242 "lp.y"
{
  if((Within_sos_decl1) && (SOStype == 0))
  {
    lp_yyerror("Unsupported SOS type (0)");
    YYABORT;
  }
}
#line 1544 "y.tab.c"
    break;

  case 40: /* $@11: %empty  */
#line 258 "lp.y"
{
  strcpy(Last_var0, Last_var);
}
#line 1552 "y.tab.c"
    break;

  case 42: /* optionalsostype: %empty  */
#line 265 "lp.y"
{
  if(Within_sos_decl1) {
    set_sos_type(SOStype);
    set_sos_weight(SOSweight, 1);
  }
}
#line 1563 "y.tab.c"
    break;

  case 43: /* $@12: %empty  */
#line 273 "lp.y"
{
  if((Within_sos_decl1) && (!SOStype))
  {
    set_sos_type(SOStype = (short) (f + .1));
  }
  else
  {
    lp_yyerror("SOS type not expected");
    YYABORT;
  }
}
#line 1579 "y.tab.c"
    break;

  case 45: /* optionalSOSweight: %empty  */
#line 288 "lp.y"
{
  set_sos_weight(SOSweight, 1);
}
#line 1587 "y.tab.c"
    break;

  case 46: /* optionalSOSweight: COLON INTCONS  */
#line 293 "lp.y"
{
  set_sos_weight((int) (f + .1), 1);
}
#line 1595 "y.tab.c"
    break;

  case 53: /* variable: %empty  */
#line 313 "lp.y"
{ /* peno1 */
  if(Within_sos_decl1 == 1)
  {
    char buf[16];

    SOSweight++;
    sprintf(buf, "SOS%d", SOSweight);
    storevarandweight(buf);

    check_int_sec_sos_decl(Within_int_decl, Within_sec_decl, 2);
    Within_sos_decl1 = 2;
    weight = 0;
    SOSNr = 0;
  }

  storevarandweight(Last_var);

  if(Within_sos_decl1 == 2)
  {
    SOSNr++;
    weight = SOSNr;
    set_sos_weight(weight, 2);
  }
}
#line 1624 "y.tab.c"
    break;

  case 54: /* variablecolon: %empty  */
#line 340 "lp.y"
{ /* peno2 */
  if(!Within_sos_decl1) {
    lp_yyerror("parse error");
    YYABORT;
  }
  if(Within_sos_decl1 == 1)
    strcpy(Last_var0, Last_var);
  if(Within_sos_decl1 == 2)
  {
    storevarandweight(Last_var);  
    SOSNr++;
    weight = SOSNr;
    set_sos_weight(weight, 2);
  }
}
#line 1644 "y.tab.c"
    break;

  case 55: /* intcons: %empty  */
#line 358 "lp.y"
{ /* peno3 */
  if(Within_sos_decl1 == 1)
  {
    char buf[16];

    SOSweight++;
    sprintf(buf, "SOS%d", SOSweight);
    storevarandweight(buf);

    check_int_sec_sos_decl(Within_int_decl, Within_sec_decl, 2);
    Within_sos_decl1 = 2;
    weight = 0;
    SOSNr = 0;

    storevarandweight(Last_var0);  
    SOSNr++;
  }

  weight = (int) (f + .1);
  set_sos_weight(weight, 2);
}
#line 1670 "y.tab.c"
    break;

  case 56: /* sosdescr: %empty  */
#line 382 "lp.y"
{ /* SOS name */ /* peno4 */
  if(Within_sos_decl1 == 1)
  {
    storevarandweight(Last_var0);
    set_sos_type(SOStype);
    check_int_sec_sos_decl(Within_int_decl, Within_sec_decl, 2);
    Within_sos_decl1 = 2;
    weight = 0;
    SOSNr = 0;
    SOSweight++;
  }
}
#line 1687 "y.tab.c"
    break;

  case 63: /* $@13: %empty  */
#line 420 "lp.y"
{
  state0 = 0;
}
#line 1695 "y.tab.c"
    break;

  case 64: /* x_lineair_sum: $@13 xxxx_lineair_sum  */
#line 424 "lp.y"
{
  if (state == 1) {
    /* RHS_STORE */
    if (    (isign0 || !make_neg)
        && !(isign0 && !make_neg)) /* but not both! */
      f0 = -f0;
    if(make_neg)
      f1 += f0;
    if(!rhs_store(f0, HadConstraint, HadVar, Had_lineair_sum))
      YYABORT;
  }
}
#line 1712 "y.tab.c"
    break;

  case 69: /* xx_lineair_term: REALCONS  */
#line 447 "lp.y"
{
  state = 1;
}
#line 1720 "y.tab.c"
    break;

  case 70: /* xx_lineair_term: optional_AR_M_OP VARIABLE  */
#line 452 "lp.y"
{
  state = 2;
}
#line 1728 "y.tab.c"
    break;

  case 71: /* x_lineair_term: x_SIGN xx_lineair_term  */
#line 459 "lp.y"
{
  if ((HadSign || state == 1) && (state0 == 1)) {
    /* RHS_STORE */
    if (    (isign0 || !make_neg)
        && !(isign0 && !make_neg)) /* but not both! */
      f0 = -f0;
    if(make_neg)
      f1 += f0;
    if(!rhs_store(f0, HadConstraint, HadVar, Had_lineair_sum))
      YYABORT;
  }
  if (state == 1) {
    f0 = f;
    isign0 = isign;
  }
  if (state == 2) {
    if((HadSign) || (state0 != 1)) {
     isign0 = isign;
     f0 = 1.0;
    }
    /* VAR_STORE */
    if (    (isign0 || make_neg)
        && !(isign0 && make_neg)) /* but not both! */
      f0 = -f0;
    if(!var_store(Last_var, f0, HadConstraint, HadVar, Had_lineair_sum)) {
      lp_yyerror("var_store failed");
      YYABORT;
    }
    HadConstraint |= HadVar;
    HadVar = HadVar0 = TRUE;
  }
  state0 = state;
}
#line 1766 "y.tab.c"
    break;

  case 74: /* x_SIGN: %empty  */
#line 502 "lp.y"
{
  isign = 0;
  HadSign = FALSE;
}
#line 1775 "y.tab.c"
    break;

  case 75: /* x_SIGN: SIGN  */
#line 507 "lp.y"
{
  isign = Sign;
  HadSign = TRUE;
}
#line 1784 "y.tab.c"
    break;

  case 76: /* $@14: %empty  */
#line 514 "lp.y"
{
  f = 1.0;
}
#line 1792 "y.tab.c"
    break;

  case 81: /* VAR_STORE: %empty  */
#line 529 "lp.y"
{
  if (    (isign || make_neg)
      && !(isign && make_neg)) /* but not both! */
    f = -f;
  if(!var_store(Last_var, f, HadConstraint, HadVar, Had_lineair_sum)) {
    lp_yyerror("var_store failed");
    YYABORT;
  }
  HadConstraint |= HadVar;
  HadVar = HadVar0 = TRUE;
  isign = 0;
}
#line 1809 "y.tab.c"
    break;

  case 83: /* xx_lineair_sum: %empty  */
#line 547 "lp.y"
{
  /* to allow a range */
  /* constraint: < max */
  if(!HadConstraint) {
    lp_yyerror("parse error");
    YYABORT;
  }
  Had_lineair_sum = FALSE;
}
#line 1823 "y.tab.c"
    break;

  case 84: /* xx_lineair_sum: xxx_lineair_sum  */
#line 557 "lp.y"
{
  Had_lineair_sum = TRUE;
}
#line 1831 "y.tab.c"
    break;

  case 85: /* objective_function: MAXIMISE real_of  */
#line 563 "lp.y"
{
  set_obj_dir(TRUE);
}
#line 1839 "y.tab.c"
    break;

  case 86: /* objective_function: MINIMISE real_of  */
#line 567 "lp.y"
{
  set_obj_dir(FALSE);
}
#line 1847 "y.tab.c"
    break;

  case 88: /* real_of: optional_lineair_sum END_C  */
#line 575 "lp.y"
{
  add_row();
  HadConstraint = FALSE;
  HadVar = HadVar0 = FALSE;
  isign = 0;
  make_neg = 0;
}
#line 1859 "y.tab.c"
    break;

  case 91: /* RHS_STORE: %empty  */
#line 589 "lp.y"
{
  if (    (isign || !make_neg)
      && !(isign && !make_neg)) /* but not both! */
    f = -f;
  if(!rhs_store(f, HadConstraint, HadVar, Had_lineair_sum))
    YYABORT;
  isign = 0;
}
#line 1872 "y.tab.c"
    break;


#line 1876 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter lp_yychar, and that requires
     that lp_yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of lp_yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering lp_yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (lp_yysymbol_kind_t, lp_yyr1[lp_yyn]), &lp_yyval, &lp_yyloc);

  YYPOPSTACK (lp_yylen);
  lp_yylen = 0;

  *++lp_yyvsp = lp_yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int lp_yylhs = lp_yyr1[lp_yyn] - YYNTOKENS;
    const int lp_yyi = lp_yypgoto[lp_yylhs] + *lp_yyssp;
    lp_yystate = (0 <= lp_yyi && lp_yyi <= YYLAST && lp_yycheck[lp_yyi] == *lp_yyssp
               ? lp_yytable[lp_yyi]
               : lp_yydefgoto[lp_yylhs]);
  }

  goto lp_yynewstate;


/*--------------------------------------.
| lp_yyerrlab -- here on detecting error.  |
`--------------------------------------*/
lp_yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  lp_yytoken = lp_yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (lp_yychar);
  /* If not already recovering from an error, report this error.  */
  if (!lp_yyerrstatus)
    {
      ++lp_yynerrs;
      lp_yyerror (YY_("syntax error"));
    }

  if (lp_yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (lp_yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (lp_yychar == YYEOF)
            YYABORT;
        }
      else
        {
          lp_yydestruct ("Error: discarding",
                      lp_yytoken, &lp_yylval);
          lp_yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto lp_yyerrlab1;


/*---------------------------------------------------.
| lp_yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
lp_yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label lp_yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (lp_yylen);
  lp_yylen = 0;
  YY_STACK_PRINT (lp_yyss, lp_yyssp);
  lp_yystate = *lp_yyssp;
  goto lp_yyerrlab1;


/*-------------------------------------------------------------.
| lp_yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
lp_yyerrlab1:
  lp_yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      lp_yyn = lp_yypact[lp_yystate];
      if (!lp_yypact_value_is_default (lp_yyn))
        {
          lp_yyn += YYSYMBOL_YYerror;
          if (0 <= lp_yyn && lp_yyn <= YYLAST && lp_yycheck[lp_yyn] == YYSYMBOL_YYerror)
            {
              lp_yyn = lp_yytable[lp_yyn];
              if (0 < lp_yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (lp_yyssp == lp_yyss)
        YYABORT;


      lp_yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (lp_yystate), lp_yyvsp);
      YYPOPSTACK (1);
      lp_yystate = *lp_yyssp;
      YY_STACK_PRINT (lp_yyss, lp_yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++lp_yyvsp = lp_yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (lp_yyn), lp_yyvsp, lp_yylsp);

  lp_yystate = lp_yyn;
  goto lp_yynewstate;


/*-------------------------------------.
| lp_yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
lp_yyacceptlab:
  lp_yyresult = 0;
  goto lp_yyreturn;


/*-----------------------------------.
| lp_yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
lp_yyabortlab:
  lp_yyresult = 1;
  goto lp_yyreturn;


#if !defined lp_yyoverflow
/*-------------------------------------------------.
| lp_yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
lp_yyexhaustedlab:
  lp_yyerror (YY_("memory exhausted"));
  lp_yyresult = 2;
  goto lp_yyreturn;
#endif


/*-------------------------------------------------------.
| lp_yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
lp_yyreturn:
  if (lp_yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      lp_yytoken = YYTRANSLATE (lp_yychar);
      lp_yydestruct ("Cleanup: discarding lookahead",
                  lp_yytoken, &lp_yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (lp_yylen);
  YY_STACK_PRINT (lp_yyss, lp_yyssp);
  while (lp_yyssp != lp_yyss)
    {
      lp_yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*lp_yyssp), lp_yyvsp);
      YYPOPSTACK (1);
    }
#ifndef lp_yyoverflow
  if (lp_yyss != lp_yyssa)
    YYSTACK_FREE (lp_yyss);
#endif

  return lp_yyresult;
}

#line 599 "lp.y"


static void lp_yy_delete_allocated_memory(void)
{
  /* free memory allocated by flex. Otherwise some memory is not freed.
     This is a bit tricky. There is not much documentation about this, but a lot of
     reports of memory that keeps allocated */

  /* If you get errors on this function call, just comment it. This will only result
     in some memory that is not being freed. */

# if defined YY_CURRENT_BUFFER
    /* flex defines the macro YY_CURRENT_BUFFER, so you should only get here if lex.c is
       generated by flex */
    /* lex doesn't define this macro and thus should not come here, but lex doesn't has
       this memory leak also ...*/

    lp_yy_delete_buffer(YY_CURRENT_BUFFER); /* comment this line if you have problems with it */
    lp_yy_init = 1; /* make sure that the next time memory is allocated again */
    lp_yy_start = 0;
# endif
}

static int parse(void)
{
  return(lp_yyparse());
}

lprec *read_lp(FILE *input, short verbose, char *lp_name)
{
  lp_yyin = input;
  return(yacc_read(verbose, lp_name, &lp_yylineno, parse, lp_yy_delete_allocated_memory));
}

lprec *read_LP(char *input, short verbose, char *lp_name)
{
  FILE *fpin;
  lprec *lp = NULL;

  if((fpin = fopen(input, "r")) != NULL) {
    lp = read_lp(fpin, verbose, lp_name);
    fclose(fpin);
  }
  return(lp);
}
