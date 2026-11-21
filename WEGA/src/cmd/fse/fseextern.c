/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 2.1     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Termcap       */
/*                                                   */
/*       Stand: 14.01.89                             */
/*                                                   */
/*****************************************************/



/* #include fse.h  wird im Hauptmodul schon gemacht */





struct parms parm = {
0xe5,   /* mask= 0 ... alles bleibt erhalten     */
0xe5,   /* incr= 0 ... Maske bleibt unveraendert */
{ 1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0,0,0,0},  /* gotoXY  */
{ 0,0,0,0,0,0,0,0,0,0}    ,  /* clrscr  */
{ 0,0,0,0,0,0,0,0,0,0}    ,  /* clreol  */
{ 0,0,0,0,0,0,0,0,0,0}    ,  /* delline */
{ 0,0,0,0,0,0,0,0,0,0}    ,  /* insline */
{ 0,0,0,0,0,0,0,0,0,0}    ,  /* ins_ch  */
{ 0,0,0,0,0,0,0,0,0,0}    ,  /* video   */
{ 0,0,0,0,0,0,0,0,0,0}    ,  /* nvideo  */
0,      /* videomod == WEGA */
0,      /* clrwait          */
0,      /* gotowait         */
0L,     /* inodepos         */
2208,   /* Auslieferungsnummer (fse -ID) */

{  /* CONTROL */
/* A */    WRDLEFT,
/* B */    FORMAT,
/* C */    PAGEEND,
/* D */    RIGHT,
/* E */    UP,
/* F */    WRDRIGHT,
/* G */    DELCHRR,
#ifdef VENIX
/* H */    DELCHRL,
#else
/* H */    LEFT,
#endif
/* I */    TAB,
/* J */    DOWN,
/* K */    UP,
/* L */    RIGHT,
/* M */    RETURN,
/* N */    NEWPAGE,
/* O */    DELSOL,
/* P */    ADDCH,
/* Q */    CNTRL2,
/* R */    PAGESTRT,
/* S */    LEFT,
/* T */    DELWRD,
/* U */    CNTRL1,
/* V */    CHEDMODE,
/* W */    CHWIND,
/* X */    DOWN,
/* Y */    DELLINE,
/* Z */    TEXTDUMP
},
{   /*  CONTROL1  */
/* A */    CNGFNAME,
/* B */    BLKKB,
/* C */    BLKCOPY,
/* D */    FILEQUIT,
/* E */    COMPILE,
/* F */    WNDCOPY,
/* G */    MARKLINE,
/* H */    BLKNOMRK,
/* I */    MARGIN,
/* J */    MARKWORD,
/* K */    BLKKK,
/* L */    UNLINK,
/* M */    CHDIR,
/* N */    SAVELOAD,
/* O */    SAVCLOAD,
/* P */    REJCLOAD,
/* Q */    FILEREJC,
/* R */    BLKREAD,
/* S */    FILESAVE,
/* T */    CHMOD,
/* U */    FILECQUT,
/* V */    BLKMOVE,
/* W */    BLKWRITE,
/* X */    FILECSAV,
/* Y */    BLKDEL,
/* Z */    DELSPACE
},
{   /*  CONTROL2 */

/* A */    REPLACE,
/* B */    SHIFTKB,
/* C */    FILEEND,
/* D */    LINEEND,
/* E */    LEARN,
/* F */    FIND,
/* G */    COMPILE,
/* H */    HELP,
/* I */    AUTOTAB,
/* J */    LASTERR,
/* K */    SHIFTKK,
/* L */    RETAKE,
/* M */    BYTEMASK,
/* N */    NEXTERR,
/* O */    BACKUP,
/* P */    POSITION,
/* Q */    FINDOLD,
/* R */    FILESTRT,
/* S */    LINESTRT,
/* T */    LEARNHLP,
/* U */    EXCHLN,
/* V */    VIEWDIR,
/* W */    CHWIND,
/* X */    CNTRMODE,
/* Y */    DELEOL,
/* Z */    MRKLN
},


0,    /* inode     */
0x7f, /* bytemask  */
3,    /* cntrmode =3 Darstellung als ^C        */
0,    /* dmpascii =0 Dump als hex -Darstellung */
1,    /* autotab  =1 Autotab Standard          */
1,    /* Insertmodus ist Standard              */
"123456789",  /* Terminalvariable */

};

