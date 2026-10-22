
	fp		:= r15;
	sp		:= r15;
.comm	512,	_dblock;
.comm	10240,	_tbuf;
.comm	2,	_ihead;
.comm	30,	_stbuf;
.comm	2,	_rflag;
.comm	2,	_xflag;
.comm	2,	_vflag;
.comm	2,	_tflag;
.comm	2,	_dflag;
.comm	2,	_cflag;
.comm	2,	_mflag;
.comm	2,	_fd;
.comm	2,	_term;
.comm	2,	_chksum;
.comm	2,	_wflag;
.comm	2,	_recno;
.comm	2,	_first;
.comm	2,	_linkerrok;
.comm	2,	_archuid;
.comm	2,	_rhead;
.comm	2,	_timeout;
    .even
_freemem::
	.word	%1
    .even
_nblock::
	.word	%1
.comm	2,	_BLOCKS;
.comm	2,	_Blocks;
.comm	2,	_disks;
.comm	1024,	_xbuf;
.comm	4,	_low;
.comm	4,	_high;
.comm	2,	_tfile;
_tname::
	.byte	%2f
	.byte	%74
	.byte	%6d
	.byte	%70
	.byte	%2f
	.byte	%74
	.byte	%61
	.byte	%72
	.byte	%58
	.byte	%58
	.byte	%58
	.byte	%58
	.byte	%58
	.byte	%58
	.byte	%0
	.byte	0
.comm	2,	_usefile;
_tardev::
	.byte	%2f
	.byte	%64
	.byte	%65
	.byte	%76
	.byte	%2f
	.byte	%74
	.byte	%61
	.byte	%72
	.byte	%64
	.byte	%65
	.byte	%76
	.byte	%0
_devrfd1::
	.byte	%2f
	.byte	%64
	.byte	%65
	.byte	%76
	.byte	%2f
	.byte	%72
	.byte	%66
	.byte	%64
	.byte	%31
	.byte	%0
L75:
	.byte	%77,%0
    .even
L78:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%63,%72,%65,%61
	.byte	%74,%65,%20,%74,%65,%6d,%70,%6f
	.byte	%72,%61,%72,%79,%20,%66,%69,%6c
	.byte	%65,%20,%28,%25,%73,%29,%a,%0
    .even
L80:
	.byte	%21,%21,%21,%21,%21,%2f,%21,%2f
	.byte	%21,%2f,%21,%2f,%21,%2f,%21,%2f
	.byte	%21,%2f,%21,%20,%30,%30,%30,%a
	.byte	%0
    .even
L84:
	.byte	%74,%61,%72,%3a,%20,%42,%6c,%6f
	.byte	%63,%6b,%65,%64,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%63,%61,%6e,%6e
	.byte	%6f,%74,%20,%62,%65,%20,%75,%70
	.byte	%64,%61,%74,%65,%64,%20,%28,%79
	.byte	%65,%74,%29,%a,%0
    .even
L98:
	.byte	%49,%6e,%76,%61,%6c,%69,%64,%20
	.byte	%62,%6c,%6f,%63,%6b,%73,%69,%7a
	.byte	%65,%20,%28,%4d,%61,%78,%20,%25
	.byte	%64,%29,%a,%0
    .even
L102:
	.byte	%74,%61,%72,%3a,%20,%25,%63,%3a
	.byte	%20,%75,%6e,%6b,%6e,%6f,%77,%6e
	.byte	%20,%6f,%70,%74,%69,%6f,%6e,%a
	.byte	%0
    .even
    .even
L103:
	.word	45
	.word	48
	.word	49
	.word	88
	.word	98
	.word	99
	.word	100
	.word	102
	.word	108
	.word	109
	.word	114
	.word	116
	.word	117
	.word	118
	.word	119
	.word	120
    .even
L106:
	.byte	%74,%61,%72,%3a,%20,%56,%65,%72
	.byte	%73,%69,%6f,%6e,%20,%31,%2e,%38
	.byte	%2e,%38,%38,%a,%0
    .even
L113:
	.byte	%2d,%0
    .even
L116:
	.byte	%43,%61,%6e,%20,%6f,%6e,%6c,%79
	.byte	%20,%63,%72,%65,%61,%74,%65,%20
	.byte	%73,%74,%61,%6e,%64,%61,%72,%64
	.byte	%20,%6f,%75,%74,%70,%75,%74,%20
	.byte	%61,%72,%63,%68,%69,%76,%65,%73
	.byte	%a,%0
    .even
L124:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%6f,%70,%65,%6e
	.byte	%20,%25,%73,%a,%0
    .even
L126:
	.byte	%74,%61,%72,%3a,%20,%74,%72,%69
	.byte	%65,%73,%20,%6f,%70,%65,%6e,%20
	.byte	%25,%73,%a,%0
    .even
L134:
	.byte	%54,%61,%6b,%65,%20,%79,%6f,%75
	.byte	%72,%20,%6f,%77,%6e,%20,%75,%69
	.byte	%64,%2f,%67,%69,%64,%a,%0
    .even
L136:
	.byte	%47,%65,%74,%20,%75,%69,%64,%2f
	.byte	%67,%69,%64,%20,%66,%72,%6f,%6d
	.byte	%20,%41,%72,%63,%68,%69,%76,%a
	.byte	%0
    .even
L137:
	.byte	%2d,%0
    .even
L142:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%6f,%70,%65,%6e
	.byte	%20,%25,%73,%a,%0
    .even
L144:
	.byte	%74,%61,%72,%3a,%20,%74,%72,%69
	.byte	%65,%73,%20,%6f,%70,%65,%6e,%20
	.byte	%25,%73,%a,%0
    .even
L148:
	.byte	%2d,%0
    .even
L153:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%6f,%70,%65,%6e
	.byte	%20,%25,%73,%a,%0
    .even
L155:
	.byte	%74,%61,%72,%3a,%20,%74,%72,%69
	.byte	%65,%73,%20,%6f,%70,%65,%6e,%20
	.byte	%25,%73,%a,%0
    .even
// /* WEGA P8000 'tar' Version: 1.8.88 */
// 
// 
// #include <stdio.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/dir.h>
// #include <signal.h>
// #include <sgtty.h>
// 
// #define TBLOCK	512	/* Blocklaenge (512 Byte) */
// #define NBLOCK	20	/* max. Blockung (512 * 20 Byte) */
// #define NAMSIZ	100
// 
// union hblock {
//     char dummy[TBLOCK];
//     struct header {
// 	char name[NAMSIZ];
// 	char mode[8];
// 	char uid[8];
// 	char gid[8];
// 	char size[12];
// 	char mtime[12];
// 	char chksum[8];
// 	char linkflag;
// 	char linkname[NAMSIZ];
//     } dbuf;
// } dblock, tbuf[NBLOCK];
// 
// struct linkbuf {
//     ino_t  inum;
//     dev_t  devnum;
//     int	   count;
//     char   pathname[NAMSIZ];
//     struct linkbuf *nextp;
// } *ihead;
// 
// struct stat stbuf;
// 
// char	*sprintf();
// char	*strcat();
// char	*malloc();
// daddr_t	bsrch();
// 
// int	rflag, xflag, vflag, tflag, dflag, cflag, mflag;
// int	fd, term, chksum, wflag, recno, first, linkerrok;
// int	archuid, rhead, timeout;
// int	freemem = 1;
// int	nblock = 1;	/* default: nur bei tardev */
// int	BLOCKS, Blocks, disks;
// int	xbuf[TBLOCK];
// 
// daddr_t	low;
// daddr_t	high;
// 
// FILE	*tfile;
// char	tname[] = "/tmp/tarXXXXXX";
// 
// /* default: Disk.-Drive 1, cpi 96, double-sided, 9 sector */
// /*          Typ 6 = rfd196ds9   (Blocklaenge : 512 Byte ) */
// /*				(Blockanzahl : 1440 )     */
// 
// char	*usefile;
// char	tardev[]	= "/dev/tardev";
// char	devrfd1[]	= "/dev/rfd1";
// 
// main(argc, argv)
// int	argc;
// char	*argv[];
// {
	.line	70	"tar.c"	
	.line	70	"tar.c"	

	.code

	_main::
	{

	jpr	L10001
L10002:
//     char *cp;
//     int onintr(), onquit(), onhup(), onterm();
// 
//     if (argc < 2)
	.line	74	"tar.c"	
	cp	r7,#2
	jpr	ge,L63
// 	usage();
	.line	75	"tar.c"	
	callr	_usage
L63:
// 
//     tfile = NULL;
	.line	77	"tar.c"	
	clr	_tfile
//     usefile =  tardev;
	.line	78	"tar.c"	
	ld	_usefile,#_tardev
//     argv[argc] = 0;
	.line	79	"tar.c"	
	ld	r2,~L1+2(fp)
	sla	r2,#1
	ld	r3,~L1(fp)
	add	r3,r2
	clr	@r3
//     argv++;
	.line	80	"tar.c"	
	inc	~L1(fp),#2
//     for (cp = *argv++; *cp; cp++) 
	.line	81	"tar.c"	
	ld	r2,~L1(fp)
	ld	r3,@r2
	ld	~L1+4(fp),r3
	inc	~L1(fp),#2
L67:
	.line	81	"tar.c"	
	ld	r2,~L1+4(fp)
	cpb	@r2,#0
	jpr	eq,L66
// 	switch(*cp) {
	.line	82	"tar.c"	
	ld	r2,~L1+4(fp)
	ldb	rl4,@r2
	extsb	r4
	ld	r3,r4
	jpr	L69
L70:
// 	case 'f':
// 	    usefile = *argv++;
	.line	84	"tar.c"	
	ld	r2,~L1(fp)
	ld	r4,@r2
	ld	_usefile,r4
	inc	~L1(fp),#2
//    	    if (nblock == 1) nblock = 0;
	.line	85	"tar.c"	
	cp	_nblock,#1
	jpr	ne,L71
	.line	85	"tar.c"	
	clr	_nblock
L71:
	jpr	L68
L72:
// 	    break;
//    	case 'c':
// 	    cflag++;
	.line	88	"tar.c"	
	inc	_cflag,#1
// 	    rflag++;
	.line	89	"tar.c"	
	inc	_rflag,#1
	jpr	L68
L73:
// 	    break;
// 	case 'u':
// 	    mktemp(tname);
	.line	92	"tar.c"	
	ld	r7,#_tname
	callr	_mktemp
// 	    if ((tfile = fopen(tname, "w")) == NULL) {
	.line	93	"tar.c"	
	ld	r7,#_tname
	ld	r6,#L75
	callr	_fopen
	ld	_tfile,r2
	cp	r2,#0
	jpr	ne,L76
// 		fprintf(stderr, "tar: cannot create temporary file (%s)\n", tname);
	.line	94	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L78
	ld	r5,#_tname
	callr	_fprintf
// 		done(1); }
	.line	95	"tar.c"	
	ldk	r7,#1
	callr	_done
L76:
// 	    fprintf(tfile, "!!!!!/!/!/!/!/!/!/! 000\n");
	.line	96	"tar.c"	
	ld	r7,_tfile
	ld	r6,#L80
	callr	_fprintf
L81:
// 	    /* kein 'break' !!! */
// 	case 'r':
// 	    rflag++;
	.line	99	"tar.c"	
	inc	_rflag,#1
// 	    if (nblock != 1 && cflag == 0) {
	.line	100	"tar.c"	
	cp	_nblock,#1
	jpr	eq,L82
	cp	_cflag,#0
	jpr	ne,L82
L83:
// noupdate:
// 		fprintf(stderr, "tar: Blocked archiv cannot be updated (yet)\n");
	.line	102	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L84
	callr	_fprintf
// 		done(1); }
	.line	103	"tar.c"	
	ldk	r7,#1
	callr	_done
L82:
	jpr	L68
L85:
// 	    break;
// 	case 'v':
// 	    vflag++;
	.line	106	"tar.c"	
	inc	_vflag,#1
	jpr	L68
L86:
// 	    break;
// 	case 'd':		/* TEST */
// 	    dflag++;
	.line	109	"tar.c"	
	inc	_dflag,#1
	jpr	L68
L87:
// 	    break;
// 	case 'w':
// 	    wflag++;
	.line	112	"tar.c"	
	inc	_wflag,#1
	jpr	L68
L88:
// 	    break;
// 	case 'X':		/* Lese mit eigenen uid/gid's */
// 	    archuid++;
	.line	115	"tar.c"	
	inc	_archuid,#1
L89:
// 	case 'x':
// 	    xflag++;
	.line	117	"tar.c"	
	inc	_xflag,#1
	jpr	L68
L90:
// 	    break;
// 	case 't':
// 	    tflag++;
	.line	120	"tar.c"	
	inc	_tflag,#1
	jpr	L68
L91:
// 	    break;
// 	case 'm':
// 	    mflag++;
	.line	123	"tar.c"	
	inc	_mflag,#1
	jpr	L68
L92:
	jpr	L68
L93:
L94:
// 	    break;
// 	case '-':
// 	    break;
// 	case '0':
// 	case '1':
// 	    devrfd1[8] = *cp;    /* d.h. /dev/rfd0 oder 1 */
	.line	129	"tar.c"	
	ld	r2,~L1+4(fp)
	ldb	rl3,@r2
	ldb	_devrfd1+8,rl3
// 	    usefile = devrfd1;
	.line	130	"tar.c"	
	ld	_usefile,#_devrfd1
	jpr	L68
L95:
// 	    break;
// 	case 'b':
// 	    nblock = atoi(*argv++);
	.line	133	"tar.c"	
	ld	r7,~L1(fp)
	inc	~L1(fp),#2
	ld	r6,@r7
	ld	r7,r6
	callr	_atoi
	ld	_nblock,r2
// 	    if (nblock > NBLOCK || nblock <= 0) {
	.line	134	"tar.c"	
	cp	_nblock,#20
	jpr	gt,L10003
	cp	_nblock,#0
	jpr	gt,L97
L10003:
// 		fprintf(stderr, "Invalid blocksize (Max %d)\n", NBLOCK);
	.line	135	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L98
	ld	r5,#20
	callr	_fprintf
// 		done(1); }
	.line	136	"tar.c"	
	ldk	r7,#1
	callr	_done
L97:
// 	    if (rflag && !cflag)
	.line	137	"tar.c"	
	cp	_rflag,#0
	jpr	eq,L99
	cp	_cflag,#0
	jpr	ne,L99
	jpr	L83
L99:
	jpr	L68
L100:
// 		goto noupdate;
// 	    break;
// 	case 'l':
// 	    linkerrok++;
	.line	141	"tar.c"	
	inc	_linkerrok,#1
	jpr	L68
L101:
// 	    break;
// 	default:
// 	    fprintf(stderr, "tar: %c: unknown option\n", *cp);
	.line	144	"tar.c"	
	ld	r5,~L1+4(fp)
	ldb	rl4,@r5
	ldb	rl5,rl4
	extsb	r5
	ld	r6,#L102
	ld	r7,#__iob+16
	callr	_fprintf
// 	    usage();
	.line	145	"tar.c"	
	callr	_usage
	jpr	L68
L69:
// 	}
	.line	146	"tar.c"	
	ld	r2,#L103
	ld	r4,#16
	cpir	r3,@r2,r4,eq
	jpr	ne,L101
	sll	r4,#1
	neg	r4
	ld	r3,L104+30(r4)
	jp	@r3
	.data
	.even
L104:
	.addr	L92
	.addr	L93
	.addr	L94
	.addr	L88
	.addr	L95
	.addr	L72
	.addr	L86
	.addr	L70
	.addr	L100
	.addr	L91
	.addr	L81
	.addr	L90
	.addr	L73
	.addr	L85
	.addr	L87
	.addr	L89
	.code
L68:
L65:
	.line	146	"tar.c"	
	inc	~L1+4(fp),#1
	jpr	L67
L66:
// 
// if (dflag) fprintf(stderr, "tar: Version 1.8.88\n");	/* TEST */
	.line	148	"tar.c"	
	cp	_dflag,#0
	jpr	eq,L105
	.line	148	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L106
	callr	_fprintf
L105:
// 
//     if (rflag) {
	.line	150	"tar.c"	
	cp	_rflag,#0
	jpr	eq,L107
// 	if (cflag && tfile != NULL) { usage(); done(1); }
	.line	151	"tar.c"	
	cp	_cflag,#0
	jpr	eq,L108
	cp	_tfile,#0
	jpr	eq,L108
	.line	151	"tar.c"	
	callr	_usage
	.line	151	"tar.c"	
	ldk	r7,#1
	callr	_done
L108:
// 	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
	.line	152	"tar.c"	
	ldk	r7,#2
	ldk	r6,#1
	callr	_signal
	cp	r2,#1
	jpr	eq,L109
// 	    signal(SIGINT, onintr);
	.line	153	"tar.c"	
	ldk	r7,#2
	ld	r6,#_onintr
	callr	_signal
L109:
// 	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
	.line	154	"tar.c"	
	ldk	r7,#1
	ldk	r6,#1
	callr	_signal
	cp	r2,#1
	jpr	eq,L110
// 	    signal(SIGHUP, onhup);
	.line	155	"tar.c"	
	ldk	r7,#1
	ld	r6,#_onhup
	callr	_signal
L110:
// 	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
	.line	156	"tar.c"	
	ldk	r7,#3
	ldk	r6,#1
	callr	_signal
	cp	r2,#1
	jpr	eq,L111
// 	    signal(SIGQUIT, onquit);
	.line	157	"tar.c"	
	ldk	r7,#3
	ld	r6,#_onquit
	callr	_signal
L111:
// /* WOFUER ??? 
// 	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
// 	    signal(SIGTERM, onterm);
// */
// 	if (strcmp(usefile, "-") == 0) {
	.line	162	"tar.c"	
	ld	r7,_usefile
	ld	r6,#L113
	callr	_strcmp
	cp	r2,#0
	jpr	ne,L114
// 	    if (cflag == 0) {
	.line	163	"tar.c"	
	cp	_cflag,#0
	jpr	ne,L115
// 		fprintf(stderr, "Can only create standard output archives\n");
	.line	164	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L116
	callr	_fprintf
// 		done(1); }
	.line	165	"tar.c"	
	ldk	r7,#1
	callr	_done
L115:
// 	    fd = dup(1);
	.line	166	"tar.c"	
	ldk	r7,#1
	callr	_dup
	ld	_fd,r2
// 	    nblock = 1; }
	.line	167	"tar.c"	
	ld	_nblock,#1
	jpr	L118
L114:
L119:
// 	else
// 	nonex1:
// 	if ((fd = open(usefile, 2)) < 0) {
	.line	170	"tar.c"	
	ld	r7,_usefile
	ldk	r6,#2
	callr	_open
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L121
// 	    if (cflag == 0 || (fd =  creat(usefile, 0666)) < 0) {
	.line	171	"tar.c"	
	cp	_cflag,#0
	jpr	eq,L10004
	ld	r7,_usefile
	ld	r6,#438
	callr	_creat
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L123
L10004:
// 	    fprintf(stderr, "tar: cannot open %s\n", usefile);
	.line	172	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L124
	ld	r5,_usefile
	callr	_fprintf
// 	    if (strcmp(usefile,tardev) == 0) {
	.line	173	"tar.c"	
	ld	r7,_usefile
	ld	r6,#_tardev
	callr	_strcmp
	cp	r2,#0
	jpr	ne,L125
// 	        usefile = devrfd1;
	.line	174	"tar.c"	
	ld	_usefile,#_devrfd1
// 	        fprintf(stderr, "tar: tries open %s\n", usefile);
	.line	175	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L126
	ld	r5,_usefile
	callr	_fprintf
	jpr	L119
L125:
// 	        goto nonex1; }
// 	        done(1); } }
	.line	177	"tar.c"	
	ldk	r7,#1
	callr	_done
L123:
L121:
L118:
// 	inBL();
	.line	178	"tar.c"	
	callr	_inBL
// 	if (cflag == 0 && nblock == 0) nblock = 1;
	.line	179	"tar.c"	
	cp	_cflag,#0
	jpr	ne,L128
	cp	_nblock,#0
	jpr	ne,L128
	.line	179	"tar.c"	
	ld	_nblock,#1
L128:
// 	dorep(argv);
	.line	180	"tar.c"	
	ld	r7,~L1(fp)
	callr	_dorep
	jpr	L130
L107:
//     }
//     else if (xflag)  {
	.line	182	"tar.c"	
	cp	_xflag,#0
	jpr	eq,L131
// 	if (dflag)
	.line	183	"tar.c"	
	cp	_dflag,#0
	jpr	eq,L132
// 	    if (archuid) fprintf(stderr, "Take your own uid/gid\n");
	.line	184	"tar.c"	
	cp	_archuid,#0
	jpr	eq,L133
	.line	184	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L134
	callr	_fprintf
	jpr	L135
L133:
// 	    else fprintf(stderr, "Get uid/gid from Archiv\n");
	.line	185	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L136
	callr	_fprintf
L135:
L132:
// 
// 	if (strcmp(usefile, "-") == 0) {
	.line	187	"tar.c"	
	ld	r7,_usefile
	ld	r6,#L137
	callr	_strcmp
	cp	r2,#0
	jpr	ne,L138
// 	    fd = dup(0); nblock = 1; }
	.line	188	"tar.c"	
	sub	r7,r7
	callr	_dup
	ld	_fd,r2
	.line	188	"tar.c"	
	ld	_nblock,#1
	jpr	L139
L138:
L140:
// 	else
// 	nonex2:
// 	if ((fd = open(usefile, 0)) < 0) {
	.line	191	"tar.c"	
	ld	r7,_usefile
	sub	r6,r6
	callr	_open
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L141
// 	    fprintf(stderr, "tar: cannot open %s\n", usefile);
	.line	192	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L142
	ld	r5,_usefile
	callr	_fprintf
// 	    if (strcmp(usefile,tardev) == 0) {
	.line	193	"tar.c"	
	ld	r7,_usefile
	ld	r6,#_tardev
	callr	_strcmp
	cp	r2,#0
	jpr	ne,L143
// 	    usefile = devrfd1;
	.line	194	"tar.c"	
	ld	_usefile,#_devrfd1
// 	    fprintf(stderr, "tar: tries open %s\n", usefile);
	.line	195	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L144
	ld	r5,_usefile
	callr	_fprintf
	jpr	L140
L143:
// 	    goto nonex2; }
// 	    done(1); }
	.line	197	"tar.c"	
	ldk	r7,#1
	callr	_done
L141:
L139:
// 	inBL();
	.line	198	"tar.c"	
	callr	_inBL
// 	doxtract(argv);
	.line	199	"tar.c"	
	ld	r7,~L1(fp)
	callr	_doxtract
	jpr	L146
L131:
//     }
//     else if (tflag) {
	.line	201	"tar.c"	
	cp	_tflag,#0
	jpr	eq,L147
// 	if (strcmp(usefile, "-") == 0) {
	.line	202	"tar.c"	
	ld	r7,_usefile
	ld	r6,#L148
	callr	_strcmp
	cp	r2,#0
	jpr	ne,L149
// 	    fd = dup(0); nblock = 1; }
	.line	203	"tar.c"	
	sub	r7,r7
	callr	_dup
	ld	_fd,r2
	.line	203	"tar.c"	
	ld	_nblock,#1
	jpr	L150
L149:
L151:
// 	else
// 	nonex3:
// 	if ((fd = open(usefile, 0)) < 0) {
	.line	206	"tar.c"	
	ld	r7,_usefile
	sub	r6,r6
	callr	_open
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L152
// 	    fprintf(stderr, "tar: cannot open %s\n", usefile);
	.line	207	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L153
	ld	r5,_usefile
	callr	_fprintf
// 	    if (strcmp(usefile,tardev) == 0) {
	.line	208	"tar.c"	
	ld	r7,_usefile
	ld	r6,#_tardev
	callr	_strcmp
	cp	r2,#0
	jpr	ne,L154
// 	    usefile = devrfd1;
	.line	209	"tar.c"	
	ld	_usefile,#_devrfd1
// 	    fprintf(stderr, "tar: tries open %s\n", usefile);
	.line	210	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L155
	ld	r5,_usefile
	callr	_fprintf
	jpr	L151
L154:
// 	    goto nonex3; }
// 	    done(1); }
	.line	212	"tar.c"	
	ldk	r7,#1
	callr	_done
L152:
L150:
// 	inBL();
	.line	213	"tar.c"	
	callr	_inBL
// 	if(BLOCKS)nblock = 1;
	.line	214	"tar.c"	
	cp	_BLOCKS,#0
	jpr	eq,L156
	.line	214	"tar.c"	
	ld	_nblock,#1
L156:
// 	dotable(); }
	.line	215	"tar.c"	
	callr	_dotable
	jpr	L158
L147:
//     else    usage();
	.line	216	"tar.c"	
	callr	_usage
L158:
L146:
L130:
//     done(0);
	.line	217	"tar.c"	
	sub	r7,r7
	callr	_done
L58:
L10000:
	inc	fp,#~L2
	ret
L10001:
	dec	fp,#~L2
	ldl	@fp,rr6
	jpr	L10002

	~L1 := 0
	~L2 := 6

	}	/* _main */

	.data

	.line	217	"tar.c"	
