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
#line 1 "parser.y"

/*
 * parser - the syntax analyser of cas (yacc grammar)
 *
 * One statement per line. The actions write the statement to the intermediate file
 * (ifile.c) and keep the location counter (locntr.c); the expression nodes are
 * evaluated by the functions of expr.c. Syntax errors are reported with SYNERR()
 * and the rest of the line is skipped.
 *
 * The grammar is the one of the original. parser.c is made from this file by the yacc of WEGA
 * (mkparser.sh), not by a yacc of this host: the tables of other generators treat the states
 * in which a syntax error is found differently (default reductions), which changes the
 * message of the error.
 */
#include "cas.h"

NODE *plist = 0;		/* the labels of the statement not yet defined */
int segmode = 0;		/* .seg / .nonseg */
static unsigned fpflag = 0;	/* flags of a floating point instruction */
static unsigned fpmode = 0;	/* its rounding and infinity mode */
static int nround = 0;		/* the modes seen so far */
static int ninf = 0;
static int nameflag = 0;	/* a name was read: it is put to the symbol file */
static char errmsg[0x100];

static NODE *listp;		/* walks over a list of nodes */
static int symmode;		/* how a name is looked up: 6 local, 0 global */

static void yyerror(const char *s);


/*
 * an error in the syntax: the message, then the rest of the line is skipped. The value of the
 * rule is a node of the invalid type (synnode): the original leaves what happens to be on the stack here, and a
 * rule that takes the operand list or an expression goes on with it before the statement
 * is dropped.
 */
#define SYNERR(m) do { \
		snprintf(errmsg, sizeof errmsg, "%s\n", m); \
		error(3, errmsg); \
		while (yychar != 29 && yychar != -1 && yychar != 0) \
			yychar = yylex(); \
		yyerrflag = 0; \
		yyval.np = synnode(); \
		nfreed = nalloc; \
	} while (0)

