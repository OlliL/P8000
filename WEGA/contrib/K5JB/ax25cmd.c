#include <stdio.h>
#include <string.h>
#include "global.h"
#include "options.h"
#include "config.h"
#ifdef AX25
#include "mbuf.h"
#include "ax25.h"
#include "timer.h"
#include "netuser.h"
#include "ftp.h"
#include "tcp.h"
#include "telnet.h"
#include "iface.h"
#include "lapb.h"
#include "finger.h"
#include "cmdparse.h"
#include "session.h"
#include "nr4.h"
#include "arp.h"

#ifdef AX25_HEARD
#include <time.h>
#include "heard.h"
#endif

long htol(),atol();	/* K5JB to fix t3 a little bit */
#ifdef SYS5
#include <sys/inode.h>	/* Need flag to detect pipe fd */
#include <sys/stat.h>
#endif
char badcall[] = "Call?";
extern char notval[];

char *ax25states[] = {
	"Disconnected",
	"Conn pending",
	"Disc pending",
	"Connected",
	"Recovery",
	"Frame Reject"
};

int domycall(),dodigipeat(),doaxstat(),dot1(),dot2(),dot3(),dot4(),
	domaxframe(),doaxwindow(),dopaclen(),don2(),doaxreset(),dopthresh(),
	doaxvers();
#if defined(SEGMENT) && defined(SEG_CMD)
int dosegment();
#endif

extern char ouraxvers;

#ifdef AX25_HEARD
int doheard();
#endif

#ifdef SID2
int dombxcall();
extern struct ax25_addr bbscall;
int bbscall_set;	/* k36 */
#endif
#ifdef VCIP_SSID
extern struct ax25_addr ip_call;
int doipcall();
#endif

void pax25(),send_ax25(),freesession();
int setcall();

static struct cmds axcmds[] = {
	"digipeat",	dodigipeat,	0, NULLCHAR,	NULLCHAR,
#ifdef AX25_HEARD	/* heard stuff */
	"heard",		doheard,		0,	NULLCHAR, "ax25 heard [on|off|clear]",
#endif
	"maxframe",	domaxframe,	0, NULLCHAR,	NULLCHAR,
	"mycall",	domycall,	0, NULLCHAR,	badcall,
#ifdef SID2
	"mboxcall",	dombxcall,	0, NULLCHAR,	badcall,
#endif
	"paclen",	dopaclen,	0, NULLCHAR,	NULLCHAR,
	"pthresh",	dopthresh,	0, NULLCHAR,	NULLCHAR,
	"reset",	doaxreset,	2, "ax25 reset <cb#>", notval,
	"retry",	don2,		0, NULLCHAR,	NULLCHAR,
	"status",	doaxstat,	0, NULLCHAR, notval,
#if defined(SEGMENT) && defined(SEG_CMD)	/* want status to come up with "s" */
	"segment",	dosegment,	0, NULLCHAR,	NULLCHAR,
#endif
	"t1",		dot1,		0, NULLCHAR,	NULLCHAR,
	"t2",		dot2,		0, NULLCHAR,	NULLCHAR,
	"t3",		dot3,		0, NULLCHAR,	NULLCHAR,
	"t4",		dot4,		0, NULLCHAR,	NULLCHAR,
#ifdef VCIP_SSID
	"vcipcall",	doipcall,	0, NULLCHAR,	badcall,
#endif
	"vers",	doaxvers,0,	NULLCHAR,	NULLCHAR,
	"window",	doaxwindow,	0, NULLCHAR,	NULLCHAR,

	NULLCHAR,	NULLFP,0, AXCMDDISP, NULLCHAR	/* AXCMDLIST in options.h */
};

/* Multiplexer for top-level ax25 command */
doax25(argc,argv)
int argc;
char *argv[];
{
	int subcmd();
	return subcmd(axcmds,argc,argv);
}

/* give this a number from 1 to ..., this will return a axp if there is one */
struct ax25_cb *
simple_getax(arg)
int arg;
{
	int i,j;
	struct ax25_cb *axp;
	for(i=0,j=1;i<NHASH;i++)
		for(axp=ax25_cb[i];axp != NULLAX25;axp = axp->next)
			if(j++ == arg)
				return axp;
	return NULLAX25;
}

