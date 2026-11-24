/*
 * exbss.c - the common data of ex in the order of /bin/ex (the name is a guess; as bss3.c of codgen). Linked first behind
 * crt0, it gives the order of the commons (the linker allocates them in the order of their first mention); no code, no data.
 * Each is declared by its size only (the headers of ex define their variables, an #include would give the order of the
 * headers); its declaration in the sources is the comment. Not referenced by any code, their places guessed by their
 * neighbours: die, firstln, poffset, vmacbuf, tad1, slines, optname, vcntcol, xPC, precbksl, llimit, tabend, tabslack.
 * How the original came to this order is not known.
 */
int	ichanged;	/* bool ichanged */
int	tchng;	/* short tchng */
int	endcore;	/* line *endcore */
int	iblock;	/* short iblock */
char	linebuf[512];	/* char linebuf[LBSIZE] */
int	oblock;	/* short oblock */
int	morargc;	/* short morargc */
int	undkind;	/* short undkind */
char	tspace[256];	/* char tspace[256] */
int	dirtcnt;	/* short dirtcnt */
int	aoftspace;	/* char *aoftspace */
int	outline;	/* short outline */
int	argv;	/* char **argv */
char	obuf[512];	/* char obuf[BUFSIZ] */
int	NONL;	/* bool NONL */
int	hush;	/* bool hush */
int	oprompt;	/* short oprompt */
int	DO;	/* char *DO */
int	EO;	/* bool EO */
int	HO;	/* char *HO */
int	io;	/* short io */
int	ruptible;	/* bool ruptible */
int	SO;	/* char *SO */
char	tty[6];
int	hitin2;	/* bool hitin2 */
int	addr1;	/* line *addr1 */
int	addr2;	/* line *addr2 */
int	vcatch;	/* bool vcatch */
int	globp;	/* char *globp */
int	nleft;	/* short nleft */
int	intty;	/* bool intty */
int	tline;	/* short tline */
int	UP;	/* char *UP */
int	iblock2;	/* short iblock2 */
int	XX;	/* bool XX */
int	IP;	/* char *IP */
char	names[54];	/* line names['z'-'a'+2] */
int	fendcore;	/* line *fendcore */
int	inglobal;	/* bool inglobal */
int	shudclob;	/* bool shudclob */
int	OCOLUMNS;	/* short OCOLUMNS */
int	firstpat;	/* char *firstpat */
int	die;	/* bool die */
int	destcol;	/* short destcol */
int	ppid;	/* int ppid */
int	rpid;	/* int rpid */
int	normtty;	/* bool normtty */
int	DA;	/* bool DA */
int	UPPERCASE;	/* bool UPPERCASE */
int	pid;	/* int pid */
int	MA;	/* char *MA */
int	MI;	/* bool MI */
int	TA;	/* char *TA */
int	firstln;	/* short firstln */
int	CA;	/* bool CA */
int	EI;	/* char *EI */
char	H[386];
int	TI;	/* char *TI */
int	undap1;	/* line *undap1 */
int	unddel;	/* line *unddel */
int	unddol;	/* line *unddol */
int	oldhup;
int	input;	/* char *input */
int	DB;	/* bool DB */
char	pkill[4];	/* char *pkill[2] */
int	VB;	/* char *VB */
int	XB;	/* bool XB */
int	HZ;	/* bool HZ */
int	SR;	/* char *SR */
int	aiflag;	/* bool aiflag */
int	undap2;	/* line *undap2 */
int	anymarks;	/* bool anymarks */
int	numberf;	/* bool numberf */
int	BC;	/* char *BC */
int	HC;	/* bool HC */
int	IC;	/* char *IC */
int	BS;	/* bool BS */
int	one;	/* line *one */
int	NC;	/* bool NC */
char	PC[1];	/* char PC */
int	NS;	/* bool NS */
int	OS;	/* bool OS */
int	xNL;	/* char *xNL */
int	DC;	/* char *DC */
int	VS;	/* char *VS */
char	frob[720];
int	loc1;	/* char *loc1 */
int	argv0;	/* char **argv0 */
int	inopen;	/* bool inopen */
int	initev;	/* char *initev */
char	ibuff[512];	/* char ibuff[BUFSIZ] */
int	chng;	/* short chng */
char	file[128];	/* char file[FNSIZE] */
int	args;	/* char *args */
int	peekc;	/* short peekc */
int	normf;	/* ttymode normf */
int	argc;	/* short argc */
int	ED;	/* char *ED */
int	AL;	/* char *AL */
int	CL;	/* char *CL */
int	DL;	/* char *DL */
int	BT;	/* char *BT */
int	ND;	/* char *ND */
int	GT;	/* bool GT */
int	LL;	/* char *LL */
char	obuff[512];	/* char obuff[BUFSIZ] */
int	UL;	/* bool UL */
int	CD;	/* char *CD */
int	XT;	/* bool XT */
char	ibuff2[512];	/* char ibuff2[BUFSIZ] */
int	holdcm;	/* bool holdcm */
int	undadot;	/* line *undadot */
int	erfile;	/* short erfile */
int	loc2;	/* char *loc2 */
char	lasttag[32];	/* char lasttag[TAGSIZE] */
int	destline;	/* short destline */
char	vreslab[20];	/* ret_buf vreslab */
int	notecnt;	/* short notecnt */
int	Command;	/* char *Command */
int	oldquit;
int	WBOT;	/* short WBOT */
char	resetlab[20];	/* ret_buf resetlab */
int	AM;	/* bool AM */
int	dol;	/* line *dol */
int	DM;	/* char *DM */
int	dot;	/* line *dot */
int	status;	/* int status */
int	IM;	/* char *IM */
int	truedol;	/* line *truedol */
int	SE;	/* char *SE */
int	VE;	/* char *VE */
int	CE;	/* char *CE */
char	altfile[128];	/* char altfile[FNSIZE] */
int	TE;	/* char *TE */
int	CM;	/* char *CM */
int	costCM;	/* short costCM */
int	argc0;	/* short argc0 */
int	xCR;	/* char *xCR */
int	edited;	/* bool edited */
int	linebp;	/* char *linebp */
char	genbuf[512];	/* char genbuf[LBSIZE] */
int	args0;	/* char *args0 */
int	ninbuf;	/* short ninbuf */
int	WECHO;	/* short WECHO */
int	outcol;	/* short outcol */
int	xchng;	/* short xchng */
int	pfast;	/* bool pfast */
int	laste;	/* bool laste */
int	zero;	/* line *zero */
char	uxb[130];	/* char uxb[UXBSIZE + 2] */
char	nttyc[6];
char	ottyc[6];
int	listf;	/* bool listf */
int	SF;	/* char *SF */
int	ichang2;	/* bool ichang2 */
int	XN;	/* bool XN */
int	IN;	/* bool IN */
char	braelist[18];	/* char *braelist[NBRA] */
char	scanre[134];	/* struct regexp scanre */
char	subre[134];	/* struct regexp subre */
char	rhsbuf[256];	/* char rhsbuf[RHSSIZE] */
char	braslist[18];	/* char *braslist[NBRA] */
char	re[134];	/* struct regexp re */
int	nflag;	/* bool pflag, nflag */
int	pflag;	/* bool pflag */
int	poffset;	/* int poffset */
int	insmode;	/* bool insmode */
char	workcmd[5];	/* char workcmd[5] */
int	basWTOP;	/* short basWTOP */
int	hold;	/* short hold */
int	vcnt;	/* short vcnt */
int	lastcnt;	/* short lastcnt */
int	notesgn;	/* char *notesgn */
int	Xcnt;	/* short Xcnt */
int	vcline;	/* short vcline */
int	holdupd;	/* short holdupd */
int	wdkind;	/* short wdkind */
int	HADUP;	/* bool HADUP */
int	CDCNT;	/* short CDCNT */
int	vutmp;	/* char *vutmp */
char	vtube[88];	/* char *vtube[TUBELINES] */
char	op[1];	/* char op */
char	vreg[1];	/* char vreg */
int	heldech;	/* bool heldech */
int	lastvgk;	/* short lastvgk */
int	WLINES;	/* short WLINES */
int	vmcurs;	/* char *vmcurs */
int	wcursor;	/* char *wcursor */
int	Vlines;	/* short Vlines */
char	DEL[128];	/* char DEL[VBSIZE] */
int	vUNDdot;	/* line *vUNDdot */
int	wdot;	/* line *wdot */
int	dir;	/* short dir */
int	WCOLS;	/* short WCOLS */
char	ncols[54];	/* char *ncols['z'-'a'+2] */
int	Peekkey;	/* short Peekkey */
char	vmacbuf[128];	/* char vmacbuf[VBSIZE] */
int	bastate;	/* short bastate */
char	tcommand[20];	/* char tcommand[20] */
int	HADZERO;	/* bool HADZERO */
int	basWLINES;	/* short basWLINES */
int	notenam;	/* char *notenam */
int	splitw;	/* bool splitw */
int	vSCROLL;	/* short vSCROLL */
char	INS[128];	/* char INS[VBSIZE] */
int	vmoving;	/* bool vmoving */
int	lastcp;	/* char *lastcp */
int	cursor;	/* char *cursor */
int	state;	/* short state */
int	vmacp;	/* char *vmacp */
int	gobblebl;	/* bool gobblebl */
int	lasthad;	/* bool lasthad */
int	Xhadcnt;	/* bool Xhadcnt */
int	rubble;	/* bool rubble */
int	vundkind;	/* short vundkind */
int	vUNDsav;	/* line vUNDsav */
int	vmovcol;	/* short vmovcol */
char	vlinfo[184];
char	lastcmd[5];	/* char lastcmd[5] */
int	vUNDcurs;	/* char *vUNDcurs */
int	lastreg;	/* short lastreg */
int	WTOP;	/* short WTOP */
int	hadcnt;	/* bool hadcnt */
int	vtube0;	/* char *vtube0 */
int	doomed;	/* short doomed */
int	vglobp;	/* char *vglobp */
int	ZERO;	/* short ZERO */
int	tad1;	/* line *tad1 */
int	zweight;	/* short zweight */
int	zhadpr;	/* bool zhadpr */
int	znoclear;	/* bool znoclear */
char	cntodd[4];	/* long cntodd */
int	cntln;	/* short cntln */
int	oldadot;	/* short oldadot */
char	cntnull[4];	/* long cntnull */
int	wasalt;	/* bool wasalt */
int	altdot;	/* short altdot */
char	cntch[4];	/* long cntch */
int	destuc;	/* bool destuc */
int	casecnt;	/* int casecnt */
int	slines;	/* int scount, slines */
int	cflag;	/* move1(cflag */
int	stotal;	/* int scount, slines, stotal */
int	locs;	/* char *locs */
int	scount;	/* int scount */
char	optname[32];	/* char optname[ONMSZ] */
int	vcntcol;	/* short vcntcol */
int	rblock;	/* short rblock */
int	havetmp;	/* int havetmp */
int	rbuf;
int	rnext;	/* short rnext */
char	rused[64];	/* short rused[32] */
int	rbufcp;	/* char *rbufcp */
char	strregs[288];
int	rnleft;	/* short rnleft */
char	rfname[40];	/* char rfname[40] */
char	tfname[40];	/* char tfname[40] */
int	strp;
int	xPC;	/* char *xPC */
int	precbksl;	/* short precbksl */
int	vUD1;	/* char *vUD1 */
int	vUA1;	/* char *vUA1 */
int	vUD2;	/* char *vUD1, *vUD2 */
int	vUA2;	/* char *vUA1, *vUA2 */
int	ogcursor;	/* char *ogcursor */
int	vsplitpt;	/* char *vsplitpt */
int	vaifirst;	/* bool vaifirst */
int	gobbled;	/* bool gobbled */
int	llimit;	/* line *llimit */
int	wasend;	/* bool wasend */
int	lf;
int	inscol;	/* int inscol */
int	shft;	/* int shft */
int	tabend;	/* int tabend */
int	linend;	/* int linend */
int	tabslack;	/* int tabslack */
int	tabstart;	/* int tabstart */
int	tabsize;	/* int tabsize */
int	slakused;	/* int slakused */
int	inssiz;	/* int inssiz */
