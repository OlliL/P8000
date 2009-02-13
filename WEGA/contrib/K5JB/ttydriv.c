/* Added redisplay of previous line with some limited line editing.  Made
 * changes to prevent array over-writes.  Note that echo() and noecho()
 * are only used by ftpcli.c during the password part of a login.  Elim-
 * inating them only reduced code size by 50 or so bytes.  The editing
 * costs 436 bytes and can be eliminated with an #undef EDIT.
 * 3/18/92 - K5JB
 */
/* TTY input driver */
/* #define CUTE_FTP in config.h to enable FTP login prompting */
#include <stdio.h>
#include <ctype.h>
#include "options.h"
#include "config.h"
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "timer.h"
#ifdef AX25
#include "ax25.h"
#include "lapb.h"
#endif
#ifdef NETROM
#include "nr4.h"
#endif
#include "iface.h"
#include "tcp.h"
#include "session.h"
#include "cmdparse.h"
#include "telnet.h"
#include "ftp.h"
#ifdef _FINGER
#include "finger.h"
#endif
#ifdef UNIX
#include "unixopt.h"
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define EDIT

int ttymode,cmdmode(),go();
#define	TTY_LIT	0		/* Send next char literally */
#define	TTY_RAW	1
#define TTY_COOKED	2

#ifdef CUTE_FTP
int ttyecho=1;
#define	TTY_NOECHO	0
#define	TTY_ECHO	1
#endif

#ifdef CUTE_VIDEO
extern unsigned int saved_attrib;
void putca();
#endif

#ifdef	FLOW
int ttyflow=1;
#endif

#define	KBLINSIZ	80

#ifdef EDIT
#define CTLE	5	/* redisplay last line */
#define CTLF	6	/* cursor forward one word */
#define CTLD	4  /* cursor forward one char */
#define CTLS	19 /* cursor backward (non-destructive) */
#endif

#define CTLA	1  /* cursor back one word */
#define CTLR	18	/* redisplay current line */
#define	CTLY	25 /* delete current line */
#define CTLU	21 /* ditto - backwards compat. */
#define	CTLV	22
#define	CTLW	23
#define	CTLZ	26
#define	RUBOUT	127

#ifdef  SYS5
extern int background;
#ifdef USE_QUIT
extern int reportquit_flag;
void report_quit();
#endif
#ifdef FORKSHELL
extern int shellpid;
#endif
#endif

#if defined(UNIX)
extern char escape;
#endif

extern int mode;
int Keyhit = -1;
int noprompt;	/* things to control prompts */

/* Process any keyboard input - removed from main() to make accessable
 * by other processes - K5JB
 */
void
check_kbd()
{
	char *ttybuf;
	int c;
	int16 cnt;
	int ttydriv(),cmdparse(),kbread();
#ifdef  FLOW
	extern int ttyflow;
#endif
	extern struct cmds cmds[];
	extern char prompt[];
#ifdef  SYS5
#ifdef USE_QUIT
	if(reportquit_flag)	/* set by quit signal and report is deferred */
		report_quit();		/* until now */
#endif
	/* note that if shellpid, kbread() always returns -1 */
	while((background == 0) && ((c = kbread()) != -1))
#else
	while((c = kbread()) != -1)
#endif
	{
#ifdef MSDOS
		/* c == -2 means the command escape key (F10) */
		Keyhit = c;	/* a global we use for haktc */
		if(c == -2){
			if(mode != CMD_MODE){
#ifdef CUTE_VIDEO
				putca('\n',saved_attrib); /* in case we were in session */
#else
				printf("\n");
#endif
				cmdmode();
			}
			continue;
		}
#endif
#if defined(UNIX) || defined(_OSK)
		if(c == escape && escape != 0){
			if(mode != CMD_MODE){
				printf("\n");
				cmdmode();
			}
			continue;
		}
#endif   /* UNIX or _OSK */

#ifndef FLOW
		if ((cnt = ttydriv(c, &ttybuf)) == 0)
			continue;
#else
		cnt = ttydriv(c, &ttybuf);
		if (ttyflow && (mode != CMD_MODE))
			go();           /* display pending chars */
		if (cnt == 0)
			continue;
#endif  /* FLOW */
		switch(mode){
			case CMD_MODE:
				(void)cmdparse(cmds,ttybuf);
				fflush(stdout);
			break;
			case CONV_MODE:
				if(current->parse != NULLFP)
					(*current->parse)(ttybuf,cnt);
			break;
		}
#if defined(FORKSHELL) && defined(SYS5)
		if(mode == CMD_MODE && !shellpid)
#else
		if(mode == CMD_MODE)
#endif
		{
			if(noprompt)	/* k35 */
				noprompt = 0;
			else{
				printf(prompt);
				fflush(stdout);
			}
		}
	}  /* while */
}

