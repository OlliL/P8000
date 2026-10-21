
# line 2 "parser.y"
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

# line 49 "parser.y"
typedef union  {
	int32_t	l;
	int	i;
	char	*p;
	struct pst *pp;
	NODE	*np;
} YYSTYPE;
# define DOT 3
# define PLUS 4
# define MINUS 5
# define DIV 9
# define MUL 10
# define SHL 11
# define SHR 12
# define AND 13
# define OR 14
# define XOR 15
# define LSEG 16
# define RSEG 17
# define BAR 18
# define COLON 19
# define DCOLON 20
# define HASH 21
# define AT 22
# define SSEG 23
# define OSEG 24
# define LBRACE 25
# define RBRACE 26
# define LPAR 27
# define RPAR 28
# define NL 29
# define COMMA 30
# define EQ 31
# define GASSIGN 32
# define LASSIGN 33
# define SYMBOL 36
# define COMM 37
# define EXTERN 38
# define EVEN 39
# define ODD 40
# define BYTE 41
# define WORD 42
# define LONG 43
# define BLKB 44
# define BLKW 45
# define BLKL 46
# define STABL 47
# define STABN 48
# define STABP 49
# define STABR 50
# define REG 51
# define REGPAIR 52
# define BREG 53
# define COND 54
# define NVI 55
# define OPCODE 56
# define FLAGS 58
# define CTLREG 59
# define REGQUAD 60
# define FPREG 61
# define TOK62 62
# define TOK63 63
# define TOK64 64
# define TOK65 65
# define TOK66 66
# define TOK67 67
# define TOK68 68
# define TOK69 69
# define TOK70 70
# define TOK71 71
# define TOK72 72
# define FPFLAG 73
# define FPMODE 74
# define CODE 75
# define DATA 76
# define BSS 77
# define SEG 78
# define NONSEG 79
# define JPR 80
# define CALLR 81
# define PSEC 82
# define ADDR 83
# define LINE 84
# define NUMBER 85
# define NEWNAME 86
# define NEWLOCAL 87
# define STRING 88
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 1279 "parser.y"


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
short yyexca[] ={
-1, 1,
	0, -1,
	3, 7,
	25, 7,
	26, 7,
	36, 7,
	37, 7,
	38, 7,
	39, 7,
	40, 7,
	41, 7,
	42, 7,
	43, 7,
	44, 7,
	45, 7,
	46, 7,
	47, 7,
	48, 7,
	49, 7,
	50, 7,
	56, 7,
	62, 7,
	63, 7,
	64, 7,
	65, 7,
	66, 7,
	67, 7,
	68, 7,
	75, 7,
	76, 7,
	77, 7,
	78, 7,
	79, 7,
	80, 7,
	81, 7,
	82, 7,
	83, 7,
	84, 7,
	86, 7,
	87, 7,
	-2, 0,
	};