L160:
	.byte	%74,%61,%72,%3a,%20,%75,%73,%61
	.byte	%67,%65,%20,%20,%74,%61,%72,%20
	.byte	%5b,%2d,%5d,%7b,%63,%72,%75,%74
	.byte	%78,%58,%7d,%5b,%4e,%62,%66,%6c
	.byte	%6d,%76,%77,%5d,%20,%5b,%62,%6c
	.byte	%6f,%63,%6b,%73,%69,%7a,%65,%5d
	.byte	%20,%5b,%61,%72,%63,%68,%69,%76
	.byte	%64,%65,%76,%5d,%20,%5b,%6e,%61
	.byte	%6d,%65,%20,%2e,%2e,%2e,%20,%5d
	.byte	%a,%0
    .even
L161:
	.byte	%20,%20,%20,%20,%20,%63,%3a,%20
	.byte	%63,%72,%65,%61,%74,%65,%20,%61
	.byte	%72,%63,%68,%69,%76,%a,%0
    .even
L162:
	.byte	%20,%20,%20,%20,%20,%72,%3a,%20
	.byte	%77,%72,%69,%74,%65,%20,%61,%66
	.byte	%74,%65,%72,%20,%61,%72,%63,%68
	.byte	%69,%76,%65,%6e,%64,%a,%0
    .even
L163:
	.byte	%20,%20,%20,%20,%20,%75,%3a,%20
	.byte	%75,%70,%64,%61,%74,%65,%20,%61
	.byte	%72,%63,%68,%69,%76,%a,%0
    .even
L164:
	.byte	%20,%20,%20,%20,%20,%74,%3a,%20
	.byte	%64,%69,%73,%70,%6c,%61,%79,%20
	.byte	%61,%72,%63,%68,%69,%76,%a,%0
    .even
L165:
	.byte	%20,%20,%20,%20,%20,%78,%3a,%20
	.byte	%72,%65,%61,%64,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%28,%67,%65,%74
	.byte	%20,%55,%49,%44,%20,%47,%49,%44
	.byte	%20,%66,%72,%6f,%6d,%20,%61,%72
	.byte	%63,%68,%69,%76,%29,%a,%0
    .even
L166:
	.byte	%20,%20,%20,%20,%20,%58,%3a,%20
	.byte	%72,%65,%61,%64,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%28,%67,%65,%74
	.byte	%20,%79,%6f,%75,%72,%20,%6f,%77
	.byte	%6e,%20,%55,%49,%44,%20,%47,%49
	.byte	%44,%29,%a,%0
    .even
L167:
	.byte	%20,%20,%20,%20,%20,%4e,%3d,%30
	.byte	%2c,%31,%3a,%20,%61,%72,%63,%68
	.byte	%69,%76,%20,%64,%65,%76,%69,%63
	.byte	%65,%20,%6e,%75,%6d,%62,%65,%72
	.byte	%20,%28,%64,%72,%69,%76,%65,%29
	.byte	%a,%0
    .even
L168:
	.byte	%20,%20,%20,%20,%20,%62,%20,%62
	.byte	%6c,%6f,%63,%6b,%73,%69,%7a,%65
	.byte	%3a,%20,%76,%65,%72,%69,%66,%79
	.byte	%20,%61,%72,%63,%68,%69,%76,%62
	.byte	%6c,%6f,%63,%6b,%73,%69,%7a,%65
	.byte	%a,%0
    .even
L169:
	.byte	%20,%20,%20,%20,%20,%66,%20,%61
	.byte	%72,%63,%68,%69,%76,%64,%65,%76
	.byte	%3a,%20,%76,%65,%72,%69,%66,%79
	.byte	%20,%61,%72,%63,%68,%69,%76,%64
	.byte	%65,%76,%69,%63,%65,%a,%0
    .even
L170:
	.byte	%20,%20,%20,%20,%20,%6c,%3a,%20
	.byte	%64,%69,%61,%67,%6e,%6f,%73,%74
	.byte	%69,%63,%20,%6c,%69,%6e,%6b,%73
	.byte	%a,%0
    .even
L171:
	.byte	%20,%20,%20,%20,%20,%6d,%3a,%20
	.byte	%74,%69,%6d,%65,%20,%6f,%66,%20
	.byte	%6d,%6f,%64,%69,%66,%69,%63,%61
	.byte	%74,%69,%6f,%6e,%20,%66,%6f,%72
	.byte	%20,%66,%69,%6c,%65,%73,%20,%62
	.byte	%65,%63,%6f,%6d,%65,%73,%20,%61
	.byte	%63,%74,%75,%61,%6c,%20,%73,%79
	.byte	%73,%74,%65,%6d,%65,%74,%69,%6d
	.byte	%65,%a,%0
    .even
L172:
	.byte	%20,%20,%20,%20,%20,%76,%3a,%20
	.byte	%64,%69,%73,%70,%6c,%61,%79,%20
	.byte	%69,%6e,%20,%76,%65,%72,%62,%6f
	.byte	%73,%65,%6d,%6f,%64,%65,%a,%0
    .even
L173:
	.byte	%20,%20,%20,%20,%20,%77,%3a,%20
	.byte	%69,%6e,%74,%65,%72,%61,%63,%74
	.byte	%69,%76,%65,%20,%61,%72,%63,%68
	.byte	%69,%76,%77,%6f,%72,%6b,%20,%28
	.byte	%78,%2c,%58,%20,%77,%69,%74,%68
	.byte	%20,%72,%65,%6e,%61,%6d,%65,%29
	.byte	%a,%0
    .even
// }
// 
// usage()
// {
	.line	221	"tar.c"	
	.line	221	"tar.c"	

	.code

	_usage::
	{

	jpr	L10006
L10007:
//     fprintf(stderr, "tar: usage  tar [-]{crutxX}[Nbflmvw] [blocksize] [archivdev] [name ... ]\n");
	.line	222	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L160
	callr	_fprintf
//     fprintf(stderr, "     c: create archiv\n");
	.line	223	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L161
	callr	_fprintf
//     fprintf(stderr, "     r: write after archivend\n");
	.line	224	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L162
	callr	_fprintf
//     fprintf(stderr, "     u: update archiv\n");
	.line	225	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L163
	callr	_fprintf
//     fprintf(stderr, "     t: display archiv\n");
	.line	226	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L164
	callr	_fprintf
//     fprintf(stderr, "     x: read archiv \(get UID GID from archiv\)\n");
	.line	227	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L165
	callr	_fprintf
//     fprintf(stderr, "     X: read archiv \(get your own UID GID\)\n");
	.line	228	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L166
	callr	_fprintf
//     fprintf(stderr, "     N=0,1: archiv device number \(drive\)\n");
	.line	229	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L167
	callr	_fprintf
//     fprintf(stderr, "     b blocksize: verify archivblocksize\n");
	.line	230	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L168
	callr	_fprintf
//     fprintf(stderr, "     f archivdev: verify archivdevice\n");
	.line	231	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L169
	callr	_fprintf
//     fprintf(stderr, "     l: diagnostic links\n");
	.line	232	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L170
	callr	_fprintf
//     fprintf(stderr, "     m: time of modification for files becomes actual systemetime\n");
	.line	233	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L171
	callr	_fprintf
//     fprintf(stderr, "     v: display in verbosemode\n");
	.line	234	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L172
	callr	_fprintf
//     fprintf(stderr, "     w: interactive archivwork \(x,X with rename\)\n");
	.line	235	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L173
	callr	_fprintf
//     done(1); }
	.line	236	"tar.c"	
	ldk	r7,#1
	callr	_done
L159:
L10005:
	ret
L10006:
	jpr	L10007

	~L1 := 0
	~L2 := 0

	}	/* _usage */

	.data

	.line	236	"tar.c"	
L177:
	.byte	%69,%6f,%63,%74,%6c,%3a,%20,%69
	.byte	%20,%3d,%20,%25,%64,%20,%42,%4c
	.byte	%4f,%43,%4b,%53,%20,%3d,%20,%25
	.byte	%64,%a,%0
    .even
L181:
	.byte	%74,%61,%72,%3a,%20,%64,%69,%73
	.byte	%6b,%20,%63,%61,%70,%61,%63,%69
	.byte	%74,%79,%20,%25,%64,%20,%62,%6c
	.byte	%6f,%63,%6b,%73,%a,%0
    .even
// 
// /* Hole BLOCKS ueber ioctl() */
// inBL()
// {
	.line	240	"tar.c"	
	.line	240	"tar.c"	

	.code

	_inBL::
	{

	jpr	L10009
L10010:
// register i;
// 
//     i = ioctl(fd,SHOWFD,&BLOCKS);
	.line	243	"tar.c"	
	ld	r7,_fd
	ld	r6,#26112
	ld	r5,#_BLOCKS
	callr	_ioctl
	ld	r14,r2
// if (dflag >= 2) fprintf(stderr, "ioctl: i = %d BLOCKS = %d\n",i,BLOCKS);
	.line	244	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L176
	.line	244	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L177
	ld	r5,r14
	ld	r4,_BLOCKS
	callr	_fprintf
L176:
//     if (i) BLOCKS = 0;
	.line	245	"tar.c"	
	cp	r14,#0
	jpr	eq,L178
	.line	245	"tar.c"	
	clr	_BLOCKS
L178:
//     if (BLOCKS && !nblock) nblock = 1;
	.line	246	"tar.c"	
	cp	_BLOCKS,#0
	jpr	eq,L179
	cp	_nblock,#0
	jpr	ne,L179
	.line	246	"tar.c"	
	ld	_nblock,#1
L179:
// if (dflag && BLOCKS) fprintf(stderr, "tar: disk capacity %d blocks\n", BLOCKS);
	.line	247	"tar.c"	
	cp	_dflag,#0
	jpr	eq,L180
	cp	_BLOCKS,#0
	jpr	eq,L180
	.line	247	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L181
	ld	r5,_BLOCKS
	callr	_fprintf
L180:
L174:
L10008:
	ld	r14,@fp
	inc	fp,#~L2
	ret
L10009:
	dec	fp,#~L2
	ld	@fp,r14
	jpr	L10010

	~L1 := 0
	~L2 := 2

	}	/* _inBL */

	.data

	.line	247	"tar.c"	
L184:
	.byte	%64,%6f,%72,%65,%70,%28,%61,%72
	.byte	%67,%76,%29,%a,%0
    .even
L194:
	.byte	%73,%6f,%72,%74,%20,%2b,%30,%20
	.byte	%2d,%31,%20,%2b,%31,%6e,%72,%20
	.byte	%0
    .even
L195:
	.byte	%20,%2d,%6f,%20,%0
    .even
L196:
	.byte	%73,%6f,%72,%74,%20,%2b,%30,%20
	.byte	%2d,%31,%20,%2b,%31,%6e,%72,%20
	.byte	%25,%73,%20,%2d,%6f,%20,%25,%73
	.byte	%3b,%20,%61,%77,%6b,%20,%27,%24
	.byte	%31,%20,%21,%3d,%20,%70,%72,%65
	.byte	%76,%20,%7b,%70,%72,%69,%6e,%74
	.byte	%3b,%20,%70,%72,%65,%76,%3d,%24
	.byte	%31,%7d,%27,%20,%25,%73,%20,%3e
	.byte	%25,%73,%58,%3b,%6d,%76,%20,%25
	.byte	%73,%58,%20,%25,%73,%0
    .even
L199:
	.byte	%72,%0
    .even
L202:
	.byte	%74,%61,%72,%3a,%20,%6e,%6f,%20
	.byte	%77,%72,%69,%74,%65,%2d,%61,%72
	.byte	%67,%75,%6d,%65,%6e,%74,%73,%a
	.byte	%0
    .even
L215:
	.byte	%67,%65,%74,%77,%64,%20,%72,%75
	.byte	%66,%74,%20,%64,%69,%73,%6b,%65
	.byte	%6e,%64,%a,%0
    .even
L223:
	.byte	%4d,%69,%73,%73,%69,%6e,%67,%20
	.byte	%6c,%69,%6e,%6b,%73,%20,%74,%6f
	.byte	%20,%25,%73,%a,%0
    .even
// }
// 
// /* Lese von Platte, Schreibe auf Archiv */
// dorep(argv)
// char	*argv[];
// {
	.line	253	"tar.c"	
	.line	253	"tar.c"	

	.code

	_dorep::
	{

	jpr	L10012
L10013:
//     register char *cp, *cp2;
//     char wdir[60];
// 
// if (dflag >= 2) fprintf(stderr, "dorep(argv)\n");	/* TEST */
	.line	257	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L183
	.line	257	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L184
	callr	_fprintf
L183:
//     if (!cflag) {		/* vorheriges Archivlesen (Opt.: u,r) */
	.line	258	"tar.c"	
	cp	_cflag,#0
	jpr	ne,L185
// 	gethead();
	.line	259	"tar.c"	
	callr	_gethead
L189:
// 	do {
// 	    passarchiv();
	.line	261	"tar.c"	
	callr	_passarchiv
// 	    if (term)
	.line	262	"tar.c"	
	cp	_term,#0
	jpr	eq,L191
// 		done(0);
	.line	263	"tar.c"	
	sub	r7,r7
	callr	_done
L191:
// 	    gethead();
	.line	264	"tar.c"	
	callr	_gethead
L188:
// 	} while (!endarchiv());
	.line	265	"tar.c"	
	callr	_endarchiv
	cp	r2,#0
	jpr	eq,L189
L187:
// 	if (tfile != NULL) {
	.line	266	"tar.c"	
	cp	_tfile,#0
	jpr	eq,L193
// 	    char buf[200];
// 
// 	    strcat(buf, "sort +0 -1 +1nr ");
	.line	269	"tar.c"	
	lda	r7,~L1+62(fp)
	ld	r6,#L194
	callr	_strcat
// 	    strcat(buf, tname);
	.line	270	"tar.c"	
	lda	r7,~L1+62(fp)
	ld	r6,#_tname
	callr	_strcat
// 	    strcat(buf, " -o ");
	.line	271	"tar.c"	
	lda	r7,~L1+62(fp)
	ld	r6,#L195
	callr	_strcat
// 	    strcat(buf, tname);
	.line	272	"tar.c"	
	lda	r7,~L1+62(fp)
	ld	r6,#_tname
	callr	_strcat
// 	    sprintf(buf, "sort +0 -1 +1nr %s -o %s; awk '$1 != prev {print; prev=$1}' %s >%sX;mv %sX %s",
// 		tname, tname, tname, tname, tname, tname);
	.line	274	"tar.c"	
	ld	r2,#_tname
	ld	@fp,r2
	ld	r2,#_tname
	ld	2(fp),r2
	lda	r7,~L1+62(fp)
	ld	r6,#L196
	ld	r5,#_tname
	ld	r4,#_tname
	ld	r3,#_tname
	ld	r2,#_tname
	callr	_sprintf
// 	    fflush(tfile);
	.line	275	"tar.c"	
	ld	r7,_tfile
	callr	_fflush
// 	    system(buf);
	.line	276	"tar.c"	
	lda	r7,~L1+62(fp)
	callr	_system
// 	    freopen(tname, "r", tfile);
	.line	277	"tar.c"	
	ld	r7,#_tname
	ld	r6,#L199
	ld	r5,_tfile
	callr	_freopen
// 	    fstat(fileno(tfile), &stbuf);
	.line	278	"tar.c"	
	ld	r7,_tfile
	ldb	rl6,7(r7)
	ldb	rl7,rl6
	extsb	r7
	ld	r6,#_stbuf
	callr	_fstat
// 	    high = stbuf.st_size;
	.line	279	"tar.c"	
	ldl	rr2,_stbuf+14
	ldl	_high,rr2
L193:
L185:
// 	}
//     }
//     if (*argv == 0) {
	.line	282	"tar.c"	
	ld	r2,~L1(fp)
	cp	@r2,#0
	jpr	ne,L201
// 	fprintf(stderr, "tar: no write-arguments\n");
	.line	283	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L202
	callr	_fprintf
// 	done(3);
	.line	284	"tar.c"	
	ldk	r7,#3
	callr	_done
L201:
//     }
//     getwdir(wdir);
	.line	286	"tar.c"	
	lda	r7,~L1+2(fp)
	callr	_getwdir
L204:
//     while (*argv && ! term) {
	.line	287	"tar.c"	
	ld	r2,~L1(fp)
	cp	@r2,#0
	jpr	eq,L205
	cp	_term,#0
	jpr	ne,L205
// 	cp2 = *argv;
	.line	288	"tar.c"	
	ld	r2,~L1(fp)
	ld	r13,@r2
// 	for (cp = *argv; *cp; cp++)
	.line	289	"tar.c"	
	ld	r2,~L1(fp)
	ld	r14,@r2
L208:
	.line	289	"tar.c"	
	cpb	@r14,#0
	jpr	eq,L207
// 	    if (*cp == '/')
	.line	290	"tar.c"	
	cpb	@r14,#47
	jpr	ne,L209
// 		cp2 = cp;
	.line	291	"tar.c"	
	ld	r13,r14
L209:
L206:
// 	if (cp2 != *argv) {
	.line	292	"tar.c"	
	inc	r14,#1
	jpr	L208
L207:
	.line	292	"tar.c"	
	ld	r2,~L1(fp)
	cp	r13,@r2
	jpr	eq,L210
// 	    *cp2 = '\0';
	.line	293	"tar.c"	
	clrb	@r13
// 	    chdir(*argv);
	.line	294	"tar.c"	
	ld	r7,~L1(fp)
	ld	r3,@r7
	ld	r7,r3
	callr	_chdir
// 	    *cp2 = '/';
	.line	295	"tar.c"	
	ldb	@r13,#47
// 	    cp2++; }
	.line	296	"tar.c"	
	inc	r13,#1
L210:
// 	putfile(*argv++, cp2);
	.line	297	"tar.c"	
	ld	r7,~L1(fp)
	inc	~L1(fp),#2
	ld	r6,@r7
	ld	r7,r6
	ld	r6,r13
	callr	_putfile
// 	chdir(wdir); }
	.line	298	"tar.c"	
	lda	r7,~L1+2(fp)
	callr	_chdir
	jpr	L204
L205:
//     putempty();
	.line	299	"tar.c"	
	callr	_putempty
// if (dflag >= 2) fprintf(stderr, "getwd ruft diskend\n");
	.line	300	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L214
	.line	300	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L215
	callr	_fprintf
L214:
//     diskend();			/* NEU */
	.line	301	"tar.c"	
	callr	_diskend
//     write(fd, tbuf, TBLOCK*nblock);	    /* ehemals flushtape(); */
	.line	302	"tar.c"	
	ld	r5,_nblock
	sla	r5,#9
	ld	r6,#_tbuf
	ld	r7,_fd
	callr	_write
//     if (linkerrok == 1)
	.line	303	"tar.c"	
	cp	_linkerrok,#1
	jpr	ne,L218
L221:
//         for (; ihead != NULL; ihead = ihead->nextp)
	.line	304	"tar.c"	
	cp	_ihead,#0
	jpr	eq,L220
// 	if (ihead->count != 0)
	.line	305	"tar.c"	
	ld	r2,_ihead
	cp	4(r2),#0
	jpr	eq,L222
// 	    fprintf(stderr, "Missing links to %s\n", ihead->pathname);
	.line	306	"tar.c"	
	inc	r2,#6
	ld	r5,r2
	ld	r6,#L223
	ld	r7,#__iob+16
	callr	_fprintf
L222:
L219:
//     if (BLOCKS) printblocks(Blocks-nblock+recno+1);
	.line	307	"tar.c"	
	ld	r2,_ihead
	ld	r3,106(r2)
	ld	_ihead,r3
	jpr	L221
L220:
L218:
	.line	307	"tar.c"	
	cp	_BLOCKS,#0
	jpr	eq,L224
	.line	307	"tar.c"	
	ld	r7,_Blocks
	sub	r7,_nblock
	add	r7,_recno
	inc	r7,#1
	callr	_printblocks
L224:
L182:
L10011:
	ldm	r13,266(fp),#2
	add	fp,#~L2
	ret
L10012:
	sub	fp,#~L2
	ldm	266(fp),r13,#2
	ld	~L1+0(fp),r7
	jpr	L10013

	~L1 := 4
	~L2 := 270

	}	/* _dorep */

	.data

	.line	307	"tar.c"	