static
doaxreset(argc,argv)
int argc;
char *argv[];
{
	int atoi();
	struct ax25_cb *axp;
	int ax25val(),reset_ax25();
	extern int noprompt; /* k35 */

	if((axp = simple_getax(atoi(argv[1]))) == NULLAX25)
/*	 || !ax25val(axp)	I doubt we need this test */
		return -1;
	else{
		if(current == (struct session *)axp->user)	/* k35 */
			noprompt = 1;
		reset_ax25(axp);
	}
	return 0;
}



/* Display AX.25 link level control blocks */
static
doaxstat(argc,argv)
int argc;
char *argv[];
{
	register int i;
	register struct ax25_cb *axp;
	char tmp[10];
	extern char notval[];
	int ax25val();
	void dumpstat();

	if(argc < 2){
		printf("AXB# IF   Snd-Q  Rcv-Q  Local     Remote    V State\n");
		for(i=1;;i++){
			if((axp = simple_getax(i)) == NULLAX25)
				break;
			pax25(tmp,&axp->addr.source);
			printf(" %3d %-5s%-7d%-7d%-10s",i,axp->interface->name,
				len_q(axp->txq),len_mbuf(axp->rxq),tmp);
			pax25(tmp,&axp->addr.dest);
			printf("%-10s%-2d%s\n",tmp,axp->proto,ax25states[axp->state]);
		}
		return 0;
	}
	if((axp = simple_getax(atoi(argv[1]))) == NULLAX25)
		/* || !ax25val(axp) try without this */
		return -1;
	else
		dumpstat(axp);
	return 0;
}

/* Dump one control block */
static void
dumpstat(axp)
register struct ax25_cb *axp;
{
	char tmp[10];
	int i;

	if(axp == NULLAX25 || axp->interface == NULLIF)
		return;
	printf("    &AXB IF   Remote   RB N(S) N(R) Unack V Retry State\n");
	pax25(tmp,&axp->addr.dest);
	printf("%8lx %-5s%-9s%c%c",(long)axp,axp->interface->name,tmp,
		axp->rejsent ? 'R' : ' ',axp->remotebusy ? 'B' : ' ');
	printf(" %-4d %-4d %02u/%02u %u %02u/%02u %s\n",
			axp->vs,axp->vr,axp->unack,axp->maxframe,axp->proto,
			axp->retries,axp->n2,ax25states[axp->state]);

	printf("T1: ");
	if(run_timer(&axp->t1))
		printf("%lu",(axp->t1.start - axp->t1.count) * MSPTICK);
	else
		printf("stop");
	printf("/%lu ms; ",axp->t1.start * MSPTICK);

	printf("T2: ");
	if(run_timer(&axp->t2))
		printf("%lu",(axp->t2.start - axp->t2.count) * MSPTICK);
	else
		printf("stop");
	printf("/%lu ms; ",axp->t2.start * MSPTICK);

	printf("T3: ");
	if(run_timer(&axp->t3))
		printf("%lu",(axp->t3.start - axp->t3.count) * MSPTICK);
	else
		printf("stop");
	printf("/%lu ms; ",axp->t3.start * MSPTICK);

	printf("T4: ");
	if(run_timer(&axp->t4))
		printf("%lu",((axp->t4.start - axp->t4.count) * MSPTICK) / 1000L);
	else
		printf("stop");
	printf("/%lu sec\n",(axp->t4.start * MSPTICK) / 1000L);

	if(axp->addr.ndigis == 0)
		return;
	printf("Digipeaters:");
	for(i=0;i<axp->addr.ndigis;i++){
		pax25(tmp,&axp->addr.digis[i]);
		printf(" %s",tmp);
	}
	printf("\n");
}

/* Display or change our AX.25 address */
static
domycall(argc,argv)
int argc;
char *argv[];
{
	char buf[15];

	if(argc < 2){
		pax25(buf,&mycall);
		printf("%s\n",buf);
		return 0;
	}
	if(setcall(&mycall,argv[1]) == -1)
		return -1;
	mycall.ssid |= E;
	bbscall_set = 0;	/* k36 */
	return 0;
}

#ifdef SID2
/* Display or change our AX.25 BBS Address */
static
dombxcall(argc,argv)
int argc;
char *argv[];
{
	char buf[15];

	if(argc < 2){
		pax25(buf,&bbscall);
		printf("%s\n",buf);
		return 0;
	}
	if(setcall(&bbscall,argv[1]) == -1)
		return -1;
	bbscall.ssid |= E;
	bbscall_set = 1;	/* k36 */
	return 0;
}
#endif

