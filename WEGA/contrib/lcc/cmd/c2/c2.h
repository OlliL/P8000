/*
 * Header for object code improver
 */

#include <stdio.h>

#define	CBR	2
#define JMP	4
#define JSW	5
#define	LABEL	6
#define	DLABEL	7
#define	EROU	9
#define	LD	11
#define	CLR	12
#define	COM	13
#define	INC	14
#define	DEC	15
#define	NEG	16
#define	TST	17
#define	ASR	18
#define	ASL	19
#define	SXT	20
#define	CMP	21
#define	ADD	22
#define	SUB	23
#define	OR	24
#define	AND	25
#define	SDL	34
#define	MUL	27
#define	DIV	28
#define	SDA	29
#define	XOR	30
#define	CP	31
#define	CODE	32
#define	CD	33
#define BSS	35
#define EVEN	36
#define	DATA	37
#define CMT	38
#define SET	39
#define RES	40
#define BIT	41
#define	EX	42
#define POP	43
#define PUSH	44
#define	CALL	45
#define	END	46
#define SRL	47
#define SLL	48
#define DJNZ	49
#define LDI	50
#define LDD	51
#define	LDK	52
#define	XL1	54
#define	XL2	55

#define	JEQ	0
#define	JNE	1
#define	PL	2
#define MI	3
#define	JLE	4
#define	JGE	5
#define	JLT	6
#define	JGT	7
#define	JLO	8
#define	JHI	9
#define	JLOS	10
#define	JHIS	11

#define	BYTE	0x40
#define LONG	0x20
#define	LSIZE	512

struct node {
	char	op;
	char	subop;
	struct	node	*forw;
	struct	node	*back;
	struct	node	*ref;
	int	labno;
	char	*code;
	int	refc;
};

struct optab {
	char	*opstring;
	int	opcode;
} optab[];

char	line[LSIZE];
struct	node	first;
char	*curlp;
int	nbrbr;
char	regc;
int	nsaddr;
int	redunm;
int	iaftbr;
int	njp1;
int	nrlab;
int	nxjump;
int	ncmot;
int	nrevbr;
int	loopiv;
int	nredunj;
int	nskip;
int	ncomj;
int	nsob;
int	nrtst;
int	nlit;
int	nchange;
int	isn;
int	debug;
int	lastseg;
char	*lasta;
char	*lastr;
char	*firstr;
char	revbr[];
char	regs[34][24];
char	regt[34];
struct node *	regp[34];
char	regh[8][24];
char	conloc[24];
char	conloct;
char	conval[24];
char	convalt;
char	ccloc[24];
char	*cpcode;
char	ccloct;
int	tildeL[10];

#define	RT1	32
#define	RT2	33
#define	FREG	32
#define	NREG	16
#define	LABHS	127
#define	OPHS	79
#define BREG	8

struct optab *ophash[OPHS];
struct	node *nonlab();
char	*copy();
char *malloc();
char *strchr();
char	*sbrk();
char	*findcon();
struct	node *insertl();
struct	node *codemove();
char	*ssgbrk();
char	*alloc();
int xl1;
int xl2;
char *address[10];	/* adressen fuer alloc */
int addrseg;