L228:
	.byte	%a,%74,%61,%72,%3a,%20,%61,%6c
	.byte	%6c,%6f,%63,%61,%74,%65,%64,%20
	.byte	%73,%70,%61,%63,%65,%20,%6f,%6e
	.byte	%20,%25,%32,%64,%20,%64,%69,%73
	.byte	%6b,%73,%3a,%20,%20,%25,%34,%64
	.byte	%20,%62,%6c,%6f,%63,%6b,%73,%a
	.byte	%0
    .even
L229:
	.byte	%20,%20,%20,%20,%20,%61,%6c,%6c
	.byte	%6f,%63,%61,%74,%65,%64,%20,%73
	.byte	%70,%61,%63,%65,%20,%6f,%6e,%20
	.byte	%6c,%61,%73,%74,%20,%64,%69,%73
	.byte	%6b,%3a,%20,%25,%34,%64,%20,%62
	.byte	%6c,%6f,%63,%6b,%73,%a,%0
    .even
L230:
	.byte	%20,%20,%20,%20,%20,%20,%20,%66
	.byte	%72,%65,%65,%20,%20,%20,%20,%73
	.byte	%70,%61,%63,%65,%20,%6f,%6e,%20
	.byte	%6c,%61,%73,%74,%20,%64,%69,%73
	.byte	%6b,%3a,%20,%25,%34,%64,%20,%66
	.byte	%72,%6f,%6d,%20,%25,%34,%64,%20
	.byte	%62,%6c,%6f,%63,%6b,%73,%7,%a
	.byte	%0
    .even
L232:
	.byte	%a,%74,%61,%72,%3a,%20,%61,%6c
	.byte	%6c,%6f,%63,%61,%74,%65,%64,%20
	.byte	%73,%70,%61,%63,%65,%20,%6f,%6e
	.byte	%20,%64,%69,%73,%6b,%3a,%20,%25
	.byte	%34,%64,%20,%62,%6c,%6f,%63,%6b
	.byte	%73,%a,%0
    .even
L233:
	.byte	%74,%61,%72,%3a,%20,%20,%20,%66
	.byte	%72,%65,%65,%20,%20,%20,%20,%73
	.byte	%70,%61,%63,%65,%20,%6f,%6e,%20
	.byte	%64,%69,%73,%6b,%3a,%20,%25,%34
	.byte	%64,%20,%66,%72,%6f,%6d,%20,%25
	.byte	%34,%64,%20,%62,%6c,%6f,%63,%6b
	.byte	%73,%7,%a,%0
    .even
// }
// 
// printblocks(lastdskspace)
// int lastdskspace;
// {
	.line	312	"tar.c"	
	.line	312	"tar.c"	

	.code

	_printblocks::
	{

	jpr	L10015
L10016:
// 	if (disks) {
	.line	313	"tar.c"	
	cp	_disks,#0
	jpr	eq,L227
// 	    fprintf(stderr, "\ntar: allocated space on %2d disks:  %4d blocks\n",
// 		disks+1, BLOCKS * disks + lastdskspace);
	.line	315	"tar.c"	
	ld	r5,_disks
	inc	r5,#1
	ld	r3,_BLOCKS
	mult	rr2,_disks
	add	r3,r7
	ld	r4,r3
	ld	r7,#__iob+16
	ld	r6,#L228
	callr	_fprintf
// 	    fprintf(stderr, "     allocated space on last disk: %4d blocks\n",
// 		lastdskspace);
	.line	317	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L229
	ld	r5,~L1(fp)
	callr	_fprintf
// 	    fprintf(stderr, "       free    space on last disk: %4d from %4d blocks\n",
// 		BLOCKS-lastdskspace, BLOCKS);
	.line	319	"tar.c"	
	ld	r5,_BLOCKS
	sub	r5,~L1(fp)
	ld	r6,#L230
	ld	r7,#__iob+16
	ld	r4,_BLOCKS
	callr	_fprintf
	jpr	L231
L227:
// 	} else {
// 	    fprintf(stderr, "\ntar: allocated space on disk: %4d blocks\n", lastdskspace);
	.line	321	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L232
	ld	r5,~L1(fp)
	callr	_fprintf
// 	    fprintf(stderr, "tar:   free    space on disk: %4d from %4d blocks\n",
// 		BLOCKS-lastdskspace, BLOCKS);
	.line	323	"tar.c"	
	ld	r5,_BLOCKS
	sub	r5,~L1(fp)
	ld	r6,#L233
	ld	r7,#__iob+16
	ld	r4,_BLOCKS
	callr	_fprintf
L231:
L226:
L10014:
	inc	fp,#~L2
	ret
L10015:
	dec	fp,#~L2
	ld	@fp,r7
	jpr	L10016

	~L1 := 0
	~L2 := 2

	}	/* _printblocks */

	.data

	.line	323	"tar.c"	
L236:
	.byte	%65,%6e,%64,%61,%72,%63,%68,%69
	.byte	%76,%a,%0
    .even
// 	}
// }
// 
// endarchiv()
// {
	.line	328	"tar.c"	
	.line	328	"tar.c"	

	.code

	_endarchiv::
	{

	jpr	L10018
L10019:
// if (dflag >= 2) fprintf(stderr, "endarchiv\n");	/* TEST */
	.line	329	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L235
	.line	329	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L236
	callr	_fprintf
L235:
//     if (dblock.dbuf.name[0] == '\0') {
	.line	330	"tar.c"	
	cpb	_dblock,#0
	jpr	ne,L237
// 	backarchiv();
	.line	331	"tar.c"	
	callr	_backarchiv
// 	return(1);
	.line	332	"tar.c"	
	ldk	r2,#1
	jpr	L234
L237:
//     }
//     else
// 	return(0);
	.line	335	"tar.c"	
	sub	r2,r2
	jpr	L234
L234:
L10017:
	inc	fp,#~L2
	ret
L10018:
	dec	fp,#~L2
	jpr	L10019

	~L1 := 0
	~L2 := 2

	}	/* _endarchiv */

	.data

	.line	335	"tar.c"	
L248:
	.byte	%25,%6f,%0
    .even
L253:
	.byte	%25,%6f,%0
    .even
L254:
	.byte	%25,%6f,%0
    .even
L255:
	.byte	%25,%6c,%6f,%0
    .even
L256:
	.byte	%25,%6c,%6f,%0
    .even
L257:
	.byte	%25,%6f,%0
    .even
L260:
	.byte	%68,%65,%61,%64,%62,%6c,%6f,%63
	.byte	%6b,%20,%63,%68,%65,%63,%6b,%73
	.byte	%75,%6d,%20,%65,%72,%72,%6f,%72
	.byte	%a,%0
    .even
L264:
	.byte	%74,%65,%73,%74,%20,%6e,%65,%78
	.byte	%74,%20,%62,%6c,%6f,%63,%6b,%73
	.byte	%20,%3f,%20,%28,%79,%2f,%6e,%29
	.byte	%20,%3a,%20,%0
    .even
L268:
	.byte	%25,%73,%20,%25,%73,%a,%0
    .even
// }
// 
// /* Lese Head-Block vom Archiv und Werte seine Informationen aus */
// gethead()
// {
	.line	340	"tar.c"	
	.line	340	"tar.c"	

	.code

	_gethead::
	{

	jpr	L10021
L10022:
//     register struct stat *sp;
//     int i, j;
// 
//     j = 0;
	.line	344	"tar.c"	
	clr	~L1+2(fp)
L240:
// readhead:
//     rhead = 1;			/* an readarchiv: "Erwarte headblock" */
	.line	346	"tar.c"	
	ld	_rhead,#1
//     readarchiv( (char *) &dblock);
	.line	347	"tar.c"	
	ld	r7,#_dblock
	callr	_readarchiv
//     if (rhead > 1) goto readhead;	/* readarchiv-Fehler */
	.line	348	"tar.c"	
	cp	_rhead,#1
	jpr	le,L242
	jpr	L240
L242:
//     for (i=0;i<TBLOCK && dblock.dummy[i] == '\0';i++) ;
	.line	349	"tar.c"	
	clr	~L1(fp)
L245:
	.line	349	"tar.c"	
	cp	~L1(fp),#512
	jpr	ge,L244
	ld	r2,~L1(fp)
	cpb	_dblock(r2),#0
	jpr	ne,L244
L243:
	.line	349	"tar.c"	
	inc	~L1(fp),#1
	jpr	L245
L244:
//     if (i==TBLOCK) return;
	.line	350	"tar.c"	
	cp	~L1(fp),#512
	jpr	ne,L246
	jpr	L239
L246:
//     sp = &stbuf;
	.line	351	"tar.c"	
	ld	r14,#_stbuf
//     sscanf(dblock.dbuf.mode, "%o", &sp->st_mode);
	.line	352	"tar.c"	
	lda	r5,4(r14)
	ld	r6,#L248
	ld	r7,#_dblock+100
	callr	_sscanf
//     if (archuid) {		/* uid/gid vom tar-Benutzer nehmen */
	.line	353	"tar.c"	
	cp	_archuid,#0
	jpr	eq,L249
// 	sp->st_uid = getuid();
	.line	354	"tar.c"	
	callr	_getuid
	ld	8(r14),r2
// 	sp->st_gid = getgid(); }
	.line	355	"tar.c"	
	callr	_getgid
	ld	10(r14),r2
	jpr	L252
L249:
//     else {			/* uid/gid vom Archiv nehmen */
// 	sscanf(dblock.dbuf.uid, "%o", &sp->st_uid);
	.line	357	"tar.c"	
	lda	r5,8(r14)
	ld	r6,#L253
	ld	r7,#_dblock+108
	callr	_sscanf
// 	sscanf(dblock.dbuf.gid, "%o", &sp->st_gid); }
	.line	358	"tar.c"	
	lda	r5,10(r14)
	ld	r6,#L254
	ld	r7,#_dblock+116
	callr	_sscanf
L252:
//     sscanf(dblock.dbuf.size, "%lo", &sp->st_size);
	.line	359	"tar.c"	
	lda	r5,14(r14)
	ld	r6,#L255
	ld	r7,#_dblock+124
	callr	_sscanf
//     sscanf(dblock.dbuf.mtime, "%lo", &sp->st_mtime);
	.line	360	"tar.c"	
	lda	r5,22(r14)
	ld	r6,#L256
	ld	r7,#_dblock+136
	callr	_sscanf
//     sscanf(dblock.dbuf.chksum, "%o", &chksum);
	.line	361	"tar.c"	
	ld	r7,#_dblock+148
	ld	r6,#L257
	ld	r5,#_chksum
	callr	_sscanf
//     if (chksum != checksum()) {
	.line	362	"tar.c"	
	callr	_checksum
	cp	r2,_chksum
	jpr	eq,L259
// 	fprintf(stderr, "headblock checksum error\n");
	.line	363	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L260
	callr	_fprintf
// 	if (dflag >= 2) dumpblock();
	.line	364	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L261
	.line	364	"tar.c"	
	callr	_dumpblock
L261:
// 	if (j) goto readhead;
	.line	365	"tar.c"	
	cp	~L1+2(fp),#0
	jpr	eq,L263
	jpr	L240
L263:
// 	else {
// 	    fprintf(stderr, "test next blocks ? (y/n) : ");
	.line	367	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L264
	callr	_fprintf
// 	    if (response() == 'y') { j++; goto readhead; } }
	.line	368	"tar.c"	
	callr	_response
	cp	r2,#121
	jpr	ne,L266
	.line	368	"tar.c"	
	inc	~L1+2(fp),#1
	jpr	L240
L266:
// 	done(2); }
	.line	369	"tar.c"	
	ldk	r7,#2
	callr	_done
L259:
//     if (tfile != NULL)
	.line	370	"tar.c"	
	cp	_tfile,#0
	jpr	eq,L267
// 	fprintf(tfile, "%s %s\n", dblock.dbuf.name, dblock.dbuf.mtime);
	.line	371	"tar.c"	
	ld	r7,_tfile
	ld	r6,#L268
	ld	r5,#_dblock
	ld	r4,#_dblock+136
	callr	_fprintf
L267:
//     rhead = 0;
	.line	372	"tar.c"	
	clr	_rhead
L239:
L10020:
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
L10021:
	dec	fp,#~L2
	ld	4(fp),r14
	jpr	L10022

	~L1 := 0
	~L2 := 6

	}	/* _gethead */

	.data

	.line	372	"tar.c"	
L271:
	.byte	%70,%61,%73,%73,%61,%72,%63,%68
	.byte	%69,%76,%a,%0
    .even
L277:
	.byte	%70,%61,%73,%73,%61,%72,%63,%68
	.byte	%69,%76,%3a,%20,%42,%6c,%6f,%63
	.byte	%6b,%73,%20,%3d,%20,%42,%6c,%6f
	.byte	%63,%6b,%73,%20,%2b,%20,%62,%6c
	.byte	%6f,%63,%6b,%73,%20,%28,%25,%64
	.byte	%20,%3d,%20,%25,%44,%20,%2b,%20
	.byte	%25,%44,%29,%a,%0
    .even
L279:
	.byte	%70,%61,%73,%73,%61,%72,%63,%68
	.byte	%69,%76,%20,%72,%75,%66,%74,%20
	.byte	%64,%69,%73,%6b,%65,%6e,%64,%a
	.byte	%0
    .even
L281:
	.byte	%70,%61,%73,%73,%61,%72,%63,%68
	.byte	%69,%76,%3a,%20,%42,%6c,%6f,%63
	.byte	%6b,%73,%20,%3d,%20,%25,%64,%20
	.byte	%3a,%20,%0
    .even
L283:
	.byte	%6c,%73,%65,%65,%6b,%20,%6d,%69
	.byte	%74,%20,%62,%6c,%6f,%63,%6b,%73
	.byte	%20,%3d,%20,%25,%44,%a,%0
    .even
// }
// 
// /* Archiv-Durchlauf zum Lesen der Header-Informationen */
// passarchiv()
// {
	.line	377	"tar.c"	
	.line	377	"tar.c"	

	.code

	_passarchiv::
	{

	jpr	L10024
L10025:
//     long blocks;
//     char buf[TBLOCK];
// 
// if (dflag >= 2) fprintf(stderr, "passarchiv\n");	/* TEST */
	.line	381	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L270
	.line	381	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L271
	callr	_fprintf
L270:
//     if (dblock.dbuf.linkflag == '1')
	.line	382	"tar.c"	
	cpb	_dblock+156,#49
	jpr	ne,L272
	jpr	L269
L272:
// 	return;
//     blocks = (stbuf.st_size + (TBLOCK-1)) / TBLOCK;
	.line	384	"tar.c"	
	ldl	rr6,_stbuf+14
	addl	rr6,#511
	extsl	rq4
	divl	rq4,#512
	ldl	~L1(fp),rr6
//     if (BLOCKS && nblock == 1) {
	.line	385	"tar.c"	
	cp	_BLOCKS,#0
	jpr	eq,L273
	cp	_nblock,#1
	jpr	ne,L273
// 	Blocks += blocks;
	.line	386	"tar.c"	
	ld	r2,~L1+2(fp)
	add	r2,_Blocks
	ld	_Blocks,r2
L274:
// 	while (Blocks >= BLOCKS) {
	.line	387	"tar.c"	
	ld	r2,_Blocks
	cp	r2,_BLOCKS
	jpr	lt,L275
// if (dflag >= 2) fprintf(stderr, "passarchiv: Blocks = Blocks + blocks (%d = %D + %D)\n",
	.line	388	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L276
// 	        Blocks, Blocks-blocks, blocks);
	.line	389	"tar.c"	
	ldl	rr4,~L1(fp)
	ldl	@fp,rr4
	ld	r7,r2
	exts	rr6
	subl	rr6,~L1(fp)
	ldl	rr2,rr6
	ld	r6,#L277
	ld	r5,_Blocks
	ld	r7,#__iob+16
	callr	_fprintf
L276:
// 	    blocks = Blocks;
	.line	390	"tar.c"	
	ld	r3,_Blocks
	exts	rr2
	ldl	~L1(fp),rr2
// if (dflag >= 2) fprintf(stderr, "passarchiv ruft diskend\n");
	.line	391	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L278
	.line	391	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L279
	callr	_fprintf
L278:
// 	    diskend();
	.line	392	"tar.c"	
	callr	_diskend
// 	    blocks -= BLOCKS;
	.line	393	"tar.c"	
	ld	r3,_BLOCKS
	exts	rr2
	ldl	rr4,~L1(fp)
	subl	rr4,rr2
	ldl	~L1(fp),rr4
// 	    Blocks = blocks; }
	.line	394	"tar.c"	
	ld	r2,~L1+2(fp)
	ld	_Blocks,r2
	jpr	L274
L275:
// if (dflag >= 2) fprintf(stderr, "passarchiv: Blocks = %d : ", Blocks);
	.line	395	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L280
	.line	395	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L281
	ld	r5,_Blocks
	callr	_fprintf
L280:
// if (dflag >= 2) fprintf(stderr, "lseek mit blocks = %D\n", blocks);
	.line	396	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L282
	.line	396	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L283
	ldl	rr4,~L1(fp)
	callr	_fprintf
L282:
// 	lseek(fd, (long)TBLOCK*blocks, 1);
	.line	397	"tar.c"	
	ldl	rr4,~L1(fp)
	slal	rr4,#9
	ld	r7,_fd
	ldk	r3,#1
	callr	_lseek
// 	blocks = 0; }
	.line	398	"tar.c"	
	subl	rr2,rr2
	ldl	~L1(fp),rr2
	jpr	L285
L273:
L286:
//     else while (blocks-- > 0)
	.line	399	"tar.c"	
	ldl	rr2,~L1(fp)
	ldl	rr4,rr2
	subl	rr2,#1
	ldl	~L1(fp),rr2
	cpl	rr4,#0
	jpr	le,L287
// 	readarchiv(buf);
	.line	400	"tar.c"	
	lda	r7,~L1+4(fp)
	callr	_readarchiv
	jpr	L286
L287:
L285:
L269:
L10023:
	add	fp,#~L2
	ret
L10024:
	sub	fp,#~L2
	jpr	L10025

	~L1 := 4
	~L2 := 520

	}	/* _passarchiv */

	.data

	.line	400	"tar.c"	
L290:
	.byte	%74,%61,%72,%3a,%20,%25,%73,%3a
	.byte	%20,%63,%61,%6e,%6e,%6f,%74,%20
	.byte	%6f,%70,%65,%6e,%20,%66,%69,%6c
	.byte	%65,%a,%0
    .even