#ifdef VCIP_SSID
/* Display or change our AX.25 IP only Call */
static
doipcall(argc,argv)
int argc;
char *argv[];
{
	char buf[15];
	int psax25(), setpath(), arp_init();

	if(argc < 2){
		pax25(buf,&ip_call);
		printf("%s\n",buf);
		return 0;
	}

	if(setcall(&ip_call,argv[1]) == -1)
		return -1;
	ip_call.ssid |= E;	/* oops!  K35 */
	return 0;

}
#endif

/* Display or change our default AX.25 protocol version */
static
doaxvers(argc,argv)
int argc;
char *argv[];
{
	if(argc == 2)
		ouraxvers = argv[1][1] == '2' ? V2 : V1;	/* see my bias? */
	else
		printf("AX.25 version is %s\n",ouraxvers == V1 ? "V1" : "V2");
	return 0;
}

#if defined(SEGMENT) && defined(SEG_CMD)
extern int rx_segment;

/* Display or change our AX.25 RX Segmentation */
static
dosegment(argc,argv)
int argc;
char *argv[];
{
	if(argc == 2)
		rx_segment = argv[1][1] == 'n' ? 1 : 0;
	else
		printf("AX.25 RX segmentation is %s\n",rx_segment ? "on" : "off");
	return 0;
}
#endif

/* Control AX.25 digipeating */
static
dodigipeat(argc,argv)
int argc;
char *argv[];
{
	extern int digipeat;

	if(argc == 1)
		printf("digipeat %s\n",digipeat ? "on" : "off");
	else
		digipeat = argv[1][1] == 'n' ? 1 : 0;	/* k34 */
}

/* Setting t1 or t2 too short causes real subtile problems.  Setting
 * t3 or t4 too small causes the user to get what he deserves - K5JB
 * (I guess that applys to the other parameters too)
 */
#define MIN_T1_TIMER 5000	/* milliseconds -- even this is kinda short */
#ifndef MIN_T2_TIMER
#define MIN_T2_TIMER 1000
#endif

/* Set retransmission timer */
static
dot1(argc,argv)
int argc;
char *argv[];
{
	extern int16 t1init;

	if(argc == 1) {
		printf("T1 %lu ms\n",(long)t1init * MSPTICK);
	} else {
		if((t1init = atol(argv[1])) < MIN_T1_TIMER)
			t1init = MIN_T1_TIMER;
		t1init /= MSPTICK;
	}
}

/* Set acknowledgement delay timer */
static
dot2(argc,argv)
int argc;
char *argv[];
{
	extern int16 t2init;
	int16 tmpt2;

	if(argc == 1)
		printf("T2 %lu ms\n",(long)t2init * MSPTICK);
	else
		if((tmpt2 = atol(argv[1])) >= MIN_T2_TIMER)
			t2init = tmpt2 / MSPTICK;
}

/* Set idle timer */
static
dot3(argc,argv)
int argc;
char *argv[];
{
	extern int16 t3init;

	if(argc == 1) {
		printf("T3 %lu ms\n",(long)t3init * MSPTICK);
	} else {
		t3init = atol(argv[1]) / MSPTICK;
	}
}

/* Set stale ckt timer */
static
dot4(argc,argv)
int argc;
char *argv[];
{
	extern int32 t4init;

	if(argc == 1) {
		printf("T4 %lu sec\n",(t4init * MSPTICK)/1000L);
	} else {
		t4init = (1000L * atol(argv[1])) / MSPTICK;
	}
}

/* Set retry limit count */
static
don2(argc,argv)
int argc;
char *argv[];
{
	extern int16 n2;
	int atoi();

	if(argc == 1)
		printf("Retry %u\n",n2);
	else
		if(!((n2 = atoi(argv[1])) > 0 && n2 < 16))
			n2 = 10;
	return 0;
}

/* Set maximum number of frames that will be allowed in flight */
static
domaxframe(argc,argv)
int argc;
char *argv[];
{
	extern int16 maxframe;

	if(argc == 1)
		printf("Maxframe %u\n",maxframe);
	else
		maxframe = atoi(argv[1]);
}

/* Set maximum length of I-frame data field */
static
dopaclen(argc,argv)
int argc;
char *argv[];
{
	extern int16 paclen;

	if(argc == 1)
		printf("Paclen %u\n",paclen);
	else
		paclen = atoi(argv[1]);
}