# define YYNPROD 287
# define YYLAST 1408
short yyact[]={

  96, 325, 121, 377,  61, 160,  16,  68,  67, 297,
 228, 229, 357, 355,  69, 109,  84,  16, 346, 245,
  64,  65,  35,  85, 217, 218, 270, 228, 229, 228,
 229, 228, 229, 228, 229, 128, 375, 131, 136, 140,
 144, 146, 148, 150, 152, 154, 156, 228, 229, 165,
 162, 228, 229, 369, 344, 367, 272, 352, 342, 350,
 132, 137, 141, 145, 340, 338, 118, 114, 113, 371,
 209, 299,  34,  33, 336, 361, 332, 348, 328, 117,
   4,  93, 354, 228, 229, 108,  94,  95, 166, 168,
 119, 265, 143, 138, 222, 224, 226, 277, 241,  35,
 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
 112,  83, 215,  35, 181, 182, 183, 184, 185, 186,
 243, 187, 188, 189, 190, 191, 192, 193, 194, 195,
 196, 197, 198, 199, 200, 201, 202, 203, 204, 205,
 206, 207, 208,  17,  35, 260, 228, 229, 116,  34,
  33, 158, 252, 126, 250,   7, 134, 110, 248, 246,
 129,  35, 221,  34,  33,   8,   9, 228, 229, 267,
  63, 378, 261, 220, 228, 229,  35,  48,  47,  27,
  28,  57,  58,  59,  40,  41,  42,  46,  43,  45,
  44, 211, 214, 258,  34,  33,  49, 276, 213, 269,
 256, 212,  50,  51,  52,  53,  54,  55,  56, 228,
 229,  34,  33, 236, 235,  24,  25,  26,  29,  30,
  22,  23,  32,  60,  31,  38,  34,  33,   2, 219,
   6,  62, 118, 114, 113, 254, 339, 372, 275,  70,
 358, 356, 366, 362,  66, 117, 347, 131, 298, 136,
  35, 140,  37, 144, 118, 317, 119, 319, 111, 321,
  36, 271, 376, 162, 327,  35, 323, 117, 274, 242,
 132, 303, 137, 307, 141, 311, 145, 315, 119, 370,
 163, 368, 345, 353,  20, 351, 343,  35, 228, 229,
  19, 330, 341,  18, 334, 115, 266, 228, 229, 300,
  34,  33, 337, 349, 333,  13, 329,   5,  12,  11,
 118, 114, 113,  10, 116,  34,  33,   3,   1, 169,
 253, 170, 159, 117, 127,  93,  92,  91,  88,  89,
  94,  95, 273, 326, 119, 278, 116,  34,  33, 118,
 114, 113,  90,  35, 309, 313, 293, 295,  21, 359,
  71, 142, 117, 363,  39,  75, 365, 327,  86,  87,
 101,  97, 100, 119, 324,  99,  98, 102, 104,  72,
 373,  74,  35, 122, 124, 118, 107, 103, 105, 106,
  82, 161, 118, 114, 113,  78, 279, 281, 117,  77,
  76,  81, 116,  34,  33, 117,  15,  93, 262, 119,
  88,  89,  94,  95, 268, 305, 119, 301,  35, 237,
 239,  80, 118, 114, 113,  35,  79,  73, 120, 259,
 139, 116,  34,  33, 135, 117, 257, 130,  14,   0,
  86,  87, 101,  97, 100,   0, 119,  99,  98, 102,
 104,   0,   0,   0,   0,  35,   0,   0, 107, 103,
 105, 106,   0,   0, 118, 114, 113, 116,  34,  33,
   0, 255,   0,   0, 116,  34,  33, 117,   0,  93,
   0,   0,  88,  89,  94,  95, 228, 229, 119, 230,
 231, 232, 233, 234,   0, 374,   0,  35,   0, 283,
 285, 287, 289, 291, 116,  34,  33,   0,   0, 251,
   0,   0,  86,  87, 101,  97, 100, 240,   0,  99,
  98, 102, 104,   0,   0,   0,   0, 228, 229,   0,
 107, 103, 105, 106,   0,   0, 118, 114, 113, 228,
 229,   0,   0,   0,   0,   0, 116,  34,  33, 117,
 249,  93,   0,   0,   0,   0,  94,  95,   0,   0,
 119,   0, 247,   0, 118, 114, 113,   0,   0,  35,
   0,   0, 263, 264, 118, 114, 113, 117,   0,  93,
   0,   0,   0,   0,  94,  95,   0, 117, 119,  93,
 118, 114, 113,   0,  94,  95,   0,  35, 119,   0,
   0,   0, 364, 117,   0,  93,   0,  35,   0,   0,
  94,  95,   0,   0, 119,   0,   0,   0, 116,  34,
  33, 133,   0,  35,   0,   0,   0,   0,   0, 118,
 114, 113,   0,   0,   0,   0,   0,   0, 238,   0,
   0,   0, 117,   0,  93, 125, 116,  34,  33,  94,
  95,   0,   0, 119,   0,   0, 116,  34,  33,   0,
   0,   0,  35, 118, 114, 113,   0,   0,   0,   0,
   0,   0, 116,  34,  33, 360, 117,   0,  93,   0,
   0,   0,   0,  94,  95,   0,   0, 119,   0,   0,
   0,   0,   0,   0,   0,   0,  35,   0,   0,   0,
   0, 118, 114, 113,   0,   0,   0,   0,   0,   0,
   0, 116,  34,  33, 117,   0,  93, 123, 118, 114,
 113,  94,  95,   0,   0, 119,   0,   0, 118, 114,
 113, 117,   0,  93,  35,   0,   0,   0,  94,  95,
   0, 117, 119,  93,   0, 116,  34,  33,  94,  95,
   0,  35, 119, 118, 114, 113,   0, 118, 114, 113,
   0,  35,   0,   0,   0,   0, 117,   0,  93,   0,
 117,   0,   0,  94,  95,   0,   0, 119,   0,   0,
   0, 119,   0, 116,  34,  33,  35,   0,   0, 302,
  35, 118, 114, 113,   0, 118, 114, 113,   0,   0,
 116,  34,  33,   0, 117, 118, 114, 113, 117,   0,
 116,  34,  33,   0,   0, 119,   0, 335, 117, 119,
  93, 118, 114, 113,  35,  94,  95, 331,  35, 119,
   0, 118, 114, 113, 117, 116,  34,  33,  35, 116,
  34,  33,   0, 316, 117, 119,  93, 118, 114, 113,
   0,  94,  95,   0,  35, 119,   0, 118, 114, 113,
 117,   0,   0,   0,  35,   0,   0,   0,   0,   0,
 117, 119,   0, 116,  34,  33,   0, 116,  34,  33,
  35, 119, 314,   0, 118, 114, 113, 116,  34,  33,
  35,   0,   0,   0, 118, 114, 113, 117,   0,   0,
   0,   0,   0, 116,  34,  33,   0, 117, 119, 118,
 114, 113,   0, 116,  34,  33, 312,  35, 119, 118,
 114, 113, 117,   0,   0,   0,   0,  35,   0, 116,
  34,  33, 117, 119,   0,   0, 118, 114, 113, 116,
  34,  33,  35, 119,   0,   0, 118, 114, 113, 117,
   0,   0,  35,   0, 310,   0,   0,   0,   0, 117,
 119,   0,   0,   0,   0,   0, 116,  34,  33,  35,
 119, 308, 118, 114, 113,   0, 116,  34,  33,  35,
   0, 306,   0,   0,   0, 117,   0,   0,   0,   0,
   0, 116,  34,  33,   0,   0, 119,   0, 118, 114,
 113, 116,  34,  33,   0,  35, 304,   0,   0,   0,
 322, 117, 118, 114, 113,   0,   0,   0, 116,  34,
  33,   0, 119,   0,   0, 117,   0,   0, 116,  34,
  33,  35,   0, 118, 114, 113, 119, 118, 114, 113,
 118, 114, 113,   0, 320,  35, 117,   0, 318,   0,
 117,   0,   0, 117, 116,  34,  33, 119, 216,   0,
   0, 119,   0,   0, 119,   0,  35,   0,   0,   0,
  35,   0,   0,  35, 296, 118, 114, 113,   0,   0,
 116,  34,  33,   0, 167, 118, 114, 113, 117,   0,
   0,   0,   0,   0, 116,  34,  33,   0, 117, 119,
 294, 118, 114, 113,   0,   0,   0,   0,  35, 119,
 292,   0,   0,   0, 117, 116,  34,  33,  35, 116,
  34,  33, 116,  34,  33, 119,   0, 118, 114, 113,
 118, 114, 113,   0,  35,   0,   0, 290,   0,   0,
 117,   0,   0, 117,   0,   0,   0, 288,   0,   0,
   0, 119,   0,   0, 119,   0,   0, 116,  34,  33,
  35,   0, 286,  35, 118, 114, 113, 116,  34,  33,
   0,   0, 284,   0,   0,   0,   0, 117,   0,   0,
   0,   0,   0, 116,  34,  33,   0,   0, 119, 282,
   0,   0,   0,   0,   0,   0,   0,  35,   0, 280,
   0,   0,   0,   0,   0,   0,   0,   0,   0, 116,
  34,  33, 116,  34,  33,   0, 118, 114, 113,   0,
   0,   0,   0,   0,   0, 244,   0,   0,   0, 117,
   0,  93,   0,   0,   0,   0,  94,  95,   0,   0,
 119,   0, 118, 114, 113,   0, 116,  34,  33,  35,
   0, 227,   0,   0,   0, 117,   0,  93,   0,   0,
   0,   0,  94,  95,   0, 225, 119,   0,   0,   0,
   0,   0,   0,   0,   0,  35,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0, 223,   0,   0,   0,
 210,   0,   0, 164, 118, 114, 113,   0, 116,  34,
  33, 133,   0,   0,   0,   0,   0, 117,   0,  93,
   0,   0,  88,  89,  94,  95,   0,   0, 119,   0,
   0,   0,   0,   0, 116,  34,  33,  35, 157,   0,
   0,   0,   0,   0,   0,   0,   0,   0, 155,   0,
   0,   0,  86,  87, 101,  97, 100,   0,   0,  99,
  98, 102, 104,   0, 153,   0,   0,   0,   0,   0,
 107, 103, 105, 106, 108, 109,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0, 116,  34,  33,   0,
 151,   0,   0, 149,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0, 147 };
