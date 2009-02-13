#include <stdio.h>
#include <ctype.h>
#include "sokname.h"	/* for SOKNAME */
#include "global.h"
#include "timer.h"
#include "mbuf.h"
#include "netuser.h"
#include "internet.h"
#include "tcp.h"
#include "cmdparse.h"

long htol(),atol();
int atoi(),tcpval();
extern char notval[];

/* TCP connection states - reduced size of several for formatting - K5JB */
char *tcpstates[] = {
	"Closed",
	"Listen",
	"SYN sent",
	"SYN rcvd",
	"Estab.",
	"FINwait1",
	"FINwait2",
	"ClosWait",
	"Closing",
	"Last ACK",
	"Timewait"
	};

/* TCP closing reasons */
char *reasons[] = {
	"Normal",
	"Reset",
	"Timeout",
	"ICMP"
	};

/* TCP subcommand table */
int domss(),doirtt(),dortt(),dotcpstat(),dowindow(),dotcpkick(),dotcpreset(),
	dotcptimer();

struct cmds tcpcmds[] = {
	"irtt",		doirtt,		0,	NULLCHAR,	NULLCHAR,
	"kick",		dotcpkick,	2,	"tcp kick <tcb>",	notval,
	"mss",		domss,		0,	NULLCHAR,	NULLCHAR,
	"reset",	dotcpreset,	2,	"tcp reset <tcb>", notval,
	"rtt",		dortt,		3,	"tcp rtt <tcb> <val>", notval,
	"status",	dotcpstat,	0,	NULLCHAR, notval,
	"timertype",	dotcptimer,	0, NULLCHAR,	NULLCHAR,
	"window",	dowindow,	0,	NULLCHAR,	NULLCHAR,
	NULLCHAR,	NULLFP,		0,
		"tcp subcommands: irtt kick mss reset rtt status timertype window",
		NULLCHAR
	};

int
dotcp(argc,argv)
int argc;
char *argv[];
{
	int subcmd();
	return subcmd(tcpcmds,argc,argv);
}

#define MAX_TCBTAB 24	/* Arbitrary */
struct tcb *tcbtable[MAX_TCBTAB + 1];

struct tcb *
simple_get(arg)
int arg;
{
	int i,j;
	struct tcb *tcb;

	if(arg == -1){	/* fill table */
		tcbtable[0] = NULLTCB;	/* [0] will contain # of entries */
		for(i=0,j=1;i<NTCB;i++)
			for(tcb=tcbs[i];tcb != NULLTCB;tcb = tcb->next){
				tcbtable[j++] = tcb;
				if(j > MAX_TCBTAB)	/* because of nested loops, this is */
					goto quit;	/* one place where a goto makes sense */
			}
	quit:
		tcbtable[0] = (struct tcb *)(--j);
		return tcbtable[0];
	}

	/* the last test is the reason for making a static table */
	if(!arg || arg > (int)tcbtable[0] || !tcpval(tcbtable[arg]))
		return NULLTCB;
	else
		return tcbtable[arg];
}

/* Eliminate a TCP connection */
static int
dotcpreset(argc,argv)
int argc;
char *argv[];
{
	register struct tcb *tcb;
	extern int noprompt;	/* something we need to clean up prompts */

	if((tcb = simple_get(atoi(argv[1]))) == NULLTCB)
		return -1;
	cmdmode();	/* k35 */
	noprompt = 1;	/* to control prompts k35 */
	close_self(tcb,RESET);
	return 0;
}

/* Force a retransmission */
static int
dotcpkick(argc,argv)
int argc;
char *argv[];
{
	int kick_tcp();
	register struct tcb *tcb;
	if((tcb = simple_get(atoi(argv[1]))) == NULLTCB || kick_tcp(tcb) == -1)
		return -1;
	return 0;
}

/* Note that in the four functions that follow, there is no constraint
 * against the user entering unrealistic numbers.  We will incorrectly
 * assume that the user wouldn't be here if he didn't know what he was
 * doing -- K5JB
 */

/* Set default maximum segment size */
static int
domss(argc,argv)
int argc;
char *argv[];
{
	if(argc < 2)
		printf("%u\n",tcp_mss);
	else
		tcp_mss = (int16)atoi(argv[1]);
	return 0;
}