/* Set size of I-frame above which polls will be sent after a timeout */
static
dopthresh(argc,argv)
int argc;
char *argv[];
{
	extern int16 pthresh;

	if(argc == 1)
		printf("Pthresh %u\n",pthresh);
	else
		pthresh = atoi(argv[1]);
}

/* Set high water mark on receive queue that triggers RNR */
static
doaxwindow(argc,argv)
int argc;
char *argv[];
{
	extern int16 axwindow;

	if(argc == 1)
		printf("Axwindow %u\n",axwindow);
	else
		axwindow = atoi(argv[1]);
}

/* End of ax25 subcommands */

/* Initiate interactive AX.25 connect to remote station */
int
doconnect(argc,argv)
int argc;
char *argv[];
{
	void ax_rx(),ax_tx(),ax_state(),free();
	int ax_parse();
	struct ax25_addr dest;
	struct ax25 addr;
	struct ax25_cb *open_ax25();
	struct interface *ifp;
	struct session *s,*ps;	/* k34 added prior session in case of bad call */
	extern int16 axwindow;
	int i, addreq(),go();

	if (strcmp(argv[1],"netrom") == 0) {
		printf("Connect on netrom interface not supported\n") ;
		return 1 ;
	}

	for(ifp = ifaces; ifp != NULLIF; ifp = ifp->next)
		if(strcmp(argv[1],ifp->name) == 0)
			break;

	if(ifp == NULLIF){
		printf("Interface %s unknown\n",argv[1]);
		return 1;
	}
	if(setcall(&dest,argv[2]))	/* k34 */
		return -1;
	/* See if a session already exists */
	for(s = sessions; s < &sessions[nsessions]; s++){
		if(s->type == AX25TNC
		 && addreq(&s->cb.ax25_cb->addr.dest,&dest)){
			printf("Session %u to %s already exists\n",s - sessions,argv[2]);
			return 1;
		}
	}
	/* Allocate a session descriptor */
	if((s = newsession()) == NULLSESSION){
		printf("Too many sessions\n");
		return 1;
	}
	if((s->name = malloc((unsigned)strlen(argv[2])+1)) != NULLCHAR)
		strcpy(s->name,argv[2]);
	s->type = AX25TNC;
	s->parse = ax_parse;
	ps = current;	/* save ps in case a call is bad k34 */
	current = s;
	ASSIGN(addr.source,mycall);
	ASSIGN(addr.dest,dest);
	for(i=3; i < argc; i++)
		if(setcall(&addr.digis[i-3],argv[i])){	/* k34 */
			current = ps;
			freesession(s);	/* will free name */
			return -1;
		}

	addr.ndigis = i - 3;
	s->cb.ax25_cb = open_ax25(&addr,axwindow,ax_rx,ax_tx,ax_state,ifp,(char *)s);
	go();
	return 0;
}


/* Display changes in AX.25 state */
void
ax_state(axp,old,new)
struct ax25_cb *axp;
int old,new;
{
	struct session *s;
	int cmdmode();

	s = (struct session *)axp->user;

	if(current != NULLSESSION && current->type == AX25TNC && current == s){
		/* Don't print transitions between CONNECTED and RECOVERY */
		if(new != RECOVERY && !(old == RECOVERY && new == CONNECTED))
			printf("%s\n",ax25states[new]);
#ifdef PESKYPROMPT
printf("<%s %s>\n",ax25states[old],ax25states[new]);
#endif
		fflush(stdout);	/* moved up - k35 */
		if(new == DISCONNECTED || new == DISCPENDING)
			cmdmode();
	}
	if(new == DISCONNECTED){
		axp->user = NULLCHAR;
		freesession(s);
	}
}

/* Handle typed characters on AX.25 connection */
int
ax_parse(buf,cnt)
char *buf;
int16 cnt;
{
	struct mbuf *bp;
	void term_send();

	if(current == NULLSESSION || current->type != AX25TNC)
		return;	/* "can't happen" */

	/* Allocate buffer and start it with the PID */
	bp = alloc_mbuf(cnt+1);
	*bp->data = PID_NO_L3;
	bp->cnt++;

	term_send(bp,buf,cnt,1);
	send_ax25(current->cb.ax25_cb,bp);
}

/* Handle new incoming terminal sessions
 * This is the default receive upcall function, used when
 * someone else connects to us
 */
void
ax_incom(axp,cnt)
register struct ax25_cb *axp;
int16 cnt;
{
	void ax_session() ;
#ifdef AXMBX
	extern int ax25mbox;
	void mbx_incom();

	if (ax25mbox)
		mbx_incom(axp,cnt) ;
	else
#endif
		ax_session(axp,cnt) ;
	return ;

}

