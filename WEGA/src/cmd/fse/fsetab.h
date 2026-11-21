/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 2.1     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Termcap       */
/*                                                   */
/*       Stand: 14.01.89                             */
/*                                                   */
/*****************************************************/



#ifndef byte
#define byte unsigned char
#endif


struct parms {
int	mask;		/* 							*/
int	incr;		/* 							*/
byte	_gotoXY[20];	/* 1,2,3,4,5,6,7                                        */
byte	_clrscr[10];	/* die Structur enthaelt 1-7 als erste Bytes		*/
byte	_clreol[10];	/* zur Kennung fuer fseinst				*/
byte	_delline[10];	/* fseinst traegt die Steuerzeichenfolgen		*/
byte	_insline[10];	/* entsprechend Termcap ein und verschluesselt die	*/
byte	_ins_ch[10];	/* gesamte Structur mit mask und incr (beide ^= 0xe5)	*/
byte	_video[10];
byte	_nvideo[10];
int	_vmod;		/* (videomode) 0 fuer P8000, 1 bei VENIX 		*/
int	clrwait;
int	gotowait;
long	inodepos;
int	auslfrno;	/* - 2207 ergibt die wahre				*/
byte	cntrl[26];
byte	cntrl1[26];
byte	cntrl2[26];
int	inode;
int	bytemask;
short	cntrmode;
short	dmpascii;
short	autotab;
short	insert;
char	TERM[10];
};