void
raw()
{
	ttymode = TTY_RAW;
}

void
cooked()
{
	ttymode = TTY_COOKED;
}

#ifdef CUTE_FTP
void
echo()
{
	ttyecho = TTY_ECHO;
}

void
noecho()
{
	ttyecho = TTY_NOECHO;
}
#endif

/* Accept characters from the incoming tty buffer and process them
 * (if in cooked mode) or just pass them directly (if in raw mode).
 * Returns the number of characters available for use; if non-zero,
 * also stashes a pointer to the character(s) in the "buf" argument.
 */
 /*Control-R added by df for retype of lines - useful in Telnet */
 /*Then df got impatient and added Control-W for erasing words  */
 /* Control-V for the literal-next function, slightly improved
  * flow control, local echo stuff -- hyc */

/* editing, put this in edit.hlp for online reminder:
Editing for commands
^E redisplay last line
^A backup one word
^S backup one char
^D fwd one char
^F fwd one word
Other editing:
^R redisplay current line buffer
^W erase word backward
Special characters:
^V next char literal
^U or ^Y kill line
^H or DEL destructive backspace
*/

/* moved to global scope so kaktc (session.c) could reset pointer */
char linebuf[KBLINSIZ];
char *kbcp = linebuf;

int
ttydriv(c,buf)
char c;
char **buf;
{
#ifdef EDIT
	static char lastline[KBLINSIZ];
	static char *ep = linebuf;
	static int editing;
	int i;
#endif
	int erase = FALSE;
	char *rp ;
	int cnt;
	extern int unix_line_mode;	/* k35c */

#ifdef EDIT
	if(*lastline == '\0')	/* only happens first time through */
		*lastline = '\015';
#endif

	if(buf == (char **)0)
		return 0;	/* paranoia check */

	cnt = 0;

	switch(ttymode){
		case TTY_LIT:
			ttymode = TTY_COOKED;	/* Reset to cooked mode */
			*kbcp++ = c;	/* run a slight risk of array violation here */
#ifdef UNIX
			putchar('.');	/* Terminal may prefer no Ctrl-chars */
#else
			putchar(c);	/* I know it isn't noxious on MS-DOS */
#endif
			break;
		case TTY_RAW:
			/* special problem with sending password when telnetting to a
			 * station that turns echo off.  We need to keep cnt = 0 until
			 * we finish line.  We must complete password without changing
			 * sessions, etc.  Not worrying about buffer overwrite, etc. k35c
			 */
			if(c == '\015' || c == '\012'){	/* k35a telnet password needs */
				printf("\n");						/* newline.  FTP cli doesn't */
				*kbcp++ = '\015';
				if(!unix_line_mode)
					*kbcp++ = '\012';
				cnt = (kbcp - linebuf);	/* now can tell world about it */
				kbcp = linebuf;
			}else
				*kbcp++ = c;
			break;
		case TTY_COOKED:
			/* Perform cooked-mode line editing */
			switch(c & 0x7f){
				case '\015':	/* Terminal may generate either */
				case '\012':
					*kbcp++ = '\015';
					*kbcp++ = '\012';	/* guaranteed to bust array! K5JB */
					printf("\n");
					cnt = kbcp - linebuf;
#ifdef EDIT
					for(i=0;i<KBLINSIZ;i++){   /* save in lastline */
						if(linebuf[i] == '\012')	/* will use '\r' as marker */
							break;
						lastline[i] = linebuf[i];
					}
					editing = FALSE;
					ep = linebuf;
#endif
               kbcp = linebuf;
					break;

				case CTLU:
				case CTLY:	/* Line kill - also active with no echo */
					erase = TRUE;	/* borrow an existing int */
#ifdef EDIT
				case CTLE:	/* redisplay last line */
					if(editing && !erase)
						break;
#endif
#ifdef CUTE_FTP
					if(!ttyecho)
						kbcp = linebuf;
					else
#endif
					while(kbcp != linebuf){
						kbcp--;
						printf("\b \b");
					}
					if(erase)
						break;
#ifdef EDIT
#ifdef CUTE_FTP
					if(!ttyecho) 	/* don't need to edit */
						break;				/* when no echo */
#endif
					kbcp = linebuf;
					for(i=0;i<KBLINSIZ;i++){
						if(lastline[i] == '\015')
							break;
						linebuf[i] = lastline[i];
						putchar(linebuf[i]);
						kbcp++;
					}
					if(i){
						ep = &linebuf[i];
						editing = TRUE;
					}
				break;

				case CTLD:	/* forward one char */
					if(editing && kbcp < ep){
						putchar(*kbcp++);
					}
					break;

				case CTLF:	/* forward one word */
					if(editing)
						while(kbcp < ep){
							putchar(*kbcp++);
							if(isspace(*kbcp)){ /* isspace() costs 258 bytes but it is*/
								if(kbcp < ep)		/* already used in smtpserv.c */
									putchar(*kbcp++);
								break;
							}
						}
					break;

				case CTLS:	/* backward one char */
					if (editing && kbcp != linebuf){
						printf("\b");
						kbcp--;
					}
					break;
#endif
				case RUBOUT:
				case '\b':		/* Backspace - note this works when no echo */
					if(kbcp != linebuf){
						kbcp--;
						printf("\b \b");	/* this isn't cool if no ttyecho, but */
					}
					break;
				case CTLR:	/* print line buffer */
#ifdef CUTE_FTP
					if(!ttyecho)
						break;
#endif
					printf("^R\n");
					rp = linebuf ;
					while (rp < kbcp)
						putchar(*rp++);
					break ;
				case CTLV:
					ttymode = TTY_LIT;
					break;
				case CTLW:	/* erase word backward */
					erase = TRUE;
				case CTLA:	/* move cursor back one word */
#ifdef CUTE_FTP
					if(!ttyecho)
						break;
#endif
					if(kbcp != linebuf && isspace(*(kbcp-1))){/* not at end of line */
						kbcp--;	/* back up onto the space */
						putchar('\b');
					}
					while (kbcp != linebuf) {
						kbcp--;
						if(erase)
							printf("\b \b") ;
						else
							printf("\b");
						if (isspace(*kbcp)) {
							putchar(*kbcp++);
							break;
						}
					}
				break ;

				default:	/* Ordinary character */
					*kbcp++ = c;

				/* ^Z is a common screen clear character - K5JB */
#ifdef CUTE_FTP
					if (ttyecho && (c != CTLZ))
#else
					if (c != CTLZ)
#endif
						putchar(c);

				/* if line is too long, we truncate it */
					if(kbcp > &linebuf[KBLINSIZ - 3]){	/* room for \r\n
														but NO null */
						linebuf[KBLINSIZ - 2] = '\015';
						linebuf[KBLINSIZ - 1] = '\012';
						cnt = KBLINSIZ;
						kbcp = linebuf;
					}
					break;
			}	/* switch ch in case TTY_COOKED */
	}	/* switch ttymode */
	if(cnt > 0)
		*buf = linebuf;
	else
		*buf = (char *)0;	/* K5JB */
#ifdef	FLOW
	if(kbcp > linebuf)
		ttyflow = 0;
	else
		ttyflow = 1;
#endif
	fflush(stdout);
	return cnt;
}