/* This function sets up an ax25 chat session */
void
ax_session(axp,unused)
register struct ax25_cb *axp ;
int16 unused;
{
	struct session *s;
#ifdef POLITE
	struct session *tmpsession;
#endif
	char remote[10];
	void ax_rx(),ax_state(),ax_tx();
	int disc_ax25(),go();
	extern struct session *current;
#ifdef AX_MOTD
	extern char ax_motd[];
#endif

	pax25(remote,&axp->addr.dest);
	if((s = newsession()) == NULLSESSION){
		/* Out of sessions */
		disc_ax25(axp);
		return;
	}
	s->type = AX25TNC;
	s->name = malloc((int16)strlen(remote)+1);
	s->cb.ax25_cb = axp;
	s->parse = ax_parse;
	strcpy(s->name,remote);
	axp->r_upcall = ax_rx;
	axp->s_upcall = ax_state;
	axp->t_upcall = ax_tx;
	axp->user = (char *)s;
	printf("\007Incoming AX25 session %u from %s\n",s - sessions,remote);
	fflush(stdout);
#ifdef POLITE
	tmpsession = current; /* may need this later */
#endif
	current = s;	/* this may be only temporary if we are busy */
#ifdef AX_MOTD
	if(ax_motd[0])
		ax_parse(ax_motd,strlen(ax_motd));
#endif
#ifdef POLITE
	if((s - sessions) == 0)	/* will prevent us from doing self test */
		go();
	else
		current = tmpsession;
#else
	go();
#endif
}

/* Handle incoming terminal traffic */
void
ax_rx(axp,cnt)
struct ax25_cb *axp;
int16 cnt;
{
	register struct mbuf *bp;
	struct mbuf *recv_ax25();
	void term_recv();
#ifdef	FLOW
	extern int ttyflow;	/* added - K5JB */
#endif

	/* Hold output if we're not the current session */
	if(mode != CONV_MODE || current == NULLSESSION
#ifdef	FLOW
	 || !ttyflow	/* Or if blocked by keyboard input -- hyc - K5JB */
#endif
	 || current->type != AX25TNC || current->cb.ax25_cb != axp)
		return;

	if((bp = recv_ax25(axp,cnt)) == NULLBUF)
		return;
	term_recv(bp,current->record);
}

/* Handle transmit upcalls. Used only for file uploading */
void
ax_tx(axp,cnt)
struct ax25_cb *axp;
int16 cnt;
{
	register char *cp;
	struct session *s;
	register struct mbuf *bp;
	int16 size;
	int c;
	void free();
#ifdef SYS5
	struct stat ss_buf;
#endif

	if((s = (struct session *)axp->user) == NULLSESSION
	 || s->upload == NULLFILE)
		return;
	while(cnt != 0){
		size = min(cnt,axp->paclen+1);
		if((bp = alloc_mbuf(size)) == NULLBUF)
			break;
		cp = bp->data;
		/* Start with the PID */
		*cp++ = PID_NO_L3;
		bp->cnt++;

		/* Now send data characters, translating between local
		 * keyboard end-of-line sequences and the (unwritten)
		 * AX.25 convention, which is carriage-return only
		 */
		while(bp->cnt < size){
			if((c = getc(s->upload)) == EOF)
				break;
#if defined(MSDOS) || defined(_OSK)
/* Explicit octals given because OS9/K compiler defines \n as a carriage
 * return - N0QBJ
 */
			/* MS-DOS gives cr-lf */
			if(c == '\012')
				continue;
#endif

#ifdef UNIX
			/* These give lf only */
			if(c == '\012')
				c = '\015';
#endif
			*cp++ = c;
			bp->cnt++;
		}
		if(bp->cnt > 1) {
			send_ax25(axp,bp);
		} else {
			/* Empty frame, don't bother sending */
			free_p(bp);
			break;
		}
		cnt -= bp->cnt;
	}
	if(cnt != 0){
#if defined(_OSK) && !defined(_UCC)
		tmpclose(s->upload);
#else
	/* Note that this area is VERY Unix system-dependent.  If you aren't
    * using the shell escape, you can just do pclose(), and if err, do
	 * fclose, but this method messes up the forked child doing shell.
	 * Note also that I included ionode.h, not types.h.
	 */
#ifdef SYS5
		if(fstat(fileno(s->upload),&ss_buf) < 0)
			perror("ax_tx");
#ifdef COH386
		if ((ss_buf.st_mode & S_IFPIP) == S_IFPIP)
#else
		if((ss_buf.st_mode & IFIFO) == IFIFO)
#endif
			pclose(s->upload);  /* close pipe from dir */
		else
#endif
			fclose(s->upload);
#endif	/* _OSK */
		s->upload = NULLFILE;
		free(s->ufile);
		s->ufile = NULLCHAR;
	}
}

