/* corrected some spurious prints to the screen when tracing to file and
 * made timestamping switchable option.  See TRACE_TIME in options.h - K5JB
 */
#include "config.h"
#include "options.h"
#ifdef TRACE
#include <stdio.h>
#include <ctype.h>
#include "global.h"
#include "mbuf.h"
#include "iface.h"
#include "trace.h"
#include "session.h"
#include "ax25.h"	/* K5JB - all the following includes to eliminate */
#include "timer.h"	/* undefined structure warnings */
#include "nr4.h"
#include "netuser.h"
#include "tcp.h"
#include "finger.h"
#include "lapb.h"
#include "telnet.h"
#include "ftp.h"
#include <string.h>
#ifdef _OSK
#include <time.h>
#endif
/* Redefined here so that programs calling dump in the library won't pull
 * in the rest of the package
 */
static char nospace[] = "No space!!\n";
static char nullpak[] = "empty packet!!\n";
#ifdef TRACE_TIME
static char if_tr_i[] = "\n%s recv: [%s]\n";
static char if_tr_o[] = "\n%s sent: [%s]\n";
#else
static char if_tr_i[] = "\n%s recv:\n";
static char if_tr_o[] = "\n%s sent:\n";
#endif

char howtotrace[] = {
		"Usage: trace [allmode|cmdmode|info_only]\n or <iface> <flags>\n or loopback <flags>\n or to <file>|console\n"
		};

FILE *trfp = stdout;            /* file pointer used for tracing */
int trcount = 0;                /* used to close file for flushing */
char trname[40];                /* name if not stdout */
int notraceall;			/* 0 = trace all, 1 = only in cmd mode */
extern int mode;		/* command mode or not */

/* Protocol tracing function pointers */
void ax25_dump(),ether_dump(),ip_dump();

void (*tracef[])() = {	/* must be in same order as in trace.h */
#ifdef  AX25
	ax25_dump,
#else
	NULLVFP,
#endif
#ifdef  ETHER
	ether_dump,
#else
	NULLVFP,
#endif
	ip_dump
};

/* Dummy structure for loopback tracing */
struct interface loopback = {
	NULLIF, "loopback"
	};

#ifndef NO_INFO_CMD	/* default is to have trace info_only command */
#ifdef TRACE_TIME
char axtrhdr[48];	/* should only need 41 */
#else
char axtrhdr[16];	/* should only need 13 */
#endif

int info_only = 1;	/* default on (no supv frames) */
#endif

/* Display the trace flags for a particular interface */
static void
showtrace(ifp)
register struct interface *ifp;
{
	char *tracingon = "Tracing on ";

	if(ifp == NULLIF)
		return;
	if(!(ifp->trace & (IF_TRACE_IN | IF_TRACE_OUT)))
		printf("%s%s: off\n",tracingon,ifp->name);
	else
		printf("%s%s:%s%s %s\n",tracingon,ifp->name,
			ifp->trace & IF_TRACE_IN ? " input" : "",
			ifp->trace & IF_TRACE_OUT ? " output" : "",
			ifp->trace & IF_TRACE_HEX ? "(Hex/ASCII dump)" :
			ifp->trace & IF_TRACE_ASCII ? "(ASCII dump)" : "(headers only)");
	fflush(stdout);
}