#line 119 "y.tab.c"

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
    DOT = 3,                       /* DOT  */
    PLUS = 4,                      /* PLUS  */
    MINUS = 5,                     /* MINUS  */
    DIV = 9,                       /* DIV  */
    MUL = 10,                      /* MUL  */
    SHL = 11,                      /* SHL  */
    SHR = 12,                      /* SHR  */
    AND = 13,                      /* AND  */
    OR = 14,                       /* OR  */
    XOR = 15,                      /* XOR  */
    LSEG = 16,                     /* LSEG  */
    RSEG = 17,                     /* RSEG  */
    BAR = 18,                      /* BAR  */
    COLON = 19,                    /* COLON  */
    DCOLON = 20,                   /* DCOLON  */
    HASH = 21,                     /* HASH  */
    AT = 22,                       /* AT  */
    SSEG = 23,                     /* SSEG  */
    OSEG = 24,                     /* OSEG  */
    LBRACE = 25,                   /* LBRACE  */
    RBRACE = 26,                   /* RBRACE  */
    LPAR = 27,                     /* LPAR  */
    RPAR = 28,                     /* RPAR  */
    NL = 29,                       /* NL  */
    COMMA = 30,                    /* COMMA  */
    EQ = 31,                       /* EQ  */
    GASSIGN = 32,                  /* GASSIGN  */
    LASSIGN = 33,                  /* LASSIGN  */
    SYMBOL = 36,                   /* SYMBOL  */
    COMM = 37,                     /* COMM  */
    EXTERN = 38,                   /* EXTERN  */
    EVEN = 39,                     /* EVEN  */
    ODD = 40,                      /* ODD  */
    BYTE = 41,                     /* BYTE  */
    WORD = 42,                     /* WORD  */
    LONG = 43,                     /* LONG  */
    BLKB = 44,                     /* BLKB  */
    BLKW = 45,                     /* BLKW  */
    BLKL = 46,                     /* BLKL  */
    STABL = 47,                    /* STABL  */
    STABN = 48,                    /* STABN  */
    STABP = 49,                    /* STABP  */
    STABR = 50,                    /* STABR  */
    REG = 51,                      /* REG  */
    REGPAIR = 52,                  /* REGPAIR  */
    BREG = 53,                     /* BREG  */
    COND = 54,                     /* COND  */
    NVI = 55,                      /* NVI  */
    OPCODE = 56,                   /* OPCODE  */
    FLAGS = 58,                    /* FLAGS  */
    CTLREG = 59,                   /* CTLREG  */
    REGQUAD = 60,                  /* REGQUAD  */
    FPREG = 61,                    /* FPREG  */
    TOK62 = 62,                    /* TOK62  */
    TOK63 = 63,                    /* TOK63  */
    TOK64 = 64,                    /* TOK64  */
    TOK65 = 65,                    /* TOK65  */
    TOK66 = 66,                    /* TOK66  */
    TOK67 = 67,                    /* TOK67  */
    TOK68 = 68,                    /* TOK68  */
    TOK69 = 69,                    /* TOK69  */
    TOK70 = 70,                    /* TOK70  */
    TOK71 = 71,                    /* TOK71  */
    TOK72 = 72,                    /* TOK72  */
    FPFLAG = 73,                   /* FPFLAG  */
    FPMODE = 74,                   /* FPMODE  */
    CODE = 75,                     /* CODE  */
    DATA = 76,                     /* DATA  */
    BSS = 77,                      /* BSS  */
    SEG = 78,                      /* SEG  */
    NONSEG = 79,                   /* NONSEG  */
    JPR = 80,                      /* JPR  */
    CALLR = 81,                    /* CALLR  */
    PSEC = 82,                     /* PSEC  */
    ADDR = 83,                     /* ADDR  */
    LINE = 84,                     /* LINE  */
    NUMBER = 85,                   /* NUMBER  */
    NEWNAME = 86,                  /* NEWNAME  */
    NEWLOCAL = 87,                 /* NEWLOCAL  */
    STRING = 88                    /* STRING  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define DOT 3
#define PLUS 4
#define MINUS 5
#define DIV 9
#define MUL 10
#define SHL 11
#define SHR 12
#define AND 13
#define OR 14
#define XOR 15
#define LSEG 16
#define RSEG 17
#define BAR 18
#define COLON 19
#define DCOLON 20
#define HASH 21
#define AT 22
#define SSEG 23
#define OSEG 24
#define LBRACE 25
#define RBRACE 26
#define LPAR 27
#define RPAR 28
#define NL 29
#define COMMA 30
#define EQ 31
#define GASSIGN 32
#define LASSIGN 33
#define SYMBOL 36
#define COMM 37
#define EXTERN 38
#define EVEN 39
#define ODD 40
#define BYTE 41
#define WORD 42
#define LONG 43
#define BLKB 44
#define BLKW 45
#define BLKL 46
#define STABL 47
#define STABN 48
#define STABP 49
#define STABR 50
#define REG 51
#define REGPAIR 52
#define BREG 53
#define COND 54
#define NVI 55
#define OPCODE 56
#define FLAGS 58
#define CTLREG 59
#define REGQUAD 60
#define FPREG 61
#define TOK62 62
#define TOK63 63
#define TOK64 64
#define TOK65 65
#define TOK66 66
#define TOK67 67
#define TOK68 68
#define TOK69 69
#define TOK70 70
#define TOK71 71
#define TOK72 72
#define FPFLAG 73
#define FPMODE 74
#define CODE 75
#define DATA 76
#define BSS 77
#define SEG 78
#define NONSEG 79
#define JPR 80
#define CALLR 81
#define PSEC 82
#define ADDR 83
#define LINE 84
#define NUMBER 85
#define NEWNAME 86
#define NEWLOCAL 87
#define STRING 88

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 49 "parser.y"

	int32_t	l;
	int	i;
	char	*p;
	struct pst *pp;
	NODE	*np;

#line 341 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_DOT = 3,                        /* DOT  */
  YYSYMBOL_PLUS = 4,                       /* PLUS  */
  YYSYMBOL_MINUS = 5,                      /* MINUS  */
  YYSYMBOL_DIV = 6,                        /* DIV  */
  YYSYMBOL_MUL = 7,                        /* MUL  */
  YYSYMBOL_SHL = 8,                        /* SHL  */
  YYSYMBOL_SHR = 9,                        /* SHR  */
  YYSYMBOL_AND = 10,                       /* AND  */
  YYSYMBOL_OR = 11,                        /* OR  */
  YYSYMBOL_XOR = 12,                       /* XOR  */
  YYSYMBOL_LSEG = 13,                      /* LSEG  */
  YYSYMBOL_RSEG = 14,                      /* RSEG  */
  YYSYMBOL_BAR = 15,                       /* BAR  */
  YYSYMBOL_COLON = 16,                     /* COLON  */
  YYSYMBOL_DCOLON = 17,                    /* DCOLON  */
  YYSYMBOL_HASH = 18,                      /* HASH  */
  YYSYMBOL_AT = 19,                        /* AT  */
  YYSYMBOL_SSEG = 20,                      /* SSEG  */
  YYSYMBOL_OSEG = 21,                      /* OSEG  */
  YYSYMBOL_LBRACE = 22,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 23,                    /* RBRACE  */
  YYSYMBOL_LPAR = 24,                      /* LPAR  */
  YYSYMBOL_RPAR = 25,                      /* RPAR  */
  YYSYMBOL_NL = 26,                        /* NL  */
  YYSYMBOL_COMMA = 27,                     /* COMMA  */
  YYSYMBOL_EQ = 28,                        /* EQ  */
  YYSYMBOL_GASSIGN = 29,                   /* GASSIGN  */
  YYSYMBOL_LASSIGN = 30,                   /* LASSIGN  */
  YYSYMBOL_SYMBOL = 31,                    /* SYMBOL  */
  YYSYMBOL_COMM = 32,                      /* COMM  */
  YYSYMBOL_EXTERN = 33,                    /* EXTERN  */
  YYSYMBOL_EVEN = 34,                      /* EVEN  */
  YYSYMBOL_ODD = 35,                       /* ODD  */
  YYSYMBOL_BYTE = 36,                      /* BYTE  */
  YYSYMBOL_WORD = 37,                      /* WORD  */
  YYSYMBOL_LONG = 38,                      /* LONG  */
  YYSYMBOL_BLKB = 39,                      /* BLKB  */
  YYSYMBOL_BLKW = 40,                      /* BLKW  */
  YYSYMBOL_BLKL = 41,                      /* BLKL  */
  YYSYMBOL_STABL = 42,                     /* STABL  */
  YYSYMBOL_STABN = 43,                     /* STABN  */
  YYSYMBOL_STABP = 44,                     /* STABP  */
  YYSYMBOL_STABR = 45,                     /* STABR  */
  YYSYMBOL_REG = 46,                       /* REG  */
  YYSYMBOL_REGPAIR = 47,                   /* REGPAIR  */
  YYSYMBOL_BREG = 48,                      /* BREG  */
  YYSYMBOL_COND = 49,                      /* COND  */
  YYSYMBOL_NVI = 50,                       /* NVI  */
  YYSYMBOL_OPCODE = 51,                    /* OPCODE  */
  YYSYMBOL_FLAGS = 52,                     /* FLAGS  */
  YYSYMBOL_CTLREG = 53,                    /* CTLREG  */
  YYSYMBOL_REGQUAD = 54,                   /* REGQUAD  */
  YYSYMBOL_FPREG = 55,                     /* FPREG  */
  YYSYMBOL_TOK62 = 56,                     /* TOK62  */
  YYSYMBOL_TOK63 = 57,                     /* TOK63  */
  YYSYMBOL_TOK64 = 58,                     /* TOK64  */
  YYSYMBOL_TOK65 = 59,                     /* TOK65  */
  YYSYMBOL_TOK66 = 60,                     /* TOK66  */
  YYSYMBOL_TOK67 = 61,                     /* TOK67  */
  YYSYMBOL_TOK68 = 62,                     /* TOK68  */
  YYSYMBOL_TOK69 = 63,                     /* TOK69  */
  YYSYMBOL_TOK70 = 64,                     /* TOK70  */
  YYSYMBOL_TOK71 = 65,                     /* TOK71  */
  YYSYMBOL_TOK72 = 66,                     /* TOK72  */
  YYSYMBOL_FPFLAG = 67,                    /* FPFLAG  */
  YYSYMBOL_FPMODE = 68,                    /* FPMODE  */
  YYSYMBOL_CODE = 69,                      /* CODE  */
  YYSYMBOL_DATA = 70,                      /* DATA  */
  YYSYMBOL_BSS = 71,                       /* BSS  */
  YYSYMBOL_SEG = 72,                       /* SEG  */
  YYSYMBOL_NONSEG = 73,                    /* NONSEG  */
  YYSYMBOL_JPR = 74,                       /* JPR  */
  YYSYMBOL_CALLR = 75,                     /* CALLR  */
  YYSYMBOL_PSEC = 76,                      /* PSEC  */
  YYSYMBOL_ADDR = 77,                      /* ADDR  */
  YYSYMBOL_LINE = 78,                      /* LINE  */
  YYSYMBOL_NUMBER = 79,                    /* NUMBER  */
  YYSYMBOL_NEWNAME = 80,                   /* NEWNAME  */
  YYSYMBOL_NEWLOCAL = 81,                  /* NEWLOCAL  */
  YYSYMBOL_STRING = 82,                    /* STRING  */
  YYSYMBOL_YYACCEPT = 83,                  /* $accept  */
  YYSYMBOL_program = 84,                   /* program  */
  YYSYMBOL_line = 85,                      /* line  */
  YYSYMBOL_lineinit = 86,                  /* lineinit  */
  YYSYMBOL_stmt = 87,                      /* stmt  */
  YYSYMBOL_simpledir = 88,                 /* simpledir  */
  YYSYMBOL_instr = 89,                     /* instr  */
  YYSYMBOL_opcode = 90,                    /* opcode  */
  YYSYMBOL_operands = 91,                  /* operands  */
  YYSYMBOL_operand = 92,                   /* operand  */
  YYSYMBOL_regop = 93,                     /* regop  */
  YYSYMBOL_regtok = 94,                    /* regtok  */
  YYSYMBOL_fpspec = 95,                    /* fpspec  */
  YYSYMBOL_fpmodes = 96,                   /* fpmodes  */
  YYSYMBOL_modes = 97,                     /* modes  */
  YYSYMBOL_mode = 98,                      /* mode  */
  YYSYMBOL_fpflags = 99,                   /* fpflags  */
  YYSYMBOL_flags = 100,                    /* flags  */
  YYSYMBOL_flag = 101,                     /* flag  */
  YYSYMBOL_idxop = 102,                    /* idxop  */
  YYSYMBOL_baseop = 103,                   /* baseop  */
  YYSYMBOL_bidxop = 104,                   /* bidxop  */
  YYSYMBOL_immop = 105,                    /* immop  */
  YYSYMBOL_addrop = 106,                   /* addrop  */
  YYSYMBOL_indop = 107,                    /* indop  */
  YYSYMBOL_datadir = 108,                  /* datadir  */
  YYSYMBOL_externdir = 109,                /* externdir  */
  YYSYMBOL_externs = 110,                  /* externs  */
  YYSYMBOL_commdir = 111,                  /* commdir  */
  YYSYMBOL_commargs = 112,                 /* commargs  */
  YYSYMBOL_commnames = 113,                /* commnames  */
  YYSYMBOL_datalist = 114,                 /* datalist  */
  YYSYMBOL_bytedir = 115,                  /* bytedir  */
  YYSYMBOL_blist = 116,                    /* blist  */
  YYSYMBOL_belem = 117,                    /* belem  */
  YYSYMBOL_worddir = 118,                  /* worddir  */
  YYSYMBOL_wlist = 119,                    /* wlist  */
  YYSYMBOL_welem = 120,                    /* welem  */
  YYSYMBOL_adrdir = 121,                   /* adrdir  */
  YYSYMBOL_alist = 122,                    /* alist  */
  YYSYMBOL_aelem = 123,                    /* aelem  */
  YYSYMBOL_longdir = 124,                  /* longdir  */
  YYSYMBOL_llist = 125,                    /* llist  */
  YYSYMBOL_lelem = 126,                    /* lelem  */
  YYSYMBOL_assign = 127,                   /* assign  */
  YYSYMBOL_keyword = 128,                  /* keyword  */
  YYSYMBOL_sexpr = 129,                    /* sexpr  */
  YYSYMBOL_expr = 130,                     /* expr  */
  YYSYMBOL_bexpr = 131,                    /* bexpr  */
  YYSYMBOL_term = 132,                     /* term  */
  YYSYMBOL_unary = 133,                    /* unary  */
  YYSYMBOL_primary = 134,                  /* primary  */
  YYSYMBOL_symref = 135,                   /* symref  */
  YYSYMBOL_label = 136,                    /* label  */
  YYSYMBOL_name = 137                      /* name  */
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
typedef yytype_int16 yy_state_t;

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
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1698

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  287
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  380

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   257


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
       0,     2,     2,     3,     4,     5,     2,     2,     2,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,     2,     2,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,     2,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   145,   145,   146,   148,   152,   156,   160,   166,   170,
     174,   178,   179,   180,   181,   182,   183,   187,   192,   196,
     200,   204,   208,   212,   216,   220,   221,   222,   226,   230,
     235,   239,   243,   247,   251,   255,   259,   264,   265,   266,
     267,   268,   269,   270,   271,   273,   277,   281,   286,   287,
     288,   289,   290,   291,   292,   294,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   313,
     314,   316,   324,   341,   345,   363,   365,   371,   375,   379,
     384,   386,   390,   394,   399,   403,   407,   411,   415,   419,
     423,   427,   432,   436,   440,   444,   449,   453,   458,   463,
     471,   475,   480,   481,   482,   484,   488,   493,   498,   502,
     508,   512,   517,   521,   525,   530,   535,   539,   545,   550,
     555,   560,   565,   570,   575,   580,   588,   598,   602,   613,
     617,   628,   632,   636,   640,   644,   648,   652,   656,   660,
     664,   668,   672,   676,   680,   684,   688,   692,   696,   700,
     704,   708,   712,   716,   720,   724,   728,   733,   738,   743,
     761,   765,   784,   795,   799,   803,   812,   817,   822,   830,
     834,   843,   854,   858,   862,   872,   877,   892,   896,   912,
     925,   929,   933,   945,   950,   958,   962,   971,   976,   980,
     984,   986,   992,   996,  1002,  1006,  1012,  1016,  1021,  1025,
    1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,  1039,
    1040,  1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,  1049,
    1050,  1051,  1052,  1053,  1054,  1055,  1056,  1057,  1058,  1059,
    1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1069,  1074,
    1078,  1082,  1087,  1091,  1096,  1100,  1102,  1106,  1110,  1114,
    1118,  1120,  1124,  1128,  1132,  1136,  1140,  1144,  1148,  1152,
    1156,  1160,  1162,  1166,  1170,  1174,  1178,  1180,  1184,  1188,
    1192,  1196,  1198,  1202,  1206,  1210,  1214,  1218,  1222,  1226,
    1230,  1234,  1236,  1245,  1251,  1258,  1265,  1272
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
  "\"end of file\"", "error", "\"invalid token\"", "DOT", "PLUS", "MINUS",
  "DIV", "MUL", "SHL", "SHR", "AND", "OR", "XOR", "LSEG", "RSEG", "BAR",
  "COLON", "DCOLON", "HASH", "AT", "SSEG", "OSEG", "LBRACE", "RBRACE",
  "LPAR", "RPAR", "NL", "COMMA", "EQ", "GASSIGN", "LASSIGN", "SYMBOL",
  "COMM", "EXTERN", "EVEN", "ODD", "BYTE", "WORD", "LONG", "BLKB", "BLKW",
  "BLKL", "STABL", "STABN", "STABP", "STABR", "REG", "REGPAIR", "BREG",
  "COND", "NVI", "OPCODE", "FLAGS", "CTLREG", "REGQUAD", "FPREG", "TOK62",
  "TOK63", "TOK64", "TOK65", "TOK66", "TOK67", "TOK68", "TOK69", "TOK70",
  "TOK71", "TOK72", "FPFLAG", "FPMODE", "CODE", "DATA", "BSS", "SEG",
  "NONSEG", "JPR", "CALLR", "PSEC", "ADDR", "LINE", "NUMBER", "NEWNAME",
  "NEWLOCAL", "STRING", "$accept", "program", "line", "lineinit", "stmt",
  "simpledir", "instr", "opcode", "operands", "operand", "regop", "regtok",
  "fpspec", "fpmodes", "modes", "mode", "fpflags", "flags", "flag",
  "idxop", "baseop", "bidxop", "immop", "addrop", "indop", "datadir",
  "externdir", "externs", "commdir", "commargs", "commnames", "datalist",
  "bytedir", "blist", "belem", "worddir", "wlist", "welem", "adrdir",
  "alist", "aelem", "longdir", "llist", "lelem", "assign", "keyword",
  "sexpr", "expr", "bexpr", "term", "unary", "primary", "symref", "label",
  "name", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-131)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-17)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     221,  -131,  -131,    59,  -131,   448,  -131,  -131,  -131,    27,
    -131,  -131,  -131,   147,    74,  -131,  -131,  -131,  -131,  -131,
     207,  1015,  1020,     0,  1051,  1056,  1073,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
     529,   598,   -14,  -131,    29,  -131,  -131,    28,  -131,  -131,
    1548,  -131,  -131,  -131,  -131,  1526,  1617,  1617,  1617,  -131,
     386,    19,  -131,  1102,  -131,  -131,   156,   556,    30,  1115,
    -131,  -131,   122,   389,    67,  -131,  -131,  -131,  -131,  -131,
    -131,    -6,  -131,   138,  -131,   138,  -131,   138,  -131,  -131,
    -131,   169,  -131,   212,  -131,   302,  -131,  1137,   736,    17,
    1154,  1188,     8,    45,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,    10,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,    47,   138,  -131,  -131,  -131,
    -131,   -41,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
     111,  -131,  -131,   118,  -131,  -131,   129,  -131,    34,  -131,
     131,  -131,   120,   157,  -131,  -131,   280,  -131,   161,  -131,
     391,  -131,  -131,  -131,  -131,   749,   313,  -131,   138,  -131,
    -131,  -131,  -131,  -131,    56,  -131,   245,  -131,  1201,  1223,
     110,  1239,  1252,  1274,  1310,  1325,  1339,  1356,   152,  -131,
    1361,  -131,  1397,  -131,  1421,  -131,   171,  -131,  -131,  -131,
    -131,  -131,  -131,   138,  -131,   138,    15,    22,   667,    26,
    -131,    21,    13,   194,   771,   818,   840,   853,   887,   922,
     956,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,    33,  -131,  -131,  -131,   389,
    -131,   389,  -131,  -131,   165,  -131,    67,  -131,    67,  -131,
      67,  -131,    67,  -131,    67,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,   304,  -131,   406,  -131,   449,  -131,  -131,  -131,
     969,    51,  -131,   991,    52,  -131,  -131,  -131,   121,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,   204,  -131,  -131,  -131,
     220,  -131,  -131,  -131,   288,  -131,  -131,  -131,   299,  -131,
    -131,   188,  -131,  1426,  -131,  1443,  -131,     1,  -131,   310,
    -131,  -131,  -131,   314,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,   468,
    -131,   473,  -131,  -131,  -131,  -131,  -131,  -131,  -131,    24,
    -131,  1457,  -131,  -131,  -131,   510,  -131,    25,  -131,  -131
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     7,     6,     0,     3,     0,     1,     2,    17,     0,
       9,    10,   287,     0,     0,    21,    22,   158,   167,   183,
       0,     0,     0,     0,     0,     0,     0,    37,    38,    39,
      40,    41,    42,    43,    44,    18,    19,    20,    23,    24,
       0,     0,    28,   175,     0,   286,   285,     0,    14,    13,
      31,    12,   103,   104,   102,   122,   123,   125,   124,    11,
       0,     0,   198,     0,   111,   277,     0,     0,     0,     0,
     272,   110,     0,   250,   261,   266,   271,   281,   282,   106,
     105,   109,   127,   126,   129,   128,   131,   130,   157,   156,
     133,     0,   147,     0,   141,     0,    34,     0,     0,     0,
       0,     0,    60,    61,    62,    56,    59,    58,    57,    63,
      65,    68,    64,    66,    67,    33,    47,    54,    55,    48,
      49,    50,    51,    52,    53,    98,   245,    36,    35,    29,
      27,    26,     5,     4,    80,    75,    30,    32,    70,    71,
      74,    69,    76,    79,   166,   118,   161,   165,   245,   119,
     170,   174,   245,   121,   178,   182,   245,   120,   186,   190,
     245,    15,   196,   284,   283,     0,     0,   199,   197,   270,
     269,   268,   267,   274,     0,   279,     0,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   134,
       0,   148,     0,   142,     0,   239,     0,    97,    96,   101,
      99,   100,   242,   241,   244,   243,     0,     0,     0,     0,
      25,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   192,   191,   194,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   195,   193,   275,     0,   280,   278,   247,   246,
     249,   248,   114,   112,   117,   252,   251,   254,   253,   256,
     255,   258,   257,   260,   259,   265,   264,   263,   262,   108,
     107,   135,     0,   149,     0,   143,     0,   240,   238,    85,
       0,     0,    89,     0,     0,    46,    45,    82,     0,    73,
      72,    78,    77,   160,   159,   163,     0,   169,   168,   172,
       0,   177,   176,   180,     0,   185,   184,   188,     0,   276,
     273,     0,   136,     0,   150,     0,   144,     0,    86,     0,
      93,    92,    90,     0,    95,    94,    83,    81,   164,   162,
     173,   171,   181,   179,   189,   187,   116,   115,   137,     0,
     151,     0,   145,   140,    87,    84,    91,    88,   138,     0,
     152,     0,   139,   132,   153,     0,   154,     0,   155,   146
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -131,  -131,    48,  -131,    18,  -131,  -131,  -131,   -26,  -131,
    -131,  -131,  -131,  -131,  -125,  -131,  -131,   -65,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,     6,  -131,  -131,
    -130,  -131,  -131,   -10,  -131,  -131,    95,  -131,  -131,  -131,
      50,  -131,     4,  -131,  -131,  -131,   -52,   -13,    62,   380,
     154,   276,  -131,  -131,     5
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    47,    48,    49,    50,   115,   116,
     117,   118,   137,   138,   139,   140,   141,   142,   143,   119,
     120,   121,   122,   123,   124,    51,    52,    80,    53,    71,
     273,    54,    55,   145,   146,    56,   149,   150,    57,   153,
     154,    58,   157,   158,    59,   262,   125,   126,    73,    74,
      75,    76,    77,    60,    78
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      72,    88,   362,   147,   151,   155,   159,    83,    85,    87,
      61,    91,    93,    95,   311,   128,   299,    12,   199,    81,
     162,   188,   309,   302,   136,   372,   378,   307,    62,   132,
     130,   173,   206,   300,   329,   163,   164,   208,   178,   179,
     303,   210,   148,   152,   156,   160,   198,   129,   165,   166,
     168,     7,   340,   344,   133,    63,   176,   264,   214,     6,
       1,   301,    -8,   200,   201,    61,    45,    46,   304,   207,
     265,   209,   308,   186,   187,    79,   341,   345,   161,    89,
     134,    -8,    -8,   363,   196,     2,   310,   203,   205,   135,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    12,   373,   379,   131,   174,
      -8,   272,   330,   222,   263,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,   346,   177,   178,   179,   178,   179,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,   211,    -8,
      -8,    12,   178,   179,   216,   212,   347,   312,    64,   180,
      65,    66,    67,   289,    45,    46,   213,   169,   215,    65,
      68,   147,   316,   151,   320,   155,   324,   159,   328,    68,
     189,    69,   297,   178,   179,   178,   179,   292,    12,   294,
      69,   296,   306,    12,   217,   274,   298,    12,   219,   356,
      45,    46,   331,    81,   290,   313,   190,    65,    66,    67,
     148,   357,   152,   314,   156,   348,   160,    68,    82,    97,
      65,    66,    67,   191,   100,   101,   178,   179,    69,    12,
      68,   350,     1,   326,    -8,    12,    70,    45,    46,   349,
       0,    69,    45,    46,     0,    70,    45,    46,    12,   192,
     269,   271,     0,    -8,    -8,   351,   266,     2,   339,   178,
     179,   343,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,   322,    45,    46,
     267,     0,    -8,    70,    45,    46,   144,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,   178,   179,    70,    45,    46,   352,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
     354,    -8,    -8,   193,   218,   332,   178,   179,   178,   179,
     318,   364,     0,   353,   223,   366,    65,    66,    67,     0,
     359,     0,   361,     0,   355,     0,    68,     0,    97,   194,
       0,   333,     0,   100,   101,   365,   274,    69,     0,   367,
     286,   288,   170,   172,    12,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,     0,     0,     0,   375,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,     8,     0,     9,
       0,     0,    70,    45,    46,   178,   179,   181,   182,   183,
     184,   185,     0,     0,     0,     0,     0,   334,    10,    11,
     178,   179,   -16,     0,     0,   220,     0,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,     0,   335,     0,     0,     0,    27,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,    34,     8,
     336,     9,     0,   178,   179,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,     0,    45,    46,     0,   368,
      10,    11,   178,   179,   370,     0,   337,   178,   179,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,     0,   369,     0,     0,     0,    27,
     371,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,   376,     0,     0,   178,   179,     0,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,     0,    45,    46,
      96,     0,    65,    66,    67,     0,     0,   377,     0,     0,
       0,     0,    68,     0,    97,     0,     0,    98,    99,   100,
     101,     0,     0,    69,     0,     0,     0,   171,     0,    65,
      12,   276,   278,   280,   282,   284,     0,     0,     0,    68,
       0,     0,     0,     0,     0,   102,   103,   104,   105,   106,
      69,   107,   108,   109,   110,     0,     0,    12,     0,     0,
       0,     0,   111,   112,   113,   114,     0,     0,     0,   127,
       0,    65,    66,    67,     0,     0,     0,     0,    70,    45,
      46,    68,     0,    97,     0,     0,    98,    99,   100,   101,
       0,     0,    69,     0,     0,     0,     0,     0,     0,    12,
       0,     0,     0,     0,     0,    70,    45,    46,     0,     0,
       0,     0,     0,     0,   102,   103,   104,   105,   106,     0,
     107,   108,   109,   110,     0,     0,     0,     0,     0,     0,
       0,   111,   112,   113,   114,     0,     0,     0,   305,     0,
      65,    66,    67,     0,     0,     0,     0,    70,    45,    46,
      68,     0,    97,     0,     0,    98,    99,   100,   101,     0,
       0,    69,     0,     0,     0,     0,     0,     0,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   102,   103,   104,   105,   106,     0,   107,
     108,   109,   110,     0,     0,     0,     0,     0,     0,     0,
     111,   112,   113,   114,     0,     0,     0,   197,     0,    65,
      66,    67,     0,     0,     0,     0,    70,    45,    46,    68,
     221,    97,    65,    66,    67,     0,   100,   101,     0,     0,
      69,     0,    68,     0,    97,     0,     0,    12,     0,   100,
     101,     0,   315,    69,    65,    66,    67,     0,     0,     0,
      12,     0,     0,     0,    68,     0,    97,     0,     0,     0,
       0,   100,   101,     0,     0,    69,     0,     0,     0,     0,
       0,     0,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    70,    45,    46,     0,   317,
       0,    65,    66,    67,     0,     0,     0,     0,    70,    45,
      46,    68,     0,    97,     0,     0,     0,     0,   100,   101,
       0,   319,    69,    65,    66,    67,     0,     0,     0,    12,
      70,    45,    46,    68,   321,    97,    65,    66,    67,     0,
     100,   101,     0,     0,    69,     0,    68,     0,    97,     0,
       0,    12,     0,   100,   101,     0,     0,    69,     0,     0,
       0,     0,     0,     0,    12,     0,     0,     0,   323,     0,
      65,    66,    67,     0,     0,     0,     0,    70,    45,    46,
      68,     0,    97,     0,     0,     0,     0,   100,   101,     0,
       0,    69,     0,     0,     0,     0,     0,     0,    12,    70,
      45,    46,     0,   325,     0,    65,    66,    67,     0,     0,
       0,     0,    70,    45,    46,    68,     0,    97,     0,     0,
       0,     0,   100,   101,     0,     0,    69,     0,     0,     0,
       0,     0,     0,    12,     0,     0,     0,   327,     0,    65,
      66,    67,     0,     0,     0,     0,    70,    45,    46,    68,
     338,    97,    65,    66,    67,     0,   100,   101,     0,     0,
      69,     0,    68,     0,    97,     0,     0,    12,     0,   100,
     101,     0,   342,    69,    65,    66,    67,     0,     0,     0,
      12,    70,    45,    46,    68,     0,    97,     0,     0,     0,
       0,   100,   101,     0,     0,    69,    84,     0,    65,    66,
      67,    86,    12,    65,    66,    67,     0,     0,    68,     0,
       0,     0,     0,    68,     0,    70,    45,    46,     0,    69,
       0,     0,     0,     0,    69,     0,    12,     0,    70,    45,
      46,    12,    90,     0,    65,    66,    67,    92,     0,    65,
      66,    67,     0,     0,    68,     0,     0,     0,     0,    68,
      70,    45,    46,     0,    94,    69,    65,    66,    67,     0,
      69,     0,    12,     0,     0,     0,    68,    12,     0,     0,
       0,     0,     0,     0,    70,    45,    46,    69,     0,    70,
      45,    46,     0,   167,    12,    65,    66,    67,     0,     0,
       0,     0,     0,     0,     0,    68,   175,     0,    65,    66,
      67,     0,     0,     0,     0,     0,    69,     0,    68,     0,
      70,    45,    46,    12,     0,    70,    45,    46,   195,    69,
      65,    66,    67,     0,     0,     0,    12,     0,     0,     0,
      68,     0,    70,    45,    46,   202,     0,    65,    66,    67,
       0,    69,     0,     0,     0,     0,     0,    68,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    69,     0,
       0,    70,    45,    46,     0,    12,     0,     0,     0,   204,
       0,    65,    66,    67,    70,    45,    46,     0,     0,     0,
       0,    68,   268,     0,    65,    66,    67,     0,     0,     0,
       0,     0,    69,     0,    68,     0,    70,    45,    46,    12,
       0,     0,     0,     0,   270,    69,    65,    66,    67,     0,
       0,     0,    12,    70,    45,    46,    68,     0,     0,     0,
     275,     0,    65,    66,    67,     0,     0,    69,     0,     0,
       0,     0,    68,   277,    12,    65,    66,    67,     0,     0,
       0,     0,     0,    69,     0,    68,     0,    70,    45,    46,
      12,     0,     0,     0,     0,   279,    69,    65,    66,    67,
      70,    45,    46,    12,     0,     0,     0,    68,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    69,     0,
       0,     0,    70,    45,    46,    12,     0,     0,     0,     0,
       0,   281,     0,    65,    66,    67,     0,     0,    70,    45,
      46,     0,     0,    68,     0,     0,   283,     0,    65,    66,
      67,    70,    45,    46,    69,     0,     0,     0,    68,     0,
     285,    12,    65,    66,    67,     0,     0,     0,     0,    69,
       0,     0,    68,    70,    45,    46,    12,   287,     0,    65,
      66,    67,   291,    69,    65,    66,    67,     0,     0,    68,
      12,     0,     0,     0,    68,     0,     0,     0,     0,     0,
      69,     0,     0,     0,     0,    69,     0,    12,     0,    70,
      45,    46,    12,     0,     0,     0,     0,     0,   293,     0,
      65,    66,    67,     0,    70,    45,    46,     0,     0,     0,
      68,     0,     0,     0,     0,     0,     0,     0,    70,    45,
      46,    69,   295,     0,    65,    66,    67,   358,    12,    65,
      66,    67,     0,     0,    68,    70,    45,    46,     0,    68,
      70,    45,    46,     0,   360,    69,    65,    66,    67,     0,
      69,     0,    12,     0,     0,     0,    68,    12,   374,     0,
      65,    66,    67,     0,     0,     0,     0,    69,     0,     0,
      68,     0,     0,     0,    12,     0,    70,    45,    46,     0,
       0,    69,     0,     0,     0,     0,     0,     0,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      70,    45,    46,     0,     0,    70,    45,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    70,    45,    46,     0,     0,     0,     0,    65,
      66,    67,     0,     0,     0,     0,    70,    45,    46,    68,
       0,    97,     0,     0,     0,     0,   100,   101,     0,     0,
      69,    65,    66,    67,     0,     0,     0,    12,     0,     0,
       0,    68,     0,    97,     0,     0,    98,    99,   100,   101,
       0,     0,    69,     0,     0,     0,     0,     0,     0,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   102,   103,   104,   105,   106,     0,
     107,   108,   109,   110,     0,    70,    45,    46,   144,     0,
       0,   111,   112,   113,   114,   134,   135,     0,     0,     0,
      65,    66,    67,     0,     0,     0,     0,    70,    45,    46,
      68,     0,    97,     0,     0,     0,     0,   100,   101,     0,
       0,    69,     0,     0,     0,     0,     0,     0,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    70,    45,    46
};