#ifdef AX25_HEARD
/* heard stuff -- Initially dredged up and contributed by N4HXH */
struct ax25_heard heard = {1,0,-1};	/* initially enabled */

/* build a file, heard.hlp, readable by ax24 heard, mailbox jheard, or
help heard.  File is over-written with each call.  Return a null pointer
if heard is not enabled, or unable to open file */

char *
buildheard(mbox)
int mbox;
{
	struct heard_stuff *hp;
	struct ax25_addr *dp;
	extern struct ax25_heard heard;
	char *pp;
	extern char *homedir;
	extern char *make_path();
	void free();
	FILE *hfp;
	char tmp[32];
	int curr;
	time_t t;

	/* we use a file so net isn't tied up so long with screen */
	if(!heard.enabled)
		return(NULLCHAR);
	if((pp = make_path(homedir,"heard.hlp",1)) == NULLCHAR)
		return(NULLCHAR);
#ifdef MSDOS
	if((hfp = fopen(pp,"w+t")) == NULL)
#else
	if((hfp = fopen(pp,"w+")) == NULL)
#endif
	{
		if(!mbox)
			printf("doheard: Cannot open file: %s\n",pp);
		free(pp);
		return(NULLCHAR);
	}
	time(&t);
	fprintf(hfp,"Heard list (info frames only) Saved at %s",ctime(&t));
	curr = heard.first;
	while (curr != -1) {
		hp = &heard.list[curr];
		fprintf(hfp,"%4s: ",hp->ifacename); /* max length enforced elsewhere */
		sprintf(tmp,ctime((time_t *)&hp->htime));
		tmp[19] = '\0';        /* shorten up time string a bit */
		fprintf(hfp,"%s ",&tmp[4]);
		if (hp->flags) {
			fprintf(hfp,"%-4s%-3s%-4s%-5s", /* Note that these are accumulated */
					(hp->flags & HEARD_ARP) ? "ARP" : "",
					(hp->flags & HEARD_IP) ? "IP" : "",
					(hp->flags & HEARD_NETROM) ? "N/R" : "",
					(hp->flags & HEARD_ROSE) ? "ROSE" : "");
		}else
			fprintf(hfp,"............... ");
		pax25(tmp,&hp->info.source);
		fprintf(hfp,"%s>",tmp);
		pax25(tmp,&hp->info.dest);
		fprintf(hfp,"%s",tmp);
		if(hp->info.ndigis > 0){
			/* print digi string */
			for(dp = &hp->info.digis[0];
					dp < &hp->info.digis[hp->info.ndigis]; dp++){
				pax25(tmp,dp);
				fprintf(hfp," %s",tmp);
				if (dp->ssid & REPEATED)
					fprintf(hfp,"*");
			}
		}
		fprintf(hfp,"\n");
		curr = hp->next;
	}	/* while */
	fprintf(hfp,"End of heard list\n");
	fclose(hfp);
	return(pp);	/* a non null pp will be freed by caller */
}

/* Display AX.25 heard list */
static
doheard(argc,argv)
int argc;
char *argv[];
{
	extern struct ax25_heard heard;
	void free();
	char *pp;
	extern int dohelpfile();

	if(argc < 2) {
		if((pp = buildheard(0)) != NULLCHAR){
			dohelpfile(pp,0);    /* ignore return value */
			free(pp);
		}else
			printf("not enabled\n");
	}else if(argv[1][1] == 'l'){	/* ell */
		heard.cnt = 0;
		heard.first = -1;
	} else if(argv[1][1] == 'n'){
		if (!heard.enabled) {
			heard.enabled = 1;
			heard.cnt = 0;
			heard.first = -1;
		}
	} else if(argv[1][1] == 'f'){
		heard.enabled = 0;
	} else
		return -1;
	return 0;
}
/* heard stuff */

#endif /* AX25_HEARD */
#endif	/* AX25 */