short yypact[]={

  51,  51,-1000, 140,-1000,-1000,-1000,-  25,-1000,-1000,
-1000,-1000,-1000,-1000, 140,-1000,-  12,-  17,-1000,-1000,
-1000,1281, 451, 379,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,  68, 214,-1000,-1000,-1000,1203,1229,1229,1229,
1151,1117,1114,1088,1072,1062,  66, 125,1027,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000, 818,  63,-1000,-1000,-1000,1024,
-1000,-1000,-1000, 161,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000, 174, 171, 165, 792,-  27,
-1000, 143, 132,1020, 999, 985, 284,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
 468, 204,-1000, 372, 251,-1000,-1000,  13,-1000, 959,
-1000,-1000,-1000,-1000,-1000,-1000,-  69,-1000,-1000,-1000,
 129, 525,-1000,-1000,-1000, 128, 513,-1000,-1000, 124,
 472,-1000, 122,-1000, 293,-1000, 284,-1000, 284,-1000,
 284,-1000, 205,-1000, 170,-1000, 163,-1000,-1000,-1000,
-1000,-1000, 115,-1000,-1000, 142,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000, 284,
-1000, 307,  40, 148,   5,-1000,-1000,-1000,-1000,-1000,
  12,-  59,  79,-1000, 284,-1000, 284,-1000, 933, 923,
 906, 896, 881, 871, 844, 834, 808,-1000,-1000,-1000,
-1000,-   8,-1000,  43,-1000,-1000, 523, 740, 715, 705,
 688, 650, 616, 577, 782,-1000, 778,-1000, 744,-1000,
 108,  77,-1000,-1000,-1000,  50,-1000, 561,-1000,  48,
 551,-1000,  46,-1000,-1000,-1000,-1000,-1000,-1000, 468,
-1000, 468,-1000, 204,-1000, 204,-1000, 204,-1000, 204,
-1000, 204,-1000,-1000,-1000,-1000,-1000,-  20,-1000,-1000,
-1000,-1000,-1000,  36,-1000,-1000,-1000,  30,-1000,-1000,
-1000,  26,-1000,-1000,-1000,-  10,-1000,  47,-1000,  29,
-1000,  27,-1000,-1000,-1000,-1000,-1000,  52,-1000,-1000,
-  15,-1000,-1000,-1000,-  16,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000, 409,-1000,
-  13,-1000, 336,-1000,-  14,-1000,-1000,-1000,-1000,  25,
-1000,-1000,-1000,  23,-1000,-1000,-1000,-  19,-1000, 229,
-1000,-1000,-1000,   6,-1000,-  85,-1000,-1000,-1000 };
short yypgo[]={

   0, 428,   2, 427, 160, 424, 156, 420,  93, 418,
 417, 350, 416, 411, 391, 390, 389, 385, 380,   0,
  23, 157,   5,   1, 371, 369, 355, 258, 295, 110,
 354,  92, 351, 348, 342, 111, 327,  16, 326, 321,
 318, 228, 317, 155, 313, 309, 308, 305, 293, 290,
 284, 280, 260, 252, 225 };
short yyr1[]={

   0,  40,  40,  41,  41,  41,  41,  42,  43,  43,
  43,  43,  43,  43,  43,  43,  43,  47,  47,  47,
  47,  47,  47,  47,  47,  47,  47,  47,  47,  46,
  46,  46,  46,  46,  46,  46,  33,  33,  33,  33,
  33,  33,  33,  33,  11,  11,  11,  10,  10,  10,
  10,  10,  10,  10,  18,  34,  34,  34,  34,  34,
  34,  34,  34,  34,  34,  34,  34,  34,  25,  25,
  26,  37,  37,  37,  38,  24,  35,  35,  35,  36,
  15,  15,  15,  16,  16,  16,  16,  16,  16,  16,
  16,  17,  17,  17,  17,  12,  12,  13,  14,  14,
  14,  45,  45,  45,  49,  49,  22,  22,  22,  50,
  50,  51,  51,  51,  23,  23,  23,  48,  48,  48,
  48,  48,  48,  48,  48,  48,  48,  48,  48,  48,
  48,  48,  48,  48,  48,  48,  48,  48,  48,  48,
  48,  48,  48,  48,  48,  48,  48,  48,  48,  48,
  48,  48,  48,  48,  48,  48,  48,  52,   4,   4,
   4,   3,   3,   3,   3,   3,  53,   6,   6,   6,
   5,   5,   5,   5,  30,  32,  32,  32,  31,  31,
  31,  31,  54,   8,   8,   8,   7,   7,   7,   7,
  44,  44,  44,  44,  44,  44,  44,  44,  44,  39,
  39,  39,  39,  39,  39,  39,  39,  39,  39,  39,
  39,  39,  39,  39,  39,  39,  39,  39,  39,  39,
  39,  39,  39,  39,  39,  39,  39,  39,  39,  39,
  39,  39,  39,  39,  39,  39,  39,  20,  20,  20,
  20,  20,  20,  20,  20,  19,  19,  19,  19,  19,
  21,  21,  21,  21,  21,  21,  21,  21,  21,  21,
  21,  27,  27,  27,  27,  27,  29,  29,  29,  29,
  29,  28,  28,  28,  28,  28,  28,  28,  28,  28,
  28,   9,   1,   1,   2,   2,   2 };
short yyr2[]={

   0,   2,   1,   3,   3,   1,   1,   0,   1,   1,
   1,   1,   1,   1,   2,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   3,   2,   2,   1,   2,   2,
   1,   2,   2,   2,   2,   2,   1,   1,   1,   1,
   1,   1,   1,   1,   3,   3,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   3,   3,   1,   1,   1,   3,   3,   1,   1,
   4,   3,   4,   5,   3,   4,   5,   5,   3,   4,
   5,   4,   4,   4,   4,   2,   2,   1,   2,   2,
   2,   1,   1,   1,   2,   2,   3,   3,   1,   2,
   2,   3,   2,   3,   3,   3,   1,   2,   2,   2,
   2,   1,   1,   1,   1,   2,   2,   2,   2,   2,
   2,   8,   2,   3,   4,   5,   6,   7,   8,   6,
   2,   3,   4,   5,   6,  10,   2,   3,   4,   5,
   6,   7,   8,   9,  10,   2,   2,   1,   3,   3,
   1,   4,   3,   4,   1,   1,   1,   3,   3,   1,
   4,   3,   4,   1,   1,   3,   3,   1,   4,   3,
   4,   1,   1,   3,   3,   1,   4,   3,   4,   1,
   3,   3,   3,   3,   3,   2,   3,   2,   3,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   3,   2,   3,
   2,   2,   2,   2,   1,   3,   3,   3,   3,   1,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   1,   3,   3,   3,   3,   1,   2,   2,   2,   2,
   1,   1,   4,   2,   3,   4,   1,   3,   2,   3,
   1,   1,   2,   2,   1,   1,   1 };
