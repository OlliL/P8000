/* Session control */
/* put all the machine dependent terminal i/o in here, term_recv(),
 * term_send(), and text_write().  K5JB k34
 */
#include <stdio.h>
#include <string.h>
#include "options.h"
#include "config.h"
/* W5GFE modified Unix version of this code to allow an incoming telnet
 * session to present in reversed video.  I added it so it also worked on
 * AX.25 and Netrom sessions.  Also made provisions for MS-DOS - K5JB
 */
#ifdef CUTE_VIDEO
#ifdef UNIX
#include <curses.h>
#include <term.h>
#else
#include <dos.h>
extern unsigned ourcute;
extern unsigned saved_attrib;
extern int htoi();
#endif
#endif

#include "global.h"
#include "sokname.h"
#include "mbuf.h"
#include "netuser.h"
#include "timer.h"
#include "tcp.h"
#ifdef AX25
#include "ax25.h"
#include "lapb.h"
#endif
#include "iface.h"
#include "ftp.h"
#ifdef _TELNET
#include "telnet.h"
#endif
#ifdef _FINGER
#include "finger.h"
#endif
#ifdef NETROM
#include "netrom.h"
#include "nr4.h"
#endif
#include "session.h"
#include "cmdparse.h"

struct session *sessions;
struct session *current;
char notval[] = "Not a valid control block\n";
char badsess[] = "Invalid session\n";
void free(),reset_tcp();

/* Convert a character string containing a decimal session index number
 * into a pointer. If the arg is NULLCHAR, use the current default session.
 * If the index is out of range or unused, return NULLSESSION.
 */
static struct session *
sessptr(cp)
char *cp;
{
	register struct session *s;
	unsigned int i;
	int atoi();

	if(cp == NULLCHAR){
		s = current;
	} else {
		if((i = (unsigned)atoi(cp)) >= nsessions)
			return NULLSESSION;
		s = &sessions[i];
	}
	if(s == NULLSESSION || s->type == FREE)
		return NULLSESSION;

	return s;
}