static const yytype_int16 yycheck[] =
{
      13,     1,     1,    55,    56,    57,    58,    20,    21,    22,
       5,    24,    25,    26,     1,    41,     1,    31,     1,    14,
       1,    27,     1,     1,    50,     1,     1,     1,     1,     1,
       1,     1,    24,    18,     1,    16,    17,    27,     4,     5,
      18,    82,    55,    56,    57,    58,    98,    42,    29,    30,
      63,     3,     1,     1,    26,    28,    69,     1,    24,     0,
       1,    46,     3,    46,    47,    60,    80,    81,    46,    24,
      14,    24,    46,     6,     7,     1,    25,    25,    60,    79,
      67,    22,    23,    82,    97,    26,   211,   100,   101,    68,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    31,    82,    82,    79,    79,
      51,     1,    79,   165,   166,    56,    57,    58,    59,    60,
      61,    62,     1,     1,     4,     5,     4,     5,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    27,    80,
      81,    31,     4,     5,    24,    27,    25,   212,     1,    27,
       3,     4,     5,     1,    80,    81,    27,     1,    27,     3,
      13,   213,   214,   215,   216,   217,   218,   219,   220,    13,
       1,    24,     1,     4,     5,     4,     5,   190,    31,   192,
      24,   194,   208,    31,    27,   180,    15,    31,    27,     1,
      80,    81,    27,   188,   188,     1,    27,     3,     4,     5,
     213,   331,   215,   213,   217,     1,   219,    13,     1,    15,
       3,     4,     5,     1,    20,    21,     4,     5,    24,    31,
      13,     1,     1,   219,     3,    31,    79,    80,    81,    25,
      -1,    24,    80,    81,    -1,    79,    80,    81,    31,    27,
     178,   179,    -1,    22,    23,    25,     1,    26,   300,     4,
       5,   303,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,   217,    80,    81,
      25,    -1,    51,    79,    80,    81,    82,    56,    57,    58,
      59,    60,    61,    62,     4,     5,    79,    80,    81,     1,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
       1,    80,    81,     1,    24,     1,     4,     5,     4,     5,
     215,     1,    -1,    25,     1,     1,     3,     4,     5,    -1,
     333,    -1,   335,    -1,    25,    -1,    13,    -1,    15,    27,
      -1,    27,    -1,    20,    21,    25,   331,    24,    -1,    25,
     186,   187,    66,    67,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,    -1,   371,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     1,    -1,     3,
      -1,    -1,    79,    80,    81,     4,     5,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,     1,    22,    23,
       4,     5,    26,    -1,    -1,    24,    -1,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    27,    -1,    -1,    -1,    51,    -1,    -1,
      -1,    -1,    56,    57,    58,    59,    60,    61,    62,     1,
       1,     3,    -1,     4,     5,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    -1,    80,    81,    -1,     1,
      22,    23,     4,     5,     1,    -1,    27,     4,     5,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    -1,    27,    -1,    -1,    -1,    51,
      27,    -1,    -1,    -1,    56,    57,    58,    59,    60,    61,
      62,     1,    -1,    -1,     4,     5,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    80,    81,
       1,    -1,     3,     4,     5,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    13,    -1,    15,    -1,    -1,    18,    19,    20,
      21,    -1,    -1,    24,    -1,    -1,    -1,     1,    -1,     3,
      31,   181,   182,   183,   184,   185,    -1,    -1,    -1,    13,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,
      24,    52,    53,    54,    55,    -1,    -1,    31,    -1,    -1,
      -1,    -1,    63,    64,    65,    66,    -1,    -1,    -1,     1,
      -1,     3,     4,     5,    -1,    -1,    -1,    -1,    79,    80,
      81,    13,    -1,    15,    -1,    -1,    18,    19,    20,    21,
      -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    -1,    -1,    -1,    -1,    79,    80,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    -1,
      52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    64,    65,    66,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,    -1,    -1,    -1,    -1,    79,    80,    81,
      13,    -1,    15,    -1,    -1,    18,    19,    20,    21,    -1,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    -1,    52,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    64,    65,    66,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,    -1,    -1,    -1,    -1,    79,    80,    81,    13,
       1,    15,     3,     4,     5,    -1,    20,    21,    -1,    -1,
      24,    -1,    13,    -1,    15,    -1,    -1,    31,    -1,    20,
      21,    -1,     1,    24,     3,     4,     5,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    13,    -1,    15,    -1,    -1,    -1,
      -1,    20,    21,    -1,    -1,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    79,    80,    81,    -1,     1,
      -1,     3,     4,     5,    -1,    -1,    -1,    -1,    79,    80,
      81,    13,    -1,    15,    -1,    -1,    -1,    -1,    20,    21,
      -1,     1,    24,     3,     4,     5,    -1,    -1,    -1,    31,
      79,    80,    81,    13,     1,    15,     3,     4,     5,    -1,
      20,    21,    -1,    -1,    24,    -1,    13,    -1,    15,    -1,
      -1,    31,    -1,    20,    21,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,    -1,    -1,    -1,    -1,    79,    80,    81,
      13,    -1,    15,    -1,    -1,    -1,    -1,    20,    21,    -1,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    79,
      80,    81,    -1,     1,    -1,     3,     4,     5,    -1,    -1,
      -1,    -1,    79,    80,    81,    13,    -1,    15,    -1,    -1,
      -1,    -1,    20,    21,    -1,    -1,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,    -1,    -1,    -1,    -1,    79,    80,    81,    13,
       1,    15,     3,     4,     5,    -1,    20,    21,    -1,    -1,
      24,    -1,    13,    -1,    15,    -1,    -1,    31,    -1,    20,
      21,    -1,     1,    24,     3,     4,     5,    -1,    -1,    -1,
      31,    79,    80,    81,    13,    -1,    15,    -1,    -1,    -1,
      -1,    20,    21,    -1,    -1,    24,     1,    -1,     3,     4,
       5,     1,    31,     3,     4,     5,    -1,    -1,    13,    -1,
      -1,    -1,    -1,    13,    -1,    79,    80,    81,    -1,    24,
      -1,    -1,    -1,    -1,    24,    -1,    31,    -1,    79,    80,
      81,    31,     1,    -1,     3,     4,     5,     1,    -1,     3,
       4,     5,    -1,    -1,    13,    -1,    -1,    -1,    -1,    13,
      79,    80,    81,    -1,     1,    24,     3,     4,     5,    -1,
      24,    -1,    31,    -1,    -1,    -1,    13,    31,    -1,    -1,
      -1,    -1,    -1,    -1,    79,    80,    81,    24,    -1,    79,
      80,    81,    -1,     1,    31,     3,     4,     5,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    13,     1,    -1,     3,     4,
       5,    -1,    -1,    -1,    -1,    -1,    24,    -1,    13,    -1,
      79,    80,    81,    31,    -1,    79,    80,    81,     1,    24,
       3,     4,     5,    -1,    -1,    -1,    31,    -1,    -1,    -1,
      13,    -1,    79,    80,    81,     1,    -1,     3,     4,     5,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    13,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,
      -1,    79,    80,    81,    -1,    31,    -1,    -1,    -1,     1,
      -1,     3,     4,     5,    79,    80,    81,    -1,    -1,    -1,
      -1,    13,     1,    -1,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    24,    -1,    13,    -1,    79,    80,    81,    31,
      -1,    -1,    -1,    -1,     1,    24,     3,     4,     5,    -1,
      -1,    -1,    31,    79,    80,    81,    13,    -1,    -1,    -1,
       1,    -1,     3,     4,     5,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    13,     1,    31,     3,     4,     5,    -1,    -1,
      -1,    -1,    -1,    24,    -1,    13,    -1,    79,    80,    81,
      31,    -1,    -1,    -1,    -1,     1,    24,     3,     4,     5,
      79,    80,    81,    31,    -1,    -1,    -1,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    79,    80,    81,    31,    -1,    -1,    -1,    -1,
      -1,     1,    -1,     3,     4,     5,    -1,    -1,    79,    80,
      81,    -1,    -1,    13,    -1,    -1,     1,    -1,     3,     4,
       5,    79,    80,    81,    24,    -1,    -1,    -1,    13,    -1,
       1,    31,     3,     4,     5,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    13,    79,    80,    81,    31,     1,    -1,     3,
       4,     5,     1,    24,     3,     4,     5,    -1,    -1,    13,
      31,    -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    -1,    -1,    -1,    24,    -1,    31,    -1,    79,
      80,    81,    31,    -1,    -1,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,    -1,    79,    80,    81,    -1,    -1,    -1,
      13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    80,
      81,    24,     1,    -1,     3,     4,     5,     1,    31,     3,
       4,     5,    -1,    -1,    13,    79,    80,    81,    -1,    13,
      79,    80,    81,    -1,     1,    24,     3,     4,     5,    -1,
      24,    -1,    31,    -1,    -1,    -1,    13,    31,     1,    -1,
       3,     4,     5,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      13,    -1,    -1,    -1,    31,    -1,    79,    80,    81,    -1,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      79,    80,    81,    -1,    -1,    79,    80,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    80,    81,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,    -1,    -1,    -1,    79,    80,    81,    13,
      -1,    15,    -1,    -1,    -1,    -1,    20,    21,    -1,    -1,
      24,     3,     4,     5,    -1,    -1,    -1,    31,    -1,    -1,
      -1,    13,    -1,    15,    -1,    -1,    18,    19,    20,    21,
      -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    -1,
      52,    53,    54,    55,    -1,    79,    80,    81,    82,    -1,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    -1,
       3,     4,     5,    -1,    -1,    -1,    -1,    79,    80,    81,
      13,    -1,    15,    -1,    -1,    -1,    -1,    20,    21,    -1,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    79,    80,    81
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    26,    84,    85,    86,     0,    85,     1,     3,
      22,    23,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    51,    56,    57,
      58,    59,    60,    61,    62,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    80,    81,    87,    88,    89,
      90,   108,   109,   111,   114,   115,   118,   121,   124,   127,
     136,   137,     1,    28,     1,     3,     4,     5,    13,    24,
      79,   112,   130,   131,   132,   133,   134,   135,   137,     1,
     110,   137,     1,   130,     1,   130,     1,   130,     1,    79,
       1,   130,     1,   130,     1,   130,     1,    15,    18,    19,
      20,    21,    46,    47,    48,    49,    50,    52,    53,    54,
      55,    63,    64,    65,    66,    91,    92,    93,    94,   102,
     103,   104,   105,   106,   107,   129,   130,     1,    91,   137,
       1,    79,     1,    26,    67,    68,    91,    95,    96,    97,
      98,    99,   100,   101,    82,   116,   117,   129,   130,   119,
     120,   129,   130,   122,   123,   129,   130,   125,   126,   129,
     130,    87,     1,    16,    17,    29,    30,     1,   130,     1,
     134,     1,   134,     1,    79,     1,   130,     1,     4,     5,
      27,     8,     9,    10,    11,    12,     6,     7,    27,     1,
      27,     1,    27,     1,    27,     1,   130,     1,   129,     1,
      46,    47,     1,   130,     1,   130,    24,    24,    27,    24,
      82,    27,    27,    27,    24,    27,    24,    27,    24,    27,
      24,     1,   129,     1,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,   128,   129,     1,    14,     1,    25,     1,   131,
       1,   131,     1,   113,   137,     1,   132,     1,   132,     1,
     132,     1,   132,     1,   132,     1,   133,     1,   133,     1,
     110,     1,   130,     1,   130,     1,   130,     1,    15,     1,
      18,    46,     1,    18,    46,     1,    91,     1,    46,     1,
      97,     1,   100,     1,   116,     1,   129,     1,   119,     1,
     129,     1,   123,     1,   129,     1,   125,     1,   129,     1,
      79,    27,     1,    27,     1,    27,     1,    27,     1,   129,
       1,    25,     1,   129,     1,    25,     1,    25,     1,    25,
       1,    25,     1,    25,     1,    25,     1,   113,     1,   130,
       1,   130,     1,    82,     1,    25,     1,    25,     1,    27,
       1,    27,     1,    82,     1,   130,     1,    27,     1,    82
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    84,    84,    85,    85,    85,    85,    86,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      89,    89,    89,    89,    89,    89,    89,    90,    90,    90,
      90,    90,    90,    90,    90,    91,    91,    91,    92,    92,
      92,    92,    92,    92,    92,    93,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    95,
      95,    96,    97,    97,    97,    98,    99,   100,   100,   100,
     101,   102,   102,   102,   103,   103,   103,   103,   103,   103,
     103,   103,   104,   104,   104,   104,   105,   105,   106,   107,
     107,   107,   108,   108,   108,   109,   109,   110,   110,   110,
     111,   111,   112,   112,   112,   113,   113,   113,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   115,   116,
     116,   116,   117,   117,   117,   117,   117,   118,   119,   119,
     119,   120,   120,   120,   120,   121,   122,   122,   122,   123,
     123,   123,   123,   124,   125,   125,   125,   126,   126,   126,
     126,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   129,   129,
     129,   129,   129,   129,   129,   129,   130,   130,   130,   130,
     130,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   132,   132,   132,   132,   132,   133,   133,   133,
     133,   133,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   135,   136,   136,   137,   137,   137
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     3,     1,     1,     0,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     2,     1,     2,
       2,     1,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     3,     3,     1,
       1,     4,     3,     4,     5,     3,     4,     5,     5,     3,
       4,     5,     4,     4,     4,     4,     2,     2,     1,     2,
       2,     2,     1,     1,     1,     2,     2,     3,     3,     1,
       2,     2,     3,     2,     3,     3,     3,     1,     2,     2,
       2,     2,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     8,     2,     3,     4,     5,     6,     7,     8,
       6,     2,     3,     4,     5,     6,    10,     2,     3,     4,
       5,     6,     7,     8,     9,    10,     2,     2,     1,     3,
       3,     1,     4,     3,     4,     1,     1,     1,     3,     3,
       1,     4,     3,     4,     1,     1,     3,     3,     1,     4,
       3,     4,     1,     1,     3,     3,     1,     4,     3,     4,
       1,     3,     3,     3,     3,     3,     2,     3,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     2,     2,     2,     2,     1,     3,     3,     3,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     3,     1,     2,     2,     2,
       2,     1,     1,     4,     2,     3,     4,     1,     3,     2,
       3,     1,     1,     2,     2,     1,     1,     1
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
  case 4: /* line: lineinit stmt NL  */
#line 149 "parser.y"
                {
		chknodes(); curline++;
		}
#line 2005 "y.tab.c"
    break;

  case 5: /* line: lineinit stmt error  */
#line 153 "parser.y"
                {
		SYNERR("Expecting carriage return or linefeed"); chknodes(); yychar = -1; curline++;
		}
#line 2013 "y.tab.c"
    break;

  case 6: /* line: NL  */
#line 157 "parser.y"
                {
		curline++;
		}
#line 2021 "y.tab.c"
    break;

  case 7: /* line: error  */
#line 161 "parser.y"
                {
		SYNERR("Expecting beginning of line");
		}
#line 2029 "y.tab.c"
    break;

  case 8: /* lineinit: %empty  */
#line 166 "parser.y"
                {
		iputline();
		}
#line 2037 "y.tab.c"
    break;

  case 9: /* stmt: LBRACE  */
#line 171 "parser.y"
                {
		deflabels(); blkopen();
		}
#line 2045 "y.tab.c"
    break;

  case 10: /* stmt: RBRACE  */
#line 175 "parser.y"
                {
		deflabels(); blkclose();
		}
#line 2053 "y.tab.c"
    break;

  case 16: /* stmt: label  */
#line 184 "parser.y"
                {
		deflabels();
		}
#line 2061 "y.tab.c"
    break;

  case 17: /* stmt: error  */
#line 188 "parser.y"
                {
		SYNERR("Expecting beginning of program");
		}
#line 2069 "y.tab.c"
    break;

  case 18: /* simpledir: CODE  */
#line 193 "parser.y"
                {
		deflabels(); setsub(0); iputb(12);
		}
#line 2077 "y.tab.c"
    break;

  case 19: /* simpledir: DATA  */
#line 197 "parser.y"
                {
		deflabels(); setsub(1); iputb(13);
		}
#line 2085 "y.tab.c"
    break;

  case 20: /* simpledir: BSS  */
#line 201 "parser.y"
                {
		deflabels(); setsub(2); iputb(14);
		}
#line 2093 "y.tab.c"
    break;

  case 21: /* simpledir: EVEN  */
#line 205 "parser.y"
                {
		even(); deflabels();
		}
#line 2101 "y.tab.c"
    break;

  case 22: /* simpledir: ODD  */
#line 209 "parser.y"
                {
		odd();
		}
#line 2109 "y.tab.c"
    break;

  case 23: /* simpledir: SEG  */
#line 213 "parser.y"
                {
		segmode++;
		}
#line 2117 "y.tab.c"
    break;

  case 24: /* simpledir: NONSEG  */
#line 217 "parser.y"
                {
		segmode = 0;
		}
#line 2125 "y.tab.c"
    break;

  case 27: /* simpledir: LINE error  */
#line 223 "parser.y"
                {
		SYNERR("Expecting line number in .line statement");
		}
#line 2133 "y.tab.c"
    break;

  case 28: /* simpledir: PSEC  */
#line 227 "parser.y"
                {
		setsec(0); iputb(0x29); iputb(0);
		}
#line 2141 "y.tab.c"
    break;

  case 29: /* simpledir: PSEC name  */
#line 231 "parser.y"
                {
		secname((yyvsp[0].np));
		}
#line 2149 "y.tab.c"
    break;

  case 30: /* instr: opcode operands  */
#line 236 "parser.y"
                {
		even(); deflabels(); putins((yyvsp[-1].pp), (yyvsp[0].np));
		}
#line 2157 "y.tab.c"
    break;

  case 31: /* instr: opcode  */
#line 240 "parser.y"
                {
		even(); deflabels(); putins((yyvsp[0].pp), mknode2(0L, 0L, 0));
		}
#line 2165 "y.tab.c"
    break;

  case 32: /* instr: opcode fpspec  */
#line 244 "parser.y"
                {
		even(); deflabels(); putins((yyvsp[-1].pp), (yyvsp[0].np));
		}
#line 2173 "y.tab.c"
    break;

  case 33: /* instr: JPR operands  */
#line 248 "parser.y"
                {
		even(); deflabels(); jpr((yyvsp[-1].pp)->p_tok, (yyvsp[0].np));
		}
#line 2181 "y.tab.c"
    break;

  case 34: /* instr: JPR error  */
#line 252 "parser.y"
                {
		SYNERR("Expecting operand(s) after jpr pseudo-instruction");
		}
#line 2189 "y.tab.c"
    break;

  case 35: /* instr: CALLR operands  */
#line 256 "parser.y"
                {
		even(); deflabels(); jpr((yyvsp[-1].pp)->p_tok, (yyvsp[0].np));
		}
#line 2197 "y.tab.c"
    break;

  case 36: /* instr: CALLR error  */
#line 260 "parser.y"
                {
		SYNERR("Expecting operand(s) after callr pseudo-instruction");
		}
#line 2205 "y.tab.c"
    break;

  case 45: /* operands: operand COMMA operands  */
#line 274 "parser.y"
                {
		(yyvsp[-2].np)->n_next = (yyvsp[0].np);
		}
#line 2213 "y.tab.c"
    break;

  case 46: /* operands: operand COMMA error  */
#line 278 "parser.y"
                {
		SYNERR("Expecting operand after comma in list of operands");
		}
#line 2221 "y.tab.c"
    break;

  case 47: /* operands: operand  */
#line 282 "parser.y"
                {
		(yyvsp[0].np)->n_next = 0L;
		}
#line 2229 "y.tab.c"
    break;

  case 55: /* regop: regtok  */
#line 295 "parser.y"
                {
		(yyval.np) = mknode2((yyvsp[0].pp)->p_tok, mknode1(1, (yyvsp[0].pp)->p_n), 0);
		}
#line 2237 "y.tab.c"
    break;

  case 71: /* fpmodes: modes  */
#line 317 "parser.y"
                {
		(yyval.np) = mknode2((yyvsp[0].pp)->p_tok, mknode1(1, fpmode), 0);
		fpmode = 0;
		nround = 0;
		ninf = 0;
		}
#line 2248 "y.tab.c"
    break;

  case 72: /* modes: mode COMMA modes  */
#line 325 "parser.y"
                {
		switch ((yyvsp[-2].pp)->p_n) {
		case 0x60: case 0x62: case 0x64: case 0x66:
			if (nround != 0)
				error(2, "Too many rounding modes specified\n");
			nround++;
			fpmode |= (yyvsp[-2].pp)->p_n;
			break;
		case 0x10: case 0x11:
			if (nround != 0)
				error(2, "Too many infinity modes specified\n");
			ninf++;
			fpmode |= (yyvsp[-2].pp)->p_n;
			break;
		}
		}
#line 2269 "y.tab.c"
    break;

  case 73: /* modes: mode COMMA error  */
#line 342 "parser.y"
                {
		SYNERR("Expecting floating point mode after comma");
		}
#line 2277 "y.tab.c"
    break;

  case 74: /* modes: mode  */
#line 346 "parser.y"
                {
		switch ((yyvsp[0].pp)->p_n) {
		case 0x60: case 0x62: case 0x64: case 0x66:
			if (nround != 0)
				error(2, "Too many rounding modes specified\n");
			nround++;
			fpmode |= (yyvsp[0].pp)->p_n;
			break;
		case 0x10: case 0x11:
			if (nround != 0)
				error(2, "Too many infinity modes specified\n");
			ninf++;
			fpmode |= (yyvsp[0].pp)->p_n;
			break;
		}
		}
#line 2298 "y.tab.c"
    break;

  case 76: /* fpflags: flags  */
#line 366 "parser.y"
                {
		(yyval.np) = mknode2((yyvsp[0].pp)->p_tok, mknode1(1, fpflag), 0);
		fpflag = 0;
		}
#line 2307 "y.tab.c"
    break;

  case 77: /* flags: flag COMMA flags  */
#line 372 "parser.y"
                {
		fpflag |= (yyvsp[-2].pp)->p_n;
		}
#line 2315 "y.tab.c"
    break;

  case 78: /* flags: flag COMMA error  */
#line 376 "parser.y"
                {
		SYNERR("Expecting floating point flag after comma");
		}
#line 2323 "y.tab.c"
    break;

  case 79: /* flags: flag  */
#line 380 "parser.y"
                {
		fpflag |= (yyvsp[0].pp)->p_n;
		}
#line 2331 "y.tab.c"
    break;

  case 81: /* idxop: sexpr LPAR REG RPAR  */
#line 387 "parser.y"
                {
		(yyval.np) = mknode2(0x61, (yyvsp[-3].np), mknode1(1, (yyvsp[-1].pp)->p_n));
		}
#line 2339 "y.tab.c"
    break;

  case 82: /* idxop: sexpr LPAR error  */
#line 391 "parser.y"
                {
		SYNERR("Expecting register number after left parenthesis in indexed operand");
		}
#line 2347 "y.tab.c"
    break;

  case 83: /* idxop: sexpr LPAR REG error  */
#line 395 "parser.y"
                {
		SYNERR("Expecting right parenthesis in indexed operand");
		}
#line 2355 "y.tab.c"
    break;

  case 84: /* baseop: REG LPAR HASH sexpr RPAR  */
#line 400 "parser.y"
                {
		(yyval.np) = mknode2(0x63, mknode1(1, (yyvsp[-4].pp)->p_n), (yyvsp[-1].np));
		}
#line 2363 "y.tab.c"
    break;

  case 85: /* baseop: REG LPAR error  */
#line 404 "parser.y"
                {
		SYNERR("Expecting # or register after left parenthesis");
		}
#line 2371 "y.tab.c"
    break;

  case 86: /* baseop: REG LPAR HASH error  */
#line 408 "parser.y"
                {
		SYNERR("Expecting address expression after # in base addressed operand");
		}
#line 2379 "y.tab.c"
    break;

  case 87: /* baseop: REG LPAR HASH sexpr error  */
#line 412 "parser.y"
                {
		SYNERR("Expecting right parenthesis in base addressed operand");
		}
#line 2387 "y.tab.c"
    break;

  case 88: /* baseop: REGPAIR LPAR HASH sexpr RPAR  */
#line 416 "parser.y"
                {
		(yyval.np) = mknode2(0x63, mknode1(1, (yyvsp[-4].pp)->p_n), (yyvsp[-1].np));
		}
#line 2395 "y.tab.c"
    break;

  case 89: /* baseop: REGPAIR LPAR error  */
#line 420 "parser.y"
                {
		SYNERR("Expecting # or register after left parenthesis");
		}
#line 2403 "y.tab.c"
    break;

  case 90: /* baseop: REGPAIR LPAR HASH error  */
#line 424 "parser.y"
                {
		SYNERR("Expecting segmented address expression in base addressed operand");
		}
#line 2411 "y.tab.c"
    break;

  case 91: /* baseop: REGPAIR LPAR HASH sexpr error  */
#line 428 "parser.y"
                {
		SYNERR("Expecting right parenthesis in segmented base addressed operand");
		}
#line 2419 "y.tab.c"
    break;

  case 92: /* bidxop: REG LPAR REG RPAR  */
#line 433 "parser.y"
                {
		(yyval.np) = mknode2(0x64, mknode1(1, (yyvsp[-3].pp)->p_n), mknode1(1, (yyvsp[-1].pp)->p_n));
		}
#line 2427 "y.tab.c"
    break;

  case 93: /* bidxop: REG LPAR REG error  */
#line 437 "parser.y"
                {
		SYNERR("Expecting right parenthesis in base indexed operand");
		}
#line 2435 "y.tab.c"
    break;

  case 94: /* bidxop: REGPAIR LPAR REG RPAR  */
#line 441 "parser.y"
                {
		(yyval.np) = mknode2(0x64, mknode1(1, (yyvsp[-3].pp)->p_n), mknode1(1, (yyvsp[-1].pp)->p_n));
		}
#line 2443 "y.tab.c"
    break;

  case 95: /* bidxop: REGPAIR LPAR REG error  */
#line 445 "parser.y"
                {
		SYNERR("Expecting right parenthesis in segmented base indexed operand");
		}
#line 2451 "y.tab.c"
    break;

  case 96: /* immop: HASH sexpr  */
#line 450 "parser.y"
                {
		(yyval.np) = mknode2(0x62, (yyvsp[0].np), 0);
		}
#line 2459 "y.tab.c"
    break;

  case 97: /* immop: HASH error  */
#line 454 "parser.y"
                {
		SYNERR("Expecting address expression after # (immediate) operator");
		}
#line 2467 "y.tab.c"
    break;

  case 98: /* addrop: sexpr  */
#line 459 "parser.y"
                {
		(yyval.np) = mknode2(0x56, (yyvsp[0].np), 0);
		}
#line 2475 "y.tab.c"
    break;

  case 99: /* indop: AT REG  */
#line 464 "parser.y"
                {
		if ((yyvsp[0].pp)->p_n == 0) {
			error(2, "Cannot perform an indirection on r0\n");
			(yyval.np) = errnode();
		} else
			(yyval.np) = mknode2(0x60, mknode1(1, (yyvsp[0].pp)->p_n), 0);
		}
#line 2487 "y.tab.c"
    break;

  case 100: /* indop: AT REGPAIR  */
#line 472 "parser.y"
                {
		(yyval.np) = mknode2(0x60, mknode1(1, (yyvsp[0].pp)->p_n), 0);
		}
#line 2495 "y.tab.c"
    break;

  case 101: /* indop: AT error  */
#line 476 "parser.y"
                {
		SYNERR("Expecting register or register pair after @ (indirect) operator");
		}
#line 2503 "y.tab.c"
    break;

  case 105: /* externdir: EXTERN externs  */
#line 485 "parser.y"
                {
		deflabels();
		}
#line 2511 "y.tab.c"
    break;

  case 106: /* externdir: EXTERN error  */
#line 489 "parser.y"
                {
		SYNERR("Expecting identifier after .extern");
		}
#line 2519 "y.tab.c"
    break;

  case 107: /* externs: name COMMA externs  */
#line 494 "parser.y"
                {
		(yyvsp[-2].np)->n_c1 = 0;
		defsym((yyvsp[-2].np), mknode1(3, 0));
		}
#line 2528 "y.tab.c"
    break;

  case 108: /* externs: name COMMA error  */
#line 499 "parser.y"
                {
		SYNERR("Expecting identifier after comma in .extern statement");
		}
#line 2536 "y.tab.c"
    break;

  case 109: /* externs: name  */
#line 503 "parser.y"
                {
		(yyvsp[0].np)->n_c1 = 0;
		defsym((yyvsp[0].np), mknode1(3, 0));
		}
#line 2545 "y.tab.c"
    break;

  case 110: /* commdir: COMM commargs  */
#line 509 "parser.y"
                {
		deflabels();
		}
#line 2553 "y.tab.c"
    break;

  case 111: /* commdir: COMM error  */
#line 513 "parser.y"
                {
		SYNERR("Expecting numeric expression after .comm");
		}
#line 2561 "y.tab.c"
    break;

  case 112: /* commargs: expr COMMA commnames  */
#line 518 "parser.y"
                {
		defcomm((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 2569 "y.tab.c"
    break;

  case 113: /* commargs: expr error  */
#line 522 "parser.y"
                {
		SYNERR("Expecting comma after numeric expression in .comm statement");
		}
#line 2577 "y.tab.c"
    break;

  case 114: /* commargs: expr COMMA error  */
#line 526 "parser.y"
                {
		SYNERR("Expecting identifier in .comm statement");
		}
#line 2585 "y.tab.c"
    break;

  case 115: /* commnames: name COMMA commnames  */
#line 531 "parser.y"
                {
		(yyvsp[-2].np)->n_c1 = 0;
		(yyvsp[-2].np)->n_next = (yyvsp[0].np);
		}
#line 2594 "y.tab.c"
    break;

  case 116: /* commnames: name COMMA error  */
#line 536 "parser.y"
                {
		SYNERR("Expecting identifier after comma in .comm statement");
		}
#line 2602 "y.tab.c"
    break;

  case 117: /* commnames: name  */
#line 540 "parser.y"
                {
		(yyvsp[0].np)->n_c1 = 0;
		(yyvsp[0].np)->n_next = 0L;
		}
#line 2611 "y.tab.c"
    break;

  case 118: /* datalist: bytedir blist  */
#line 546 "parser.y"
                {
		for (listp = (yyvsp[0].np); listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
#line 2620 "y.tab.c"
    break;

  case 119: /* datalist: worddir wlist  */
#line 551 "parser.y"
                {
		for (listp = (yyvsp[0].np); listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
#line 2629 "y.tab.c"
    break;

  case 120: /* datalist: longdir llist  */
#line 556 "parser.y"
                {
		for (listp = (yyvsp[0].np); listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
#line 2638 "y.tab.c"
    break;

  case 121: /* datalist: adrdir alist  */
#line 561 "parser.y"
                {
		for (listp = (yyvsp[0].np); listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
#line 2647 "y.tab.c"
    break;

  case 122: /* datalist: bytedir  */
#line 566 "parser.y"
                {
		locadv(1);
		iputloc();
		}
#line 2656 "y.tab.c"
    break;

  case 123: /* datalist: worddir  */
#line 571 "parser.y"
                {
		locadv(2);
		iputloc();
		}
#line 2665 "y.tab.c"
    break;

  case 124: /* datalist: longdir  */
#line 576 "parser.y"
                {
		locadv(4);
		iputloc();
		}
#line 2674 "y.tab.c"
    break;

  case 125: /* datalist: adrdir  */
#line 581 "parser.y"
                {
		if (segmode != 0)
			locadv(4);
		else
			locadv(2);
		iputloc();
		}
#line 2686 "y.tab.c"
    break;

  case 126: /* datalist: BLKB expr  */
#line 589 "parser.y"
                {
		deflabels();
		if (exprtype((yyvsp[0].np)) == 3)
			locadv((int16_t)(yyvsp[0].np)->n_val);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		}
#line 2700 "y.tab.c"
    break;

  case 127: /* datalist: BLKB error  */
#line 599 "parser.y"
                {
		SYNERR("Expecting numeric expression in .blkb statement");
		}
#line 2708 "y.tab.c"
    break;

  case 128: /* datalist: BLKW expr  */
#line 603 "parser.y"
                {
		even();
		deflabels();
		if (exprtype((yyvsp[0].np)) == 3)
			locadv((int16_t)(yyvsp[0].np)->n_val * 2);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		}
#line 2723 "y.tab.c"
    break;

  case 129: /* datalist: BLKW error  */
#line 614 "parser.y"
                {
		SYNERR("Expecting numeric expression in .blkw statement");
		}
#line 2731 "y.tab.c"
    break;

  case 130: /* datalist: BLKL expr  */
#line 618 "parser.y"
                {
		even();
		deflabels();
		if (exprtype((yyvsp[0].np)) == 3)
			locadv((int16_t)(yyvsp[0].np)->n_val * 4);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		}
#line 2746 "y.tab.c"
    break;

  case 131: /* datalist: BLKL error  */
#line 629 "parser.y"
                {
		SYNERR("Expecting numeric expression in .blkl statement");
		}
#line 2754 "y.tab.c"
    break;

  case 132: /* datalist: STABN expr COMMA expr COMMA expr COMMA STRING  */
#line 633 "parser.y"
                {
		putstabn((yyvsp[-6].np), (yyvsp[-4].np), (yyvsp[-2].np), (yyvsp[0].p));
		}
#line 2762 "y.tab.c"
    break;

  case 133: /* datalist: STABN error  */
#line 637 "parser.y"
                {
		SYNERR("Expecting first argument in .stabn statement");
		}
#line 2770 "y.tab.c"
    break;

  case 134: /* datalist: STABN expr error  */
#line 641 "parser.y"
                {
		SYNERR("Expecting comma after first argument in .stabn statement");
		}
#line 2778 "y.tab.c"
    break;

  case 135: /* datalist: STABN expr COMMA error  */
#line 645 "parser.y"
                {
		SYNERR("Expecting second argument in .stabn statement");
		}
#line 2786 "y.tab.c"
    break;

  case 136: /* datalist: STABN expr COMMA expr error  */
#line 649 "parser.y"
                {
		SYNERR("Expecting comma after second argument in .stabn statement");
		}
#line 2794 "y.tab.c"
    break;

  case 137: /* datalist: STABN expr COMMA expr COMMA error  */
#line 653 "parser.y"
                {
		SYNERR("Expecting third argument in .stabn statement");
		}
#line 2802 "y.tab.c"
    break;

  case 138: /* datalist: STABN expr COMMA expr COMMA expr error  */
#line 657 "parser.y"
                {
		SYNERR("Expecting comma after third argument in .stabn statement");
		}
#line 2810 "y.tab.c"
    break;

  case 139: /* datalist: STABN expr COMMA expr COMMA expr COMMA error  */
#line 661 "parser.y"
                {
		SYNERR("Expecting fourth argument in .stabn statement. Must be a string");
		}
#line 2818 "y.tab.c"
    break;

  case 140: /* datalist: STABR expr COMMA expr COMMA STRING  */
#line 665 "parser.y"
                {
		putstabr((yyvsp[-4].np), (yyvsp[-2].np), (yyvsp[0].p));
		}
#line 2826 "y.tab.c"
    break;

  case 141: /* datalist: STABR error  */
#line 669 "parser.y"
                {
		SYNERR("Expecting first argument in .stabr statement");
		}
#line 2834 "y.tab.c"
    break;

  case 142: /* datalist: STABR expr error  */
#line 673 "parser.y"
                {
		SYNERR("Expecting comma after first argument in .stabr statement");
		}
#line 2842 "y.tab.c"
    break;

  case 143: /* datalist: STABR expr COMMA error  */
#line 677 "parser.y"
                {
		SYNERR("Expecting second argument in .stabr statement");
		}
#line 2850 "y.tab.c"
    break;

  case 144: /* datalist: STABR expr COMMA expr error  */
#line 681 "parser.y"
                {
		SYNERR("Expecting comma after second argument in .stabr statement");
		}
#line 2858 "y.tab.c"
    break;

  case 145: /* datalist: STABR expr COMMA expr COMMA error  */
#line 685 "parser.y"
                {
		SYNERR("Expecting third argument in .stabr statement. Must be a string");
		}
#line 2866 "y.tab.c"
    break;

  case 146: /* datalist: STABP expr COMMA expr COMMA expr COMMA expr COMMA STRING  */
#line 689 "parser.y"
                {
		putstabp((yyvsp[-8].np), (yyvsp[-6].np), (yyvsp[-4].np), (yyvsp[-2].np), (yyvsp[0].p));
		}
#line 2874 "y.tab.c"
    break;

  case 147: /* datalist: STABP error  */
#line 693 "parser.y"
                {
		SYNERR("Expecting first argument in .stabp statement");
		}
#line 2882 "y.tab.c"
    break;

  case 148: /* datalist: STABP expr error  */
#line 697 "parser.y"
                {
		SYNERR("Expecting comma after first argument in .stabp statement");
		}
#line 2890 "y.tab.c"
    break;

  case 149: /* datalist: STABP expr COMMA error  */
#line 701 "parser.y"
                {
		SYNERR("Expecting second argument in .stabp statement");
		}
#line 2898 "y.tab.c"
    break;

  case 150: /* datalist: STABP expr COMMA expr error  */
#line 705 "parser.y"
                {
		SYNERR("Expecting comma after second argument in .stabp statement");
		}
#line 2906 "y.tab.c"
    break;

  case 151: /* datalist: STABP expr COMMA expr COMMA error  */
#line 709 "parser.y"
                {
		SYNERR("Expecting third argument in .stabp statement");
		}
#line 2914 "y.tab.c"
    break;

  case 152: /* datalist: STABP expr COMMA expr COMMA expr error  */
#line 713 "parser.y"
                {
		SYNERR("Expecting comma after third argument in .stabp statement");
		}
#line 2922 "y.tab.c"
    break;

  case 153: /* datalist: STABP expr COMMA expr COMMA expr COMMA error  */
#line 717 "parser.y"
                {
		SYNERR("Expecting fourth argument in .stabp statement");
		}
#line 2930 "y.tab.c"
    break;

  case 154: /* datalist: STABP expr COMMA expr COMMA expr COMMA expr error  */
#line 721 "parser.y"
                {
		SYNERR("Expecting comma after fourth argument in .stabp statement");
		}
#line 2938 "y.tab.c"
    break;

  case 155: /* datalist: STABP expr COMMA expr COMMA expr COMMA expr COMMA error  */
#line 725 "parser.y"
                {
		SYNERR("Expecting fifth argument in .stabp statement. Must be a string");
		}
#line 2946 "y.tab.c"
    break;

  case 156: /* datalist: STABL NUMBER  */
#line 729 "parser.y"
                {
		iputb(0x2c);
		iputw((int)(yyvsp[0].l));
		}
#line 2955 "y.tab.c"
    break;

  case 157: /* datalist: STABL error  */
#line 734 "parser.y"
                {
		SYNERR("Expecting line number in .stabl statement");
		}
#line 2963 "y.tab.c"
    break;

  case 158: /* bytedir: BYTE  */
#line 739 "parser.y"
                {
		deflabels();
		}
#line 2971 "y.tab.c"
    break;

  case 159: /* blist: belem COMMA blist  */
#line 744 "parser.y"
                {
		switch ((yyvsp[-2].np)->n_type) {
		case 6:
			locadv((yyvsp[-2].np)->n_w);
			(yyvsp[-2].np)->n_next = (yyvsp[0].np);
			break;
		case 1: case 9:
			(yyvsp[-2].np)->n_next = (yyvsp[0].np);
			locadv(1);
			break;
		case 0x16:
			(yyvsp[-2].np)->n_a->n_next = (yyvsp[0].np);
			(yyval.np) = (yyvsp[-2].np)->n_next;
			nfreed++;
			break;
		}
		}
#line 2993 "y.tab.c"
    break;

  case 160: /* blist: belem COMMA error  */
#line 762 "parser.y"
                {
		SYNERR("Expecting address expression after comma in .byte statement");
		}
#line 3001 "y.tab.c"
    break;

  case 161: /* blist: belem  */
#line 766 "parser.y"
                {
		switch ((yyvsp[0].np)->n_type) {
		case 6:
			locadv((yyvsp[0].np)->n_w);
			(yyvsp[0].np)->n_next = 0L;
			break;
		case 1: case 9:
			(yyvsp[0].np)->n_next = 0L;
			locadv(1);
			break;
		case 0x16:
			(yyvsp[0].np)->n_a->n_next = 0L;
			(yyval.np) = (yyvsp[0].np)->n_next;
			nfreed++;
			break;
		}
		}
#line 3023 "y.tab.c"
    break;

  case 162: /* belem: expr LPAR sexpr RPAR  */
#line 785 "parser.y"
                {
		if ((yyvsp[-1].np)->n_type == 3)
			(yyvsp[-1].np)->n_type = 1;
		else if ((yyvsp[-1].np)->n_type == 7)
			(yyvsp[-1].np)->n_type = 9;
		else if ((yyvsp[-1].np)->n_type == 0x19)
			(yyvsp[-1].np)->n_type = 23;
		(yyval.np) = mknode6((int16_t)(yyvsp[-3].np)->n_val, (yyvsp[-1].np));
		nfreed++;
		}
#line 3038 "y.tab.c"
    break;

  case 163: /* belem: expr LPAR error  */
#line 796 "parser.y"
                {
		SYNERR("Expecting address expression in repeat count in .byte statement");
		}
#line 3046 "y.tab.c"
    break;

  case 164: /* belem: expr LPAR sexpr error  */
#line 800 "parser.y"
                {
		SYNERR("Expecting right parenthesis after repeat count in .byte statement");
		}
#line 3054 "y.tab.c"
    break;

  case 165: /* belem: sexpr  */
#line 804 "parser.y"
                {
		if ((yyvsp[0].np)->n_type == 3)
			(yyvsp[0].np)->n_type = 1;
		else if ((yyvsp[0].np)->n_type == 7)
			(yyvsp[0].np)->n_type = 9;
		else if ((yyvsp[0].np)->n_type == 0x19)
			(yyvsp[0].np)->n_type = 23;
		}
#line 3067 "y.tab.c"
    break;

  case 166: /* belem: STRING  */
#line 813 "parser.y"
                {
		(yyval.np) = mkstring((yyvsp[0].p));
		}
#line 3075 "y.tab.c"
    break;

  case 167: /* worddir: WORD  */
#line 818 "parser.y"
                {
		even(); deflabels();
		}
#line 3083 "y.tab.c"
    break;

  case 168: /* wlist: welem COMMA wlist  */
#line 823 "parser.y"
                {
		if ((yyvsp[-2].np)->n_type == 6)
			locadv((yyvsp[-2].np)->n_w * 2);
		else
			locadv(2);
		(yyvsp[-2].np)->n_next = (yyvsp[0].np);
		}
#line 3095 "y.tab.c"
    break;

  case 169: /* wlist: welem COMMA error  */
#line 831 "parser.y"
                {
		SYNERR("Expecting address expression after comma in .word statement");
		}
#line 3103 "y.tab.c"
    break;

  case 170: /* wlist: welem  */
#line 835 "parser.y"
                {
		if ((yyvsp[0].np)->n_type == 6)
			locadv((yyvsp[0].np)->n_w * 2);
		else
			locadv(2);
		(yyvsp[0].np)->n_next = 0L;
		}
#line 3115 "y.tab.c"
    break;

  case 171: /* welem: expr LPAR sexpr RPAR  */
#line 844 "parser.y"
                {
		if ((yyvsp[-1].np)->n_type == 3)
			(yyvsp[-1].np)->n_type = 2;
		else if ((yyvsp[-1].np)->n_type == 7)
			(yyvsp[-1].np)->n_type = 8;
		else if ((yyvsp[-1].np)->n_type == 0x19)
			(yyvsp[-1].np)->n_type = 24;
		(yyval.np) = mknode6((int16_t)(yyvsp[-3].np)->n_val, (yyvsp[-1].np));
		nfreed++;
		}
#line 3130 "y.tab.c"
    break;

  case 172: /* welem: expr LPAR error  */
#line 855 "parser.y"
                {
		SYNERR("Expecting address expression in repeat count in .word statement");
		}
#line 3138 "y.tab.c"
    break;

  case 173: /* welem: expr LPAR sexpr error  */
#line 859 "parser.y"
                {
		SYNERR("Expecting right parenthesis after repeat count in .word statement");
		}
#line 3146 "y.tab.c"
    break;

  case 174: /* welem: sexpr  */
#line 863 "parser.y"
                {
		if ((yyvsp[0].np)->n_type == 3)
			(yyvsp[0].np)->n_type = 2;
		else if ((yyvsp[0].np)->n_type == 7)
			(yyvsp[0].np)->n_type = 8;
		else if ((yyvsp[0].np)->n_type == 0x19)
			(yyvsp[0].np)->n_type = 24;
		}
#line 3159 "y.tab.c"
    break;

  case 175: /* adrdir: ADDR  */
#line 873 "parser.y"
                {
		even(); deflabels();
		}
#line 3167 "y.tab.c"
    break;

  case 176: /* alist: alist COMMA aelem  */
#line 878 "parser.y"
                {
		if ((yyvsp[-2].np)->n_type == 6) {
			if (segmode != 0)
				locadv((yyvsp[-2].np)->n_w * 4);
			else
				locadv((yyvsp[-2].np)->n_w * 2);
		} else {
			if (segmode != 0)
				locadv(4);
			else
				locadv(2);
		}
		(yyvsp[-2].np)->n_next = (yyvsp[0].np);
		}
#line 3186 "y.tab.c"
    break;

  case 177: /* alist: alist COMMA error  */
#line 893 "parser.y"
                {
		SYNERR("Expecting address expression after comma in .addr statement");
		}
#line 3194 "y.tab.c"
    break;

  case 178: /* alist: aelem  */
#line 897 "parser.y"
                {
		if ((yyvsp[0].np)->n_type == 6) {
			if (segmode != 0)
				locadv((yyvsp[0].np)->n_w * 4);
			else
				locadv((yyvsp[0].np)->n_w * 2);
		} else {
			if (segmode != 0)
				locadv(4);
			else
				locadv(2);
		}
		(yyvsp[0].np)->n_next = 0L;
		}
#line 3213 "y.tab.c"
    break;

  case 179: /* aelem: expr LPAR sexpr RPAR  */
#line 913 "parser.y"
                {
		if (segmode == 0) {
			if ((yyvsp[-1].np)->n_type == 3)
				(yyvsp[-1].np)->n_type = 2;
			else if ((yyvsp[-1].np)->n_type == 7)
				(yyvsp[-1].np)->n_type = 8;
			else if ((yyvsp[-1].np)->n_type == 0x19)
				(yyvsp[-1].np)->n_type = 24;
		}
		(yyval.np) = mknode6((int16_t)(yyvsp[-3].np)->n_val, (yyvsp[-1].np));
		nfreed++;
		}
#line 3230 "y.tab.c"
    break;

  case 180: /* aelem: expr LPAR error  */
#line 926 "parser.y"
                {
		SYNERR("Expecting address expression in repeat count in .addr statement");
		}
#line 3238 "y.tab.c"
    break;

  case 181: /* aelem: expr LPAR sexpr error  */
#line 930 "parser.y"
                {
		SYNERR("Expecting right parenthesis after repeat count in .addr statement");
		}
#line 3246 "y.tab.c"
    break;

  case 182: /* aelem: sexpr  */
#line 934 "parser.y"
                {
		if (segmode == 0) {
			if ((yyvsp[0].np)->n_type == 3)
				(yyvsp[0].np)->n_type = 2;
			else if ((yyvsp[0].np)->n_type == 7)
				(yyvsp[0].np)->n_type = 8;
			else if ((yyvsp[0].np)->n_type == 0x19)
				(yyvsp[0].np)->n_type = 24;
		}
		}
#line 3261 "y.tab.c"
    break;

  case 183: /* longdir: LONG  */
#line 946 "parser.y"
                {
		even(); deflabels();
		}
#line 3269 "y.tab.c"
    break;

  case 184: /* llist: lelem COMMA llist  */
#line 951 "parser.y"
                {
		if ((yyvsp[-2].np)->n_type == 6)
			locadv((yyvsp[-2].np)->n_w * 4);
		else
			locadv(4);
		(yyvsp[-2].np)->n_next = (yyvsp[0].np);
		}
#line 3281 "y.tab.c"
    break;

  case 185: /* llist: lelem COMMA error  */
#line 959 "parser.y"
                {
		SYNERR("Expecting address expression after comma in .long statement");
		}
#line 3289 "y.tab.c"
    break;

  case 186: /* llist: lelem  */
#line 963 "parser.y"
                {
		if ((yyvsp[0].np)->n_type == 6)
			locadv((yyvsp[0].np)->n_w * 4);
		else
			locadv(4);
		(yyvsp[0].np)->n_next = 0L;
		}
#line 3301 "y.tab.c"
    break;

  case 187: /* lelem: expr LPAR sexpr RPAR  */
#line 972 "parser.y"
                {
		(yyval.np) = mknode6((int16_t)(yyvsp[-3].np)->n_val, (yyvsp[-1].np));
		nfreed++;
		}
#line 3310 "y.tab.c"
    break;

  case 188: /* lelem: expr LPAR error  */
#line 977 "parser.y"
                {
		SYNERR("Expecting address expression in repeat count in .long statement");
		}
#line 3318 "y.tab.c"
    break;

  case 189: /* lelem: expr LPAR sexpr error  */
#line 981 "parser.y"
                {
		SYNERR("Expecting right parenthesis after repeat count in .long statement");
		}
#line 3326 "y.tab.c"
    break;

  case 191: /* assign: name GASSIGN sexpr  */
#line 987 "parser.y"
                {
		deflabels();
		(yyvsp[-2].np)->n_c1 = 2;
		defsym((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3336 "y.tab.c"
    break;

  case 192: /* assign: name GASSIGN error  */
#line 993 "parser.y"
                {
		SYNERR("Expecting address expression in global assignment statement");
		}
#line 3344 "y.tab.c"
    break;

  case 193: /* assign: name LASSIGN sexpr  */
#line 997 "parser.y"
                {
		deflabels();
		(yyvsp[-2].np)->n_c1 = 4;
		defsym((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3354 "y.tab.c"
    break;

  case 194: /* assign: name LASSIGN error  */
#line 1003 "parser.y"
                {
		SYNERR("Expecting address expression or reserved word in local assignment statement");
		}
#line 3362 "y.tab.c"
    break;

  case 195: /* assign: name LASSIGN keyword  */
#line 1007 "parser.y"
                {
		deflabels();
		(yyvsp[-2].np)->n_c1 = 4;
		defalias((yyvsp[-2].np), (yyvsp[0].pp));
		}
#line 3372 "y.tab.c"
    break;

  case 196: /* assign: name error  */
#line 1013 "parser.y"
                {
		SYNERR("Expecting := , ::= , :: or : after identifier");
		}
#line 3380 "y.tab.c"
    break;

  case 197: /* assign: DOT EQ expr  */
#line 1017 "parser.y"
                {
		deflabels();
		setdot((yyvsp[0].np));
		}
#line 3389 "y.tab.c"
    break;

  case 198: /* assign: DOT error  */
#line 1022 "parser.y"
                {
		SYNERR("Expecting = after . for assignment of pc.");
		}
#line 3397 "y.tab.c"
    break;

  case 199: /* assign: DOT EQ error  */
#line 1026 "parser.y"
                {
		SYNERR("Expecting numeric expression after .=");
		}
#line 3405 "y.tab.c"
    break;

  case 238: /* sexpr: BAR expr BAR  */
#line 1070 "parser.y"
                {
		(yyval.np) = opshort((yyvsp[-1].np));
		shortaddr++;
		}
#line 3414 "y.tab.c"
    break;

  case 239: /* sexpr: BAR error  */
#line 1075 "parser.y"
                {
		SYNERR("Expecting numeric expression after short segmented address operator");
		}
#line 3422 "y.tab.c"
    break;

  case 240: /* sexpr: BAR expr error  */
#line 1079 "parser.y"
                {
		SYNERR("Expecting | at end of expression");
		}
#line 3430 "y.tab.c"
    break;

  case 241: /* sexpr: SSEG expr  */
#line 1083 "parser.y"
                {
		(yyval.np) = opseg((yyvsp[0].np));
		shortaddr++;
		}
#line 3439 "y.tab.c"
    break;

  case 242: /* sexpr: SSEG error  */
#line 1088 "parser.y"
                {
		SYNERR("Expecting numeric expression after ^S operator");
		}
#line 3447 "y.tab.c"
    break;

  case 243: /* sexpr: OSEG expr  */
#line 1092 "parser.y"
                {
		(yyval.np) = opoff((yyvsp[0].np));
		shortaddr++;
		}
#line 3456 "y.tab.c"
    break;

  case 244: /* sexpr: OSEG error  */
#line 1097 "parser.y"
                {
		SYNERR("Expecting numeric expression after ^O operator");
		}
#line 3464 "y.tab.c"
    break;

  case 246: /* expr: expr PLUS bexpr  */
#line 1103 "parser.y"
                {
		(yyval.np) = opadd((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3472 "y.tab.c"
    break;

  case 247: /* expr: expr PLUS error  */
#line 1107 "parser.y"
                {
		SYNERR("Expecting expression to add");
		}
#line 3480 "y.tab.c"
    break;

  case 248: /* expr: expr MINUS bexpr  */
#line 1111 "parser.y"
                {
		(yyval.np) = opsub((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3488 "y.tab.c"
    break;

  case 249: /* expr: expr MINUS error  */
#line 1115 "parser.y"
                {
		SYNERR("Expecting expression to subtract");
		}
#line 3496 "y.tab.c"
    break;

  case 251: /* bexpr: bexpr SHL term  */
#line 1121 "parser.y"
                {
		(yyval.np) = opshl((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3504 "y.tab.c"
    break;

  case 252: /* bexpr: bexpr SHL error  */
#line 1125 "parser.y"
                {
		SYNERR("Expecting numeric expression after ^< (shift left)");
		}
#line 3512 "y.tab.c"
    break;

  case 253: /* bexpr: bexpr SHR term  */
#line 1129 "parser.y"
                {
		(yyval.np) = opshr((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3520 "y.tab.c"
    break;

  case 254: /* bexpr: bexpr SHR error  */
#line 1133 "parser.y"
                {
		SYNERR("Expecting numeric expression after ^> (shift right)");
		}
#line 3528 "y.tab.c"
    break;

  case 255: /* bexpr: bexpr AND term  */
#line 1137 "parser.y"
                {
		(yyval.np) = opand((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3536 "y.tab.c"
    break;

  case 256: /* bexpr: bexpr AND error  */
#line 1141 "parser.y"
                {
		SYNERR("Expecting numeric expression after ^& (and)");
		}
#line 3544 "y.tab.c"
    break;

  case 257: /* bexpr: bexpr OR term  */
#line 1145 "parser.y"
                {
		(yyval.np) = opor((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3552 "y.tab.c"
    break;

  case 258: /* bexpr: bexpr OR error  */
#line 1149 "parser.y"
                {
		SYNERR("Expecting numeric expression after ^| (or)");
		}
#line 3560 "y.tab.c"
    break;

  case 259: /* bexpr: bexpr XOR term  */
#line 1153 "parser.y"
                {
		(yyval.np) = opxor((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3568 "y.tab.c"
    break;

  case 260: /* bexpr: bexpr XOR error  */
#line 1157 "parser.y"
                {
		SYNERR("Expecting numeric expression after ^X (exclusive-or)");
		}
#line 3576 "y.tab.c"
    break;

  case 262: /* term: term MUL unary  */
#line 1163 "parser.y"
                {
		(yyval.np) = opmul((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3584 "y.tab.c"
    break;

  case 263: /* term: term MUL error  */
#line 1167 "parser.y"
                {
		SYNERR("Expecting multiplier expression");
		}
#line 3592 "y.tab.c"
    break;

  case 264: /* term: term DIV unary  */
#line 1171 "parser.y"
                {
		(yyval.np) = opdiv((yyvsp[-2].np), (yyvsp[0].np));
		}
#line 3600 "y.tab.c"
    break;

  case 265: /* term: term DIV error  */
#line 1175 "parser.y"
                {
		SYNERR("Expecting divisor expression");
		}
#line 3608 "y.tab.c"
    break;

  case 267: /* unary: MINUS primary  */
#line 1181 "parser.y"
                {
		(yyval.np) = opneg((yyvsp[0].np));
		}
#line 3616 "y.tab.c"
    break;

  case 268: /* unary: MINUS error  */
#line 1185 "parser.y"
                {
		SYNERR("Expecting numeric expression after unary minus");
		}
#line 3624 "y.tab.c"
    break;

  case 269: /* unary: PLUS primary  */
#line 1189 "parser.y"
                {
		(yyval.np) = opplus((yyvsp[0].np));
		}
#line 3632 "y.tab.c"
    break;

  case 270: /* unary: PLUS error  */
#line 1193 "parser.y"
                {
		SYNERR("Expecting numeric expression after unary plus");
		}
#line 3640 "y.tab.c"
    break;

  case 272: /* primary: NUMBER  */
#line 1199 "parser.y"
                {
		(yyval.np) = mknode1(3, (yyvsp[0].l));
		}
#line 3648 "y.tab.c"
    break;

  case 273: /* primary: LSEG NUMBER RSEG NUMBER  */
#line 1203 "parser.y"
                {
		(yyval.np) = mknode1(3, ((uint32_t)(yyvsp[-2].l) << 24) | (yyvsp[0].l));
		}
#line 3656 "y.tab.c"
    break;

  case 274: /* primary: LSEG error  */
#line 1207 "parser.y"
                {
		SYNERR("Expecting segment number after <<");
		}
#line 3664 "y.tab.c"
    break;

  case 275: /* primary: LSEG NUMBER error  */
#line 1211 "parser.y"
                {
		SYNERR("Expecting >> after segment number");
		}
#line 3672 "y.tab.c"
    break;

  case 276: /* primary: LSEG NUMBER RSEG error  */
#line 1215 "parser.y"
                {
		SYNERR("Expecting segment offset number");
		}
#line 3680 "y.tab.c"
    break;

  case 277: /* primary: DOT  */
#line 1219 "parser.y"
                {
		(yyval.np) = dotval();
		}
#line 3688 "y.tab.c"
    break;

  case 278: /* primary: LPAR expr RPAR  */
#line 1223 "parser.y"
                {
		(yyval.np) = (yyvsp[-1].np);
		}
#line 3696 "y.tab.c"
    break;

  case 279: /* primary: LPAR error  */
#line 1227 "parser.y"
                {
		SYNERR("Expecting numeric expression after left parenthesis");
		}
#line 3704 "y.tab.c"
    break;

  case 280: /* primary: LPAR expr error  */
#line 1231 "parser.y"
                {
		SYNERR("Expecting right parenthesis in expression");
		}
#line 3712 "y.tab.c"
    break;

  case 282: /* symref: name  */
#line 1237 "parser.y"
                {
		if (nameflag != 0) {
			sputb(0x15);
			sputw((yyvsp[0].np)->n_w);
			nameflag = 0;
		}
		}
#line 3724 "y.tab.c"
    break;

  case 283: /* label: name DCOLON  */
#line 1246 "parser.y"
                {
		(yyvsp[-1].np)->n_c1 = 3;
		(yyvsp[-1].np)->n_next = plist;
		plist = (yyvsp[-1].np);
		}
#line 3734 "y.tab.c"
    break;

  case 284: /* label: name COLON  */
#line 1252 "parser.y"
                {
		(yyvsp[-1].np)->n_c1 = 5;
		(yyvsp[-1].np)->n_next = plist;
		plist = (yyvsp[-1].np);
		}
#line 3744 "y.tab.c"
    break;

  case 285: /* name: NEWLOCAL  */
#line 1259 "parser.y"
                {
		nameflag = 0;
		symmode = 6;
		(yyval.np) = mknode7(stenter((yyvsp[0].p), (unsigned char)symmode));
		nameflag++;
		}
#line 3755 "y.tab.c"
    break;

  case 286: /* name: NEWNAME  */
#line 1266 "parser.y"
                {
		nameflag = 0;
		symmode = 0;
		(yyval.np) = mknode7(stenter((yyvsp[0].p), (unsigned char)symmode));
		nameflag++;
		}
#line 3766 "y.tab.c"
    break;

  case 287: /* name: SYMBOL  */
#line 1273 "parser.y"
                {
		nameflag = 0;
		(yyval.np) = mknode7((yyvsp[0].i));
		}
#line 3775 "y.tab.c"
    break;


#line 3779 "y.tab.c"

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

#line 1279 "parser.y"


/* the messages of the parser itself; the others are made by SYNERR() */
static void
yyerror(const char *s)
{
	if (strcmp(s, "yacc stack overflow") == 0) {
		error(4, "yacc stack overflow\n");
	} else if (synerr == 0) {
		fprintf(stderr, "Syntax error(s) found\n");
		fprintf(stderr, "Syntax checking will continue\n");
		fprintf(stderr, "No object file will be created\n");
		synerr++;
	}
}