L304:
	.byte	%2e,%0
    .even
L305:
	.byte	%2e,%2e,%0
    .even
L310:
	.byte	%2e,%0
    .even
L311:
	.byte	%2e,%2e,%0
    .even
L313:
	.byte	%74,%61,%72,%3a,%20,%25,%73,%20
	.byte	%69,%73,%20,%6e,%6f,%74,%20,%61
	.byte	%20,%66,%69,%6c,%65,%2e,%20,%4e
	.byte	%6f,%74,%20,%61,%72,%63,%68,%69
	.byte	%76,%65,%64,%a,%0
    .even
L319:
	.byte	%25,%73,%3a,%20,%66,%69,%6c,%65
	.byte	%20,%6e,%61,%6d,%65,%20,%74,%6f
	.byte	%6f,%20,%6c,%6f,%6e,%67,%a,%0
    .even
L327:
	.byte	%25,%36,%6f,%0
    .even
L330:
	.byte	%61,%20,%25,%73,%20,%0
    .even
L331:
	.byte	%6c,%69,%6e,%6b,%20,%74,%6f,%20
	.byte	%25,%73,%a,%0
    .even
L334:
	.byte	%4f,%75,%74,%20,%6f,%66,%20,%6d
	.byte	%65,%6d,%6f,%72,%79,%2e,%20,%4c
	.byte	%69,%6e,%6b,%20,%69,%6e,%66,%6f
	.byte	%72,%6d,%61,%74,%69,%6f,%6e,%20
	.byte	%6c,%6f,%73,%74,%a,%0
    .even
L337:
	.byte	%61,%20,%25,%73,%20,%0
    .even
L338:
	.byte	%25,%6c,%64,%20,%62,%6c,%6f,%63
	.byte	%6b,%73,%a,%0
    .even
L339:
	.byte	%25,%36,%6f,%0
    .even
L343:
	.byte	%25,%73,%3a,%20,%66,%69,%6c,%65
	.byte	%20,%63,%68,%61,%6e,%67,%65,%64
	.byte	%20,%73,%69,%7a,%65,%a,%0
    .even
// }
// 
// /* Schreibe Datei auf Archiv */
// putfile(longname, shortname)
// char *longname;
// char *shortname;
// {
	.line	407	"tar.c"	
	.line	407	"tar.c"	

	.code

	_putfile::
	{

	jpr	L10027
L10028:
//     int infile;
//     long blocks;
//     char buf[TBLOCK];
//     register char *cp, *cp2;
//     struct direct dbuf;
//     int i, j;
// 
//     infile = open(shortname, 0);
	.line	415	"tar.c"	
	ld	r7,~L1(fp)
	sub	r6,r6
	callr	_open
	ld	~L1+4(fp),r2
//     if (infile < 0) {
	.line	416	"tar.c"	
	cp	~L1+4(fp),#0
	jpr	ge,L289
// 	fprintf(stderr, "tar: %s: cannot open file\n", longname);
	.line	417	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L290
	ld	r5,~L1+2(fp)
	callr	_fprintf
	jpr	L288
L289:
// 	return; }
// 
//     fstat(infile, &stbuf);
	.line	420	"tar.c"	
	ld	r7,~L1+4(fp)
	ld	r6,#_stbuf
	callr	_fstat
// 
//     if (tfile != NULL && checkupdate(longname) == 0) {
	.line	422	"tar.c"	
	cp	_tfile,#0
	jpr	eq,L292
	ld	r7,~L1+2(fp)
	callr	_checkupdate
	cp	r2,#0
	jpr	ne,L292
// 	close(infile);
	.line	423	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
	jpr	L288
L292:
// 	return; }
//     if (checkw('r', longname) == 0) {
	.line	425	"tar.c"	
	ld	r7,#114
	ld	r6,~L1+2(fp)
	callr	_checkw
	cp	r2,#0
	jpr	ne,L295
// 	close(infile);
	.line	426	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
	jpr	L288
L295:
// 	return; }
// 
//     if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {		/* = Directory */
	.line	429	"tar.c"	
	ld	r2,_stbuf+4
	and	r2,#61440
	cp	r2,#16384
	jpr	ne,L296
// 	for (i = 0, cp = buf; *cp++ = longname[i++];);
	.line	430	"tar.c"	
	clr	~L1+538(fp)
	lda	r2,~L1+10(fp)
	ld	r14,r2
L299:
	.line	430	"tar.c"	
	ld	r2,~L1+538(fp)
	inc	~L1+538(fp),#1
	ld	r3,~L1+2(fp)
	add	r3,r2
	ld	r2,r14
	inc	r14,#1
	ldb	rl4,@r3
	ldb	@r2,rl4
	cpb	rl4,#0
	jpr	eq,L298
L297:
	jpr	L299
L298:
// 	*--cp = '/';
	.line	431	"tar.c"	
	dec	r14,#1
	ldb	@r14,#47
// 	cp++;
	.line	432	"tar.c"	
	inc	r14,#1
// 	i = 0;
	.line	433	"tar.c"	
	clr	~L1+538(fp)
// 	chdir(shortname);
	.line	434	"tar.c"	
	ld	r7,~L1(fp)
	callr	_chdir
L301:
// 	while (read(infile, (char *)&dbuf, sizeof(dbuf)) > 0 && !term) {
	.line	435	"tar.c"	
	lda	r6,~L1+522(fp)
	ld	r7,~L1+4(fp)
	ld	r5,#16
	callr	_read
	cp	r2,#0
	jpr	le,L302
	cp	_term,#0
	jpr	ne,L302
// 	    if (dbuf.d_ino == 0) {
	.line	436	"tar.c"	
	cp	~L1+522(fp),#0
	jpr	ne,L303
// 		i++; continue; }
	.line	437	"tar.c"	
	inc	~L1+538(fp),#1
	jpr	L301
L303:
// 	    if (strcmp(".", dbuf.d_name) == 0 ||
// 		strcmp("..", dbuf.d_name) == 0)  {
	.line	439	"tar.c"	
	lda	r6,~L1+524(fp)
	ld	r7,#L304
	callr	_strcmp
	cp	r2,#0
	jpr	eq,L10029
	lda	r6,~L1+524(fp)
	ld	r7,#L305
	callr	_strcmp
	cp	r2,#0
	jpr	ne,L306
L10029:
// 		i++; continue; }
	.line	440	"tar.c"	
	inc	~L1+538(fp),#1
	jpr	L301
L306:
// 	    cp2 = cp;
	.line	441	"tar.c"	
	ld	r13,r14
// 	    for (j=0; j < DIRSIZ; j++)
	.line	442	"tar.c"	
	clr	~L1+540(fp)
L309:
	.line	442	"tar.c"	
	cp	~L1+540(fp),#14
	jpr	ge,L308
// 		*cp2++ = dbuf.d_name[j];
	.line	443	"tar.c"	
	ld	r2,~L1+540(fp)
	lda	r3,~L1+524(fp)
	add	r3,r2
	ldb	rl4,@r3
	ldb	@r13,rl4
	inc	r13,#1
L307:
	.line	443	"tar.c"	
	inc	~L1+540(fp),#1
	jpr	L309
L308:
// 	    *cp2 = '\0';
	.line	444	"tar.c"	
	clrb	@r13
// 	    close(infile);
	.line	445	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
// 	    putfile(buf, cp);		/* rekursiv */
	.line	446	"tar.c"	
	lda	r7,~L1+10(fp)
	ld	r6,r14
	callr	_putfile
// 	    infile = open(".", 0);
	.line	447	"tar.c"	
	ld	r7,#L310
	sub	r6,r6
	callr	_open
	ld	~L1+4(fp),r2
// 	    i++;
	.line	448	"tar.c"	
	inc	~L1+538(fp),#1
// 	    lseek(infile, (long) (sizeof(dbuf) * i), 0);
	.line	449	"tar.c"	
	ld	r4,~L1+538(fp)
	sla	r4,#4
	ld	r3,r4
	exts	rr2
	ldl	rr4,rr2
	ld	r7,~L1+4(fp)
	sub	r3,r3
	callr	_lseek
	jpr	L301
L302:
// 	}
// 	close(infile);
	.line	451	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
// 	chdir("..");
	.line	452	"tar.c"	
	ld	r7,#L311
	callr	_chdir
	jpr	L288
L296:
// 	return;
//     }
//     if ((stbuf.st_mode & S_IFMT) != S_IFREG) {
	.line	455	"tar.c"	
	ld	r2,_stbuf+4
	and	r2,#61440
	cp	r2,#32768
	jpr	eq,L312
// 	fprintf(stderr, "tar: %s is not a file. Not archived\n", longname);
	.line	456	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L313
	ld	r5,~L1+2(fp)
	callr	_fprintf
	jpr	L288
L312:
// 	return;
//     }
// 
//     tomodes(&stbuf);
	.line	460	"tar.c"	
	ld	r7,#_stbuf
	callr	_tomodes
// 
//     cp2 = longname;
	.line	462	"tar.c"	
	ld	r13,~L1+2(fp)
//     for (cp = dblock.dbuf.name, i=0; (*cp++ = *cp2++) && i < NAMSIZ; i++);
	.line	463	"tar.c"	
	ld	r14,#_dblock
	clr	~L1+538(fp)
L317:
	.line	463	"tar.c"	
	ld	r2,r13
	inc	r13,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	cpb	rl4,#0
	jpr	eq,L316
	cp	~L1+538(fp),#100
	jpr	ge,L316
L315:
	.line	463	"tar.c"	
	inc	~L1+538(fp),#1
	jpr	L317
L316:
//     if (i >= NAMSIZ) {
	.line	464	"tar.c"	
	cp	~L1+538(fp),#100
	jpr	lt,L318
// 	fprintf(stderr, "%s: file name too long\n", longname);
	.line	465	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L319
	ld	r5,~L1+2(fp)
	callr	_fprintf
// 	close(infile);
	.line	466	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
	jpr	L288
L318:
// 	return;
//     }
// 
//     if (stbuf.st_nlink > 1) {
	.line	470	"tar.c"	
	cp	_stbuf+6,#1
	jpr	le,L320
// 	struct linkbuf *lp;
// 	int found = 0;
	.line	472	"tar.c"	
	clr	~L1+544(fp)
// 
// 	for (lp = ihead; lp != NULL; lp = lp->nextp) {
	.line	474	"tar.c"	
	ld	r2,_ihead
	ld	~L1+542(fp),r2
L323:
	.line	474	"tar.c"	
	cp	~L1+542(fp),#0
	jpr	eq,L322
// 	    if (lp->inum == stbuf.st_ino && lp->devnum == stbuf.st_dev) {
	.line	475	"tar.c"	
	ld	r2,~L1+542(fp)
	ld	r3,@r2
	cp	r3,_stbuf+2
	jpr	ne,L324
	ld	r2,~L1+542(fp)
	ld	r3,2(r2)
	cp	r3,_stbuf
	jpr	ne,L324
// 		found++;
	.line	476	"tar.c"	
	inc	~L1+544(fp),#1
	jpr	L322
L324:
L321:
// 		break;
// 	    }
// 	}
	.line	479	"tar.c"	
	ld	r2,~L1+542(fp)
	ld	r3,106(r2)
	ld	~L1+542(fp),r3
	jpr	L323
L322:
// 	if (found) {
	.line	480	"tar.c"	
	cp	~L1+544(fp),#0
	jpr	eq,L325
// 	    strcpy(dblock.dbuf.linkname, lp->pathname);
	.line	481	"tar.c"	
	ld	r6,~L1+542(fp)
	inc	r6,#6
	ld	r7,#_dblock+157
	callr	_strcpy
// 	    dblock.dbuf.linkflag = '1';
	.line	482	"tar.c"	
	ldb	_dblock+156,#49
// 	    sprintf(dblock.dbuf.chksum, "%6o", checksum());
	.line	483	"tar.c"	
	callr	_checksum
	ld	~L1+546(fp),r2
	ld	r7,#_dblock+148
	ld	r6,#L327
	ld	r5,~L1+546(fp)
	callr	_sprintf
// 	    writearchiv( (char *) &dblock);
	.line	484	"tar.c"	
	ld	r7,#_dblock
	callr	_writearchiv
// 	    if (vflag) {
	.line	485	"tar.c"	
	cp	_vflag,#0
	jpr	eq,L329
// 		fprintf(stderr, "a %s ", longname);
	.line	486	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L330
	ld	r5,~L1+2(fp)
	callr	_fprintf
// 		fprintf(stderr, "link to %s\n", lp->pathname);
	.line	487	"tar.c"	
	ld	r5,~L1+542(fp)
	inc	r5,#6
	ld	r6,#L331
	ld	r7,#__iob+16
	callr	_fprintf
L329:
// 	    }
// 	    lp->count--;
	.line	489	"tar.c"	
	ld	r2,~L1+542(fp)
	inc	r2,#4
	ld	r2,~L1+542(fp)
	dec	4(r2),#1
// 	    close(infile);
	.line	490	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
	jpr	L288
L325:
// 	    return;
// 	}
// 	else {
// 	    lp = (struct linkbuf *) malloc(sizeof(*lp));
	.line	494	"tar.c"	
	ld	r7,#108
	callr	_malloc
	ld	~L1+542(fp),r2
// 	    if (lp == NULL) {
	.line	495	"tar.c"	
	cp	~L1+542(fp),#0
	jpr	ne,L332
// 		if (freemem) {
	.line	496	"tar.c"	
	cp	_freemem,#0
	jpr	eq,L333
// 		    fprintf(stderr, "Out of memory. Link information lost\n");
	.line	497	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L334
	callr	_fprintf
// 		    freemem = 0;
	.line	498	"tar.c"	
	clr	_freemem
L333:
	jpr	L335
L332:
// 		}
// 	    }
// 	    else {
// 		lp->nextp = ihead;
	.line	502	"tar.c"	
	ld	r2,~L1+542(fp)
	ld	r3,_ihead
	ld	106(r2),r3
// 		ihead = lp;
	.line	503	"tar.c"	
	ld	r3,~L1+542(fp)
	ld	_ihead,r3
// 		lp->inum = stbuf.st_ino;
	.line	504	"tar.c"	
	ld	r2,~L1+542(fp)
	ld	r4,_stbuf+2
	ld	@r2,r4
// 		lp->devnum = stbuf.st_dev;
	.line	505	"tar.c"	
	ld	r2,~L1+542(fp)
	ld	r4,_stbuf
	ld	2(r2),r4
// 		lp->count = stbuf.st_nlink - 1;
	.line	506	"tar.c"	
	ld	r4,_stbuf+6
	dec	r4,#1
	ld	r2,~L1+542(fp)
	ld	4(r2),r4
// 		strcpy(lp->pathname, longname);
	.line	507	"tar.c"	
	ld	r7,~L1+542(fp)
	inc	r7,#6
	ld	r6,~L1+2(fp)
	callr	_strcpy
L335:
L320:
// 	    }
// 	}
//     }
// 
//     blocks = (stbuf.st_size + (TBLOCK-1)) / TBLOCK;
	.line	512	"tar.c"	
	ldl	rr6,_stbuf+14
	addl	rr6,#511
	extsl	rq4
	divl	rq4,#512
	ldl	~L1+6(fp),rr6
//     if (vflag) {
	.line	513	"tar.c"	
	cp	_vflag,#0
	jpr	eq,L336
// 	fprintf(stderr, "a %s ", longname);
	.line	514	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L337
	ld	r5,~L1+2(fp)
	callr	_fprintf
// 	fprintf(stderr, "%ld blocks\n", blocks);
	.line	515	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L338
	ldl	rr4,~L1+6(fp)
	callr	_fprintf
L336:
//     }
//     sprintf(dblock.dbuf.chksum, "%6o", checksum());
	.line	517	"tar.c"	
	callr	_checksum
	ld	~L1+542(fp),r2
	ld	r7,#_dblock+148
	ld	r6,#L339
	ld	r5,~L1+542(fp)
	callr	_sprintf
//     writearchiv( (char *) &dblock);
	.line	518	"tar.c"	
	ld	r7,#_dblock
	callr	_writearchiv
L340:
// 
//     while ((i = read(infile, buf, TBLOCK)) > 0 && blocks > 0) {
	.line	520	"tar.c"	
	lda	r6,~L1+10(fp)
	ld	r7,~L1+4(fp)
	ld	r5,#512
	callr	_read
	ld	~L1+538(fp),r2
	cp	r2,#0
	jpr	le,L341
	ldl	rr2,~L1+6(fp)
	cpl	rr2,#0
	jpr	le,L341
// 	writearchiv(buf);
	.line	521	"tar.c"	
	lda	r7,~L1+10(fp)
	callr	_writearchiv
// 	blocks--;
	.line	522	"tar.c"	
	ldl	rr2,~L1+6(fp)
	subl	rr2,#1
	ldl	~L1+6(fp),rr2
	jpr	L340
L341:
//     }
//     close(infile);
	.line	524	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
//     if (blocks != 0 || i != 0)
	.line	525	"tar.c"	
	ldl	rr2,~L1+6(fp)
	cpl	rr2,#0
	jpr	ne,L10030
	cp	~L1+538(fp),#0
	jpr	eq,L342
L10030:
// 	fprintf(stderr, "%s: file changed size\n", longname);
	.line	526	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L343
	ld	r5,~L1+2(fp)
	callr	_fprintf
L342:
L344:
//     while (blocks-- >  0)
	.line	527	"tar.c"	
	ldl	rr2,~L1+6(fp)
	ldl	rr4,rr2
	subl	rr2,#1
	ldl	~L1+6(fp),rr2
	cpl	rr4,#0
	jpr	le,L345
// 	putempty();
	.line	528	"tar.c"	
	callr	_putempty
	jpr	L344
L345:
L288:
L10026:
	ldm	r13,548(fp),#2
	add	fp,#~L2
	ret
L10027:
	sub	fp,#~L2
	ldm	548(fp),r13,#2
	ldl	@fp,rr6
	jpr	L10028

	~L1 := 0
	~L2 := 552

	}	/* _putfile */

	.data

	.line	528	"tar.c"	
L365:
	.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
	.byte	%6f,%74,%20,%6c,%69,%6e,%6b,%a
	.byte	%0
    .even
L367:
	.byte	%25,%73,%20,%6c,%69,%6e,%6b,%65
	.byte	%64,%20,%74,%6f,%20,%25,%73,%a
	.byte	%0
    .even
L369:
	.byte	%74,%61,%72,%3a,%20,%25,%73,%20
	.byte	%2d,%20,%63,%61,%6e,%6e,%6f,%74
	.byte	%20,%63,%72,%65,%61,%74,%65,%a
	.byte	%0
    .even
L372:
	.byte	%78,%20,%25,%73,%2c,%20,%25,%6c
	.byte	%64,%20,%62,%79,%74,%65,%73,%2c
	.byte	%20,%25,%6c,%64,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%62,%6c,%6f,%63
	.byte	%6b,%73,%a,%0
    .even
L378:
	.byte	%a,%0
    .even
L379:
	.byte	%74,%61,%72,%3a,%20,%25,%73,%3a
	.byte	%20,%48,%45,%4c,%50,%31,%20,%2d
	.byte	%20,%65,%78,%74,%72,%61,%63,%74
	.byte	%20,%77,%72,%69,%74,%65,%20,%65
	.byte	%72,%72,%6f,%72,%a,%0
    .even
L383:
	.byte	%a,%0
    .even
L384:
	.byte	%74,%61,%72,%3a,%20,%25,%73,%3a
	.byte	%20,%48,%45,%4c,%50,%32,%20,%2d
	.byte	%20,%65,%78,%74,%72,%61,%63,%74
	.byte	%20,%77,%72,%69,%74,%65,%20,%65
	.byte	%72,%72,%6f,%72,%a,%0
    .even
L393:
	.byte	%74,%61,%72,%3a,%20,%25,%73,%20
	.byte	%6e,%6f,%74,%20,%69,%6e,%20,%61
	.byte	%72,%63,%68,%69,%76,%a,%0
    .even