/* Select and display sessions */
dosession(argc,argv)
int argc;
char *argv[];
{
	struct session *s;
	extern char *tcpstates[];
	char *psocket();
#ifdef AX25
	extern char *ax25states[];
#endif
	char *tcp_port();
	int go();

	if(argc > 1){
		if((current = sessptr(argv[1])) != NULLSESSION)
			go();
		return 0;
	}
	printf(" #       &CB Type   Rcv-Q  State        Remote socket\n");
	for(s=sessions; s < &sessions[nsessions];s++){
		switch(s->type){
#ifdef _TELNET
		case TELNET:
#ifdef SOKNAME
			printf("%c%-3d%8lx Telnet  %4d  %-13s%-s",
#else
			printf("%c%-3d%8lx Telnet  %4d  %-13s%-s:%s",
#endif
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.telnet->tcb,
			 s->cb.telnet->tcb->rcvcnt,
			 tcpstates[s->cb.telnet->tcb->state],
#ifdef SOKNAME
			 s->name);
#else
			 s->name,
			 tcp_port(s->cb.telnet->tcb->conn.remote.port));
#endif
			break;
#endif /* _TELNET */
		case FTP:
#ifdef SOKNAME
			printf("%c%-3d%8lx FTP     %4d  %-13s%-s",
#else
			printf("%c%-3d%8lx FTP     %4d  %-13s%-s:%s",
#endif
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.ftp->control,
			 s->cb.ftp->control->rcvcnt,
			 tcpstates[s->cb.ftp->control->state],
#ifdef SOKNAME
			 s->name);
#else
			 s->name,
			 tcp_port(s->cb.ftp->control->conn.remote.port));
#endif
			break;
#ifdef	AX25
		case AX25TNC:
			printf("%c%-3d%8lx AX25    %4d  %-13s%-s",
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.ax25_cb,
			len_mbuf(s->cb.ax25_cb->rxq),
			 ax25states[s->cb.ax25_cb->state],
			 s->name);
			break;
#endif
#ifdef	_FINGER
		case FINGER:
#ifdef SOKNAME
			printf("%c%-3d%8lx Finger  %4d  %-13s%-s",
#else
			printf("%c%-3d%8lx Finger  %4d  %-13s%-s:%s",
#endif
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.finger->tcb,
			 s->cb.finger->tcb->rcvcnt,
			 tcpstates[s->cb.finger->tcb->state],
#ifdef SOKNAME
			 s->name);
#else
			 s->name,
			 s->cb.finger->tcb->conn.remote.port);
#endif
			break;
#endif
#ifdef NETROM
		case NRSESSION:
			printf("%c%-3d%8lx NET/ROM %4d  %-13s%-s",
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.nr4_cb,
			len_mbuf(s->cb.nr4_cb->rxq),
			 Nr4states[s->cb.nr4_cb->state],
			 s->name);
			break;
#endif
		default:
			continue;
		}
		if(s->rfile != NULLCHAR || s->ufile != NULLCHAR)
			printf("\t");
		if(s->rfile != NULLCHAR)
			printf("Record: %s ",s->rfile);
		if(s->ufile != NULLCHAR)
			printf("Upload: %s",s->ufile);
		printf("\n");
	}
	return 0;
}
/* Enter conversational mode with current session */
int
go()
{
	void rcv_char(),ftpccr(),ax_rx(),fingcli_rcv(),raw() ;

	if(current == NULLSESSION || current->type == FREE ||
			current->parse == NULLFP)	/* k35 */
		return 0;
	mode = CONV_MODE;
	switch(current->type){
#ifdef _TELNET
	case TELNET:
		if(current->cb.telnet->remote[TN_ECHO])
			raw();	/* Re-establish raw mode if it was set */
		rcv_char(current->cb.telnet->tcb,0); /* Get any pending input */
		break;
#endif
	case FTP:
		ftpccr(current->cb.ftp->control,0);
		break;
#ifdef	AX25
	case AX25TNC:
		ax_rx(current->cb.ax25_cb,0);
		break;
#endif
#ifdef _FINGER
	case FINGER:
		fingcli_rcv(current->cb.finger->tcb,0) ;
		break ;
#endif
#ifdef	NETROM
	case NRSESSION:
		nr4_rx(current->cb.nr4_cb,0) ;
		break ;
#endif
	}
	return 0;
}