int
dotrace(argc,argv)
int argc;
char *argv[];
{
	struct interface *ifp;
	int htoi();

	if(argc < 2){
#ifndef NO_INFO_CMD
		printf("Info_only trace is %s\n",info_only ? "on" : "off");
#endif
		printf("Trace mode is %s\nTrace to %s\n",
		notraceall ? "cmdmode" : "allmode",trfp == stdout ? "console" : trname);
		showtrace(&loopback);
		for(ifp = ifaces; ifp != NULLIF; ifp = ifp->next)
			showtrace(ifp);
		return 0;
	}
	for(ifp = ifaces; ifp != NULLIF; ifp = ifp->next)
		if(strcmp(ifp->name,argv[1]) == 0)
			break;

	if(ifp == NULLIF && argv[1][0] == 'l')	/* loopback? */
		ifp = &loopback;
	if(ifp != NULLIF){
		if(argc < 3)
			showtrace(ifp);
		else
			ifp->trace = htoi(argv[2]);
		return 0;
	}

	switch(argv[1][0]){
		case 'c':	/* cmdmode */
			notraceall = 1;
			break;
		case 'a':	/* allmode */
			notraceall = 0;
			break;
#ifndef NO_INFO_CMD
		case 'i':
			info_only = !info_only;
			break;
#endif
		case 't':	/* to */
			if(argc < 3)
				printf("trace to %s\n",trfp == stdout? "console" : trname);
			else{
				if(trfp != stdout)
					fclose(trfp);
				if(strncmp(argv[2],"con",3) == 0)
					trfp = stdout;
				else {
#ifdef MSDOS    /* K5JB - fix tracefile treatment of \n */
					if((trfp = fopen(argv[2],"at")) == NULLFILE)
#else
					if((trfp = fopen(argv[2],"a")) == NULLFILE)
#endif
					{
						printf("%s: cannot open\n",argv[2]);
						trfp = stdout;
						return -1;
					}
				}
				strcpy(trname,argv[2]);
			} /* else */
			break;

		default:
			printf("Interface %s unknown\n",argv[1]);
			return -1;
	}
	return 0;
}

void
dump(interface,direction,type,bp)
register struct interface *interface;
int direction;
unsigned type;
struct mbuf *bp;
{
	struct mbuf *tbp;
	void ascii_dump(),hex_dump();
	void (*func)();
#ifdef TRACE_TIME
	char *cp;
	long t;
#endif
	char *errmsg = "";
	int16 size;
	void rip();

	if((interface->trace & direction) == 0)
		return;	/* Nothing to trace */

	if (notraceall && mode != CMD_MODE)
		return; /* No trace while in session, if mode */

	if(bp == NULLBUF || (size = len_mbuf(bp)) == 0)
		return;

#ifdef TRACE_TIME
	time(&t);			/* DG2KK: get time */
	cp = ctime(&t);
	rip(cp);
#endif

	switch(direction){
	case IF_TRACE_IN:
#ifndef NO_INFO_CMD
#ifdef TRACE_TIME
		sprintf(axtrhdr,if_tr_i,interface->name,cp);
#else
		sprintf(axtrhdr,if_tr_i,interface->name);
#endif
		break;
	case IF_TRACE_OUT:
#ifdef TRACE_TIME
		sprintf(axtrhdr,if_tr_o,interface->name,cp);
#else
		sprintf(axtrhdr,if_tr_o,interface->name);
#endif
#else  /* NO_INFO_CMD */
#ifdef TRACE_TIME
		fprintf(trfp,if_tr_i,interface->name,cp);
#else
		fprintf(trfp,if_tr_i,interface->name);
#endif
		break;
	case IF_TRACE_OUT:
#ifdef TRACE_TIME
		fprintf(trfp,if_tr_o,interface->name,cp);
#else
		fprintf(trfp,if_tr_o,interface->name);
#endif
#endif	/* NO_INFO_CMD */
		break;
	}
	if(bp == NULLBUF || (size = len_mbuf(bp)) == 0){
		errmsg = nullpak;
		goto trdone;
	}
	if(type < NTRACE)
		func = tracef[type];
	else
		func = NULLVFP;

