/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 2.1     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Termcap       */
/*                                                   */
/*       Stand: 14.01.89                             */
/*                                                   */
/*****************************************************/

#include "system"

#ifdef WEGA             				/* auf P8000 */
#define far                   /* bei WEGA gibt es keine far -Pointer */
#define TTYLINES  24
#define TTYCOLUMN 79
#define BUFFSIZE  64500
#include <fcntl.h>
#endif

#ifdef VENIX                                   /* auf Schneider -PC */
#define far                 /* bei VENIX gibt es keine far -Pointer */
#define TTYLINES  25
#define TTYCOLUMN 79
#define O_RDONLY  0
#define O_WRONLY  1
#define O_RDWR    2
#define BUFFSIZE  52000
#endif

#ifdef XENIX                                        /* auf IBM -PC */
#define far       /* bei XENIX kommt man auch ohne far -Pointer aus */
#define TTYLINES  25
#define TTYCOLUMN 79
#define BUFFSIZE  64500
#include <fcntl.h>
#endif


#define MKEYLEN   15
#define TTYBUFF   512
#define FNAMELEN  30       /* max. Laenge der verw. Filenamen   */
#define M_SCROLL  5        /* bei Rollen von mehr als M_SCROLL Zeilen wird der Bildschirm */
                           /* nicht gerollt, sondern neu aufgebaut, um Zeit zu sparen     */

/*   Modi fuer den Editor :    */
#define TEXTMODE  0
#define DUMPMODE  1
#define QUITMODE  2

/* Aktionszeichen in gotoXY -Zeichenkette */
#define GOTOINCR  255 /* x,y -Werte um 1 erhoehen */
#define GOTOCOUT  254 /* y bzw x -Wert um folgenden Wert erhoehen und als */
                      /*          Zeichen ausgeben                        */
#define GOTOSOUT  253 /* y bzw x -Wert als Zeichenfolge (2) ausgeben      */
#define GOTOEND   252 /* Ende der Zeichenfolge                            */


/*  folgende Vereinbahrungen duerfen nicht geaendert werden !!! */
#define FALSE 0
#define TRUE  1
#define CNTRL(X)  (X -'@')

#ifndef integer
#define integer unsigned int
#endif
#ifndef byte
#define byte unsigned char
#endif


#include <sgtty.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <errno.h>
#include "fsetab.h"
#include "fseerror.h"





/*
 * Konstanten fuer die Editorfunktionsauswahl
 * ******************************************
 */

#define CNTRL1     255
#define CNTRL2     254
#define ESC        253
#define LEFT       252
#define RIGHT      251
#define UP         250
#define DOWN       249
#define SHIFTKB    248
#define SHIFTKK    247
#define TAB        246
#define RETURN     245
#define NEWPAGE    244
#define WRDLEFT    243
#define WRDRIGHT   242
#define LINESTRT   241
#define LINEEND    240
#define PAGESTRT   239
#define PAGEEND    238
#define FILESTRT   237
#define FILEEND    236
#define POSITION   235
#define DELCHRL    234
#define DELCHRR    232
#define DELWRD     231
#define DELSOL     230
#define DELEOL     229
#define FORMAT     228
#define DELLINE    227
#define DELSPACE   226
#define FILESAVE   225
#define FILECSAV   224
#define FILEQUIT   223
#define FILECQUT   222
#define FILEREJC   221
#define SAVELOAD   220
#define SAVCLOAD   219
#define REJCLOAD   218
#define CNGFNAME   217
#define CHMOD      216
#define CHDIR      215
#define UNLINK     214
#define CHWIND     213
#define TEXTDUMP   212
#define ADDCH      211
#define CHEDMODE   210
#define MRKLN      209
#define MARKLINE   208
#define MARKWORD   207
#define BLKNOMRK   206
#define BLKCOPY    205
#define WNDCOPY    204
#define BLKREAD    203
#define BLKMOVE    202
#define BLKWRITE   201
#define BLKDEL     200
#define FIND       199
#define FINDOLD    198
#define REPLACE    197
#define HELP       196
#define AUTOTAB    195
#define BYTEMASK   194
#define CNTRMODE   193
#define RETAKE     192
#define BACKUP     191
#define VIEWDIR    190
#define LASTERR	   189
#define NEXTERR	   188
#define COMPILE    187
#define LEARN      186
#define LEARNHLP   185
#define BLKKB      184
#define BLKKK      183
#define EXCHLN     182
#define MARGIN     181
#define OPCODE     181    /* letzter verwendeter Code */

#define E_SYSTEM 0
#define E_TOOBIG 1
#define E_BADFND 2