doclose(argc,argv)
int argc;
char *argv[];
{
	struct session *s;
#ifdef AX25
	int disc_ax25(),reset_ax25();
#endif
	int kick_tcp();
	extern int noprompt;	/* something we need to clean up prompts */

	if((s = sessptr(argc > 1 ? argv[1] : NULLCHAR)) == NULLSESSION){
		printf(badsess);
		return -1;
	}
	cmdmode();	/* k35 */
	noprompt = 1;	/* to control prompts k35 */

	switch(s->type){
#ifdef _TELNET
	case TELNET:
		close_tcp(s->cb.telnet->tcb);
		break;
#endif
	case FTP:
		close_tcp(s->cb.ftp->control);
		break;
#ifdef	AX25
	case AX25TNC:
		disc_ax25(s->cb.ax25_cb);
		break;
#endif
#ifdef _FINGER
	case FINGER:
		close_tcp(s->cb.finger->tcb);
		break;
#endif
#ifdef NETROM
	case NRSESSION:
		disc_nr4(s->cb.nr4_cb) ;
		break ;
#endif
	}
	current->parse = NULLFP;	/* No further need to send things - k35 */
	return 0;
}
doreset(argc,argv)
int argc;
char *argv[];
{
	struct session *s;
	extern int noprompt;

	if((s = sessptr(argc > 1 ? argv[1] : NULLCHAR)) == NULLSESSION){
		printf(badsess);
		return -1;
	}
	noprompt = 1;	/* to control prompts k35 */
	switch(s->type){
#ifdef _TELNET
	case TELNET:
		reset_tcp(s->cb.telnet->tcb);
		break;
#endif
	case FTP:
		if(s->cb.ftp->data != NULLTCB){
			reset_tcp(s->cb.ftp->data);
			s->cb.ftp->data = NULLTCB;
		}
		reset_tcp(s->cb.ftp->control);
		break;
#ifdef	AX25
	case AX25TNC:
		reset_ax25(s->cb.ax25_cb);
		break;
#endif
#ifdef _FINGER
	case FINGER:
		reset_tcp(s->cb.finger->tcb);
		break;
#endif
#ifdef NETROM
	case NRSESSION:
		reset_nr4(s->cb.nr4_cb) ;
		break ;
#endif
	}
	return 0;
}
int
dokick(argc,argv)
int argc;
char *argv[];
{
#ifdef AX25
	int kick_ax25();
#endif
	void tcp_timeout();
	struct session *s;

	if((s = sessptr(argc > 1 ? argv[1] : NULLCHAR)) == NULLSESSION){
		printf(badsess);
		return -1;
	}
	switch(s->type){
#ifdef _TELNET
	case TELNET:
		if(kick_tcp(s->cb.telnet->tcb) == -1){
			printf(notval);
			return 1;
		}
		break;
#endif
	case FTP:
		if(kick_tcp(s->cb.ftp->control) == -1){

			printf(notval);
			return 1;
		}
		if(s->cb.ftp->data != NULLTCB)
			kick_tcp(s->cb.ftp->data);
		break;
#ifdef	AX25
	case AX25TNC:
		if(kick_ax25(s->cb.ax25_cb) == -1){
			printf(notval);
			return 1;
		}
		return 1;
#endif
#ifdef _FINGER
	case FINGER:
		if(kick_tcp(s->cb.finger->tcb) == -1){
			printf(notval);
			return 1;
		}
		break;
#endif
#ifdef NETROM
	case NRSESSION:
		if(kick_nr4(s->cb.nr4_cb) == -1) {
			printf(notval) ;
			return 1 ;
		}
		break ;
#endif
	}
	return 0;
}
struct session *
newsession()
{
	register int i;

	for(i=0;i<nsessions;i++)
		if(sessions[i].type == FREE)
			return &sessions[i];
	return NULLSESSION;
}
void
freesession(s)
struct session *s;
{
void free();

	if(s == NULLSESSION)
		return;
	if(s->record != NULLFILE){
		fclose(s->record);
		s->record = NULLFILE;
	}
	if(s->rfile != NULLCHAR){
		free(s->rfile);
		s->rfile = NULLCHAR;
	}
	if(s->upload != NULLFILE){
		fclose(s->upload);
		s->upload = NULLFILE;
	}
	if(s->ufile != NULLCHAR){
		free(s->ufile);
		s->ufile = NULLCHAR;
	}
	if(s->name != NULLCHAR){
		free(s->name);
		s->name = NULLCHAR;
	}
	s->type = FREE;
}
/* Control session recording */
dorecord(argc,argv)
int argc;
char *argv[];
{
	if(current == NULLSESSION){
		printf("No current session\n");
		return 1;
	}
	if(argc > 1){
		if(current->rfile != NULLCHAR){
			fclose(current->record);
			free(current->rfile);
			current->record = NULLFILE;
			current->rfile = NULLCHAR;
		}
		/* Open new record file, unless file name is "off", which means
		 * disable recording
		 */
		if(strcmp(argv[1],"off") != 0
		 && (current->record = fopen(argv[1],"a")) != NULLFILE){
			current->rfile = malloc((unsigned)strlen(argv[1])+1);
			strcpy(current->rfile,argv[1]);
		}
	}
	if(current->rfile != NULLCHAR)
		printf("Recording into %s\n",current->rfile);
	else
		printf("Recording off\n");
	return 0;
}