// }
// 
// /* Lese von Archiv, Schreibe auf Platte */
// doxtract(argv)
// char	*argv[];
// {
	.line	534	"tar.c"	
	.line	534	"tar.c"	

	.code

	_doxtract::
	{

	jpr	L10032
L10033:
L349:
//     long blocks, bytes;
//     char buf[TBLOCK];
//     char **cp;
//     int ofile, i;
// 
//     for (;;) {
// 	gethead();
	.line	541	"tar.c"	
	callr	_gethead
// 	if (endarchiv())
	.line	542	"tar.c"	
	callr	_endarchiv
	cp	r2,#0
	jpr	eq,L350
	jpr	L348
L350:
// 	    break;
// 
// 	if (*argv == 0)
	.line	545	"tar.c"	
	ld	r2,~L1(fp)
	cp	@r2,#0
	jpr	ne,L351
	jpr	L352
L351:
// 	    goto gotit;
// 
// 	for (cp = argv, i = 0; *cp; cp++, i++)
	.line	548	"tar.c"	
	ld	r2,~L1(fp)
	ld	~L1+522(fp),r2
	clr	~L1+526(fp)
L355:
	.line	548	"tar.c"	
	ld	r2,~L1+522(fp)
	cp	@r2,#0
	jpr	eq,L354
// 	    if (prefix(*cp, dblock.dbuf.name))
	.line	549	"tar.c"	
	ld	r7,~L1+522(fp)
	ld	r3,@r7
	ld	r7,r3
	ld	r6,#_dblock
	callr	_prefix
	cp	r2,#0
	jpr	eq,L357
	jpr	L358
L357:
L353:
// 		goto vgotit;
// 	passarchiv();
	.line	551	"tar.c"	
	inc	~L1+522(fp),#2
	inc	~L1+526(fp),#1
	jpr	L355
L354:
	.line	551	"tar.c"	
	callr	_passarchiv
	jpr	L347
L358:
// 	continue;
// 
// vgotit:
// 	xbuf[i]++;
	.line	555	"tar.c"	
	ld	r2,~L1+526(fp)
	sla	r2,#1
	add	r2,#_xbuf
	ld	r2,~L1+526(fp)
	sla	r2,#1
	inc	_xbuf(r2),#1
L352:
// gotit:
// 	if (checkw('x', dblock.dbuf.name) == 0) {
	.line	557	"tar.c"	
	ld	r7,#120
	ld	r6,#_dblock
	callr	_checkw
	cp	r2,#0
	jpr	ne,L359
// 	    passarchiv();
	.line	558	"tar.c"	
	callr	_passarchiv
	jpr	L347
L359:
// 	    continue;
// 	}
// 
// 	checkdir(dblock.dbuf.name);
	.line	562	"tar.c"	
	ld	r7,#_dblock
	callr	_checkdir
// 
// 	if (dblock.dbuf.linkflag == '1') {
	.line	564	"tar.c"	
	cpb	_dblock+156,#49
	jpr	ne,L361
// 	    unlink(dblock.dbuf.name);
	.line	565	"tar.c"	
	ld	r7,#_dblock
	callr	_unlink
// 	    if (link(dblock.dbuf.linkname, dblock.dbuf.name) < 0) {
	.line	566	"tar.c"	
	ld	r7,#_dblock+157
	ld	r6,#_dblock
	callr	_link
	cp	r2,#0
	jpr	ge,L364
// 		fprintf(stderr, "%s: cannot link\n", dblock.dbuf.name);
	.line	567	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L365
	ld	r5,#_dblock
	callr	_fprintf
	jpr	L347
L364:
// 		continue;
// 	    }
// 	    if (vflag)
	.line	570	"tar.c"	
	cp	_vflag,#0
	jpr	eq,L366
// 		fprintf(stderr, "%s linked to %s\n", dblock.dbuf.name, dblock.dbuf.linkname);
	.line	571	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L367
	ld	r5,#_dblock
	ld	r4,#_dblock+157
	callr	_fprintf
L366:
	jpr	L347
L361:
// 	    continue;
// 	}
// 	if ((ofile = creat(dblock.dbuf.name, stbuf.st_mode & 07777)) < 0) {
	.line	574	"tar.c"	
	ld	r6,_stbuf+4
	and	r6,#4095
	ld	r7,#_dblock
	callr	_creat
	ld	~L1+524(fp),r2
	cp	r2,#0
	jpr	ge,L368
// 	    fprintf(stderr, "tar: %s - cannot create\n", dblock.dbuf.name);
	.line	575	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L369
	ld	r5,#_dblock
	callr	_fprintf
// 	    passarchiv();
	.line	576	"tar.c"	
	callr	_passarchiv
	jpr	L347
L368:
// 	    continue;
// 	}
// 
// 	chown(dblock.dbuf.name, stbuf.st_uid, stbuf.st_gid);
	.line	580	"tar.c"	
	ld	r7,#_dblock
	ld	r6,_stbuf+8
	ld	r5,_stbuf+10
	callr	_chown
// 
// 	blocks = ((bytes = stbuf.st_size) + TBLOCK-1)/TBLOCK;
	.line	582	"tar.c"	
	ldl	rr6,_stbuf+14
	ldl	~L1+6(fp),rr6
	addl	rr6,#511
	extsl	rq4
	divl	rq4,#512
	ldl	~L1+2(fp),rr6
// 	if (vflag)
	.line	583	"tar.c"	
	cp	_vflag,#0
	jpr	eq,L371
// 	    fprintf(stderr, "x %s, %ld bytes, %ld archiv blocks\n", dblock.dbuf.name, bytes, blocks);
	.line	584	"tar.c"	
	ldl	rr2,~L1+2(fp)
	ldl	@fp,rr2
	ld	r7,#__iob+16
	ld	r6,#L372
	ld	r5,#_dblock
	ldl	rr2,~L1+6(fp)
	callr	_fprintf
L371:
L373:
// 	while (blocks-- > 0) {
	.line	585	"tar.c"	
	ldl	rr2,~L1+2(fp)
	ldl	rr4,rr2
	subl	rr2,#1
	ldl	~L1+2(fp),rr2
	cpl	rr4,#0
	jpr	le,L374
// 	    readarchiv(buf);
	.line	586	"tar.c"	
	lda	r7,~L1+10(fp)
	callr	_readarchiv
// 	    if (bytes > TBLOCK) {
	.line	587	"tar.c"	
	ldl	rr2,~L1+6(fp)
	cpl	rr2,#512
	jpr	le,L375
// 		if (write(ofile, buf, TBLOCK) < 0) {
	.line	588	"tar.c"	
	lda	r6,~L1+10(fp)
	ld	r7,~L1+524(fp)
	ld	r5,#512
	callr	_write
	cp	r2,#0
	jpr	ge,L376
// 		    if (dflag >= 2) fprintf(stderr, "\n");
	.line	589	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L377
	.line	589	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L378
	callr	_fprintf
L377:
// 		    fprintf(stderr, "tar: %s: HELP1 - extract write error\n", dblock.dbuf.name);
	.line	590	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L379
	ld	r5,#_dblock
	callr	_fprintf
// 		    done(2);
	.line	591	"tar.c"	
	ldk	r7,#2
	callr	_done
L376:
	jpr	L380
L375:
// 		}
// 	    } else
// 		if (write(ofile, buf, (int) bytes) < 0) {
	.line	594	"tar.c"	
	lda	r6,~L1+10(fp)
	ld	r5,~L1+8(fp)
	ld	r7,~L1+524(fp)
	callr	_write
	cp	r2,#0
	jpr	ge,L381
// 		    if (dflag >= 2) fprintf(stderr, "\n");
	.line	595	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L382
	.line	595	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L383
	callr	_fprintf
L382:
// 		    fprintf(stderr, "tar: %s: HELP2 - extract write error\n", dblock.dbuf.name);
	.line	596	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L384
	ld	r5,#_dblock
	callr	_fprintf
// 		    done(2);
	.line	597	"tar.c"	
	ldk	r7,#2
	callr	_done
L381:
L380:
// 		}
// 	    bytes -= TBLOCK;
	.line	599	"tar.c"	
	ldl	rr2,~L1+6(fp)
	subl	rr2,#512
	ldl	~L1+6(fp),rr2
	jpr	L373
L374:
// 	}
// 	close(ofile);
	.line	601	"tar.c"	
	ld	r7,~L1+524(fp)
	callr	_close
// 	if (mflag == 0) {
	.line	602	"tar.c"	
	cp	_mflag,#0
	jpr	ne,L385
// 	    time_t timep[2];
// 
// 	    timep[0] = time(NULL);
	.line	605	"tar.c"	
	sub	r7,r7
	callr	_time
	ld	r5,r2
	exts	rr4
	ldl	~L1+528(fp),rr4
// 	    timep[1] = stbuf.st_mtime;
	.line	606	"tar.c"	
	ldl	rr2,_stbuf+22
	ldl	~L1+532(fp),rr2
// 	    utime(dblock.dbuf.name, timep); }
	.line	607	"tar.c"	
	lda	r6,~L1+528(fp)
	ld	r7,#_dblock
	callr	_utime
L385:
L347:
	jpr	L349
L348:
//     }
//     if (*argv) {
	.line	609	"tar.c"	
	ld	r2,~L1(fp)
	cp	@r2,#0
	jpr	eq,L388
// 	for (cp = argv, i = 0; *cp; cp++, i++)
	.line	610	"tar.c"	
	ld	r3,~L1(fp)
	ld	~L1+522(fp),r3
	clr	~L1+526(fp)
L391:
	.line	610	"tar.c"	
	ld	r2,~L1+522(fp)
	cp	@r2,#0
	jpr	eq,L390
// 	    if (!xbuf[i])
	.line	611	"tar.c"	
	ld	r3,~L1+526(fp)
	sla	r3,#1
	cp	_xbuf(r3),#0
	jpr	ne,L392
// 		fprintf(stderr, "tar: %s not in archiv\n", *cp); }
	.line	612	"tar.c"	
	ld	r5,~L1+522(fp)
	ld	r3,@r5
	ld	r5,r3
	ld	r6,#L393
	ld	r7,#__iob+16
	callr	_fprintf
L392:
L389:
	.line	612	"tar.c"	
	inc	~L1+522(fp),#2
	inc	~L1+526(fp),#1
	jpr	L391
L390:
L388:
L346:
L10031:
	add	fp,#~L2
	ret
L10032:
	sub	fp,#~L2
	ld	~L1+0(fp),r7
	jpr	L10033

	~L1 := 4
	~L2 := 540

	}	/* _doxtract */

	.data

	.line	612	"tar.c"	
L402:
	.byte	%25,%73,%0
    .even
L404:
	.byte	%20,%6c,%69,%6e,%6b,%65,%64,%20
	.byte	%74,%6f,%20,%25,%73,%0
    .even
L405:
	.byte	%a,%0
    .even
// }
// 
// /* Lese von Archiv (Realisierung t-Option) */
// dotable()
// {
	.line	617	"tar.c"	
	.line	617	"tar.c"	

	.code

	_dotable::
	{

	jpr	L10035
L10036:
L397:
//     for (;;) {
// 	gethead();
	.line	619	"tar.c"	
	callr	_gethead
// 	if (endarchiv()) break;
	.line	620	"tar.c"	
	callr	_endarchiv
	cp	r2,#0
	jpr	eq,L398
	jpr	L396
L398:
// 	if (vflag) longt(&stbuf);
	.line	621	"tar.c"	
	cp	_vflag,#0
	jpr	eq,L399
	.line	621	"tar.c"	
	ld	r7,#_stbuf
	callr	_longt
L399:
// 	printf("%s", dblock.dbuf.name);
	.line	622	"tar.c"	
	ld	r7,#L402
	ld	r6,#_dblock
	callr	_printf
// 	if (dblock.dbuf.linkflag == '1')
	.line	623	"tar.c"	
	cpb	_dblock+156,#49
	jpr	ne,L403
// 	    printf(" linked to %s", dblock.dbuf.linkname);
	.line	624	"tar.c"	
	ld	r7,#L404
	ld	r6,#_dblock+157
	callr	_printf
L403:
// 	printf("\n");
	.line	625	"tar.c"	
	ld	r7,#L405
	callr	_printf
// 	passarchiv();
	.line	626	"tar.c"	
	callr	_passarchiv
L395:
	jpr	L397
L396:
L394:
L10034:
	ret
L10035:
	jpr	L10036

	~L1 := 0
	~L2 := 0

	}	/* _dotable */

	.data

	.line	626	"tar.c"	
L408:
	.byte	%70,%75,%74,%65,%6d,%70,%74,%79
	.byte	%a,%0
    .even
//     }
// }
// 
// putempty()
// {
	.line	631	"tar.c"	
	.line	631	"tar.c"	

	.code

	_putempty::
	{

	jpr	L10038
L10039:
//     char buf[TBLOCK];
//     char *cp;
// 
// if (dflag >= 2) fprintf(stderr, "putempty\n");	/* TEST */
	.line	635	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L407
	.line	635	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L408
	callr	_fprintf
L407:
//     for (cp = buf; cp < &buf[TBLOCK]; )
	.line	636	"tar.c"	
	lda	r2,~L1(fp)
	ld	~L1+512(fp),r2
L411:
	.line	636	"tar.c"	
	lda	r2,~L1+512(fp)
	cp	r2,~L1+512(fp)
	jpr	ule,L410
// 	*cp++ = '\0';
	.line	637	"tar.c"	
	ld	r2,~L1+512(fp)
	clrb	@r2
	inc	~L1+512(fp),#1
L409:
	jpr	L411
L410:
//     writearchiv(buf);
	.line	638	"tar.c"	
	lda	r7,~L1(fp)
	callr	_writearchiv
L406:
L10037:
	add	fp,#~L2
	ret
L10038:
	sub	fp,#~L2
	jpr	L10039

	~L1 := 0
	~L2 := 514

	}	/* _putempty */

	.data

	.line	638	"tar.c"	
L415:
	.byte	%25,%33,%64,%2f,%25,%31,%64,%0
    .even
L416:
	.byte	%25,%37,%44,%0
    .even
L417:
	.byte	%20,%25,%2d,%31,%32,%2e,%31,%32
	.byte	%73,%20,%25,%2d,%34,%2e,%34,%73
	.byte	%20,%0
    .even
// }
// 
// /* v-Option fuer Terminalanzeige von: mode, uid, gid, size, ctime */
// longt(st)
// register struct stat *st;
// {
	.line	644	"tar.c"	
	.line	644	"tar.c"	

	.code

	_longt::
	{

	jpr	L10041
L10042:
	ld	r14,r7
//     register char *cp;
//     char *ctime();
// 
//     pmode(st);
	.line	648	"tar.c"	
	ld	r7,r14
	callr	_pmode
//     printf("%3d/%1d", st->st_uid, st->st_gid);
	.line	649	"tar.c"	
	ld	r7,#L415
	ld	r6,8(r14)
	ld	r5,10(r14)
	callr	_printf
//     printf("%7D", st->st_size);
	.line	650	"tar.c"	
	ld	r7,#L416
	ldl	rr4,14(r14)
	callr	_printf
//     cp = ctime(&st->st_mtime);
	.line	651	"tar.c"	
	lda	r7,22(r14)
	callr	_ctime
	ld	r13,r2
//     printf(" %-12.12s %-4.4s ", cp+4, cp+20);
	.line	652	"tar.c"	
	lda	r6,4(r13)
	lda	r5,20(r13)
	ld	r7,#L417
	callr	_printf
L412:
L10040:
	ldm	r13,2(fp),#2
	inc	fp,#~L2
	ret
L10041:
	dec	fp,#~L2
	ldm	2(fp),r13,#2
	jpr	L10042

	~L1 := 0
	~L2 := 6

	}	/* _longt */

	.data

	.line	652	"tar.c"	
    .even
_m1::
	.word	%1
	.word	%100
	.word	%72
	.word	%2d
    .even
_m2::
	.word	%1
	.word	%80
	.word	%77
	.word	%2d
    .even
_m3::
	.word	%2
	.word	%800
	.word	%73
	.word	%40
	.word	%78
	.word	%2d
    .even
_m4::
	.word	%1
	.word	%20
	.word	%72
	.word	%2d
    .even
_m5::
	.word	%1
	.word	%10
	.word	%77
	.word	%2d
    .even
_m6::
	.word	%2
	.word	%400
	.word	%73
	.word	%8
	.word	%78
	.word	%2d
    .even
_m7::
	.word	%1
	.word	%4
	.word	%72
	.word	%2d
    .even
_m8::
	.word	%1
	.word	%2
	.word	%77
	.word	%2d
    .even
_m9::
	.word	%2
	.word	%200
	.word	%74
	.word	%1
	.word	%78
	.word	%2d
    .even
_m::
	.addr	_m1
	.addr	_m2
	.addr	_m3
	.addr	_m4
	.addr	_m5
	.addr	_m6
	.addr	_m7
	.addr	_m8
	.addr	_m9
// }
// 
// #define	SUID	04000
// #define	SGID	02000
// #define	ROWN	0400
// #define	WOWN	0200
// #define	XOWN	0100
// #define	RGRP	040
// #define	WGRP	020
// #define	XGRP	010
// #define	ROTH	04
// #define	WOTH	02
// #define	XOTH	01
// #define	STXT	01000
// int	m1[] = { 1, ROWN, 'r', '-' };
// int	m2[] = { 1, WOWN, 'w', '-' };
// int	m3[] = { 2, SUID, 's', XOWN, 'x', '-' };
// int	m4[] = { 1, RGRP, 'r', '-' };
// int	m5[] = { 1, WGRP, 'w', '-' };
// int	m6[] = { 2, SGID, 's', XGRP, 'x', '-' };
// int	m7[] = { 1, ROTH, 'r', '-' };
// int	m8[] = { 1, WOTH, 'w', '-' };
// int	m9[] = { 2, STXT, 't', XOTH, 'x', '-' };
// 
// int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};
// 
// pmode(st)
// register struct stat *st;
// {
	.line	681	"tar.c"	
	.line	681	"tar.c"	

	.code

	_pmode::
	{

	jpr	L10044
L10045:
	ld	r14,r7
//     register int **mp;
// 
//     for (mp = &m[0]; mp < &m[9];)
	.line	684	"tar.c"	
	ld	r13,#_m
L431:
	.line	684	"tar.c"	
	cp	r13,#_m+18
	jpr	uge,L430
// 	select(*mp++, st);
	.line	685	"tar.c"	
	ld	r7,r13
	inc	r13,#2
	ld	r6,@r7
	ld	r7,r6
	ld	r6,r14
	callr	_select
L429:
	jpr	L431
L430:
L428:
L10043:
	ldm	r13,2(fp),#2
	inc	fp,#~L2
	ret
L10044:
	dec	fp,#~L2
	ldm	2(fp),r13,#2
	jpr	L10045

	~L1 := 0
	~L2 := 6

	}	/* _pmode */

	.data

	.line	685	"tar.c"	
L436:
	.byte	%25,%63,%0
    .even
// }
// 
// select(pairp, st)
// int *pairp;
// struct stat *st;
// {
	.line	691	"tar.c"	
	.line	691	"tar.c"	

	.code

	_select::
	{

	jpr	L10047
L10048:
//     register int n, *ap;
// 
//     ap = pairp;
	.line	694	"tar.c"	
	ld	r13,r7
//     n = *ap++;
	.line	695	"tar.c"	
	ld	r14,@r13
	inc	r13,#2
L434:
//     while (--n>=0 && (st->st_mode&*ap++)==0)
	.line	696	"tar.c"	
	dec	r14,#1
	cp	r14,#0
	jpr	lt,L435
	ld	r2,~L1(fp)
	ld	r3,r13
	inc	r13,#2
	ld	r4,@r3
	and	r4,4(r2)
	cp	r4,#0
	jpr	ne,L435
// 	ap++;
	.line	697	"tar.c"	
	inc	r13,#2
	jpr	L434
L435:
//     printf("%c", *ap);
	.line	698	"tar.c"	
	ld	r7,#L436
	ld	r6,@r13
	callr	_printf
L433:
L10046:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
L10047:
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ldl	@fp,rr6
	jpr	L10048

	~L1 := 0
	~L2 := 8

	}	/* _select */

	.data

	.line	698	"tar.c"	
L447:
	.byte	%2f,%62,%69,%6e,%2f,%6d,%6b,%64
	.byte	%69,%72,%0
    .even
L448:
	.byte	%6d,%6b,%64,%69,%72,%0
    .even
L449:
	.byte	%2f,%75,%73,%72,%2f,%62,%69,%6e
	.byte	%2f,%6d,%6b,%64,%69,%72,%0
    .even
L450:
	.byte	%6d,%6b,%64,%69,%72,%0
    .even
L451:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%66,%69,%6e,%64
	.byte	%20,%6d,%6b,%64,%69,%72,%21,%a
	.byte	%0
    .even
// }
// 
// checkdir(name)
// register char *name;
// {
	.line	703	"tar.c"	
	.line	703	"tar.c"	

	.code

	_checkdir::
	{

	jpr	L10050
L10051:
	ld	r14,r7
//     register char *cp;
//     int i;
//     for (cp = name; *cp; cp++) {
	.line	706	"tar.c"	
	ld	r13,r14
L440:
	.line	706	"tar.c"	
	cpb	@r13,#0
	jpr	eq,L439
// 	if (*cp == '/') {
	.line	707	"tar.c"	
	cpb	@r13,#47
	jpr	ne,L441
// 	    *cp = '\0';
	.line	708	"tar.c"	
	clrb	@r13
// 	    if (access(name, 01) < 0) {
	.line	709	"tar.c"	
	ld	r7,r14
	ldk	r6,#1
	callr	_access
	cp	r2,#0
	jpr	ge,L443
// 		if (fork() == 0) {
	.line	710	"tar.c"	
	callr	_fork
	cp	r2,#0
	jpr	ne,L445
// 		    execl("/bin/mkdir", "mkdir", name, 0);
	.line	711	"tar.c"	
	ld	r7,#L447
	ld	r6,#L448
	ld	r5,r14
	sub	r4,r4
	callr	_execl
// 		    execl("/usr/bin/mkdir", "mkdir", name, 0);
	.line	712	"tar.c"	
	ld	r7,#L449
	ld	r6,#L450
	ld	r5,r14
	sub	r4,r4
	callr	_execl
// 		    fprintf(stderr, "tar: cannot find mkdir!\n");
	.line	713	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L451
	callr	_fprintf
// 		    done(0);
	.line	714	"tar.c"	
	sub	r7,r7
	callr	_done
L445:
L453:
// 		}
// 		while (wait(&i) >= 0);
	.line	716	"tar.c"	
	lda	r7,~L1+2(fp)
	callr	_wait
	cp	r2,#0
	jpr	lt,L454
	jpr	L453
L454:
// 		chown(name, stbuf.st_uid, stbuf.st_gid);
	.line	717	"tar.c"	
	ld	r7,r14
	ld	r6,_stbuf+8
	ld	r5,_stbuf+10
	callr	_chown
L443:
// 	    }
// 	    *cp = '/';
	.line	719	"tar.c"	
	ldb	@r13,#47
L441:
L438:
// 	}
//     }
	.line	721	"tar.c"	
	inc	r13,#1
	jpr	L440
L439:
L437:
L10049:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
L10050:
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	jpr	L10051

	~L1 := 0
	~L2 := 8

	}	/* _checkdir */

	.data

	.line	721	"tar.c"	