#define backstep() ttych('\010')
#define clreol()   ttystr(parm._clreol)
#define insertch() ttystr(parm._ins_ch)

extern int		errno;
extern struct parms	parm;

typedef unsigned char keystr[MKEYLEN];
extern struct kbds
{ char   fname[FNAMELEN];
  long   strtline;
  short  longmem;
  keystr  k[26];
} kbd;

extern struct fcbs  {
  char     fname[FNAMELEN];
  short    y,minY,maxY,errfile;
  byte far *membegin, *memend,
           *bpos    , *fpos,
           *blkbegin, *blkend,
           *linestrt, *pagestrt,
           *oldfpos ,  *oldbpos;
  long     dumpstrt,
           oldlncnt,linecnt,mrkln,
           offset;
  short    oldmode,mode,
           autotab,
           insert,
           modifyed,
           cntrmode,
           dmpascii,
           linelen;
  byte     bytemask;
} fcb,fcb2;

extern	int	margin;
extern	long	errline;
extern	int	errpos,errfile;
extern	int	openfile;
extern	struct	errlist err[MAXERR];
extern	int	tty;
extern	char	fsememory[];
extern	long	strtline;
extern	byte	far * kbdp;
extern	int	kbdr;
extern	short	metakey;
extern	short	yz;
extern	short	restart;
extern	short	backup;
extern	short	copyr;
extern	short	klau;
extern	short	lfcr;
extern	byte	_display[];
extern	short	Qwindow,Qcol;
extern	short	X,Y;
extern	long	lndiff,_inscnt,_delcnt;
extern	short	_insy,_dely;
extern	short	BlkCol,helpmode,longmem;
extern	long	Col;
extern	long	nl_cnt,ff_cnt,cnt_opt;
extern	short	ask_opt,disp_opt,fw_opt,u_opt;
extern	short	ml,rl,ff,ffr;
extern	byte	muster[28];
extern	byte	repl[28];
extern	byte	txtihelp[];
extern	byte	txtohelp[];
extern	byte	dmphelp[];
extern	char	far *helptext[];
extern	byte	far *buff;
extern	byte	far *p;
extern	byte	far *np;
extern	byte	scratch[sizeof(fcb)];


/* fsetty.c */
extern	void	ttyflush();
extern	void	ttych();
extern	void	ttystr();
extern	void	clrscr();
extern	void	gotoXY();
extern	char	getc();
extern	char	getoc();
extern	char	getch();
extern	char	getletter();
extern	char	normch();
extern	byte	getkey();
extern	void	ttyinit();
extern	void	ttydrop();
extern	void	delins();
extern	void	nvideo();
extern	void	video();
extern	byte	numeral();
extern	void	hex2out();
extern	void	decout();
extern	void	bell();

/* fseglob.c */
extern	void	options();
extern	void	shift();
extern	void	helpln();
extern	short	letter();
extern	short	letter1();
extern	void	makewind();
extern	void	putwind();
extern	void	gotoWIND();
extern	void	getstr();
extern	void	ask();
extern	void	shortask();
extern	void	display();
extern	void	scrollup();
extern	void	scrolldw();
extern	void	scrdown();
extern	void	scrup();
extern	void	modscr();
extern	void	error();
extern	short	getfname();
extern	void	delspace();
extern	short	save();
extern	short	savefile();
extern	short	load();
extern	void	loadfile();
extern	short	reject();
extern	void	delwind();
extern	void	dline();
extern	void	aline();
extern	void	clrerr();


/* fsetxt1.c */
extern	byte	far *last_ln();
extern	byte	far *next_ln();
extern	void	count_ln();
extern	void	gotoLine();
extern	long	gotoByte();
extern	byte	far *atCol();
extern	long	getCol();
extern	void	i_tab();
extern	void	a_tab();
extern	void	newline();
extern	void	textline();
extern	void	textpage();
extern	void	buildtxt();
extern	void	newscr();
extern	void	buildscr();
extern	short	step();
extern	short	findfw();
extern	short	findbw();
extern	short	findfwu();
extern	short	findbwu();
extern	void	format();
extern	int	wwrap();

/* fsetxt2.c */
extern	void	text_ed();

/* fsedmp.c */
extern	void	dumpline();
extern	void	dumppage();
extern	void	builddmp();
extern	void	dump_ed();

/* fsemain.c */

/* allgemeine Routinen */
extern	byte	getkey(),chdir(),chmod();
extern	char	far *malloc();
extern	int	open(),close();
extern	integer	read();
extern	long	getCol();
extern	short	getfname();
extern	long	lseek();
extern	char	far *getenv();
extern	char	far *tgetstr();