/* Set default window size */
static int
dowindow(argc,argv)
int argc;
char *argv[];
{
	if(argc < 2)
		printf("%u\n",tcp_window);
	else
		tcp_window = (int16)atoi(argv[1]);
	return 0;
}

/* Set initial round trip time for new connections */
static int
doirtt(argc,argv)
int argc;
char *argv[];
{
	if(argc < 2)
		printf("%lu\n",tcp_irtt);
	else
		tcp_irtt = (int32)atol(argv[1]);
	return 0;
}

/* Set smoothed round trip time for specified TCB */
static int
dortt(argc,argv)
int argc;
char *argv[];
{
	register struct tcb *tcb;
	if((tcb = simple_get(atoi(argv[1]))) == NULLTCB)
		return -1;
	tcb->srtt = (int32)atol(argv[2]);
	return 0;
}

/* Display status of TCBs */
static int
dotcpstat(argc,argv)
int argc;
char *argv[];
{
	int tstat();
	struct tcb *tcb;

	if(argc < 2)
		tstat();
	else
		if((tcb = simple_get(atoi(argv[1]))) == NULLTCB)
			return -1;
		else
			state_tcp(tcb);
	return 0;
}

/* Dump TCP stats and summary of all TCBs
 *     &TCB Rcv-Q Snd-Q Local socket           Remote socket           State
 * If SOKNAME defined:
 *     1234     0     0 k5jb.okla.ampr:xxxxxxx n5owk.okla.ampr:xxxxxxx Estab.
 * Else:
 *     1234     0     0 44.78.0.2:xxxx         44.78.0.12:xxxx         Estab.
 */
static int
tstat()
{
	register int i,j;
	register struct tcb *tcb;
#ifdef SOKNAME
	char *puname();
#else
	char *psocket();
#endif

	printf("conout %u, conin %u, reset out %u, runt %u, chksum err %u, bdcsts %u\n",
		tcp_stat.conout,tcp_stat.conin,tcp_stat.resets,tcp_stat.runt,
		tcp_stat.checksum,tcp_stat.bdcsts);
	printf("TCB# RcvQ SndQ Local socket            Remote socket           State\n");

	j = (int)simple_get(-1);	/* fill the table */
	for(i=1;i <= j;i++){
		tcb = tcbtable[i];
		printf("%3d%6u%5u ",i,tcb->rcvcnt,tcb->sndcnt);
#ifdef SOKNAME
		/* note that puname and psocket use a static buffer for return */
		printf("%-24s",puname(&tcb->conn.local));
		printf("%-24s%-8s%s\n",	puname(&tcb->conn.remote),
				tcpstates[tcb->state],
				tcb->state == LISTEN && (tcb->flags & CLONE) ? " (S)" : "");
#else
		printf("%-24s",psocket(&tcb->conn.local));
		printf("%-24s%-8s%s\n", psocket(&tcb->conn.remote),
				tcpstates[tcb->state],
				tcb->state == LISTEN && (tcb->flags & CLONE) ? " (S)" : "");
#endif
	}
	fflush(stdout);
	return 0;
}

/* Dump a TCP control block in detail */
static void
state_tcp(tcb)
struct tcb *tcb;
{
	int32 sent,recvd;
#ifdef SOKNAME
	char *puname();
#else
	char *psocket();
#endif

#ifdef TIMEZOMBIE
	int32 timenow;
	int32 time();
#endif

