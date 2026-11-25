static char sccsid[] = "@[$]file.c\t1.2  07/24/82 00:15:39 - 87wega3.2";

/*
 * determine type of file
 *
 * WEGA: file.c of System III rewritten for the Z8000 (written from /bin/file): the s.out magic numbers (Z8 and Z80,
 * segmented and nonsegmented, also byte swapped), a buffer of 1024 bytes read again by refill, the C preprocessor and
 * csh tests, PLZ comments, Z8, plz/asm and plz/sys program text; no fifo, cpio, PDP-11, utime. The names of the new
 * tables and functions are guesses.
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <s.out.h>
#include <ar.h>

#define	BUFSZ	1024

int i  = 0;
char *fort[] = {
	"function","subroutine","common","dimension","block","integer",
	"real","data","double",0};
char *asc[] = {
	"sys","mov","tst","jmp",0};
char *c[] = {
	"int","char","float","double","struct","extern","typedef","static",0};
char *cpp[] = {
	"define","include","ifndef",0};
char *csh[] = {
	"set","foreach",0};
char *as[] = {
	"globl","even","text","data","bss","comm",0};
char *z8[] = {
	"ccf","da","decw","incw","ldci","lde","rcf","scf","srp","tcm","tm",
	"CCF","DA","DECW","INCW","LDCI","LDE","RCF","SCF","SRP","SWAP","TCM","TM",0};
char *plzasm[] = {
	"jr","inc","dec","ret","call","LD","CP","JR","INC","DEC","RET","CALL",0};
char *seg[] = {
	"$segmented","rr0","rr2","rr4","offset","seg",
	"$SEGMENTED","RR0","RR2","RR4","OFFSET","SEG",0};
char *plzsys[] = {
	"module","CONSTANT","TYPE","MODULE","EXTERNAL","GLOBAL","LOCAL",0};
char *z8name = "Z8";
char *z80name = "Z80";
char *nstrip = ",not stripped";
int	ifile;
char buf[BUFSZ];
int in;

main(argc, argv)
char **argv;
{
	register	char	*p;
	FILE		*fl;
	char		ap[128];

	if (argc>1 && argv[1][0]=='-' && argv[1][1]=='f') {
		if ((fl = fopen(argv[2], "r")) == NULL) {
			printf("Can't open %s\n", argv[2]);
			exit(2);
		}
		while ((p = fgets(ap, 128, fl)) != NULL) {
			int l = strlen(p);
			if (l>0)
				p[l-1] = '\0';
			printf("%s:	", p);
			type(p);
			if (ifile>=0)
				close(ifile);
		}
		exit(1);
	}
	while(argc > 1) {
		printf("%s:	", argv[1]);
		type(argv[1]);
		argc--;
		argv++;
		if (ifile >= 0)
			close(ifile);
	}
}

type(file)
char *file;
{
	int	j,nl;
	char	ch;
	struct	stat	mbuf;

	ifile = -1;
	if(stat(file, &mbuf) < 0) {
		printf("cannot stat\n");
		return;
	}
	switch (mbuf.st_mode & S_IFMT) {

	case S_IFCHR:
		printf("character");
		goto spcl;

	case S_IFDIR:
		printf("directory\n");
		return;

	case S_IFBLK:
		printf("block");

spcl:
		printf(" special (%d/%d)\n", major(mbuf.st_rdev), minor(mbuf.st_rdev));
		return;
	}

	ifile = open(file, 0);
	if(ifile < 0) {
		printf("cannot open\n");
		return;
	}
	in = read(ifile, buf, BUFSZ);
	if(in == 0){
		printf("empty\n");
		return;
	}
	switch(*(int *)buf) {

	case X_MAGIC1:
	case 0x07e5:
		printf("%s ", ((struct s_exec *)buf)->s_flag & SF_Z8 ? z8name : z80name);
		printf("executable");
		printf("%s\n", ((struct s_exec *)buf)->s_syms ? nstrip : "");
		return;

	case X_MAGIC3:
	case 0x11e5:
		printf("%s ", ((struct s_exec *)buf)->s_flag & SF_Z8 ? z8name : z80name);
		printf("separate executable");
		printf("%s\n", ((struct s_exec *)buf)->s_syms ? nstrip : "");
		return;

	case X_MAGIC4:
	case 0x05e5:
		printf("%s ", ((struct s_exec *)buf)->s_flag & SF_Z8 ? z8name : z80name);
		printf("overlay executable");
		printf("%s\n", ((struct s_exec *)buf)->s_syms ? nstrip : "");
		return;

	case S_MAGIC4:
	case N_MAGIC4:
	case 0x05e6:
	case 0x05e7:
		printf("overlay ");
		goto exec;

	case S_MAGIC3:
	case N_MAGIC3:
	case 0x11e6:
	case 0x11e7:
		printf("separate ");

	case S_MAGIC1:
	case N_MAGIC1:
	case 0x07e6:
	case 0x07e7:
exec:
		printf("executable");
		if (((struct s_exec *)buf)->s_syms)
			printf("%s ", nstrip);
		if ((*(int *)buf & 0x100) != 0)
			printf(" non-");
		printf("segmented\n");
		return;

	case 0177555:
		printf("old archive\n");
		return;

	case ARMAG:
		printf("archive\n");
		return;
	}

	i = 0;
	if(ccom() == 0)goto notc;
	while(buf[i] == '#'){
		j = i;
		while(buf[i+1] == ' ')
			i++;
		if(buf[i+1] == '\n')
			i += 2;
		else
			while(buf[i] != '\n'){
				if(i - j > 255){
					printf("data\n");
					return;
				}
				if(lookup(cpp)){
					printf("c program text");
					goto outa;
				}
				if(i >= in){
					if(refill() == 0)goto notc;
					i = -1;
					j -= BUFSZ;
				}
				i++;
			}
		if(ccom() == 0)goto notc;
	}
	if(lookup(csh)){
		printf("csh command text");
		goto outa;
	}
check:
	if(lookup(c) == 1){
		while((ch = buf[i++]) != ';' && ch != '{')if(i >= in)goto notc;
		printf("c program text");
		goto outa;
	}
	nl = 0;
	while(buf[i] != '('){
		if(buf[i] <= 0)
			goto notas;
		if(buf[i] == ';'){
			i++;
			goto check;
		}
		if(buf[i++] == '\n')
			if(nl++ > 6)goto notc;
		if(i >= in)goto notc;
	}
	while(buf[i] != ')'){
		if(buf[i++] == '\n')
			if(nl++ > 6)goto notc;
		if(i >= in)goto notc;
	}
	while(buf[i] != '{'){
		if(buf[i++] == '\n')
			if(nl++ > 6)goto notc;
		if(i >= in)goto notc;
	}
	printf("c program text");
	goto outa;
notc:
	i = 0;
	while(buf[i] == 'c' || buf[i] == '#'){
		while(buf[i++] != '\n')if(i >= in)goto notfort;
	}
	if(lookup(fort) == 1){
		printf("fortran program text");
		goto outa;
	}
notfort:
	i=0;
	if(ascom() == 0)goto notasm;
	j = i-1;
	if(buf[i] == '.'){
		i++;
		if(lookup(as) == 1){
			printf("assembler program text");
			goto outa;
		}
		else if(buf[j] == '\n' && isalpha(buf[j+2])){
			printf("roff, nroff, or eqn input text");
			goto outa;
		}
	}
	while(lookup(asc) == 0){
		if(ascom() == 0)goto notasm;
		while(buf[i] != '\n' && buf[i++] != ':')
			if(i >= in)goto notasm;
		while(buf[i] == '\n' || buf[i] == ' ' || buf[i] == '\t')if(i++ >= in)goto notasm;
		j = i-1;
		if(buf[i] == '.'){
			i++;
			if(lookup(as) == 1){
				printf("assembler program text");
				goto outa;
			}
			else if(buf[j] == '\n' && isalpha(buf[j+2])){
				printf("roff, nroff, or eqn input text");
				goto outa;
			}
		}
	}
	printf("assembler program text");
	goto outa;
notasm:
	i = 0;
	if(plzcom() == 0)goto notas;
	while(lookup(z8) == 0)
		if(skipw() == 0 || plzcom() == 0)goto notz8;
	printf("Z8 program text");
	goto outa;
notz8:
	i = 0;
	while(lookup(plzasm) == 0)
		if(skipw() == 0 || plzcom() == 0)goto notpa;
	printf("plz/asm program text");
	i = 0;
	while(lookup(seg) == 0)
		if(skipw() == 0 || plzcom() == 0)goto outa;
	printf(" segmented");
	goto outa;
notpa:
	i = 0;
	while(lookup(plzsys) == 0)
		if(skipw() == 0 || plzcom() == 0)goto notas;
	printf("plz/sys program text");
	goto outa;
notas:
	for(i=0; i < in; i++)if(buf[i]&0200){
		if (buf[0]=='\100' && buf[1]=='\357') {
			printf("troff output\n");
			return;
		}
		printf("data\n");
		return;
	}
	if (mbuf.st_mode&((S_IEXEC)|(S_IEXEC>>3)|(S_IEXEC>>6)))
		printf("commands text");
	else
	    if (english(buf, in))
		printf("English text");
	else
	    printf("ascii text");
outa:
	while(i < in)
		if((buf[i++]&0377) > 127){
			printf(" with garbage\n");
			return;
		}
	printf("\n");
}
lookup(tab)
char *tab[];
{
	char r;
	int k,j,l;
	while(buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n')i++;
	for(j=0; tab[j] != 0; j++){
		l=0;
		for(k=i; ((r=tab[j][l++]) == buf[k] && r != '\0');k++);
		if(r == '\0')
			if(buf[k] == ' ' || buf[k] == '\n' || buf[k] == '\t'
			    || buf[k] == '{' || buf[k] == '/'){
				i=k;
				return(1);
			}
	}
	return(0);
}
/* to the next white space */
skipw(){
	while(buf[i] != ' ' && buf[i] != '\t' && buf[i] != '\n')
		if(i++ > in)return(0);
	return(1);
}
ccom(){
	char cc;
	while((cc = buf[i]) == ' ' || cc == '\t' || cc == '\n')if(i++ >= in)return(0);
	if(buf[i] == '/' && buf[i+1] == '*'){
		i += 2;
		while(buf[i] != '*' || buf[i+1] != '/'){
			if(buf[i] == '\\')i += 2;
			else i++;
			if(i >= in)if(refill() == 0)return(0);
		}
		if((i += 2) >= in)if(refill() == 0)return(0);
	}
	if(buf[i] == '\n')if(ccom() == 0)return(0);
	return(1);
}
/* a PLZ comment: ! ... ! (sic: '!!' is a char constant that no char equals) */
plzcom(){
	char cc;
	while((cc = buf[i]) == ' ' || cc == '\t' || cc == '\n')if(i++ >= in)return(0);
	if(buf[i] == '!'){
		i++;
		while(buf[i] != '!' && buf[i] != '!!'){
			if(buf[i] == '!!')i += 2;
			else i++;
			if(i >= in)return(0);
		}
		if(++i > in)return(0);
	}
	if(buf[i] == '\n')if(plzcom() == 0)return(0);
	return(1);
}
ascom(){
	while(buf[i] == '/'){
		i++;
		while(buf[i++] != '\n')if(i >= in)return(0);
		while(buf[i] == '\n')if(i++ >= in)return(0);
	}
	return(1);
}

