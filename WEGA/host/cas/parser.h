
typedef union  {
	int32_t	l;
	int	i;
	char	*p;
	struct pst *pp;
	NODE	*np;
} YYSTYPE;
extern YYSTYPE yylval;
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