	if(tcb == NULLTCB)
		return;
	/* Compute total data sent and received; take out SYN and FIN */
	sent = tcb->snd.una - tcb->iss;	/* Acknowledged data only */
	recvd = tcb->rcv.nxt - tcb->irs;
	switch(tcb->state){
	case LISTEN:
	case SYN_SENT:		/* Nothing received or acked yet */
		sent = recvd = 0;
		break;
	case SYN_RECEIVED:
		recvd--;	/* Got SYN, no data acked yet */
		sent = 0;
		break;
	case ESTABLISHED:	/* Got and sent SYN */
	case FINWAIT1:		/* FIN not acked yet */
		sent--;
		recvd--;
		break;
	case FINWAIT2:		/* Our SYN and FIN both acked */
		sent -= 2;
		recvd--;
		break;
	case CLOSE_WAIT:	/* Got SYN and FIN, our FIN not yet acked */
	case CLOSING:
	case LAST_ACK:
		sent--;
		recvd -= 2;
		break;
	case TIME_WAIT:		/* Sent and received SYN/FIN, all acked */
		sent -= 2;
		recvd -= 2;
		break;
	}
#ifdef SOKNAME
	printf("Local: %s ",puname(&tcb->conn.local));
	printf("Remote: %s",	puname(&tcb->conn.remote));
#else
	printf("Local: %s ",psocket(&tcb->conn.local));
	printf("Remote: %s",psocket(&tcb->conn.remote));
#endif
	printf(
	" State: %s\n      Init seq    Unack     Next Resent CWind Thrsh  Wind  MSS Queue   Total\n",
			tcpstates[tcb->state]);
	printf("Send:%9lx%9lx%9lx%7lu%6u%6u%6u%5u%6u%8lu\n",
			tcb->iss,tcb->snd.una,tcb->snd.nxt,tcb->resent,tcb->cwind,
			tcb->ssthresh,tcb->snd.wnd,tcb->mss,tcb->sndcnt,sent);

	printf("Recv:%9lx%18lx%7lu%18u%11u%8lu\n",
			tcb->irs,tcb->rcv.nxt,tcb->rerecv,tcb->rcv.wnd,tcb->rcvcnt,recvd);
	if(tcb->reseq != (struct reseq *)0){	/* K5JB, also below */
		register struct reseq *rp;

		printf("Reassembly queue:\n");
		for(rp = tcb->reseq;rp != (struct reseq *)0; rp = rp->next){
			printf("  seq x%lx %u bytes\n",rp->seg.seq,rp->length);
		}
	}
	if(tcb->backoff > 0)
		printf("Backoff %u ",tcb->backoff);
	if(tcb->flags & RETRAN)
		printf("Retrying ");
	switch(tcb->timer.state){
	case TIMER_STOP:
		printf("Timer stopped");
		break;
	case TIMER_RUN:
		printf("Timer running (%lu/%lu ms)",
		 (int32)MSPTICK * (tcb->timer.start - tcb->timer.count),
		 (int32)MSPTICK * tcb->timer.start);
		break;
	case TIMER_EXPIRE:
		printf("Timer expired");
	}
#ifdef TIMEZOMBIE
	time(&timenow);
	printf("\nSRTT %lu ms. Mean dev %lu ms. Last input %lu min. ago.\n",
			tcb->srtt,tcb->mdev,(timenow - tcb->lastheard)/60);
#else
	printf("\nSRTT %lu ms Mean dev %lu ms\n",tcb->srtt,tcb->mdev);
#endif
	fflush(stdout);
}

int32 backoffcap = 1800000L/(int32)MSPTICK; /* initially 30 minutes */

/* tcp timers type - linear v exponential */
static int
dotcptimer(argc,argv)
int argc ;
char *argv[] ;
{
	extern int tcptimertype;
	int32 bkoff;

	if (argc < 2) {
		printf("Tcp timer type is %s.  Max backoff is %lu min.\n",
			tcptimertype ? "linear" : "exponential",
#ifdef UNIX
			(backoffcap * (int32)MSPTICK)/60000L);
#else
			((backoffcap * (int32)MSPTICK)/60000L) + 1);	/* white lie here */
#endif
		return 0 ;
	}

	switch (argv[1][0]) {
		case 'l':
		case 's':
			tcptimertype = 1 ;
			break ;
		case 'e':
			tcptimertype = 0 ;
			break ;
		default:
			printf("use: tcp timertype [linear|exponential] [<max backoff minutes>]\n");
			return -1 ;
	}
	/* interpret arg as minutes and keep it reasonable */
	if(argc == 3 && (bkoff = (int32)atol(argv[2])) > 4){
		backoffcap  = (bkoff * 60000L)/(int32)MSPTICK;
		if(argv[1][0] == 's')	/* a hustler for testing datagram mode */
			backoffcap >>= 6;
	}
	return 0 ;
}