// }
// 
// onintr()
// {
	.line	725	"tar.c"	
	.line	725	"tar.c"	

	.code

	_onintr::
	{

	jpr	L10053
L10054:
//     signal(SIGINT, SIG_IGN);
	.line	726	"tar.c"	
	ldk	r7,#2
	ldk	r6,#1
	callr	_signal
//     term++;
	.line	727	"tar.c"	
	inc	_term,#1
L455:
L10052:
	ret
L10053:
	jpr	L10054

	~L1 := 0
	~L2 := 0

	}	/* _onintr */

	.data

	.line	727	"tar.c"	
// }
// 
// onquit()
// {
	.line	731	"tar.c"	
	.line	731	"tar.c"	

	.code

	_onquit::
	{

	jpr	L10056
L10057:
//     signal(SIGQUIT, SIG_IGN);
	.line	732	"tar.c"	
	ldk	r7,#3
	ldk	r6,#1
	callr	_signal
//     term++;
	.line	733	"tar.c"	
	inc	_term,#1
L456:
L10055:
	ret
L10056:
	jpr	L10057

	~L1 := 0
	~L2 := 0

	}	/* _onquit */

	.data

	.line	733	"tar.c"	
// }
// 
// onhup()
// {
	.line	737	"tar.c"	
	.line	737	"tar.c"	

	.code

	_onhup::
	{

	jpr	L10059
L10060:
//     signal(SIGHUP, SIG_IGN);
	.line	738	"tar.c"	
	ldk	r7,#1
	ldk	r6,#1
	callr	_signal
//     term++;
	.line	739	"tar.c"	
	inc	_term,#1
L457:
L10058:
	ret
L10059:
	jpr	L10060

	~L1 := 0
	~L2 := 0

	}	/* _onhup */

	.data

	.line	739	"tar.c"	
// }
// 
// onterm()
// {
	.line	743	"tar.c"	
	.line	743	"tar.c"	

	.code

	_onterm::
	{

	jpr	L10062
L10063:
//     signal(SIGTERM, SIG_IGN);
	.line	744	"tar.c"	
	ldk	r7,#15
	ldk	r6,#1
	callr	_signal
//     term++;
	.line	745	"tar.c"	
	inc	_term,#1
L458:
L10061:
	ret
L10062:
	jpr	L10063

	~L1 := 0
	~L2 := 0

	}	/* _onterm */

	.data

	.line	745	"tar.c"	
L463:
	.byte	%25,%36,%6f,%20,%0
    .even
L464:
	.byte	%25,%36,%6f,%20,%0
    .even
L465:
	.byte	%25,%36,%6f,%20,%0
    .even
L466:
	.byte	%25,%31,%31,%6c,%6f,%20,%0
    .even
L467:
	.byte	%25,%31,%31,%6c,%6f,%20,%0
    .even
// }
// 
// tomodes(sp)
// register struct stat *sp;
// {
	.line	750	"tar.c"	
	.line	750	"tar.c"	

	.code

	_tomodes::
	{

	jpr	L10065
L10066:
	ld	r14,r7
//     register char *cp;
// 
//     for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
	.line	753	"tar.c"	
	ld	r13,#_dblock
L462:
	.line	753	"tar.c"	
	cp	r13,#_dblock+512
	jpr	uge,L461
// 	*cp = '\0';
	.line	754	"tar.c"	
	clrb	@r13
L460:
	.line	754	"tar.c"	
	inc	r13,#1
	jpr	L462
L461:
//     sprintf(dblock.dbuf.mode, "%6o ", sp->st_mode & 07777);
	.line	755	"tar.c"	
	ld	r5,4(r14)
	and	r5,#4095
	ld	r6,#L463
	ld	r7,#_dblock+100
	callr	_sprintf
//     sprintf(dblock.dbuf.uid, "%6o ", sp->st_uid);
	.line	756	"tar.c"	
	ld	r7,#_dblock+108
	ld	r6,#L464
	ld	r5,8(r14)
	callr	_sprintf
//     sprintf(dblock.dbuf.gid, "%6o ", sp->st_gid);
	.line	757	"tar.c"	
	ld	r7,#_dblock+116
	ld	r6,#L465
	ld	r5,10(r14)
	callr	_sprintf
//     sprintf(dblock.dbuf.size, "%11lo ", sp->st_size);
	.line	758	"tar.c"	
	ld	r7,#_dblock+124
	ld	r6,#L466
	ldl	rr4,14(r14)
	callr	_sprintf
//     sprintf(dblock.dbuf.mtime, "%11lo ", sp->st_mtime);
	.line	759	"tar.c"	
	ld	r7,#_dblock+136
	ld	r6,#L467
	ldl	rr4,22(r14)
	callr	_sprintf
L459:
L10064:
	ldm	r13,2(fp),#2
	inc	fp,#~L2
	ret
L10065:
	dec	fp,#~L2
	ldm	2(fp),r13,#2
	jpr	L10066

	~L1 := 0
	~L2 := 6

	}	/* _tomodes */

	.data

	.line	759	"tar.c"	
L476:
	.byte	%63,%68,%65,%63,%6b,%73,%75,%6d
	.byte	%20,%3d,%20,%25,%6f,%a,%0
    .even
// }
// 
// checksum()
// {
	.line	763	"tar.c"	
	.line	763	"tar.c"	

	.code

	_checksum::
	{

	jpr	L10068
L10069:
//     register i;
//     register char *cp;
// 
//     for (cp = dblock.dbuf.chksum; cp < &dblock.dbuf.chksum[sizeof(dblock.dbuf.chksum)]; cp++)
	.line	767	"tar.c"	
	ld	r13,#_dblock+148
L471:
	.line	767	"tar.c"	
	cp	r13,#_dblock+156
	jpr	uge,L470
// 	*cp = ' ';
	.line	768	"tar.c"	
	ldb	@r13,#32
L469:
	.line	768	"tar.c"	
	inc	r13,#1
	jpr	L471
L470:
//     i = 0;
	.line	769	"tar.c"	
	ldk	r14,#0
//     for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
	.line	770	"tar.c"	
	ld	r13,#_dblock
L474:
	.line	770	"tar.c"	
	cp	r13,#_dblock+512
	jpr	uge,L473
// 	i += *cp;
	.line	771	"tar.c"	
	ldb	rl2,@r13
	extsb	r2
	add	r14,r2
L472:
	.line	771	"tar.c"	
	inc	r13,#1
	jpr	L474
L473:
// if (dflag >= 2) fprintf(stderr, "checksum = %o\n",i);	/* TEST */
	.line	772	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L475
	.line	772	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L476
	ld	r5,r14
	callr	_fprintf
L475:
//     return(i);
	.line	773	"tar.c"	
	ld	r2,r14
	jpr	L468
L468:
L10067:
	ldm	r13,2(fp),#2
	inc	fp,#~L2
	ret
L10068:
	dec	fp,#~L2
	ldm	2(fp),r13,#2
	jpr	L10069

	~L1 := 0
	~L2 := 6

	}	/* _checksum */

	.data

	.line	773	"tar.c"	
L484:
	.byte	%25,%30,%32,%78,%0
    .even
L486:
	.byte	%22,%25,%63,%22,%0
    .even
L488:
	.byte	%20,%20,%20,%0
    .even
L489:
	.byte	%43,%68,%65,%63,%6b,%73,%75,%6d
	.byte	%6d,%6d,%65,%3a,%20,%67,%65,%6c
	.byte	%65,%73,%65,%6e,%20,%3d,%20,%25
	.byte	%6f,%20,%62,%65,%72,%65,%63,%68
	.byte	%6e,%65,%74,%20,%3d,%20,%25,%6f
	.byte	%a,%0
    .even
// }
// 
// /* Dump the header block in hex (dflag >= 2) */
// dumpblock()
// {
	.line	778	"tar.c"	
	.line	778	"tar.c"	

	.code

	_dumpblock::
	{

	jpr	L10071
L10072:
//     register i, n;
//     register char *cp;
// 
//     for (cp = dblock.dbuf.chksum; cp < &dblock.dbuf.chksum[sizeof(dblock.dbuf.chksum)]; cp++)
	.line	782	"tar.c"	
	ld	r12,#_dblock+148
L480:
	.line	782	"tar.c"	
	cp	r12,#_dblock+156
	jpr	uge,L479
// 	*cp = ' ';
	.line	783	"tar.c"	
	ldb	@r12,#32
L478:
	.line	783	"tar.c"	
	inc	r12,#1
	jpr	L480
L479:
//     n = 0;
	.line	784	"tar.c"	
	ldk	r13,#0
//     i = 0;
	.line	785	"tar.c"	
	ldk	r14,#0
//     for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++) {
	.line	786	"tar.c"	
	ld	r12,#_dblock
L483:
	.line	786	"tar.c"	
	cp	r12,#_dblock+512
	jpr	uge,L482
// 	i += *cp;
	.line	787	"tar.c"	
	ldb	rl2,@r12
	extsb	r2
	add	r14,r2
// 	n++;
	.line	788	"tar.c"	
	inc	r13,#1
// 	fprintf(stderr, "%02x", *cp & 0xff);
	.line	789	"tar.c"	
	ldb	rl5,@r12
	extsb	r5
	and	r5,#255
	ld	r6,#L484
	ld	r7,#__iob+16
	callr	_fprintf
// 	if (*cp > 037 && *cp < 0177)
	.line	790	"tar.c"	
	cpb	@r12,#31
	jpr	le,L485
	cpb	@r12,#127
	jpr	ge,L485
// 	    fprintf(stderr, "\"%c\"", *cp & 0xff);
	.line	791	"tar.c"	
	ldb	rl5,@r12
	extsb	r5
	and	r5,#255
	ld	r6,#L486
	ld	r7,#__iob+16
	callr	_fprintf
	jpr	L487
L485:
// 	else
// 	    fprintf(stderr, "   ");
	.line	793	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L488
	callr	_fprintf
L487:
L481:
//     }
	.line	794	"tar.c"	
	inc	r12,#1
	jpr	L483
L482:
//     fprintf(stderr, "Checksummme: gelesen = %o berechnet = %o\n", chksum, i);
	.line	795	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L489
	ld	r5,_chksum
	ld	r4,r14
	callr	_fprintf
L477:
L10070:
	ldm	r12,@fp,#3
	inc	fp,#~L2
	ret
L10071:
	dec	fp,#~L2
	ldm	@fp,r12,#3
	jpr	L10072

	~L1 := 0
	~L2 := 6

	}	/* _dumpblock */

	.data

	.line	795	"tar.c"	
L492:
	.byte	%25,%63,%20,%0
    .even
L494:
	.byte	%25,%73,%3a,%20,%0
    .even
// }
// 
// checkw(c, name)
// char *name;
// {
	.line	800	"tar.c"	
	.line	800	"tar.c"	

	.code

	_checkw::
	{

	jpr	L10074
L10075:
// char estring[100];
// register i;
// 
//     if (wflag) {
	.line	804	"tar.c"	
	cp	_wflag,#0
	jpr	eq,L491
// 	printf("%c ", c);
	.line	805	"tar.c"	
	ld	r7,#L492
	ld	r6,~L1+2(fp)
	callr	_printf
// 	if (vflag) longt(&stbuf);
	.line	806	"tar.c"	
	cp	_vflag,#0
	jpr	eq,L493
	.line	806	"tar.c"	
	ld	r7,#_stbuf
	callr	_longt
L493:
// 	printf("%s: ", name);
	.line	807	"tar.c"	
	ld	r7,#L494
	ld	r6,~L1(fp)
	callr	_printf
// 	gets(estring);
	.line	808	"tar.c"	
	lda	r7,~L1+4(fp)
	callr	_gets
// 	if (estring[0] == 'y') {
	.line	809	"tar.c"	
	cpb	~L1+4(fp),#121
	jpr	ne,L495
// 	    if (c == 'x' && estring[1] == ' ' && strlen(estring) > 2) {
	.line	810	"tar.c"	
	cp	~L1+2(fp),#120
	jpr	ne,L497
	cpb	~L1+5(fp),#32
	jpr	ne,L497
	lda	r7,~L1+4(fp)
	callr	_strlen
	cp	r2,#2
	jpr	le,L497
// 		for (i=1; estring[i] == ' '; i++) ;
	.line	811	"tar.c"	
	ldk	r14,#1
L500:
	.line	811	"tar.c"	
	lda	r2,~L1+4(fp)
	add	r2,r14
	cpb	@r2,#32
	jpr	ne,L499
L498:
	.line	811	"tar.c"	
	inc	r14,#1
	jpr	L500
L499:
L503:
// 		for ( ; estring[i] != '\0'; i++) *name++ = estring[i];
	.line	812	"tar.c"	
	lda	r2,~L1+4(fp)
	add	r2,r14
	cpb	@r2,#0
	jpr	eq,L502
	.line	812	"tar.c"	
	lda	r2,~L1+4(fp)
	add	r2,r14
	ld	r3,~L1(fp)
	ldb	rl4,@r2
	ldb	@r3,rl4
	inc	~L1(fp),#1
L501:
	.line	812	"tar.c"	
	inc	r14,#1
	jpr	L503
L502:
// 		*name = '\0'; }
	.line	813	"tar.c"	
	ld	r2,~L1(fp)
	clrb	@r2
L497:
// 	    return(1); }
	.line	814	"tar.c"	
	ldk	r2,#1
	jpr	L490
L495:
// 	else return(0); }
	.line	815	"tar.c"	
	sub	r2,r2
	jpr	L490
L491:
//     return(1);
	.line	816	"tar.c"	
	ldk	r2,#1
	jpr	L490
L490:
L10073:
	ld	r14,106(fp)
	add	fp,#~L2
	ret
L10074:
	sub	fp,#~L2
	ld	106(fp),r14
	ldl	@fp,rr6
	jpr	L10075

	~L1 := 0
	~L2 := 108

	}	/* _checkw */

	.data

	.line	816	"tar.c"	
// }
// 
// response()
// {
	.line	820	"tar.c"	
	.line	820	"tar.c"	

	.code

	_response::
	{

	jpr	L10077
L10078:
//     char c;
// 
//     c = getchar();
	.line	823	"tar.c"	
	dec	__iob+2,#1
	cp	__iob+2,#0
	jpr	ge,L10079
	ld	r7,#__iob
	callr	__filbuf
	jpr	L10080
L10079:
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10080:
	ldb	~L1+1(fp),rl2
//     if (c != '\n')
	.line	824	"tar.c"	
	cpb	~L1+1(fp),#10
	jpr	eq,L506
L507:
// 	while (getchar() != '\n');
	.line	825	"tar.c"	
	dec	__iob+2,#1
	cp	__iob+2,#0
	jpr	ge,L10081
	ld	r7,#__iob
	callr	__filbuf
	jpr	L10082
L10081:
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10082:
	cp	r2,#10
	jpr	eq,L508
	jpr	L507
L508:
	jpr	L509
L506:
//     else c = 'n';
	.line	826	"tar.c"	
	ldb	~L1+1(fp),#110
L509:
//     return(c);
	.line	827	"tar.c"	
	ldb	rl2,~L1+1(fp)
	extsb	r2
	jpr	L504
L504:
L10076:
	inc	fp,#~L2
	ret
L10077:
	dec	fp,#~L2
	jpr	L10078

	~L1 := 0
	~L2 := 4

	}	/* _response */

	.data

	.line	827	"tar.c"	
L519:
	.byte	%25,%73,%20,%25,%6c,%6f,%0
    .even
// }
// 
// checkupdate(arg)
// char	*arg;
// {
	.line	832	"tar.c"	
	.line	832	"tar.c"	

	.code

	_checkupdate::
	{

	jpr	L10084
L10085:
//     char name[100];
//     long	mtime;
//     daddr_t seekp;
//     daddr_t	lookup();
// 
//     rewind(tfile);
	.line	838	"tar.c"	
	ld	r7,_tfile
	callr	_rewind
L515:
//     for (;;) {
// 	if ((seekp = lookup(arg)) < 0)
	.line	840	"tar.c"	
	ld	r7,~L1(fp)
	callr	_lookup
	ldl	~L1+106(fp),rr2
	cpl	rr2,#0
	jpr	ge,L516
// 	    return(1);
	.line	841	"tar.c"	
	ldk	r2,#1
	jpr	L510
L516:
// 	fseek(tfile, seekp, 0);
	.line	842	"tar.c"	
	ld	r7,_tfile
	ldl	rr4,~L1+106(fp)
	sub	r3,r3
	callr	_fseek
// 	fscanf(tfile, "%s %lo", name, &mtime);
	.line	843	"tar.c"	
	lda	r5,~L1+2(fp)
	lda	r4,~L1+102(fp)
	ld	r7,_tfile
	ld	r6,#L519
	callr	_fscanf
// 	if (stbuf.st_mtime > mtime)
	.line	844	"tar.c"	
	ldl	rr2,_stbuf+22
	cpl	rr2,~L1+102(fp)
	jpr	le,L520
// 	    return(1);
	.line	845	"tar.c"	
	ldk	r2,#1
	jpr	L510
L520:
// 	else
// 	    return(0);
	.line	847	"tar.c"	
	sub	r2,r2
	jpr	L510
L513:
	jpr	L515
L514:
L510:
L10083:
	add	fp,#~L2
	ret
L10084:
	sub	fp,#~L2
	ld	@fp,r7
	jpr	L10085

	~L1 := 0
	~L2 := 112

	}	/* _checkupdate */

	.data

	.line	847	"tar.c"	
//     }
// }
// 
// done(n)
// {
	.line	852	"tar.c"	
	.line	852	"tar.c"	

	.code

	_done::
	{

	jpr	L10087
L10088:
//     unlink(tname);
	.line	853	"tar.c"	
	ld	r7,#_tname
	callr	_unlink
//     exit(n);
	.line	854	"tar.c"	
	ld	r7,~L1(fp)
	callr	_exit
L521:
L10086:
	inc	fp,#~L2
	ret
L10087:
	dec	fp,#~L2
	ld	@fp,r7
	jpr	L10088

	~L1 := 0
	~L2 := 2

	}	/* _done */

	.data

	.line	854	"tar.c"	
// }
// 
// prefix(s1, s2)
// register char *s1, *s2;
// {
	.line	859	"tar.c"	
	.line	859	"tar.c"	

	.code

	_prefix::
	{

	jpr	L10090
L10091:
	ld	r13,r6
	ld	r14,r7
L524:
//     while (*s1)
	.line	860	"tar.c"	
	cpb	@r14,#0
	jpr	eq,L525
// 	if (*s1++ != *s2++)
	.line	861	"tar.c"	
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	ld	r2,r13
	inc	r13,#1
	ldb	rl4,@r2
	cpb	rl3,rl4
	jpr	eq,L526
// 	    return(0);
	.line	862	"tar.c"	
	sub	r2,r2
	jpr	L523
L526:
	jpr	L524
L525:
//     if (*s2)
	.line	863	"tar.c"	
	cpb	@r13,#0
	jpr	eq,L527
// 	return(*s2 == '/');
	.line	864	"tar.c"	
	cpb	@r13,#47
	jpr	ne,L10092
L10092:
	ldk	r2,#0
	tcc	eq,r2
	jpr	L523
L527:
//     return(1);
	.line	865	"tar.c"	
	ldk	r2,#1
	jpr	L523
L523:
L10089:
	ldm	r13,6(fp),#2
	inc	fp,#~L2
	ret
L10090:
	dec	fp,#~L2
	ldm	6(fp),r13,#2
	jpr	L10091

	~L1 := 0
	~L2 := 10

	}	/* _prefix */

	.data

	.line	865	"tar.c"	
L531:
	.byte	%2f,%62,%69,%6e,%2f,%70,%77,%64
	.byte	%0
    .even
L532:
	.byte	%70,%77,%64,%0
    .even
L533:
	.byte	%2f,%75,%73,%72,%2f,%62,%69,%6e
	.byte	%2f,%70,%77,%64,%0
    .even
L534:
	.byte	%70,%77,%64,%0
    .even
L535:
	.byte	%70,%77,%64,%20,%66,%61,%69,%6c
	.byte	%65,%64,%21,%a,%0
    .even
L536:
	.byte	%2f,%a,%0
    .even