	dup_p(&tbp,bp,0,size);
	if(tbp == NULLBUF){
		errmsg = nospace;
		goto trdone;
	}
	if(func != NULLVFP)
		(*func)(&tbp,1);
#ifndef NO_INFO_CMD
	if(info_only && !*axtrhdr){  /* ax25_dump puts null if supv frame */
		free_p(tbp);
		goto trdone;
	}
#endif
	if(interface->trace & IF_TRACE_ASCII)
		/* Dump only data portion of packet in ascii */
		ascii_dump(&tbp);
	else if(interface->trace & IF_TRACE_HEX){
		/* Dump entire packet in hex/ascii */
		free_p(tbp);
		dup_p(&tbp,bp,0,len_mbuf(bp));
		if(tbp != NULLBUF)
			hex_dump(&tbp);
		else{
			errmsg = nospace;	/* will do ugly display but should be rare */
			goto trdone;
		}
	}
	free_p(tbp);

trdone:

	if(*errmsg){
#ifndef NO_INFO_CMD
		fprintf(trfp,axtrhdr);
#endif
		fprintf(trfp,errmsg);
	}
	if(trfp != stdout){
		if(errmsg == nospace)
			printf(errmsg);	/* send to screen too */
		fflush(trfp);
#ifdef MSDOS
		if (++trcount > 25){
			fclose(trfp);	/* MS-DOS must close to flush file */
			trfp = fopen(trname,"at"); /* fix EOL problem.  See also pc.c */
			trcount = 0;
		}
#endif

		/* in Unix version, watch out for this one if we go background */

		if(trfp == NULLFILE || ferror(trfp)){
			printf("Error on %s, will trace to console\n",trname);
#ifdef OBSCURE
			if(trfp != NULLFILE && trfp != stdout)
#else
			if(trfp != NULLFILE)
#endif
				fclose(trfp);
			trfp = stdout;
		}
	}
	fflush(stdout);
}

/* Dump an mbuf in hex */
void
hex_dump(bpp)
register struct mbuf **bpp;
{
	int16 n;
	int16 address;
	void fmtline();
	char buf[16];

	if(bpp == NULLBUFP || *bpp == NULLBUF)
		return;

	address = 0;
	while((n = pullup(bpp,buf,sizeof(buf))) != 0){
		fmtline(address,buf,n);
		address += n;
	}
}
/* Dump an mbuf in ascii */
void
ascii_dump(bpp)
register struct mbuf **bpp;
{
	char c;
	register int16 tot;

	if(bpp == NULLBUFP || *bpp == NULLBUF)
		return;

	tot = 0;
	while(pullup(bpp,&c,1) == 1){
		if((tot % 64) == 0)
			fprintf(trfp,"%04x  ",tot);
		if(isprint(c))
			fprintf(trfp,"%c",c);  /* was putchar */
		else
			fprintf(trfp,".");	/* ditto */
		tot++;
		if((tot % 64) == 0)
			fprintf(trfp,"\n");
	}
	if((tot % 64) != 0)
		fprintf(trfp,"\n");
}

/* Print a buffer up to 16 bytes long in formatted hex with ascii
 * translation, e.g.,
 * 0000: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f  0123456789:;<=>?
 */
void
fmtline(addr,buf,len)
int16 addr;
char *buf;
int16 len;
{
	char line[80];
	register char *aptr,*cptr;
	register int16 c;
#ifdef UNIX
	char *memset();
#else
	void *memset();
#endif
	void ctohex();

	memset(line,' ',sizeof(line));
	ctohex(line,(int16)hibyte(addr));
	ctohex(line+2,(int16)lobyte(addr));
	aptr = &line[6];
	cptr = &line[55];
	while(len-- != 0){
		c = *buf++ & 0xff;
		ctohex(aptr,c);
		aptr += 3;
		c &= 0x7f;
		*cptr++ = isprint(c) ? c : '.';
	}
/*	*cptr++ = '\r';	don't need this */
	*cptr++ = '\n';
	fwrite(line,1,(unsigned)(cptr-line),trfp); /* was stdout - K5JB */
	/* added */
}
/* Convert byte to two ascii-hex characters */
static
void
ctohex(buf,c)
register char *buf;
register int16 c;
{
	static char hex[] = "0123456789abcdef";

	buf[0] = hex[hinibble(c)];
	buf[1] = hex[lonibble(c)];
}
#endif /* TRACE */