int	margin= 0; /* dh: keinen Randausgleich */
long	errline;
int	errpos, errfile;
struct	errlist err[MAXERR];
long	strtline;
short	metakey;
short	yz;
short	restart;
short	backup;
short	copyr;
short	klau;
short	lfcr;
byte	_display[TTYLINES];
short	Qwindow,Qcol;
short	X,Y;
long	lndiff,_inscnt,_delcnt;
short	_insy,_dely;
short	BlkCol,helpmode,longmem;
long	Col;
long	nl_cnt,ff_cnt,cnt_opt;
short	ask_opt,disp_opt,fw_opt,u_opt;
short	ml,rl,ff,ffr;
byte	muster[28];
byte	repl[28];
byte	far *buff;

byte txtihelp[]= "\021H help \032 dump \026 overwrite \021I autotab on/off \027 window \025D save,quit";

byte txtohelp[]="\021H help \032 dump \026 insert \021I autotab on/off\ \027 window \025D save,quit";

byte dmphelp[]= "\021H help \032 text \026 ascii/hex \021P position \027 window \025D save,quit";

char far *helptext[]= {
"\005 up\210\022 start of page\206\021S start of line\205\021P goto line",
"\023 left\206\003 end of page\210\021D end of line\207\021U exchange location",
"\030 down\206\021R start of file\205\001 word left\212\021B goto blkbegin",
"\004 right\205\021C end of file\207\006 word right\211\021K goto blkend",
"",
"\021E def\202metakey\204\021J goto last error\204\021Z save linenumber \027 change window",
"\021T help metakey\204\021N goto next error\204\011\202tabulator\207\032 dump/text",
"\025I set margin\206\002\202format paragraph\203\021I autotab on/off\202\016 new page",
"\021F find string\205\025L delete file\210\021L retake\211\021B backup",
"\021A replace\211\025M change directory\203\021V view current directory",
"\021Q repeat \021F\206\025T change filemode\204\026 insert/overwrite | ascii/hex",
"",
"\025A new filename\212\024\202del word\216\025B,\025K set blockmark",
"\025S save\222\017\202del start of line\205\025\H\205del blockmark",
"\025Q quit,not save\211\021Y del end of line\207\025C\205copy block",
"\025Z compress\250\025F\205copy from window",
"\025D save,quit\215\031\202del line\216\025Y\205del  block",
"\025X compr,save\212<DEL> del left\216\025V\205move block",
"\025U compr,save,quit\207\007\202del right\215\025R\205read block",
"\025P forget old,load new\203\020\202ins control\213\025W\205write block",
"\025N save,load\215\021M bytemask\216\025J\205mark word",
"\025E return to system\206\021X displaymode\213\025G\205mark line",
"",    /* zur Sicherheit fuer Bildschirme mit mehr Zeilen */
"","","","",""};

struct kbds kbd= {
"12345678901234567890123456789",
0L,   /* strtline */
0,    /* longmem  */
{ " ARRAY OF\001 \0\0\0",         /* a */
  "\015BEGIN\015  \0\0\0\0\0",    /* b */
  " : CARDINAL \0\0",             /* c */
  "DEFINITION \033m\0",           /* d */
  "\377\377END \0\0\0\0\0\0\0\0", /* e */
  "FROM IMPORT\001 \0",           /* f */
  "g\0\0\0\0\0\0\0\0\0\0\0\0\0",  /* g */
  "HALT;\015\0\0\0\0\0\0\0\0",    /* h */
  " : INTEGER \0\0\0",            /* i */
  "RECORD \0\0\0\0\0\0\0",        /* j */
  " : POINTER TO ",               /* k */
  "LOOP \0\0\0\0\0\0\0\0\0",      /* l */
  "MODULE \0\0\0\0\0\0\0",        /* m */
  " NOT \0\0\0\0\0\0\0\0\0",      /* n */
  " ORD(\0\0\0\0\0\0\0\0\0",      /* o */
  "PROCEDURE \0\0\0\0",           /* p */
  "QUALIFIED \0\0\0\0",           /* q */
  "REPEAT \0\0\0\0\0\0\0",        /* r */
  "SIZE( \0\0\0\0\0\0\0\0",       /* s */
  " THEN \0\0\0\0\0\0\0\0",       /* t */
  "UNTIL \0\0\0\0\0\0\0\0",       /* u */
  "VAR \0\0\0\0\0\0\0\0\0\0",     /* v */
  "WHILE \0\0\0\0\0\0\0\0",       /* w */
  "EXPORT \0\0\0\0\0\0\0",        /* x */ 
  " : BOOLEAN \0\0\0",            /* y */
  "NIL\0\0\0\0\0\0\0\0\0\0\0"     /* z */
}
};

byte far *kbdp;
int     kbdr;
int	openfile;
int	tty;
struct	fcbs  fcb,fcb2;
byte	far  *p, *np;
byte	scratch[sizeof(fcb)];
char	fsememory[]= ".fse";