#if defined(_TELNET) || defined(AX25)
/* Control file transmission */
doupload(argc,argv)
int argc;
char *argv[];
{
#ifdef _TELNET	/* actually this whole function could be ifdefed if slip only */
	struct tcb *tcb;
	int16 ip_mtu();
#endif
#ifdef AX25
	struct ax25_cb *axp;
#endif
#ifdef NETROM
	struct nr4cb *cb ;
#endif

	if(current == NULLSESSION){
		printf("No current session\n");
		return 1;
	}
	if(argc > 1){
		switch(current->type){
#ifdef _TELNET
		case TELNET:
			tcb = current->cb.telnet->tcb;
			break;
#endif
#ifdef	AX25
		case AX25TNC:
			axp = current->cb.ax25_cb;
			break;
#endif
#ifdef NETROM
		case NRSESSION:
			cb = current->cb.nr4_cb ;
			break ;
#endif
		case FTP:
			printf("Uploading on FTP control channel not supported\n");
			return 1;
		}
		/* arg here was "stop" (inconsistent).  Also added _OSK ifdef. - k35
		 * Could just as well eliminate this and just disconnect to abort
		 * an upload.  Would save a few bytes
		 */
		if(strcmp(argv[1],"off") == 0 && current->upload != NULLFILE){
			/* Abort upload - see note in ax25cmd.c on how to handle pipes
			 * if such process is added to any of the interactive sessions.
			 * Don't know if the following is needed for OSK but it was used
			 * in ax25cmd.c */
#if defined(_OSK) && !defined(_UCC)
			tmpclose(current->upload);
#else
			fclose(current->upload);
#endif  /* _OSK */
			current->upload = NULLFILE;
			if(current->ufile != NULLCHAR){
				free(current->ufile);
				current->ufile = NULLCHAR;
			}
		}else{	/* this else was missing! */
			/* Open upload file */
			if((current->upload = fopen(argv[1],"r")) == NULLFILE){
				printf("Can't read %s\n",argv[1]);
				return 1;
			}
			current->ufile = malloc((unsigned)strlen(argv[1])+1);
			strcpy(current->ufile,argv[1]);
			/* All set, kick transmit upcall to get things rolling */
			switch(current->type){
#ifdef	AX25
			case AX25TNC:
				(*axp->t_upcall)(axp,axp->paclen * axp->maxframe);
				break;
#endif
#ifdef NETROM
			case NRSESSION:
				(*cb->t_upcall)(cb, NR4MAXINFO) ;
				break ;
#endif
#ifdef _TELNET
			case TELNET:
				/* remarked line below caused some interesting crashing! K5JB k34
				 * Note that FTP also uses tcb->window, which is may not be best
				 */
				/* (*tcb->t_upcall)(tcb,tcb->snd.wnd - tcb->sndcnt); */
				(*tcb->t_upcall)(tcb,tcb->window - tcb->sndcnt);
				break;
#endif
			}	/* switch */
		}	/* ! "off" */
	}	/* if argc > 1 */
#ifdef _TELNET
	if(current->ufile != NULLCHAR)
		printf("Uploading %s\n",current->ufile);
	else
		printf("Uploading off\n");
#endif
	return 0;
}

#if defined(MSDOS) && defined(CUTE_VIDEO)
/* a simple method of sending attribute along with a character, but it
 * will not handle backspace, tab, or c/r correctly.
 */
void
putca(c,attrib)
char c;
unsigned int attrib;
{
	union REGS regs;

	/* Where is cursor? */
	regs.h.ah = 0x03;	/* get cursor position */
	regs.h.bh = 0;
	int86(0x10,&regs,&regs);
	/* dh has row, dl has column */

	if(c != '\n'){	/* ordinary character */
		regs.h.ah = 0x09;	/* write char and attrib at cursor */
		regs.h.al = c;
		regs.h.bh = 0;
		regs.h.bl = attrib;
		regs.x.cx = 1;
		int86(0x10,&regs,&regs);
	}
	/* now we have to place our cursor.  (Ignoring backspacing, etc) */
	if(c == '\n' || regs.h.dl == 79){	/* must go to next line */
		if(regs.h.dh == 24){	/* bottom line */
			regs.h.ah = 0x06;	/* scroll screen */
			regs.h.al = 1;	/* do one line */
			regs.h.ch = 0;
			regs.h.cl = 0;
			regs.h.dl = 79;
			regs.h.bh = saved_attrib; /* this can be local or global */
			int86(0x10,&regs,&regs);

		}else
			/* no scroll, just use next line */
			regs.h.dh++;

		regs.h.dl = 0;	/* column, set up for what follows */
	}else
		/* stay on same line, just bumping position */
		regs.h.dl++;	/* column */

	regs.h.ah = 0x02;	/* set cursor position */
	regs.h.bh = 0;		/* page */
	int86(0x10,&regs,&regs);
}