// }
// 
// getwdir(s)
// char *s;
// {
	.line	870	"tar.c"	
	.line	870	"tar.c"	

	.code

	_getwdir::
	{

	jpr	L10094
L10095:
//     int	pipdes[2];
// 
//     pipe(pipdes);
	.line	873	"tar.c"	
	lda	r7,~L1+2(fp)
	callr	_pipe
//     if (fork() == 0) {
	.line	874	"tar.c"	
	callr	_fork
	cp	r2,#0
	jpr	ne,L530
// 	close(1);
	.line	875	"tar.c"	
	ldk	r7,#1
	callr	_close
// 	dup(pipdes[1]);
	.line	876	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_dup
// 	execl("/bin/pwd", "pwd", 0);
	.line	877	"tar.c"	
	ld	r7,#L531
	ld	r6,#L532
	sub	r5,r5
	callr	_execl
// 	execl("/usr/bin/pwd", "pwd", 0);
	.line	878	"tar.c"	
	ld	r7,#L533
	ld	r6,#L534
	sub	r5,r5
	callr	_execl
// 	fprintf(stderr, "pwd failed!\n");
	.line	879	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L535
	callr	_fprintf
// 	printf("/\n");
	.line	880	"tar.c"	
	ld	r7,#L536
	callr	_printf
// 	exit(1);
	.line	881	"tar.c"	
	ldk	r7,#1
	callr	_exit
L530:
L537:
//     }
//     while (wait((int *)NULL) != -1)
	.line	883	"tar.c"	
	sub	r7,r7
	callr	_wait
	cp	r2,#-1
	jpr	eq,L538
	jpr	L537
L538:
// 	;
//     read(pipdes[0], s, 50);
	.line	885	"tar.c"	
	ld	r7,~L1+2(fp)
	ld	r6,~L1(fp)
	ld	r5,#50
	callr	_read
L539:
//     while(*s != '\n')
	.line	886	"tar.c"	
	ld	r2,~L1(fp)
	cpb	@r2,#10
	jpr	eq,L540
// 	s++;
	.line	887	"tar.c"	
	inc	~L1(fp),#1
	jpr	L539
L540:
//     *s = '\0';
	.line	888	"tar.c"	
	ld	r2,~L1(fp)
	clrb	@r2
//     close(pipdes[0]);
	.line	889	"tar.c"	
	ld	r7,~L1+2(fp)
	callr	_close
//     close(pipdes[1]);
	.line	890	"tar.c"	
	ld	r7,~L1+4(fp)
	callr	_close
L528:
L10093:
	inc	fp,#~L2
	ret
L10094:
	dec	fp,#~L2
	ld	@fp,r7
	jpr	L10095

	~L1 := 0
	~L2 := 6

	}	/* _getwdir */

	.data

	.line	890	"tar.c"	
.comm	2,	_njab;
// }
// 
// #define	N	200
// int	njab;
// daddr_t
// lookup(s)
// char *s;
// {
	.line	898	"tar.c"	
	.line	898	"tar.c"	

	.code

	_lookup::
	{

	jpr	L10097
L10098:
//     register i;
//     daddr_t a;
// 
//     for(i=0; s[i]; i++)
	.line	902	"tar.c"	
	ldk	r14,#0
L545:
	.line	902	"tar.c"	
	ld	r2,~L1(fp)
	add	r2,r14
	cpb	@r2,#0
	jpr	eq,L544
// 	if(s[i] == ' ')
	.line	903	"tar.c"	
	ld	r2,~L1(fp)
	add	r2,r14
	cpb	@r2,#32
	jpr	ne,L546
	jpr	L544
L546:
L543:
// 	    break;
//     a = bsrch(s, i, low, high);
	.line	905	"tar.c"	
	inc	r14,#1
	jpr	L545
L544:
	.line	905	"tar.c"	
	ld	r7,~L1(fp)
	ld	r6,r14
	ldl	rr4,_low
	ldl	rr2,_high
	callr	_bsrch
	ldl	~L1+2(fp),rr2
//     return(a);
	.line	906	"tar.c"	
	ldl	rr2,~L1+2(fp)
	jpr	L542
L542:
L10096:
	ld	r14,10(fp)
	inc	fp,#~L2
	ret
L10097:
	dec	fp,#~L2
	ld	10(fp),r14
	ld	@fp,r7
	jpr	L10098

	~L1 := 0
	~L2 := 12

	}	/* _lookup */

	.data

	.line	906	"tar.c"	
// }
// 
// daddr_t
// bsrch(s, n, l, h)
// daddr_t l, h;
// char *s;
// {
	.line	913	"tar.c"	
	.line	913	"tar.c"	

	.code

	_bsrch::
	{

	jpr	L10100
L10101:
//     register i, j;
//     char b[N];
//     daddr_t m, m1;
// 
//     njab = 0;
	.line	918	"tar.c"	
	clr	_njab
L548:
// 
// loop:
//     if(l >= h)
	.line	921	"tar.c"	
	ldl	rr2,~L1+4(fp)
	cpl	rr2,~L1(fp)
	jpr	lt,L549
// 	return(-1L);
	.line	922	"tar.c"	
	ldl	rr2,#-1
	jpr	L547
L549:
//     m = l + (h-l)/2 - N/2;
	.line	923	"tar.c"	
	ldl	rr6,~L1(fp)
	subl	rr6,~L1+4(fp)
	extsl	rq4
	divl	rq4,#2
	addl	rr6,~L1+4(fp)
	subl	rr6,#100
	ldl	~L1+212(fp),rr6
//     if(m < l)
	.line	924	"tar.c"	
	ldl	rr2,~L1+212(fp)
	cpl	rr2,~L1+4(fp)
	jpr	ge,L550
// 	m = l;
	.line	925	"tar.c"	
	ldl	rr4,~L1+4(fp)
	ldl	~L1+212(fp),rr4
L550:
//     fseek(tfile, m, 0);
	.line	926	"tar.c"	
	ld	r7,_tfile
	ldl	rr4,~L1+212(fp)
	sub	r3,r3
	callr	_fseek
//     fread(b, 1, N, tfile);
	.line	927	"tar.c"	
	lda	r7,~L1+12(fp)
	ldk	r6,#1
	ld	r5,#200
	ld	r4,_tfile
	callr	_fread
//     njab++;
	.line	928	"tar.c"	
	inc	_njab,#1
//     for(i=0; i<N; i++) {
	.line	929	"tar.c"	
	ldk	r14,#0
L554:
	.line	929	"tar.c"	
	cp	r14,#200
	jpr	ge,L553
// 	if(b[i] == '\n')
	.line	930	"tar.c"	
	lda	r2,~L1+12(fp)
	add	r2,r14
	cpb	@r2,#10
	jpr	ne,L555
	jpr	L553
L555:
// 	    break;
// 	m++;
	.line	932	"tar.c"	
	ldl	rr2,~L1+212(fp)
	addl	rr2,#1
	ldl	~L1+212(fp),rr2
L552:
//     }
	.line	933	"tar.c"	
	inc	r14,#1
	jpr	L554
L553:
//     if(m >= h)
	.line	934	"tar.c"	
	ldl	rr2,~L1+212(fp)
	cpl	rr2,~L1(fp)
	jpr	lt,L556
// 	return(-1L);
	.line	935	"tar.c"	
	ldl	rr2,#-1
	jpr	L547
L556:
//     m1 = m;
	.line	936	"tar.c"	
	ldl	rr2,~L1+212(fp)
	ldl	~L1+216(fp),rr2
//     j = i;
	.line	937	"tar.c"	
	ld	r13,r14
//     for(i++; i<N; i++) {
	.line	938	"tar.c"	
	inc	r14,#1
L559:
	.line	938	"tar.c"	
	cp	r14,#200
	jpr	ge,L558
// 	m1++;
	.line	939	"tar.c"	
	ldl	rr2,~L1+216(fp)
	addl	rr2,#1
	ldl	~L1+216(fp),rr2
// 	if(b[i] == '\n')
	.line	940	"tar.c"	
	lda	r2,~L1+12(fp)
	add	r2,r14
	cpb	@r2,#10
	jpr	ne,L560
	jpr	L558
L560:
L557:
// 	    break;
//     }
	.line	942	"tar.c"	
	inc	r14,#1
	jpr	L559
L558:
//     i = cmp(b+j, s, n);
	.line	943	"tar.c"	
	lda	r7,~L1+12(fp)
	add	r7,r13
	ld	r6,~L1+10(fp)
	ld	r5,~L1+8(fp)
	callr	_cmp
	ld	r14,r2
//     if(i < 0) {
	.line	944	"tar.c"	
	cp	r14,#0
	jpr	ge,L562
// 	h = m;
	.line	945	"tar.c"	
	ldl	rr2,~L1+212(fp)
	ldl	~L1(fp),rr2
	jpr	L548
L562:
// 	goto loop;
//     }
//     if(i > 0) {
	.line	948	"tar.c"	
	cp	r14,#0
	jpr	le,L563
// 	l = m1;
	.line	949	"tar.c"	
	ldl	rr2,~L1+216(fp)
	ldl	~L1+4(fp),rr2
	jpr	L548
L563:
// 	goto loop;
//     }
//     return(m);
	.line	952	"tar.c"	
	ldl	rr2,~L1+212(fp)
	jpr	L547
L547:
L10099:
	ldm	r13,224(fp),#2
	add	fp,#~L2
	ret
L10100:
	sub	fp,#~L2
	ldm	224(fp),r13,#2
	ldm	@fp,r2,#6
	jpr	L10101

	~L1 := 0
	~L2 := 228

	}	/* _bsrch */

	.data

	.line	952	"tar.c"	
// }
// 
// cmp(b, s, n)
// char *b, *s;
// {
	.line	957	"tar.c"	
	.line	957	"tar.c"	

	.code

	_cmp::
	{

	jpr	L10103
L10104:
//     register i;
// 
//     if(b[0] != '\n')
	.line	960	"tar.c"	
	ld	r4,r7
	cpb	@r4,#10
	jpr	eq,L565
// 	exit(2);
	.line	961	"tar.c"	
	ldk	r7,#2
	callr	_exit
L565:
//     for(i=0; i<n; i++) {
	.line	962	"tar.c"	
	ldk	r14,#0
L568:
	.line	962	"tar.c"	
	cp	r14,~L1(fp)
	jpr	ge,L567
// 	if(b[i+1] > s[i])
	.line	963	"tar.c"	
	lda	r2,1(r14)
	ld	r3,~L1+4(fp)
	add	r3,r2
	ldb	rl4,@r3
	ld	r3,~L1+2(fp)
	add	r3,r14
	ldb	rl5,@r3
	cpb	rl4,rl5
	jpr	le,L569
// 	    return(-1);
	.line	964	"tar.c"	
	ld	r2,#-1
	jpr	L564
L569:
// 	if(b[i+1] < s[i])
	.line	965	"tar.c"	
	lda	r2,1(r14)
	ld	r3,~L1+4(fp)
	add	r3,r2
	ldb	rl4,@r3
	ld	r3,~L1+2(fp)
	add	r3,r14
	ldb	rl5,@r3
	cpb	rl4,rl5
	jpr	ge,L570
// 	    return(1);
	.line	966	"tar.c"	
	ldk	r2,#1
	jpr	L564
L570:
L566:
//     }
	.line	967	"tar.c"	
	inc	r14,#1
	jpr	L568
L567:
//     return(b[i+1] == ' '? 0 : -1);
	.line	968	"tar.c"	
	lda	r2,1(r14)
	ld	r3,~L1+4(fp)
	add	r3,r2
	cpb	@r3,#32
	jpr	ne,L10105
	sub	r2,r2
	jpr	L10106
L10105:
	ld	r2,#-1
L10106:
	jpr	L564
L564:
L10102:
	ld	r14,8(fp)
	inc	fp,#~L2
	ret
L10103:
	dec	fp,#~L2
	ld	8(fp),r14
	ldm	@fp,r5,#3
	jpr	L10104

	~L1 := 0
	~L2 := 10

	}	/* _cmp */

	.data

	.line	968	"tar.c"	
L577:
	.byte	%72,%65,%61,%64,%61,%72,%63,%68
	.byte	%69,%76,%20,%72,%75,%66,%74,%20
	.byte	%64,%69,%73,%6b,%65,%6e,%64,%a
	.byte	%0
    .even
L579:
	.byte	%74,%61,%72,%3a,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%72,%65,%61,%64
	.byte	%20,%65,%72,%72,%6f,%72,%a,%0
    .even
L581:
	.byte	%74,%61,%72,%3a,%20,%63,%68,%61
	.byte	%6e,%67,%65,%20,%65,%72,%72,%6f
	.byte	%72,%2d,%64,%69,%73,%6b,%20,%3f
	.byte	%20,%28,%79,%2f,%6e,%29,%20,%3a
	.byte	%20,%0
    .even
L584:
	.byte	%74,%61,%72,%3a,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%72,%65,%61,%64
	.byte	%20,%65,%72,%72,%6f,%72,%20,%69
	.byte	%6e,%20,%68,%65,%61,%64,%2d,%62
	.byte	%6c,%6f,%63,%6b,%a,%0
    .even
L586:
	.byte	%74,%61,%72,%3a,%20,%66,%69,%6c
	.byte	%65,%20,%25,%73,%20,%69,%73,%20
	.byte	%6e,%6f,%74,%20,%63,%6f,%6d,%70
	.byte	%6c,%65,%74,%65,%a,%0
    .even
L587:
	.byte	%20,%20,%20,%20,%20,%63,%6f,%6e
	.byte	%74,%69,%6e,%75,%65,%20,%28,%79
	.byte	%2f,%6e,%29,%20,%3a,%20,%0
    .even
L591:
	.byte	%74,%61,%72,%3a,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%62,%6c,%6f,%63
	.byte	%6b,%73,%69,%7a,%65,%20,%65,%72
	.byte	%72,%6f,%72,%a,%0
    .even
L593:
	.byte	%74,%61,%72,%3a,%20,%43,%61,%6e
	.byte	%6e,%6f,%74,%20,%75,%70,%64,%61
	.byte	%74,%65,%20,%62,%6c,%6f,%63,%6b
	.byte	%65,%64,%20,%61,%72,%63,%68,%69
	.byte	%76,%20,%28,%79,%65,%74,%29,%a
	.byte	%0
    .even
L596:
	.byte	%69,%20,%3d,%20,%25,%64,%2c,%20
	.byte	%6e,%62,%6c,%6f,%63,%6b,%20,%3d
	.byte	%20,%25,%64,%a,%0
    .even
L597:
	.byte	%74,%61,%72,%3a,%20,%62,%6c,%6f
	.byte	%63,%6b,%73,%69,%7a,%65,%20,%3d
	.byte	%20,%25,%64,%a,%0
    .even
// }
// 
// readarchiv(buffer)
// char *buffer;
// {
	.line	973	"tar.c"	
	.line	973	"tar.c"	

	.code

	_readarchiv::
	{

	jpr	L10108
L10109:
//     int i, j;
// 
//     if (recno >= nblock || first == 0) {
	.line	976	"tar.c"	
	ld	r6,_recno
	cp	r6,_nblock
	jpr	ge,L10110
	cp	_first,#0
	jpr	ne,L572
L10110:
// 	if (first == 0 && nblock == 0) j = NBLOCK;
	.line	977	"tar.c"	
	cp	_first,#0
	jpr	ne,L573
	cp	_nblock,#0
	jpr	ne,L573
	.line	977	"tar.c"	
	ld	~L1+4(fp),#20
	jpr	L574
L573:
// 	else j = nblock;
	.line	978	"tar.c"	
	ld	r2,_nblock
	ld	~L1+4(fp),r2
L574:
L575:
// readblock:
// if (dflag >= 2) fprintf(stderr, "readarchiv ruft diskend\n");
	.line	980	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L576
	.line	980	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L577
	callr	_fprintf
L576:
// 	diskend();			/* NEU */
	.line	981	"tar.c"	
	callr	_diskend
// 	if ((i = read(fd, tbuf, TBLOCK*j)) < 0) {
	.line	982	"tar.c"	
	ld	r5,~L1+4(fp)
	sla	r5,#9
	ld	r6,#_tbuf
	ld	r7,_fd
	callr	_read
	ld	~L1+2(fp),r2
	cp	r2,#0
	jpr	ge,L578
// 	    fprintf(stderr, "tar: archiv read error\n");
	.line	983	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L579
	callr	_fprintf
// 	    if (BLOCKS && Blocks == nblock) {
	.line	984	"tar.c"	
	cp	_BLOCKS,#0
	jpr	eq,L580
	ld	r2,_Blocks
	cp	r2,_nblock
	jpr	ne,L580
// 		fprintf(stderr, "tar: change error-disk ? (y/n) : ");
	.line	985	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L581
	callr	_fprintf
// 		if (response() == 'y') {
	.line	986	"tar.c"	
	callr	_response
	cp	r2,#121
	jpr	ne,L582
// 		    Blocks = 0;
	.line	987	"tar.c"	
	clr	_Blocks
	jpr	L575
L582:
L580:
// 		    goto readblock; } }
// 	    if (rhead) { rhead++;	/* Fehlermeldung an 'gethead' */
	.line	989	"tar.c"	
	cp	_rhead,#0
	jpr	eq,L583
	.line	989	"tar.c"	
	inc	_rhead,#1
// 		fprintf(stderr, "tar: archiv read error in head-block\n"); }
	.line	990	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L584
	callr	_fprintf
L583:
// 	    if (!tflag)
	.line	991	"tar.c"	
	cp	_tflag,#0
	jpr	ne,L585
// 		fprintf(stderr, "tar: file %s is not complete\n", dblock.dbuf.name);
	.line	992	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L586
	ld	r5,#_dblock
	callr	_fprintf
L585:
// 	    fprintf(stderr, "     continue (y/n) : ");
	.line	993	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L587
	callr	_fprintf
// 	    if (response() != 'y') done(3);
	.line	994	"tar.c"	
	callr	_response
	cp	r2,#121
	jpr	eq,L588
	.line	994	"tar.c"	
	ldk	r7,#3
	callr	_done
L588:
// 	    lseek(fd, (long)(TBLOCK*j), 1); }		/*? *j ?*/
	.line	995	"tar.c"	
	ld	r4,~L1+4(fp)
	sla	r4,#9
	ld	r3,r4
	exts	rr2
	ldl	rr4,rr2
	ld	r7,_fd
	ldk	r3,#1
	callr	_lseek
L578:
// 	if (first == 0) {
	.line	996	"tar.c"	
	cp	_first,#0
	jpr	ne,L589
// 	    if (i % TBLOCK) {
	.line	997	"tar.c"	
	ld	r3,~L1+2(fp)
	exts	rr2
	div	rr2,#512
	cp	r2,#0
	jpr	eq,L590
// 		fprintf(stderr, "tar: archiv blocksize error\n");
	.line	998	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L591
	callr	_fprintf
// 		done(3);
	.line	999	"tar.c"	
	ldk	r7,#3
	callr	_done
L590:
// 	    }
// 	    i /= TBLOCK;
	.line	1001	"tar.c"	
	ld	r3,~L1+2(fp)
	exts	rr2
	div	rr2,#512
	ld	~L1+2(fp),r3
// 	    if (rflag && i != 1) {
	.line	1002	"tar.c"	
	cp	_rflag,#0
	jpr	eq,L592
	cp	~L1+2(fp),#1
	jpr	eq,L592
// 		fprintf(stderr, "tar: Cannot update blocked archiv (yet)\n");
	.line	1003	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L593
	callr	_fprintf
// 		done(4);
	.line	1004	"tar.c"	
	ldk	r7,#4
	callr	_done
L592:
// 	    }
// 	    if (i != nblock && i != 1) {
	.line	1006	"tar.c"	
	ld	r2,~L1+2(fp)
	cp	r2,_nblock
	jpr	eq,L594
	cp	~L1+2(fp),#1
	jpr	eq,L594
// if (dflag >= 2) fprintf(stderr, "i = %d, nblock = %d\n",i, nblock);
	.line	1007	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L595
	.line	1007	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L596
	ld	r5,~L1+2(fp)
	ld	r4,_nblock
	callr	_fprintf
L595:
// 		fprintf(stderr, "tar: blocksize = %d\n", i);
	.line	1008	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L597
	ld	r5,~L1+2(fp)
	callr	_fprintf
// 		nblock = i;
	.line	1009	"tar.c"	
	ld	r2,~L1+2(fp)
	ld	_nblock,r2
L594:
L589:
// 	    }
// 	}
// 	recno = 0;
	.line	1012	"tar.c"	
	clr	_recno
L572:
//     }
//     first = 1;
	.line	1014	"tar.c"	
	ld	_first,#1
//     copy(buffer, &tbuf[recno++]);
	.line	1015	"tar.c"	
	ld	r2,_recno
	inc	_recno,#1
	sla	r2,#9
	add	r2,#_tbuf
	ld	r6,r2
	ld	r7,~L1(fp)
	callr	_copy
//     return(TBLOCK);
	.line	1016	"tar.c"	
	ld	r2,#512
	jpr	L571
L571:
L10107:
	inc	fp,#~L2
	ret
L10108:
	dec	fp,#~L2
	ld	@fp,r7
	jpr	L10109

	~L1 := 0
	~L2 := 8

	}	/* _readarchiv */

	.data

	.line	1016	"tar.c"	
L601:
	.byte	%77,%72,%69,%74,%65,%61,%72,%63
	.byte	%68,%69,%76,%3a,%20,%72,%65,%63
	.byte	%6e,%6f,%3d,%25,%64,%20,%6e,%62
	.byte	%6c,%6f,%63,%6b,%3d,%25,%64,%a
	.byte	%0
    .even