short yychk[]={

-1000,-  40,-  41,-  42,  29, 256,-  41,-  43,  25,  26,
-  44,-  45,-  46,-  47,-   1, 256,-   2,   3,-  48,-  49,
-  50,-  33,  80,  81,  75,  76,  77,  39,  40,  78,
  79,  84,  82,  87,  86,  36,-  52,-  53,-  54,-  30,
  44,  45,  46,  48,  50,  49,  47,  38,  37,  56,
  62,  63,  64,  65,  66,  67,  68,  41,  42,  43,
  83,  29, 256,-  43,  32,  33, 256,  20,  19,  31,
 256,-  11,-  25,-  10,-  24,-  26,-  15,-  16,-  17,-  12,
-  13,-  14,-  18,-  35,-  37,-  20,  51,  52,  21,  22,
-  34,-  36,-  38,  18,  23,  24,-  19,  54,  59,  58,
  55,  53,  60,  70,  61,  71,  72,  69,  73,  74,
-  21,-  27,-  29,   5,   4,-  28,  85,  16,   3,  27,
-   9,-   2,-  11, 256,-  11, 256,  85, 256,-   2,-   4,
-   3,-  19,-  20,  88,-   6,-   5,-  19,-  20,-   8,-   7,
-  19,-  20,-  32,-  31,-  19,-  20,-  19, 256,-  19, 256,
-  19, 256,-  19, 256,-  19, 256,-  19, 256,  85, 256,
-  22, 256,-   2,-  51, 256,-  19,-  20, 256,-  20, 256,
-  39,  37,  38,  39,  40,  41,  42,  43,  44,  45,
  46,  51,  52,  53,  54,  55,  56,  58,  59,  60,
  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,
  71,  72,  73,  74,  75,  76,  77,  78,  79,-  19,
 256,  30,  27,  27,  27,-  20, 256,  51,  52, 256,
  30,  30,-  19, 256,-  19, 256,-  19, 256,   4,   5,
  11,  12,  13,  14,  15,  10,   9,-  28, 256,-  28,
 256,  85, 256,-  19, 256,  88,  30,  27,  30,  27,
  30,  27,  30,  27,  30, 256,  30, 256,  30, 256,
  30,  30, 256,-  11, 256,  51, 256,  21, 256,  51,
  21, 256,  51,-  35, 256,-  37, 256,  18, 256,-  21,
 256,-  21, 256,-  27, 256,-  27, 256,-  27, 256,-  27,
 256,-  27, 256,-  29, 256,-  29, 256,  17, 256,  28,
 256,-   4, 256,-  20, 256,-   6, 256,-  20, 256,-   8,
 256,-  20, 256,-  31, 256,-  20, 256,-  19, 256,-  19,
 256,-  19, 256,-  22, 256,-  23, 256,-   2,  28, 256,
-  20, 256,  28, 256,-  20, 256,  28, 256,  85, 256,
  28, 256,  28, 256,  28, 256,  28, 256,  30, 256,
  30, 256,  30, 256,  30,  28, 256,  28, 256,-  19,
 256,  88, 256,-  19, 256,-  23, 256,  30, 256,  30,
 256,  88, 256,-  19, 256,  30, 256,  88, 256 };
short yydef[]={

   7,-   2,   2,   0,   5,   6,   1,   0,   8,   9,
  10,  11,  12,  13,  15,  16,   0,   0, 101, 102,
 103,  30,   0,   0,  17,  18,  19,  20,  21,  22,
  23,   0,  27, 284, 285, 286, 121, 122, 123, 124,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,
  37,  38,  39,  40,  41,  42,  43, 157, 166, 182,
 174,   3,   4,  14,   0,   0, 195, 282, 283,   0,
 197,  29,  31,  46,  68,  69,  47,  48,  49,  50,
  51,  52,  53,  75,  70,  97,  59,  60,   0,   0,
  54,  78,  73,   0,   0,   0, 244,  55,  56,  57,
  58,  61,  62,  63,  64,  65,  66,  67,  79,  74,
 249, 260, 265,   0,   0, 270, 271,   0, 276,   0,
 280, 281,  32,  33,  34,  35,  25,  26,  28, 117,
 160, 244, 164, 165, 118, 169, 244, 173, 119, 185,
 244, 189, 120, 177, 244, 181, 125, 126, 127, 128,
 129, 130,   0, 132,   0, 140,   0, 146, 155, 156,
 104, 105, 108, 109, 110,   0, 190, 191, 192, 193,
 194, 199, 200, 201, 202, 203, 204, 205, 206, 207,
 208, 209, 210, 211, 212, 213, 214, 215, 216, 217,
 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
 228, 229, 230, 231, 232, 233, 234, 235, 236, 196,
 198,   0,   0,   0,   0,  95,  96,  98,  99, 100,
   0,   0,   0, 238, 240, 241, 242, 243,   0,   0,
   0,   0,   0,   0,   0,   0,   0, 266, 267, 268,
 269,   0, 273,   0, 278,  24,   0,   0,   0,   0,
   0,   0,   0,   0,   0, 133,   0, 141,   0, 147,
   0,   0, 112,  44,  45,   0,  81,   0,  84,   0,
   0,  88,   0,  76,  77,  71,  72, 237, 239, 245,
 246, 247, 248, 250, 251, 252, 253, 254, 255, 256,
 257, 258, 259, 261, 262, 263, 264,   0, 274, 277,
 279, 158, 159,   0, 162, 167, 168,   0, 171, 183,
 184,   0, 187, 175, 176,   0, 179,   0, 134,   0,
 142,   0, 148, 106, 107, 111, 113, 116,  80,  82,
   0,  85,  91,  92,   0,  89,  93,  94, 272, 275,
 161, 163, 170, 172, 186, 188, 178, 180,   0, 135,
   0, 143,   0, 149,   0,  83,  86,  87,  90,   0,
 136, 139, 144,   0, 150, 114, 115,   0, 137,   0,
 151, 131, 138,   0, 152,   0, 153, 145, 154 };
#
/* @[$]yaccpar	1.1  07/24/82 02:17:32 - 87wega3.2 */
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

int yydebug = 0; /* 1 for debugging */
YYSTYPE yyv[YYMAXDEPTH + 1]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