void
putsa(ch,attrib)
char *ch;
unsigned int attrib;
{
	while(*ch)
		putca(*ch++,attrib);
}

unsigned int
get_attr()
{
	union REGS regs;

	regs.h.ah = 0x08;	/* read char and attrib at cursor */
	regs.h.bh = 0;
	int86(0x10,&regs,&regs);

	return regs.h.ah;
/*	regs.h.al; has character */

}

int	/* called from main */
doattrib(argc,argv)
int argc;
char *argv[];
{
	if(argc < 2)
		printf("Usage: vattrib bold|reverse|none|color <hex#>\n");
	else{
		switch(argv[1][0]){
			case 'b':
				ourcute = OUR_BOLD;
				break;
			case 'r':
				ourcute = OUR_REVERSE;
				break;
			case 'c':
				if((ourcute = (unsigned)htoi(argv[2])) != 0)
					break;	/* else fall through */
			default:
				ourcute = OUR_NORMAL;
				break;
		}
	}
	return 0;
}
#endif


/* take received or keyboarded characters, save them in a file in the
 * native machine's text format. Do all filtering necessary here.
 */
void
text_write(buf,size,fp)
char *buf;
int size;
FILE *fp;
{
	char c;

	while(size--){	/* assume CR is common denominator */
		if((c = *buf++) == '\012') /* linefeed */
			continue;
		if(c == '\015'){	/* C/R */
#if defined(_OSK) || defined(MSDOS)
			fprintf(fp,"%c",c);
#endif
#if (defined(MSDOS) || defined(UNIX)) && !defined(_OSK)
			fprintf(fp,"%c",'\012');	/* linefeed */
#endif
			continue;
		}
		fprintf(fp,"%c",c);
	}
}

/* used by ax25cmd, nrcmd, and telnet. */

void
term_recv(bp,fp)
struct mbuf *bp;
FILE *fp;	/* possible session recording */
{
	char c;

	/* Display received characters, translating CR's to newlines for screen
	and CR/LF.  text_write handles this if recording.
	 */
	while(bp != NULLBUF){
		if(fp)
			text_write(bp->data,bp->cnt,fp);
		while(bp->cnt--){
#if defined(UNIX) && defined(CUTE_VIDEO)
			vidattr(A_STANDOUT);
#ifdef notdef	/* if your terminal prints a extra space when it */
					/* switches to STANDOUT, enable this part - W5GFE */
			putchar('\b');
#endif
#endif
			if((c = *bp->data++) == '\012')	/* linefeed */
				continue;
			if(c == '\015'){	/* C/R */
#if defined(UNIX) && defined(CUTE_VIDEO)
				vidattr(A_NORMAL);
#endif
#ifndef _OSK
				c = '\n';
#endif
			}
#if defined(MSDOS) && defined(CUTE_VIDEO)
				putca(c,ourcute);
#else
			printf("%c",c);	/* was putchar, but this is smaller code */
#endif

		}
#if defined(UNIX) && defined(CUTE_VIDEO)
		vidattr(A_NORMAL);
#endif
		bp = free_mbuf(bp);
	}
	if(fp)
		fflush(fp);
#if !(defined(MSDOS) && defined(CUTE_VIDEO))
	fflush(stdout);
#endif
}

void
term_send(bp,from,size,offset)
struct mbuf *bp;
char *from;
int16 size;
int offset;
{
	register char *cp;
	char c;

	/* If recording is on, record outgoing stuff too */
	if(current->record)
		text_write(from,size,current->record);

	/* Copy keyboard buffer to output, stripping line feeds */
/* could be portability problems here -- test with non UNIX or MS-DOS - K5JB */
	cp = bp->data + offset;
	while(size-- != 0){
		c = *from++;
		if(c != '\012'){
			*cp++ = c;
			bp->cnt++;
		}
	}
}
#endif /* _TELNET or AX25 */