L605:
	.byte	%77,%72,%69,%74,%65,%61,%72,%63
	.byte	%68,%69,%76,%20,%72,%75,%66,%74
	.byte	%20,%64,%69,%73,%6b,%65,%6e,%64
	.byte	%20,%28,%31,%29,%a,%0
    .even
L607:
	.byte	%74,%61,%72,%3a,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%77,%72,%69,%74
	.byte	%65,%20,%65,%72,%72,%6f,%72,%a
	.byte	%0
    .even
L611:
	.byte	%77,%72,%69,%74,%65,%61,%72,%63
	.byte	%68,%69,%76,%20,%72,%75,%66,%74
	.byte	%20,%64,%69,%73,%6b,%65,%6e,%64
	.byte	%20,%28,%32,%29,%a,%0
    .even
L613:
	.byte	%74,%61,%72,%3a,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%77,%72,%69,%74
	.byte	%65,%20,%65,%72,%72,%6f,%72,%a
	.byte	%0
    .even
L615:
	.byte	%74,%61,%72,%3a,%20,%66,%6f,%72
	.byte	%6d,%61,%74,%20,%65,%72,%72,%6f
	.byte	%72,%2d,%64,%69,%73,%6b,%20,%3f
	.byte	%20,%28,%79,%2f,%6e,%29,%20,%3a
	.byte	%20,%0
    .even
L617:
	.byte	%20,%20,%20,%20,%20,%0
    .even
L618:
	.byte	%66,%6f,%72,%6d,%61,%74,%20,%25
	.byte	%73,%0
    .even
L621:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%6f,%70,%65,%6e
	.byte	%20,%25,%73,%a,%0
    .even
L622:
	.byte	%74,%61,%72,%3a,%20,%63,%68,%61
	.byte	%6e,%67,%65,%20,%65,%72,%72,%6f
	.byte	%72,%2d,%64,%69,%73,%6b,%20,%3f
	.byte	%20,%28,%79,%2f,%6e,%29,%20,%3a
	.byte	%20,%0
    .even
// }
// 
// writearchiv(buffer)
// char *buffer;
// {
	.line	1021	"tar.c"	
	.line	1021	"tar.c"	

	.code

	_writearchiv::
	{

	jpr	L10112
L10113:
//     char cmd[98];
// 
//     first = 1;
	.line	1024	"tar.c"	
	ld	_first,#1
// if (dflag >= 2) fprintf(stderr, "writearchiv: recno=%d nblock=%d\n",recno, nblock);
	.line	1025	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L600
	.line	1025	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L601
	ld	r5,_recno
	ld	r4,_nblock
	callr	_fprintf
L600:
//     if (nblock == 0)
	.line	1026	"tar.c"	
	cp	_nblock,#0
	jpr	ne,L602
// 	nblock = 1;
	.line	1027	"tar.c"	
	ld	_nblock,#1
L602:
//     if (recno >= nblock) {
	.line	1028	"tar.c"	
	ld	r2,_recno
	cp	r2,_nblock
	jpr	lt,L603
// if (dflag >= 2) fprintf(stderr, "writearchiv ruft diskend (1)\n");
	.line	1029	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L604
	.line	1029	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L605
	callr	_fprintf
L604:
// 	diskend();			/* NEU */
	.line	1030	"tar.c"	
	callr	_diskend
// 	if (write(fd, tbuf, TBLOCK*nblock) < 0) {
	.line	1031	"tar.c"	
	ld	r5,_nblock
	sla	r5,#9
	ld	r6,#_tbuf
	ld	r7,_fd
	callr	_write
	cp	r2,#0
	jpr	ge,L606
// 	    fprintf(stderr, "tar: archiv write error\n");
	.line	1032	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L607
	callr	_fprintf
// 	    done(2);
	.line	1033	"tar.c"	
	ldk	r7,#2
	callr	_done
L606:
// 	}
// 	recno = 0;
	.line	1035	"tar.c"	
	clr	_recno
L603:
//     }
//     copy(&tbuf[recno++], buffer);
	.line	1037	"tar.c"	
	ld	r2,_recno
	inc	_recno,#1
	sla	r2,#9
	add	r2,#_tbuf
	ld	r7,r2
	ld	r6,~L1(fp)
	callr	_copy
//     if (recno >= nblock) {
	.line	1038	"tar.c"	
	ld	r2,_recno
	cp	r2,_nblock
	jpr	lt,L608
L609:
// writeblock:
// if (dflag >= 2) fprintf(stderr, "writearchiv ruft diskend (2)\n");
	.line	1040	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L610
	.line	1040	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L611
	callr	_fprintf
L610:
// 	diskend();			/* NEU */
	.line	1041	"tar.c"	
	callr	_diskend
// 	if (write(fd, tbuf, TBLOCK*nblock) < 0) {
	.line	1042	"tar.c"	
	ld	r5,_nblock
	sla	r5,#9
	ld	r6,#_tbuf
	ld	r7,_fd
	callr	_write
	cp	r2,#0
	jpr	ge,L612
// 	    fprintf(stderr, "tar: archiv write error\n");
	.line	1043	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L613
	callr	_fprintf
// 	    if (BLOCKS && Blocks == nblock) {
	.line	1044	"tar.c"	
	cp	_BLOCKS,#0
	jpr	eq,L614
	ld	r2,_Blocks
	cp	r2,_nblock
	jpr	ne,L614
// 		fprintf(stderr, "tar: format error-disk ? (y/n) : ");
	.line	1045	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L615
	callr	_fprintf
// 		if (response() == 'y') {
	.line	1046	"tar.c"	
	callr	_response
	cp	r2,#121
	jpr	ne,L616
// 		    close(fd);
	.line	1047	"tar.c"	
	ld	r7,_fd
	callr	_close
// 		    fprintf(stderr, "     ", usefile);
	.line	1048	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L617
	ld	r5,_usefile
	callr	_fprintf
// 		    sprintf(cmd, "format %s", usefile);
	.line	1049	"tar.c"	
	lda	r7,~L1+2(fp)
	ld	r6,#L618
	ld	r5,_usefile
	callr	_sprintf
// 		    system(cmd);
	.line	1050	"tar.c"	
	lda	r7,~L1+2(fp)
	callr	_system
// 		    if ((fd = open(usefile, 2)) < 0) {
	.line	1051	"tar.c"	
	ld	r7,_usefile
	ldk	r6,#2
	callr	_open
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L619
// 			if (cflag == 0 || (fd = creat(usefile, 0666)) < 0) {
	.line	1052	"tar.c"	
	cp	_cflag,#0
	jpr	eq,L10114
	ld	r7,_usefile
	ld	r6,#438
	callr	_creat
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L620
L10114:
// 			    fprintf(stderr, "tar: cannot open %s\n", usefile);
	.line	1053	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L621
	ld	r5,_usefile
	callr	_fprintf
// 			    done(1); } }
	.line	1054	"tar.c"	
	ldk	r7,#1
	callr	_done
L620:
L619:
// 		    Blocks = 0;
	.line	1055	"tar.c"	
	clr	_Blocks
	jpr	L609
L616:
// 		    goto writeblock; }
// 		fprintf(stderr, "tar: change error-disk ? (y/n) : ");
	.line	1057	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L622
	callr	_fprintf
// 		if (response() == 'y') {
	.line	1058	"tar.c"	
	callr	_response
	cp	r2,#121
	jpr	ne,L623
// 		    Blocks = 0;
	.line	1059	"tar.c"	
	clr	_Blocks
	jpr	L609
L623:
L614:
// 		    goto writeblock; } }
// 	    done(2);
	.line	1061	"tar.c"	
	ldk	r7,#2
	callr	_done
L612:
// 	}
// 	recno = 0;
	.line	1063	"tar.c"	
	clr	_recno
L608:
//     }
//     return(TBLOCK);
	.line	1065	"tar.c"	
	ld	r2,#512
	jpr	L599
L599:
L10111:
	add	fp,#~L2
	ret
L10112:
	sub	fp,#~L2
	ld	@fp,r7
	jpr	L10113

	~L1 := 0
	~L2 := 102

	}	/* _writearchiv */

	.data

	.line	1065	"tar.c"	
L626:
	.byte	%62,%61,%63,%6b,%61,%72,%63,%68
	.byte	%69,%76,%a,%0
    .even
L627:
	.byte	%6c,%73,%65,%65,%6b,%3a,%20,%42
	.byte	%6c,%6f,%63,%6b,%73,%20,%3a,%3d
	.byte	%20,%42,%6c,%6f,%63,%6b,%73,%2d
	.byte	%6e,%62,%6c,%6f,%63,%6b,%20,%3d
	.byte	%20,%25,%64,%a,%0
    .even
L630:
	.byte	%62,%61,%63,%6b,%61,%72,%63,%68
	.byte	%69,%76,%20,%72,%75,%66,%74,%20
	.byte	%64,%69,%73,%6b,%65,%6e,%64,%a
	.byte	%0
    .even
L632:
	.byte	%74,%61,%72,%3a,%20,%61,%72,%63
	.byte	%68,%69,%76,%20,%72,%65,%61,%64
	.byte	%20,%65,%72,%72,%6f,%72,%20,%61
	.byte	%66,%74,%65,%72,%20,%73,%65,%65
	.byte	%6b,%a,%0
    .even
L634:
	.byte	%6c,%73,%65,%65,%6b,%3a,%20,%42
	.byte	%6c,%6f,%63,%6b,%73,%20,%3a,%3d
	.byte	%20,%42,%6c,%6f,%63,%6b,%73,%2d
	.byte	%6e,%62,%6c,%6f,%63,%6b,%20,%3d
	.byte	%20,%25,%64,%a,%0
    .even
// }
// 
// /* Wird nur beim Archiv-Lesen aufgerufen (Optionen: r,u,x,t) */
// backarchiv()
// {
	.line	1070	"tar.c"	
	.line	1070	"tar.c"	

	.code

	_backarchiv::
	{

	jpr	L10116
L10117:
//     Blocks -= nblock;
	.line	1071	"tar.c"	
	ld	r2,_Blocks
	sub	r2,_nblock
	ld	_Blocks,r2
// if (dflag >= 2) {					/* TEST */
	.line	1072	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L625
//     fprintf(stderr, "backarchiv\n");
	.line	1073	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L626
	callr	_fprintf
//     fprintf(stderr, "lseek: Blocks := Blocks-nblock = %d\n", Blocks); }
	.line	1074	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L627
	ld	r5,_Blocks
	callr	_fprintf
L625:
// 
//     lseek(fd, (long)(-TBLOCK*nblock), 1);
	.line	1076	"tar.c"	
	ld	r5,_nblock
	mult	rr4,#-512
	ld	r3,r5
	exts	rr2
	ldl	rr4,rr2
	ld	r7,_fd
	ldk	r3,#1
	callr	_lseek
//     if (recno >= nblock) {
	.line	1077	"tar.c"	
	ld	r2,_recno
	cp	r2,_nblock
	jpr	lt,L628
// 	recno = nblock - 1;
	.line	1078	"tar.c"	
	ld	r3,_nblock
	dec	r3,#1
	ld	_recno,r3
// if (dflag >= 2) fprintf(stderr, "backarchiv ruft diskend\n");
	.line	1079	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L629
	.line	1079	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L630
	callr	_fprintf
L629:
// 	diskend();			/* NEU */
	.line	1080	"tar.c"	
	callr	_diskend
// 	if (read(fd, tbuf, TBLOCK*nblock) < 0) {
	.line	1081	"tar.c"	
	ld	r5,_nblock
	sla	r5,#9
	ld	r6,#_tbuf
	ld	r7,_fd
	callr	_read
	cp	r2,#0
	jpr	ge,L631
// 	    fprintf(stderr, "tar: archiv read error after seek\n");
	.line	1082	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L632
	callr	_fprintf
// 	    done(4);
	.line	1083	"tar.c"	
	ldk	r7,#4
	callr	_done
L631:
// 	}
// 	Blocks -= nblock;
	.line	1085	"tar.c"	
	ld	r2,_Blocks
	sub	r2,_nblock
	ld	_Blocks,r2
// if (dflag >= 2)					/* TEST */
	.line	1086	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L633
//     fprintf(stderr, "lseek: Blocks := Blocks-nblock = %d\n", Blocks);
	.line	1087	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L634
	ld	r5,_Blocks
	callr	_fprintf
L633:
// if (BLOCKS && tflag)
	.line	1088	"tar.c"	
	cp	_BLOCKS,#0
	jpr	eq,L635
	cp	_tflag,#0
	jpr	eq,L635
//     printblocks(Blocks+recno+2);
	.line	1089	"tar.c"	
	ld	r7,_Blocks
	add	r7,_recno
	inc	r7,#2
	callr	_printblocks
L635:
// 	lseek(fd, (long)(-TBLOCK*nblock), 1);
	.line	1090	"tar.c"	
	ld	r5,_nblock
	mult	rr4,#-512
	ld	r3,r5
	exts	rr2
	ldl	rr4,rr2
	ld	r7,_fd
	ldk	r3,#1
	callr	_lseek
L628:
L624:
L10115:
	ret
L10116:
	jpr	L10117

	~L1 := 0
	~L2 := 0

	}	/* _backarchiv */

	.data

	.line	1090	"tar.c"	
//     }
// }
// 
// copy(to, from)
// register char *to, *from;
// {
	.line	1096	"tar.c"	
	.line	1096	"tar.c"	

	.code

	_copy::
	{

	jpr	L10119
L10120:
	ld	r13,r6
	ld	r14,r7
//     register i;
// 
//     i = TBLOCK;
	.line	1099	"tar.c"	
	ld	r12,#512
L639:
//     do {
// 	*to++ = *from++;
	.line	1101	"tar.c"	
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
L638:
//     } while (--i);
	.line	1102	"tar.c"	
	dec	r12,#1
	jpr	ne,L639
L637:
L636:
L10118:
	ldm	r12,4(fp),#3
	inc	fp,#~L2
	ret
L10119:
	dec	fp,#~L2
	ldm	4(fp),r12,#3
	jpr	L10120

	~L1 := 0
	~L2 := 10

	}	/* _copy */

	.data

	.line	1102	"tar.c"	
L644:
	.byte	%64,%69,%73,%6b,%65,%6e,%64,%3a
	.byte	%20,%42,%6c,%6f,%63,%6b,%73,%20
	.byte	%3d,%20,%25,%64,%a,%0
    .even
L646:
	.byte	%7,%a,%69,%6e,%73,%65,%72,%74
	.byte	%20,%70,%72,%65,%76,%69,%6f,%75
	.byte	%73,%20,%64,%69,%73,%6b,%20,%3a
	.byte	%20,%0
    .even
L651:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%6f,%70,%65,%6e
	.byte	%20,%25,%73,%a,%0
    .even
L656:
	.byte	%7,%a,%69,%6e,%73,%65,%72,%74
	.byte	%20,%6e,%65,%78,%74,%20,%64,%69
	.byte	%73,%6b,%20,%3a,%20,%0
    .even
L661:
	.byte	%74,%61,%72,%3a,%20,%63,%61,%6e
	.byte	%6e,%6f,%74,%20,%6f,%70,%65,%6e
	.byte	%20,%25,%73,%a,%0
    .even
// }
// 
// diskend()
// {
	.line	1106	"tar.c"	
	.line	1106	"tar.c"	

	.code

	_diskend::
	{

	jpr	L10122
L10123:
// #define SEC 2
// int	time_out();
// 
//     if (BLOCKS == 0) return;
	.line	1110	"tar.c"	
	cp	_BLOCKS,#0
	jpr	ne,L642
	jpr	L640
L642:
// if (dflag >= 2) fprintf(stderr, "diskend: Blocks = %d\n", Blocks+nblock);	/* TEST */
	.line	1111	"tar.c"	
	cp	_dflag,#2
	jpr	lt,L643
	.line	1111	"tar.c"	
	ld	r5,_Blocks
	add	r5,_nblock
	ld	r6,#L644
	ld	r7,#__iob+16
	callr	_fprintf
L643:
//     if ((Blocks += nblock) == 0) {
	.line	1112	"tar.c"	
	ld	r2,_Blocks
	add	r2,_nblock
	ld	_Blocks,r2
	cp	r2,#0
	jpr	ne,L645
// 	close(fd);
	.line	1113	"tar.c"	
	ld	r7,_fd
	callr	_close
// 	fprintf(stderr, "\007\ninsert previous disk : ");
	.line	1114	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L646
	callr	_fprintf
// 	disks--;
	.line	1115	"tar.c"	
	dec	_disks,#1
// 	Blocks = 1;	/* da 'diskend' mit Blocks += nblock beginnt */
	.line	1116	"tar.c"	
	ld	_Blocks,#1
L647:
// 	while (getchar() != '\n') ;
	.line	1117	"tar.c"	
	dec	__iob+2,#1
	cp	__iob+2,#0
	jpr	ge,L10124
	ld	r7,#__iob
	callr	__filbuf
	jpr	L10125
L10124:
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10125:
	cp	r2,#10
	jpr	eq,L648
	jpr	L647
L648:
// 	if ((fd = open(usefile, 2)) < 0) {
	.line	1118	"tar.c"	
	ld	r7,_usefile
	ldk	r6,#2
	callr	_open
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L649
// 	    if (cflag == 0 || (fd =  creat(usefile, 0666)) < 0) {
	.line	1119	"tar.c"	
	cp	_cflag,#0
	jpr	eq,L10126
	ld	r7,_usefile
	ld	r6,#438
	callr	_creat
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L650
L10126:
// 	        fprintf(stderr, "tar: cannot open %s\n", usefile);
	.line	1120	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L651
	ld	r5,_usefile
	callr	_fprintf
// 	        done(1); } } }
	.line	1121	"tar.c"	
	ldk	r7,#1
	callr	_done
L650:
L649:
L645:
//     if (Blocks > BLOCKS) {
	.line	1122	"tar.c"	
	ld	r2,_Blocks
	cp	r2,_BLOCKS
	jpr	le,L652
// 	close(fd);
	.line	1123	"tar.c"	
	ld	r7,_fd
	callr	_close
// 	timeout = 0;
	.line	1124	"tar.c"	
	clr	_timeout
// 	signal(SIGALRM,time_out);
	.line	1125	"tar.c"	
	ldk	r7,#14
	ld	r6,#_time_out
	callr	_signal
// 	alarm(SEC);
	.line	1126	"tar.c"	
	ldk	r7,#2
	callr	_alarm
L654:
// 	while (!timeout) getchar();
	.line	1127	"tar.c"	
	cp	_timeout,#0
	jpr	ne,L655
	.line	1127	"tar.c"	
	dec	__iob+2,#1
	cp	__iob+2,#0
	jpr	ge,L10127
	ld	r7,#__iob
	callr	__filbuf
	jpr	L10128
L10127:
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10128:
	jpr	L654
L655:
// 	alarm(0);
	.line	1128	"tar.c"	
	sub	r7,r7
	callr	_alarm
// 	fprintf(stderr, "\007\ninsert next disk : ");
	.line	1129	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L656
	callr	_fprintf
// 	disks++;
	.line	1130	"tar.c"	
	inc	_disks,#1
// 	Blocks = 1;
	.line	1131	"tar.c"	
	ld	_Blocks,#1
L657:
// 	while (getchar() != '\n') ;
	.line	1132	"tar.c"	
	dec	__iob+2,#1
	cp	__iob+2,#0
	jpr	ge,L10129
	ld	r7,#__iob
	callr	__filbuf
	jpr	L10130
L10129:
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10130:
	cp	r2,#10
	jpr	eq,L658
	jpr	L657
L658:
// 	if ((fd = open(usefile, 2)) < 0) {
	.line	1133	"tar.c"	
	ld	r7,_usefile
	ldk	r6,#2
	callr	_open
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L659
// 	    if (cflag == 0 || (fd =  creat(usefile, 0666)) < 0) {
	.line	1134	"tar.c"	
	cp	_cflag,#0
	jpr	eq,L10131
	ld	r7,_usefile
	ld	r6,#438
	callr	_creat
	ld	_fd,r2
	cp	r2,#0
	jpr	ge,L660
L10131:
// 	        fprintf(stderr, "tar: cannot open %s\n", usefile);
	.line	1135	"tar.c"	
	ld	r7,#__iob+16
	ld	r6,#L661
	ld	r5,_usefile
	callr	_fprintf
// 	        done(1); } } }
	.line	1136	"tar.c"	
	ldk	r7,#1
	callr	_done
L660:
L659:
L652:
L640:
L10121:
	ret
L10122:
	jpr	L10123

	~L1 := 0
	~L2 := 0

	}	/* _diskend */

	.data

	.line	1136	"tar.c"	
// }
// 
// time_out()
// {
	.line	1140	"tar.c"	
	.line	1140	"tar.c"	

	.code

	_time_out::
	{

	jpr	L10133
L10134:
//     signal(SIGALRM,time_out);
	.line	1141	"tar.c"	
	ldk	r7,#14
	ld	r6,#_time_out
	callr	_signal
//     timeout++;
	.line	1142	"tar.c"	
	inc	_timeout,#1
L662:
L10132:
	ret
L10133:
	jpr	L10134

	~L1 := 0
	~L2 := 0

	}	/* _time_out */

	.data

	.line	1142	"tar.c"	
	.line	1142	"tar.c"	
// }