english (bp, n)
char *bp;
{
# define NASC 128
	int ct[NASC], j, vow, freq, rare;
	int badpun = 0, punct = 0;
	if (n<50) return(0); /* no point in statistics on squibs */
	for(j=0; j<NASC; j++)
		ct[j]=0;
	for(j=0; j<n; j++)
	{
		if (bp[j]<NASC)
			ct[bp[j]|040]++;
		switch (bp[j])
		{
		case '.':
		case ',':
		case ')':
		case '%':
		case ';':
		case ':':
		case '?':
			punct++;
			if ( j < n-1 &&
			    bp[j+1] != ' ' &&
			    bp[j+1] != '\n')
				badpun++;
		}
	}
	if (badpun*5 > punct)
		return(0);
	vow = ct['a'] + ct['e'] + ct['i'] + ct['o'] + ct['u'];
	freq = ct['e'] + ct['t'] + ct['a'] + ct['i'] + ct['o'] + ct['n'];
	rare = ct['v'] + ct['j'] + ct['k'] + ct['q'] + ct['x'] + ct['z'];
	if (2*ct[';'] > ct['e']) return(0);
	if ( (ct['>']+ct['<']+ct['/'])>ct['e']) return(0); /* shell file test */
	return (vow*5 >= n-ct[' '] && freq >= 10*rare);
}

/* the next block of the file; at its end the first one again */
refill(){
	in = read(ifile, buf, BUFSZ);
	if(in == 0){
		lseek(ifile, 0L, 0);
		in = read(ifile, buf, BUFSZ);
		return(0);
	}
	i = 0;
	return(1);
}