int yyparse(void) {

	short yys[YYMAXDEPTH + 1];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

			if( yydebug ) printf( "error recovery discards char %d\n", yychar );

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

		if( yydebug ) printf("reduce %d\n",yyn);
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 3:
# line 149 "parser.y"
{
		chknodes(); curline++;
		} break;
case 4:
# line 153 "parser.y"
{
		SYNERR("Expecting carriage return or linefeed"); chknodes(); yychar = -1; curline++;
		} break;
case 5:
# line 157 "parser.y"
{
		curline++;
		} break;
case 6:
# line 161 "parser.y"
{
		SYNERR("Expecting beginning of line");
		} break;
case 7:
# line 166 "parser.y"
{
		iputline();
		} break;
case 8:
# line 171 "parser.y"
{
		deflabels(); blkopen();
		} break;
case 9:
# line 175 "parser.y"
{
		deflabels(); blkclose();
		} break;
case 15:
# line 184 "parser.y"
{
		deflabels();
		} break;
case 16:
# line 188 "parser.y"
{
		SYNERR("Expecting beginning of program");
		} break;
case 17:
# line 193 "parser.y"
{
		deflabels(); setsub(0); iputb(12);
		} break;
case 18:
# line 197 "parser.y"
{
		deflabels(); setsub(1); iputb(13);
		} break;
case 19:
# line 201 "parser.y"
{
		deflabels(); setsub(2); iputb(14);
		} break;
case 20:
# line 205 "parser.y"
{
		even(); deflabels();
		} break;
case 21:
# line 209 "parser.y"
{
		odd();
		} break;
case 22:
# line 213 "parser.y"
{
		segmode++;
		} break;
case 23:
# line 217 "parser.y"
{
		segmode = 0;
		} break;
case 26:
# line 223 "parser.y"
{
		SYNERR("Expecting line number in .line statement");
		} break;
case 27:
# line 227 "parser.y"
{
		setsec(0); iputb(0x29); iputb(0);
		} break;
case 28:
# line 231 "parser.y"
{
		secname(yypvt[-0].np);
		} break;
case 29:
# line 236 "parser.y"
{
		even(); deflabels(); putins(yypvt[-1].pp, yypvt[-0].np);
		} break;
case 30:
# line 240 "parser.y"
{
		even(); deflabels(); putins(yypvt[-0].pp, mknode2(0L, 0L, 0));
		} break;
case 31:
# line 244 "parser.y"
{
		even(); deflabels(); putins(yypvt[-1].pp, yypvt[-0].np);
		} break;
case 32:
# line 248 "parser.y"
{
		even(); deflabels(); jpr(yypvt[-1].pp->p_tok, yypvt[-0].np);
		} break;
case 33:
# line 252 "parser.y"
{
		SYNERR("Expecting operand(s) after jpr pseudo-instruction");
		} break;
case 34:
# line 256 "parser.y"
{
		even(); deflabels(); jpr(yypvt[-1].pp->p_tok, yypvt[-0].np);
		} break;
case 35:
# line 260 "parser.y"
{
		SYNERR("Expecting operand(s) after callr pseudo-instruction");
		} break;
case 44:
# line 274 "parser.y"
{
		yypvt[-2].np->n_next = yypvt[-0].np;
		} break;
case 45:
# line 278 "parser.y"
{
		SYNERR("Expecting operand after comma in list of operands");
		} break;
case 46:
# line 282 "parser.y"
{
		yypvt[-0].np->n_next = 0L;
		} break;
case 54:
# line 295 "parser.y"
{
		yyval.np = mknode2(yypvt[-0].pp->p_tok, mknode1(1, yypvt[-0].pp->p_n), 0);
		} break;
case 70:
# line 317 "parser.y"
{
		yyval.np = mknode2(yypvt[-0].pp->p_tok, mknode1(1, fpmode), 0);
		fpmode = 0;
		nround = 0;
		ninf = 0;
		} break;
case 71:
# line 325 "parser.y"
{
		switch (yypvt[-2].pp->p_n) {
		case 0x60: case 0x62: case 0x64: case 0x66:
			if (nround != 0)
				error(2, "Too many rounding modes specified\n");
			nround++;
			fpmode |= yypvt[-2].pp->p_n;
			break;
		case 0x10: case 0x11:
			if (nround != 0)
				error(2, "Too many infinity modes specified\n");
			ninf++;
			fpmode |= yypvt[-2].pp->p_n;
			break;
		}
		} break;
case 72:
# line 342 "parser.y"
{
		SYNERR("Expecting floating point mode after comma");
		} break;
case 73:
# line 346 "parser.y"
{
		switch (yypvt[-0].pp->p_n) {
		case 0x60: case 0x62: case 0x64: case 0x66:
			if (nround != 0)
				error(2, "Too many rounding modes specified\n");
			nround++;
			fpmode |= yypvt[-0].pp->p_n;
			break;
		case 0x10: case 0x11:
			if (nround != 0)
				error(2, "Too many infinity modes specified\n");
			ninf++;
			fpmode |= yypvt[-0].pp->p_n;
			break;
		}
		} break;
case 75:
# line 366 "parser.y"
{
		yyval.np = mknode2(yypvt[-0].pp->p_tok, mknode1(1, fpflag), 0);
		fpflag = 0;
		} break;
case 76:
# line 372 "parser.y"
{
		fpflag |= yypvt[-2].pp->p_n;
		} break;
case 77:
# line 376 "parser.y"
{
		SYNERR("Expecting floating point flag after comma");
		} break;
case 78:
# line 380 "parser.y"
{
		fpflag |= yypvt[-0].pp->p_n;
		} break;
case 80:
# line 387 "parser.y"
{
		yyval.np = mknode2(0x61, yypvt[-3].np, mknode1(1, yypvt[-1].pp->p_n));
		} break;
case 81:
# line 391 "parser.y"
{
		SYNERR("Expecting register number after left parenthesis in indexed operand");
		} break;
case 82:
# line 395 "parser.y"
{
		SYNERR("Expecting right parenthesis in indexed operand");
		} break;
case 83:
# line 400 "parser.y"
{
		yyval.np = mknode2(0x63, mknode1(1, yypvt[-4].pp->p_n), yypvt[-1].np);
		} break;
case 84:
# line 404 "parser.y"
{
		SYNERR("Expecting # or register after left parenthesis");
		} break;
case 85:
# line 408 "parser.y"
{
		SYNERR("Expecting address expression after # in base addressed operand");
		} break;
case 86:
# line 412 "parser.y"
{
		SYNERR("Expecting right parenthesis in base addressed operand");
		} break;
case 87:
# line 416 "parser.y"
{
		yyval.np = mknode2(0x63, mknode1(1, yypvt[-4].pp->p_n), yypvt[-1].np);
		} break;
case 88:
# line 420 "parser.y"
{
		SYNERR("Expecting # or register after left parenthesis");
		} break;
case 89:
# line 424 "parser.y"
{
		SYNERR("Expecting segmented address expression in base addressed operand");
		} break;
case 90:
# line 428 "parser.y"
{
		SYNERR("Expecting right parenthesis in segmented base addressed operand");
		} break;
case 91:
# line 433 "parser.y"
{
		yyval.np = mknode2(0x64, mknode1(1, yypvt[-3].pp->p_n), mknode1(1, yypvt[-1].pp->p_n));
		} break;
case 92:
# line 437 "parser.y"
{
		SYNERR("Expecting right parenthesis in base indexed operand");
		} break;
case 93:
# line 441 "parser.y"
{
		yyval.np = mknode2(0x64, mknode1(1, yypvt[-3].pp->p_n), mknode1(1, yypvt[-1].pp->p_n));
		} break;
case 94:
# line 445 "parser.y"
{
		SYNERR("Expecting right parenthesis in segmented base indexed operand");
		} break;
case 95:
# line 450 "parser.y"
{
		yyval.np = mknode2(0x62, yypvt[-0].np, 0);
		} break;
case 96:
# line 454 "parser.y"
{
		SYNERR("Expecting address expression after # (immediate) operator");
		} break;
case 97:
# line 459 "parser.y"
{
		yyval.np = mknode2(0x56, yypvt[-0].np, 0);
		} break;
case 98:
# line 464 "parser.y"
{
		if (yypvt[-0].pp->p_n == 0) {
			error(2, "Cannot perform an indirection on r0\n");
			yyval.np = errnode();
		} else
			yyval.np = mknode2(0x60, mknode1(1, yypvt[-0].pp->p_n), 0);
		} break;
case 99:
# line 472 "parser.y"
{
		yyval.np = mknode2(0x60, mknode1(1, yypvt[-0].pp->p_n), 0);
		} break;
case 100:
# line 476 "parser.y"
{
		SYNERR("Expecting register or register pair after @ (indirect) operator");
		} break;
case 104:
# line 485 "parser.y"
{
		deflabels();
		} break;
case 105:
# line 489 "parser.y"
{
		SYNERR("Expecting identifier after .extern");
		} break;
case 106:
# line 494 "parser.y"
{
		yypvt[-2].np->n_c1 = 0;
		defsym(yypvt[-2].np, mknode1(3, 0));
		} break;
case 107:
# line 499 "parser.y"
{
		SYNERR("Expecting identifier after comma in .extern statement");
		} break;
case 108:
# line 503 "parser.y"
{
		yypvt[-0].np->n_c1 = 0;
		defsym(yypvt[-0].np, mknode1(3, 0));
		} break;
case 109:
# line 509 "parser.y"
{
		deflabels();
		} break;
case 110:
# line 513 "parser.y"
{
		SYNERR("Expecting numeric expression after .comm");
		} break;
case 111:
# line 518 "parser.y"
{
		defcomm(yypvt[-2].np, yypvt[-0].np);
		} break;
case 112:
# line 522 "parser.y"
{
		SYNERR("Expecting comma after numeric expression in .comm statement");
		} break;
case 113:
# line 526 "parser.y"
{
		SYNERR("Expecting identifier in .comm statement");
		} break;
case 114:
# line 531 "parser.y"
{
		yypvt[-2].np->n_c1 = 0;
		yypvt[-2].np->n_next = yypvt[-0].np;
		} break;
case 115:
# line 536 "parser.y"
{
		SYNERR("Expecting identifier after comma in .comm statement");
		} break;
case 116:
# line 540 "parser.y"
{
		yypvt[-0].np->n_c1 = 0;
		yypvt[-0].np->n_next = 0L;
		} break;
case 117:
# line 546 "parser.y"
{
		for (listp = yypvt[-0].np; listp != 0; listp = listp->n_next)
			iputnode(listp);
		} break;
case 118:
# line 551 "parser.y"
{
		for (listp = yypvt[-0].np; listp != 0; listp = listp->n_next)
			iputnode(listp);
		} break;
case 119:
# line 556 "parser.y"
{
		for (listp = yypvt[-0].np; listp != 0; listp = listp->n_next)
			iputnode(listp);
		} break;
case 120:
# line 561 "parser.y"
{
		for (listp = yypvt[-0].np; listp != 0; listp = listp->n_next)
			iputnode(listp);
		} break;
case 121:
# line 566 "parser.y"
{
		locadv(1);
		iputloc();
		} break;
case 122:
# line 571 "parser.y"
{
		locadv(2);
		iputloc();
		} break;
case 123:
# line 576 "parser.y"
{
		locadv(4);
		iputloc();
		} break;
case 124:
# line 581 "parser.y"
{
		if (segmode != 0)
			locadv(4);
		else
			locadv(2);
		iputloc();
		} break;
case 125:
# line 589 "parser.y"
{
		deflabels();
		if (exprtype(yypvt[-0].np) == 3)
			locadv((int16_t)yypvt[-0].np->n_val);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		} break;
case 126:
# line 599 "parser.y"
{
		SYNERR("Expecting numeric expression in .blkb statement");
		} break;
case 127:
# line 603 "parser.y"
{
		even();
		deflabels();
		if (exprtype(yypvt[-0].np) == 3)
			locadv((int16_t)yypvt[-0].np->n_val * 2);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		} break;
case 128:
# line 614 "parser.y"
{
		SYNERR("Expecting numeric expression in .blkw statement");
		} break;
case 129:
# line 618 "parser.y"
{
		even();
		deflabels();
		if (exprtype(yypvt[-0].np) == 3)
			locadv((int16_t)yypvt[-0].np->n_val * 4);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		} break;
case 130:
# line 629 "parser.y"
{
		SYNERR("Expecting numeric expression in .blkl statement");
		} break;
case 131:
# line 633 "parser.y"
{
		putstabn(yypvt[-6].np, yypvt[-4].np, yypvt[-2].np, yypvt[-0].p);
		} break;
case 132:
# line 637 "parser.y"
{
		SYNERR("Expecting first argument in .stabn statement");
		} break;
case 133:
# line 641 "parser.y"
{
		SYNERR("Expecting comma after first argument in .stabn statement");
		} break;
case 134:
# line 645 "parser.y"
{
		SYNERR("Expecting second argument in .stabn statement");
		} break;
case 135:
# line 649 "parser.y"
{
		SYNERR("Expecting comma after second argument in .stabn statement");
		} break;
case 136:
# line 653 "parser.y"
{
		SYNERR("Expecting third argument in .stabn statement");
		} break;
case 137:
# line 657 "parser.y"
{
		SYNERR("Expecting comma after third argument in .stabn statement");
		} break;
case 138:
# line 661 "parser.y"
{
		SYNERR("Expecting fourth argument in .stabn statement. Must be a string");
		} break;
case 139:
# line 665 "parser.y"
{
		putstabr(yypvt[-4].np, yypvt[-2].np, yypvt[-0].p);
		} break;
case 140:
# line 669 "parser.y"
{
		SYNERR("Expecting first argument in .stabr statement");
		} break;
case 141:
# line 673 "parser.y"
{
		SYNERR("Expecting comma after first argument in .stabr statement");
		} break;
case 142:
# line 677 "parser.y"
{
		SYNERR("Expecting second argument in .stabr statement");
		} break;
case 143:
# line 681 "parser.y"
{
		SYNERR("Expecting comma after second argument in .stabr statement");
		} break;
case 144:
# line 685 "parser.y"
{
		SYNERR("Expecting third argument in .stabr statement. Must be a string");
		} break;
case 145:
# line 689 "parser.y"
{
		putstabp(yypvt[-8].np, yypvt[-6].np, yypvt[-4].np, yypvt[-2].np, yypvt[-0].p);
		} break;
case 146:
# line 693 "parser.y"
{
		SYNERR("Expecting first argument in .stabp statement");
		} break;
case 147:
# line 697 "parser.y"
{
		SYNERR("Expecting comma after first argument in .stabp statement");
		} break;
case 148:
# line 701 "parser.y"
{
		SYNERR("Expecting second argument in .stabp statement");
		} break;
case 149:
# line 705 "parser.y"
{
		SYNERR("Expecting comma after second argument in .stabp statement");
		} break;
case 150:
# line 709 "parser.y"
{
		SYNERR("Expecting third argument in .stabp statement");
		} break;
case 151:
# line 713 "parser.y"
{
		SYNERR("Expecting comma after third argument in .stabp statement");
		} break;
case 152:
# line 717 "parser.y"
{
		SYNERR("Expecting fourth argument in .stabp statement");
		} break;
case 153:
# line 721 "parser.y"
{
		SYNERR("Expecting comma after fourth argument in .stabp statement");
		} break;
case 154:
# line 725 "parser.y"
{
		SYNERR("Expecting fifth argument in .stabp statement. Must be a string");
		} break;
case 155:
# line 729 "parser.y"
{
		iputb(0x2c);
		iputw((int)yypvt[-0].l);
		} break;
case 156:
# line 734 "parser.y"
{
		SYNERR("Expecting line number in .stabl statement");
		} break;
case 157:
# line 739 "parser.y"
{
		deflabels();
		} break;
case 158:
# line 744 "parser.y"
{
		switch (yypvt[-2].np->n_type) {
		case 6:
			locadv(yypvt[-2].np->n_w);
			yypvt[-2].np->n_next = yypvt[-0].np;
			break;
		case 1: case 9:
			yypvt[-2].np->n_next = yypvt[-0].np;
			locadv(1);
			break;
		case 0x16:
			yypvt[-2].np->n_a->n_next = yypvt[-0].np;
			yyval.np = yypvt[-2].np->n_next;
			nfreed++;
			break;
		}
		} break;
case 159:
# line 762 "parser.y"
{
		SYNERR("Expecting address expression after comma in .byte statement");
		} break;
case 160:
# line 766 "parser.y"
{
		switch (yypvt[-0].np->n_type) {
		case 6:
			locadv(yypvt[-0].np->n_w);
			yypvt[-0].np->n_next = 0L;
			break;
		case 1: case 9:
			yypvt[-0].np->n_next = 0L;
			locadv(1);
			break;
		case 0x16:
			yypvt[-0].np->n_a->n_next = 0L;
			yyval.np = yypvt[-0].np->n_next;
			nfreed++;
			break;
		}
		} break;
case 161:
# line 785 "parser.y"
{
		if (yypvt[-1].np->n_type == 3)
			yypvt[-1].np->n_type = 1;
		else if (yypvt[-1].np->n_type == 7)
			yypvt[-1].np->n_type = 9;
		else if (yypvt[-1].np->n_type == 0x19)
			yypvt[-1].np->n_type = 23;
		yyval.np = mknode6((int16_t)yypvt[-3].np->n_val, yypvt[-1].np);
		nfreed++;
		} break;
case 162:
# line 796 "parser.y"
{
		SYNERR("Expecting address expression in repeat count in .byte statement");
		} break;
case 163:
# line 800 "parser.y"
{
		SYNERR("Expecting right parenthesis after repeat count in .byte statement");
		} break;
case 164:
# line 804 "parser.y"
{
		if (yypvt[-0].np->n_type == 3)
			yypvt[-0].np->n_type = 1;
		else if (yypvt[-0].np->n_type == 7)
			yypvt[-0].np->n_type = 9;
		else if (yypvt[-0].np->n_type == 0x19)
			yypvt[-0].np->n_type = 23;
		} break;
case 165:
# line 813 "parser.y"
{
		yyval.np = mkstring(yypvt[-0].p);
		} break;
case 166:
# line 818 "parser.y"
{
		even(); deflabels();
		} break;
case 167:
# line 823 "parser.y"
{
		if (yypvt[-2].np->n_type == 6)
			locadv(yypvt[-2].np->n_w * 2);
		else
			locadv(2);
		yypvt[-2].np->n_next = yypvt[-0].np;
		} break;
case 168:
# line 831 "parser.y"
{
		SYNERR("Expecting address expression after comma in .word statement");
		} break;
case 169:
# line 835 "parser.y"
{
		if (yypvt[-0].np->n_type == 6)
			locadv(yypvt[-0].np->n_w * 2);
		else
			locadv(2);
		yypvt[-0].np->n_next = 0L;
		} break;
case 170:
# line 844 "parser.y"
{
		if (yypvt[-1].np->n_type == 3)
			yypvt[-1].np->n_type = 2;
		else if (yypvt[-1].np->n_type == 7)
			yypvt[-1].np->n_type = 8;
		else if (yypvt[-1].np->n_type == 0x19)
			yypvt[-1].np->n_type = 24;
		yyval.np = mknode6((int16_t)yypvt[-3].np->n_val, yypvt[-1].np);
		nfreed++;
		} break;
case 171:
# line 855 "parser.y"
{
		SYNERR("Expecting address expression in repeat count in .word statement");
		} break;
case 172:
# line 859 "parser.y"
{
		SYNERR("Expecting right parenthesis after repeat count in .word statement");
		} break;
case 173:
# line 863 "parser.y"
{
		if (yypvt[-0].np->n_type == 3)
			yypvt[-0].np->n_type = 2;
		else if (yypvt[-0].np->n_type == 7)
			yypvt[-0].np->n_type = 8;
		else if (yypvt[-0].np->n_type == 0x19)
			yypvt[-0].np->n_type = 24;
		} break;
case 174:
# line 873 "parser.y"
{
		even(); deflabels();
		} break;
case 175:
# line 878 "parser.y"
{
		if (yypvt[-2].np->n_type == 6) {
			if (segmode != 0)
				locadv(yypvt[-2].np->n_w * 4);
			else
				locadv(yypvt[-2].np->n_w * 2);
		} else {
			if (segmode != 0)
				locadv(4);
			else
				locadv(2);
		}
		yypvt[-2].np->n_next = yypvt[-0].np;
		} break;
case 176:
# line 893 "parser.y"
{
		SYNERR("Expecting address expression after comma in .addr statement");
		} break;
case 177:
# line 897 "parser.y"
{
		if (yypvt[-0].np->n_type == 6) {
			if (segmode != 0)
				locadv(yypvt[-0].np->n_w * 4);
			else
				locadv(yypvt[-0].np->n_w * 2);
		} else {
			if (segmode != 0)
				locadv(4);
			else
				locadv(2);
		}
		yypvt[-0].np->n_next = 0L;
		} break;
case 178:
# line 913 "parser.y"
{
		if (segmode == 0) {
			if (yypvt[-1].np->n_type == 3)
				yypvt[-1].np->n_type = 2;
			else if (yypvt[-1].np->n_type == 7)
				yypvt[-1].np->n_type = 8;
			else if (yypvt[-1].np->n_type == 0x19)
				yypvt[-1].np->n_type = 24;
		}
		yyval.np = mknode6((int16_t)yypvt[-3].np->n_val, yypvt[-1].np);
		nfreed++;
		} break;
case 179:
# line 926 "parser.y"
{
		SYNERR("Expecting address expression in repeat count in .addr statement");
		} break;
case 180:
# line 930 "parser.y"
{
		SYNERR("Expecting right parenthesis after repeat count in .addr statement");
		} break;
case 181:
# line 934 "parser.y"
{
		if (segmode == 0) {
			if (yypvt[-0].np->n_type == 3)
				yypvt[-0].np->n_type = 2;
			else if (yypvt[-0].np->n_type == 7)
				yypvt[-0].np->n_type = 8;
			else if (yypvt[-0].np->n_type == 0x19)
				yypvt[-0].np->n_type = 24;
		}
		} break;
case 182:
# line 946 "parser.y"
{
		even(); deflabels();
		} break;
case 183:
# line 951 "parser.y"
{
		if (yypvt[-2].np->n_type == 6)
			locadv(yypvt[-2].np->n_w * 4);
		else
			locadv(4);
		yypvt[-2].np->n_next = yypvt[-0].np;
		} break;
case 184:
# line 959 "parser.y"
{
		SYNERR("Expecting address expression after comma in .long statement");
		} break;
case 185:
# line 963 "parser.y"
{
		if (yypvt[-0].np->n_type == 6)
			locadv(yypvt[-0].np->n_w * 4);
		else
			locadv(4);
		yypvt[-0].np->n_next = 0L;
		} break;
case 186:
# line 972 "parser.y"
{
		yyval.np = mknode6((int16_t)yypvt[-3].np->n_val, yypvt[-1].np);
		nfreed++;
		} break;
case 187:
# line 977 "parser.y"
{
		SYNERR("Expecting address expression in repeat count in .long statement");
		} break;
case 188:
# line 981 "parser.y"
{
		SYNERR("Expecting right parenthesis after repeat count in .long statement");
		} break;
case 190:
# line 987 "parser.y"
{
		deflabels();
		yypvt[-2].np->n_c1 = 2;
		defsym(yypvt[-2].np, yypvt[-0].np);
		} break;
case 191:
# line 993 "parser.y"
{
		SYNERR("Expecting address expression in global assignment statement");
		} break;
case 192:
# line 997 "parser.y"
{
		deflabels();
		yypvt[-2].np->n_c1 = 4;
		defsym(yypvt[-2].np, yypvt[-0].np);
		} break;
case 193:
# line 1003 "parser.y"
{
		SYNERR("Expecting address expression or reserved word in local assignment statement");
		} break;
case 194:
# line 1007 "parser.y"
{
		deflabels();
		yypvt[-2].np->n_c1 = 4;
		defalias(yypvt[-2].np, yypvt[-0].pp);
		} break;
case 195:
# line 1013 "parser.y"
{
		SYNERR("Expecting := , ::= , :: or : after identifier");
		} break;
case 196:
# line 1017 "parser.y"
{
		deflabels();
		setdot(yypvt[-0].np);
		} break;
case 197:
# line 1022 "parser.y"
{
		SYNERR("Expecting = after . for assignment of pc.");
		} break;
case 198:
# line 1026 "parser.y"
{
		SYNERR("Expecting numeric expression after .=");
		} break;
case 237:
# line 1070 "parser.y"
{
		yyval.np = opshort(yypvt[-1].np);
		shortaddr++;
		} break;
case 238:
# line 1075 "parser.y"
{
		SYNERR("Expecting numeric expression after short segmented address operator");
		} break;
case 239:
# line 1079 "parser.y"
{
		SYNERR("Expecting | at end of expression");
		} break;
case 240:
# line 1083 "parser.y"
{
		yyval.np = opseg(yypvt[-0].np);
		shortaddr++;
		} break;
case 241:
# line 1088 "parser.y"
{
		SYNERR("Expecting numeric expression after ^S operator");
		} break;
case 242:
# line 1092 "parser.y"
{
		yyval.np = opoff(yypvt[-0].np);
		shortaddr++;
		} break;
case 243:
# line 1097 "parser.y"
{
		SYNERR("Expecting numeric expression after ^O operator");
		} break;
case 245:
# line 1103 "parser.y"
{
		yyval.np = opadd(yypvt[-2].np, yypvt[-0].np);
		} break;
case 246:
# line 1107 "parser.y"
{
		SYNERR("Expecting expression to add");
		} break;
case 247:
# line 1111 "parser.y"
{
		yyval.np = opsub(yypvt[-2].np, yypvt[-0].np);
		} break;
case 248:
# line 1115 "parser.y"
{
		SYNERR("Expecting expression to subtract");
		} break;
case 250:
# line 1121 "parser.y"
{
		yyval.np = opshl(yypvt[-2].np, yypvt[-0].np);
		} break;
case 251:
# line 1125 "parser.y"
{
		SYNERR("Expecting numeric expression after ^< (shift left)");
		} break;
case 252:
# line 1129 "parser.y"
{
		yyval.np = opshr(yypvt[-2].np, yypvt[-0].np);
		} break;
case 253:
# line 1133 "parser.y"
{
		SYNERR("Expecting numeric expression after ^> (shift right)");
		} break;
case 254:
# line 1137 "parser.y"
{
		yyval.np = opand(yypvt[-2].np, yypvt[-0].np);
		} break;
case 255:
# line 1141 "parser.y"
{
		SYNERR("Expecting numeric expression after ^& (and)");
		} break;
case 256:
# line 1145 "parser.y"
{
		yyval.np = opor(yypvt[-2].np, yypvt[-0].np);
		} break;
case 257:
# line 1149 "parser.y"
{
		SYNERR("Expecting numeric expression after ^| (or)");
		} break;
case 258:
# line 1153 "parser.y"
{
		yyval.np = opxor(yypvt[-2].np, yypvt[-0].np);
		} break;
case 259:
# line 1157 "parser.y"
{
		SYNERR("Expecting numeric expression after ^X (exclusive-or)");
		} break;
case 261:
# line 1163 "parser.y"
{
		yyval.np = opmul(yypvt[-2].np, yypvt[-0].np);
		} break;
case 262:
# line 1167 "parser.y"
{
		SYNERR("Expecting multiplier expression");
		} break;
case 263:
# line 1171 "parser.y"
{
		yyval.np = opdiv(yypvt[-2].np, yypvt[-0].np);
		} break;
case 264:
# line 1175 "parser.y"
{
		SYNERR("Expecting divisor expression");
		} break;
case 266:
# line 1181 "parser.y"
{
		yyval.np = opneg(yypvt[-0].np);
		} break;
case 267:
# line 1185 "parser.y"
{
		SYNERR("Expecting numeric expression after unary minus");
		} break;
case 268:
# line 1189 "parser.y"
{
		yyval.np = opplus(yypvt[-0].np);
		} break;
case 269:
# line 1193 "parser.y"
{
		SYNERR("Expecting numeric expression after unary plus");
		} break;
case 271:
# line 1199 "parser.y"
{
		yyval.np = mknode1(3, yypvt[-0].l);
		} break;
case 272:
# line 1203 "parser.y"
{
		yyval.np = mknode1(3, ((uint32_t)yypvt[-2].l << 24) | yypvt[-0].l);
		} break;
case 273:
# line 1207 "parser.y"
{
		SYNERR("Expecting segment number after <<");
		} break;
case 274:
# line 1211 "parser.y"
{
		SYNERR("Expecting >> after segment number");
		} break;
case 275:
# line 1215 "parser.y"
{
		SYNERR("Expecting segment offset number");
		} break;
case 276:
# line 1219 "parser.y"
{
		yyval.np = dotval();
		} break;
case 277:
# line 1223 "parser.y"
{
		yyval.np = yypvt[-1].np;
		} break;
case 278:
# line 1227 "parser.y"
{
		SYNERR("Expecting numeric expression after left parenthesis");
		} break;
case 279:
# line 1231 "parser.y"
{
		SYNERR("Expecting right parenthesis in expression");
		} break;
case 281:
# line 1237 "parser.y"
{
		if (nameflag != 0) {
			sputb(0x15);
			sputw(yypvt[-0].np->n_w);
			nameflag = 0;
		}
		} break;
case 282:
# line 1246 "parser.y"
{
		yypvt[-1].np->n_c1 = 3;
		yypvt[-1].np->n_next = plist;
		plist = yypvt[-1].np;
		} break;
case 283:
# line 1252 "parser.y"
{
		yypvt[-1].np->n_c1 = 5;
		yypvt[-1].np->n_next = plist;
		plist = yypvt[-1].np;
		} break;
case 284:
# line 1259 "parser.y"
{
		nameflag = 0;
		symmode = 6;
		yyval.np = mknode7(stenter(yypvt[-0].p, (unsigned char)symmode));
		nameflag++;
		} break;
case 285:
# line 1266 "parser.y"
{
		nameflag = 0;
		symmode = 0;
		yyval.np = mknode7(stenter(yypvt[-0].p, (unsigned char)symmode));
		nameflag++;
		} break;
case 286:
# line 1273 "parser.y"
{
		nameflag = 0;
		yyval.np = mknode7(yypvt[-0].i);
		} break;
		}
		goto yystack;  /* stack new state and value */

	}
