/* net/rom user command processing
 * Copyright 1989 by Daniel M. Frank, W9NK.  Permission granted for
 * non-commercial distribution only.
 * 10/18/92 Added a netrom bcstifle <iface> 0|1 command - K5JB
 * 10/19/92 Munged a little to recover size lost to the above, added
 * test for bad format callsigns in netrom connect command.
 * Put constraints on most vital values.  Left some alone though - K5JB
 * Put n ro report in aligned columns.
 */

#include "config.h"
#ifdef NETROM		/* K5JB - also moved config.h up one place */
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "mbuf.h"
#include "ax25.h"
#include "timer.h"
#include "iface.h"
#include "lapb.h"
#include "netrom.h"
#include "nr4.h"
#include "netuser.h"
#include "tcp.h"
#include "ftp.h"
#include "telnet.h"
#include "finger.h"
#include "cmdparse.h"
#include "session.h"
#include <ctype.h>
#ifdef SYS5
#include <sys/types.h>
#include <sys/inode.h>
#include <sys/stat.h>
#endif
#undef NRDEBUG

void donodetick(),stop_timer(),start_timer();
long htol(),atol();	/* K5JB fix some wierd timer values with this */
void free();
int pax25(),atoi();

extern struct session *current;

char *Nr4states[] = {
	"Disconnected",
	"Conn Pending",
	"Connected",
	"Disc Pending"
} ;

char *Nr4reasons[] = {
	"Normal",
	"By Peer",
	"Timeout",
	"Reset",
	"Refused"
} ;

static int donrverbose(),dointerface(), dobcnodes(), donodetimer(),
	donrroute(),donrttl(), doobsotimer(), donodefilter(),donrconnect(),
	donrreset(), donrwindow(), donrirtt(),donracktime(), donrqlimit(),
	donrchoketime(), donrretries(),donrstatus(), donrstifle(), donrkick();

extern char notval[];
extern char range[];	/* in smtpcli.c */

static struct cmds nrcmds[] = {
	"acktime",	donracktime,	0,	NULLCHAR, range,
	"bcnodes",	dobcnodes,	2,	"netrom bcnodes <iface>", NULLCHAR,
	"bcstifle",	donrstifle, 3, "netrom bcstifle <iface> on|off", NULLCHAR,


/* Put connect before choketime to make it the default expansion of 'c' */

	"connect",	donrconnect,2,	"netrom connect <node>", "Too many sessions",
	"choketime",	donrchoketime,	0,	NULLCHAR, range,	/* k34, in global.h */
	"interface",	dointerface,	4,
		"netrom interface <iface> <alias> <qual>", "Attach netrom iface first",
	"irtt",			donrirtt,		0,	NULLCHAR, range,
	"kick",			donrkick,		2,	"netrom kick <cb#>",	notval,
	"nodefilter",	donodefilter,	0,	NULLCHAR,	NULLCHAR,
	"nodetimer",	donodetimer,	0,	NULLCHAR,	NULLCHAR,
	"obsotimer",	doobsotimer,	0,	NULLCHAR,	NULLCHAR,
	"qlimit",	donrqlimit,	0,	NULLCHAR, range,
	"reset",	donrreset,	2,	"netrom reset <cb#>",	notval,
	"retries",	donrretries,0,	NULLCHAR,	range,
	"route",	donrroute,	0,	NULLCHAR,	NULLCHAR,
	"status",	donrstatus,	0,	NULLCHAR, notval,
	"ttl",		donrttl,	0,	NULLCHAR, "ttl must be between 2 and 20",
#ifdef NR_VERBOSE
	"verbose",	donrverbose,0, NULLCHAR, "netrom verbose [on|off]",
#else
	"verbose",	donrverbose,0, NULLCHAR, NULLCHAR,
#endif
	"window",	donrwindow,	0,	NULLCHAR, range,
	NULLCHAR,	NULLFP,0,
	"netrom subcmds: acktime bcnodes bcstifle connect choketime interface irtt kick\n  nodefilter nodetimer obsotimer qlimit reset retries route status ttl verbose\n  window",
		NULLCHAR
} ;

static struct timer nodetimer ;	/* timer for nodes broadcasts */
static struct timer obsotimer ;	/* timer for aging routes */

/* Command multiplexer */
donetrom(argc,argv)
int argc ;
char *argv[] ;
{
	int subcmd();
	return subcmd(nrcmds,argc,argv) ;
}

static int dorouteadd(), doroutedrop(), doroutedump(), dorouteinfo() ;

static struct cmds routecmds[] = {
	"add",	dorouteadd,	6,
		"netrom route add <alias> <dest> <iface> <qual> <neighbor>",
		"add failed",
	"drop",	doroutedrop, 4,
		"netrom route drop <dest> <neighbor> <iface>",
		"drop failed",
	"info", dorouteinfo, 2,
		"netrom route info <dest>", NULLCHAR,
	NULLCHAR,	NULLFP,	0,
		"netrom route subcommands: add drop info",
		NULLCHAR
} ;

/* Route command multiplexer */
static
donrroute(argc, argv)
int argc ;
char *argv[] ;
{
	if (argc < 2) {
		doroutedump() ;
		return 0 ;
	}
	return subcmd(routecmds,argc,argv) ;
}

/* Dump a list of known routes */
static
doroutedump()
{
	register struct nrroute_tab *rp ;
	register int i, column ;
	char buf[16] ;
#ifdef ORIG_DISPLAY
	char *cp ;
#endif

	column = 1 ;

	for (i = 0 ; i < NRNUMCHAINS ; i++)
		for (rp = nrroute_tab[i] ; rp != NULLNRRTAB ; rp = rp->next) {
#ifdef ORIG_DISPLAY
			strcpy(buf,rp->alias) ;
			/* remove trailing spaces */
			if ((cp = index(buf,' ')) == NULLCHAR)
				cp = &buf[strlen(buf)] ;
			if (cp != buf)		/* don't include colon for null alias */
				*cp++ = ':' ;
			pax25(cp,&rp->call) ;
			printf("%-16s  ",buf) ;
			if (column++ == 4) {
				printf("\n") ;
				column = 1 ;
			}
#else	/* I find it easier to scan if in straight columns */
			pax25(buf,&rp->call);
			printf("%-7s%-9s%s",rp->alias,buf,column++ % 4 ? "     " : "\n");
#endif
		}
/*	printf("%s",column % 4 == 1 ? "" : "\n");  Who cares? */
	printf("\n");
	return 0 ;
}

int    /* called six times - K5JB */
try_setcall(what,call,try)
char *what;
struct ax25_addr *call;
char *try;
{
	int setcall();
	if (setcall(call,try) == -1) {
		printf ("Bad %s callsign\n",what) ;
		return -1 ;
	}
	return 0;
}

/* print detailed information on an individual route */
static int
dorouteinfo(argc,argv)
int argc ;
char *argv[] ;
{
	register struct nrroute_tab *rp ;
	register struct nr_bind *bp ;
	register struct nrnbr_tab *np ;
	struct ax25_addr dest ;
	char neighbor[60] ;
	int psax25();

	if (try_setcall("dest",&dest,argv[1]) == -1)
		return -1 ;

	if ((rp = find_nrroute(&dest)) == NULLNRRTAB) {
		printf("no such route\n") ;
		return -1 ;
	}

	for (bp = rp->routes ; bp != NULLNRBIND ; bp = bp->next) {
		np = bp->via ;
		psax25(neighbor,np->call) ;
		printf("%1s %3d  %3d  %-8s  %s\n",
				(bp->flags & NRB_PERMANENT ? "P" :
				 bp->flags & NRB_RECORDED ? "R" : " "),
				bp->quality,bp->obsocnt,
				nrifaces[np->interface].interface->name,
				neighbor) ;
	}
	return 0 ;
}
		
/* convert a null-terminated alias name to a blank-filled, upcased */
/* version.  Return -1 on failure. */
static int
putalias(to,from,complain)
register char *to, *from ;
int complain ;
{
	int len, i ;
	
	if ((len = strlen(from)) > ALEN) {
		if (complain)
			printf ("alias too long - six characters max\n") ;
		return -1 ;
	}

	for (i = 0 ; i < ALEN ; i++) {
		if (i < len) {
			if (islower(*from))
				*to++ = toupper(*from++) ;
			else
				*to++ = *from++ ;
		}
		else
			*to++ = ' ' ;
	}

	*to = '\0' ;
	return 0 ;
}

int	/* called six times - K5JB */
find_iface(iname)
char *iname;
{
	int i;
	/* find interface */
	for (i = 0 ; i < nr_numiface ; i++)
		if (!strcmp(nrifaces[i].interface->name,iname))
			break ;
	if (i == nr_numiface) {
		printf("Iface \"%s\" not found\n",iname) ;
		return -1 ;
	}
	return i;
}

/* Add a route */
static int
dorouteadd(argc, argv)
int argc ;
char *argv[] ;
{
	char alias[7] ;
	struct ax25_addr dest ;
	unsigned quality ;
	char neighbor[AXALEN * (NRMAXDIGI + 1)] ;	/* to accomodate ROSE - K5JB */
	int i ;
	int naddr,setpath();

	/* format alias (putalias prints error message if necessary) */
	if (putalias(alias,argv[1],1) == -1)
		return -1 ;

	/* format destination callsign */
	if (try_setcall("dest",&dest,argv[2]) == -1)
		return -1 ;

	if((i = find_iface(argv[3])) == -1)
		return -1;

	/* get and check quality value */
	if ((quality = atoi(argv[4])) > 255) {
		printf("maximum route qual is 255\n") ;
		return -1 ;
	}

	/* make sure no more than (currently)3 digis */
	naddr = argc - 5 ;
	if (naddr > NRMAXDIGI + 1) {
		printf("no more than %d digis for a net/rom neighbor\n",NRMAXDIGI) ;
		return -1 ;
	}

	/* to be idiot proof, this should have some test for valid calls - K5JB */

	/* format neighbor address string */
	setpath(neighbor,&argv[5],naddr) ;

	return nr_routeadd(alias,&dest,(unsigned)i,quality,neighbor,1,0) ;
}


/* drop a route */
static
doroutedrop(argc,argv)
int argc ;
char *argv[] ;
{
	struct ax25_addr dest, neighbor ;
	int i ;

	/* format destination and neighbor callsigns */
	if (try_setcall("dest",&dest,argv[1]) == -1)
		return -1 ;

	if (try_setcall("neighbor",&neighbor,argv[2]) == -1)
		return -1 ;

	if((i = find_iface(argv[3])) == -1)
		return -1;

	return nr_routedrop(&dest,&neighbor,i) ;
}


/* make an interface available to net/rom */
static int
dointerface(argc,argv)
int argc ;
char *argv[] ;
{
	int i;
	register struct interface *ifp ;
	extern struct interface *ifaces ;

	if (nr_interface == NULLIF)
		return -1;                 

	if (nr_numiface >= NRNUMIFACE) {
		printf("Only %d net/rom ifaces available\n",NRNUMIFACE) ;
		return 1 ;
	}

	for(ifp=ifaces;ifp != NULLIF;ifp = ifp->next){
		if(strcmp(argv[1],ifp->name) == 0)
			break;
	}
	if(ifp == NULLIF){
		printf("Iface \"%s\" unknown\n",argv[1]);
		return 1;
	}
	for (i = 0 ; i < nr_numiface ; i++)
		if (nrifaces[i].interface == ifp) {
			printf("Iface \"%s\" is already registered\n",argv[1]) ;
			return 1 ;
		}

	nrifaces[nr_numiface].interface = ifp ;

	if (putalias(nrifaces[nr_numiface].alias,argv[2],1) == -1)
		return 1 ;

	if ((nrifaces[nr_numiface].quality = atoi(argv[3])) > 255) {
		printf("Quality cannot be greater than 255\n") ;
		return 1 ;
	}
	nrifaces[nr_numiface].bcstifle = 0;	/* default to node bc not stifled */

	nr_numiface++ ;			/* accept this interface */
	return 0 ;
}

/* Broadcast nodes list on named interface. */

static int
dobcnodes(argc,argv)
int argc ;
char *argv[] ;
{
	int i ;
	void nr_bcnodes();
	if((i = find_iface(argv[1])) == -1)
		return 1;

	nr_bcnodes(i) ;
		return 0;
}

/* Stifle (or enable) node broadcast on named interface. */

static int
donrstifle(argc,argv)
int argc ;
char *argv[] ;
{
	int i ;
	if((i = find_iface(argv[1])) == -1)
		return 1;
	/* this command doesn't report state.  On error it will switch to
	 * default, not stifled.  Only looks for "on".
	 */
	nrifaces[i].bcstifle = argv[2][1] == 'n' ? 1 : 0;
	return 0;
}

#define TICKSPERSEC	(1000L / MSPTICK)	/* Ticks per second */

/* Set outbound node broadcast interval */
static int
donodetimer(argc,argv)
int argc;
char *argv[];
{

	if(argc < 2){
		printf("%lu/%lu\n",
				(nodetimer.start - nodetimer.count)/TICKSPERSEC,
				nodetimer.start/TICKSPERSEC);
		return 0;
	}
	stop_timer(&nodetimer) ;	/* in case it's already running */
	nodetimer.count = 0;	/* k34 */
	nodetimer.func = (void (*)())donodetick;/* what to call on timeout */
	nodetimer.arg = NULLCHAR;		/* dummy value */
	nodetimer.start = atol(argv[1])*TICKSPERSEC;	/* set timer duration */
	start_timer(&nodetimer);		/* and fire it up */
	return 0;
}

static void
donodetick()
{
	register int i ;
	void nr_bcnodes();

	for (i = 0 ; i < nr_numiface ; i++)
		nr_bcnodes(i) ;

	/* Restart timer */
	start_timer(&nodetimer) ;
}

/* Set timer for aging routes */
static int
doobsotimer(argc,argv)
int argc;
char *argv[];
{
	void doobsotick();

	if(argc < 2){
		printf("%lu/%lu\n",(obsotimer.start - obsotimer.count)/TICKSPERSEC,
		obsotimer.start/TICKSPERSEC);
		return 0;
	}
	stop_timer(&obsotimer) ;	/* just in case it's already running */
	obsotimer.count = 0;	/* k34 */
	obsotimer.func = (void (*)())doobsotick;/* what to call on timeout */
	obsotimer.arg = NULLCHAR;		/* dummy value */
	obsotimer.start = atol(argv[1])*TICKSPERSEC;	/* set timer duration */
	start_timer(&obsotimer);		/* and fire it up */
	return 0;
}


/* Go through the routing table, reducing the obsolescence count of
 * non-permanent routes, and purging them if the count reaches 0
 */
static void
doobsotick()
{
	register struct nrnbr_tab *np ;
	register struct nrroute_tab *rp, *rpnext ;
	register struct nr_bind *bp, *bpnext ;
	struct ax25_addr neighbor ;
	int i ;
	int16 nrhash();

	for (i = 0 ; i < NRNUMCHAINS ; i++) {
		for (rp = nrroute_tab[i] ; rp != NULLNRRTAB ; rp = rpnext) {
			rpnext = rp->next ; 	/* save in case we free this route */
			for (bp = rp->routes ; bp != NULLNRBIND ; bp = bpnext) {
				bpnext = bp->next ;	/* in case we free this binding */
				if (bp->flags & NRB_PERMANENT)	/* don't age these */
					continue ;
				if (--bp->obsocnt == 0) {		/* time's up! */
					if (bp->next != NULLNRBIND)
						bp->next->prev = bp->prev ;
					if (bp->prev != NULLNRBIND)
						bp->prev->next = bp->next ;
					else
						rp->routes = bp->next ;
					rp->num_routes-- ;			/* one less binding */
					np = bp->via ;				/* find the neighbor */
					free(bp) ;					/* now we can free the bind */
					/* Check to see if we can free the neighbor */
					if (--np->refcnt == 0) {
						if (np->next != NULLNTAB)
							np->next->prev = np->prev ;
						if (np->prev != NULLNTAB)
							np->prev->next = np->next ;
						else {
							memcpy(neighbor.call,np->call,ALEN) ;
							neighbor.ssid = np->call[ALEN] ;
							nrnbr_tab[nrhash(&neighbor)] = np->next ;
						}
						free(np) ;	/* free the storage */
					}
				}
			}
			if (rp->num_routes == 0) {		/* did we free them all? */
				if (rp->next != NULLNRRTAB)
					rp->next->prev = rp->prev ;
				if (rp->prev != NULLNRRTAB)
					rp->prev->next = rp->next ;
				else
					nrroute_tab[i] = rp->next ;

				free(rp) ;
			}
		}
	}
	start_timer(&obsotimer) ;
}


static int donfadd(), donfdrop(), donfmode() ;

static struct cmds nfcmds[] = {
	"add",	donfadd,	3,
		"netrom nodefilter add <neighbor> <iface>",
		"add failed",
	"drop",	donfdrop,	3,
		"netrom nodefilter drop <neighbor> <iface>",
		"drop failed",
	"mode",	donfmode,	0,
		NULLCHAR,
		"modes: none accept reject",
	NULLCHAR,	NULLFP,	0,
		"nodefilter subcommands: add drop mode",
		NULLCHAR
} ;

/* nodefilter command multiplexer */
static
donodefilter(argc,argv)
int argc ;
char *argv[] ;
{
	void donfdump();

	if (argc < 2) {
		donfdump() ;
		return 0 ;
	}
	return subcmd(nfcmds,argc,argv) ;
}

/* display a list of <callsign,interface> pairs from the filter
 * list.
 */
static void
donfdump()
{
	int i;
	struct nrnf_tab *fp ;
	char buf[16] ;

	for (i = 0 ; i < NRNUMCHAINS ; i++)
		for (fp = nrnf_tab[i] ; fp != NULLNRNFTAB ; fp = fp->next) {
			pax25(buf,&fp->neighbor) ;
			printf("%-10s%s\n",	/* removed columns */
					buf,nrifaces[fp->interface].interface->name);
		}
}

/* add an entry to the filter table */
static
donfadd(argc,argv)
int argc ;
char *argv[] ;
{
	struct ax25_addr neighbor ;
	int i ;

	/* format callsign */
	if (try_setcall("neighbor",&neighbor,argv[1]) == -1)
		return -1 ;

	if((i = find_iface(argv[2])) == -1)
		return -1;

	return nr_nfadd(&neighbor,i) ;
}

/* drop an entry from the filter table */
static
donfdrop(argc,argv)
int argc ;
char *argv[] ;
{
	struct ax25_addr neighbor ;
	int i ;

	/* format neighbor callsign */
	if (try_setcall("neighbor",&neighbor,argv[1]) == -1)
		return -1 ;

	if((i = find_iface(argv[2])) == -1)
		return -1;

	return nr_nfdrop(&neighbor,i) ;
}

/* nodefilter mode subcommand */
static
donfmode(argc,argv)
int argc ;
char *argv[] ;
{
	if (argc < 2)
		printf("filter mode is %s\n",nr_nfmode == NRNF_NOFILTER ? "none" :
			nr_nfmode == NRNF_ACCEPT ? "accept" : "reject");
	else
		if(argv[1][0] == 'n' || argv[1][0] == 'a' || argv[1][0] == 'r')
			nr_nfmode = argv[1][0] == 'n' ? NRNF_NOFILTER :
			argv[1][0] == 'a' ? NRNF_ACCEPT : NRNF_REJECT;
		else
			return -1;

	return 0;
}


/* netrom network packet time-to-live initializer */
static
donrttl(argc, argv)
int argc ;
char *argv[] ;
{
	int val ;

	if (argc < 2)
		printf("%u\n", (unsigned)nr_ttl);
	else	/* purely arbitrary, but reasonable limits, default is 16 */
		if((val = atoi(argv[1])) < 2 || val > 20)
			return -1;
		else
			nr_ttl = (unsigned char)val ;	/* K5JB */

	return 0 ;
}

/* verbose route broadcast - silently do nothing if NR_VERBOSE not enabled */
static
donrverbose(argc,argv)
int argc ;
char *argv[] ;
{
#ifdef NR_VERBOSE
	if(argc > 1)
		if(argv[1][1] == 'f' || argv[1][1] == 'n')
			nr_verbose = argv[1][1] == 'n' ? 1 : 0;
		else
			return -1;
	else
		printf("verbose is %s\n", nr_verbose ? "on" : "off" ) ;
#endif
	return 0;
}

/* Initiate a NET/ROM transport connection */
static int
donrconnect(argc,argv)
int argc ;
char *argv[] ;
{
	struct ax25_addr node, *np ;
	struct session *s ;
	char alias[7] ;
	int go();

	/* See if the requested destination could be an alias, and */
	/* find and use it if it is.  Otherwise assume it is an ax.25 */
	/* address. */

	if (putalias(alias,argv[1],0) != -1 &&
		(np = find_nralias(alias)) != NULLAXADDR)
		  node = *np ;
	else	/* never noticed this error, but let's test anyway - K5JB */
		if (try_setcall("node",&node,argv[1]) == -1)
			return 1;

	/* Get a session descriptor */

	if ((s = newsession()) == NULLSESSION)
		return -1;

	if((s->name = malloc((unsigned)strlen(argv[1])+1)) != NULLCHAR)
		strcpy(s->name,argv[1]);
	s->type = NRSESSION ;
	s->parse = (int(*)())nr4_parse ;
	current = s;

	s->cb.nr4_cb = open_nr4(&node,&mycall,nr4_rx,nr4_tx,nr4_state,(char *)s) ;
	go() ;
	return 0 ;
}

/* Display changes in NET/ROM state */
void
nr4_state(cb,old,new)
struct nr4cb *cb ;
int old,new;
{
	struct session *s;
	int cmdmode();
	void freesession();
	extern int noprompt;

	s = (struct session *)cb->puser;

	if(current != NULLSESSION && current->type == NRSESSION && current == s){
		printf("%s",Nr4states[new]);
		if(new == NR4STDISC) {
			if(cb->dreason == NR4RRESET)	/* k35 */
				noprompt = 1;
			printf(" (%s)\n", Nr4reasons[cb->dreason]) ;
			cmdmode();
		} else
			printf("\n") ;
		fflush(stdout);
	}
	if(new == NR4STDISC){
		cb->puser = NULLCHAR;
		freesession(s);
	}
}

/* Handle typed characters on a NET/ROM connection */
void
nr4_parse(buf,cnt)
char *buf;
int16 cnt;
{
	struct mbuf *bp;
	int16 size;
	void term_send();

	if(current == NULLSESSION || current->type != NRSESSION)
		return;	/* "can't happen" */

	/* Parse it out, splitting at transport frame boundaries */

	while (cnt != 0) {
#ifdef NRDEBUG
		printf("Once around the parse loop - cnt = %d\n", cnt) ;
#endif
		size = min(cnt, NR4MAXINFO) ;
		if ((bp = alloc_mbuf(size)) == NULLBUF)
			break ;
		term_send(bp,buf,size,0);
		cnt -= size ;
		send_nr4(current->cb.nr4_cb,bp);
	}
}

/* Handle new incoming terminal sessions
 * This is the default state change upcall function, used when
 * someone else connects to us
 */
void
nr4_incom(cb,oldstate,newstate)
struct nr4cb *cb ;
int oldstate ;
int newstate ;
{
	void nr4_session() ;
#ifdef AXMBX
	void mbx_nr4incom();
	extern int ax25mbox;
#endif

	if (newstate != NR4STCON)		/* why are you bothering us? */
		return ;					/* (shouldn't happen) */

#ifdef AXMBX
	if (ax25mbox)
		mbx_nr4incom(cb) ;
	else
#endif
		nr4_session(cb) ;
	return ;

}

/* This function sets up a NET/ROM chat session */
void
nr4_session(cb)
struct nr4cb *cb ;
{
	struct session *s;
#ifdef POLITE
	struct session *tmpsession;
#endif
	char remote[10];
	extern struct session *current;
	int go();
#ifdef AX_MOTD
	extern char ax_motd[];
#endif

	pax25(remote,&cb->user);
	if((s = newsession()) == NULLSESSION){
		/* Out of sessions */
		disc_nr4(cb);
		return;
	}
	s->type = NRSESSION ;
	s->name = malloc((int16)strlen(remote)+1);
	s->cb.nr4_cb = cb ;
	s->parse = (int(*)())nr4_parse;
	strcpy(s->name,remote);
	cb->r_upcall = nr4_rx;
	cb->s_upcall = nr4_state;
	cb->t_upcall = nr4_tx;
	cb->puser = (char *)s;
	printf("\007Incoming NET/ROM session %u from %s\n",s - sessions,remote);
	fflush(stdout);
#ifdef POLITE
	tmpsession = current; /* may need this later */
#endif
	current = s;    /* this may be only temporary if we are busy */
#ifdef AX_MOTD
	if(ax_motd[0])
		nr4_parse(ax_motd,(int16)strlen(ax_motd));
#endif
#ifdef POLITE
	if((s - sessions) == 0) /* if not busy, */
		go();
	else
		current = tmpsession;	/* back to what we were doing */
#else
	go();
#endif
}

/* Handle incoming terminal traffic */
void
nr4_rx(cb,cnt)
struct nr4cb *cb ;
int16 cnt;
{
	register struct mbuf *bp;
	void term_recv();
#ifdef FLOW
	extern int ttyflow;	/* k34 */
#endif

	/* Hold output if we're not the current session or typing a line */
	if(mode != CONV_MODE || current == NULLSESSION
#ifdef FLOW
		|| !ttyflow /* blocked by keyboard input */
#endif
	 || current->type != NRSESSION || current->cb.nr4_cb != cb)
		return;

	if((bp = recv_nr4(cb,cnt)) == NULLBUF)
		return;
	term_recv(bp,current->record);
}

/* Handle transmit upcalls. Used only for file uploading */
void
nr4_tx(cb,cnt)
struct nr4cb *cb ;
int16 cnt;
{
	register char *cp;
	struct session *s;
	register struct mbuf *bp;
	int16 size;
	int c;
#ifdef SYS5
	struct stat ss_buf;
#endif

	if((s = (struct session *)cb->puser) == NULLSESSION
	 || s->upload == NULLFILE)
		return;
	while(cnt != 0){
		size = min(cnt,NR4MAXINFO);
		if((bp = alloc_mbuf(size)) == NULLBUF)
			break;
		cp = bp->data;

		/* Now send data characters, translating between local
		 * keyboard end-of-line sequences and the (unwritten)
		 * AX.25 convention, which is carriage-return only
		 */

		while(bp->cnt < size){
			if((c = getc(s->upload)) == EOF)
				break;
/* twiddling here is to convert from native file EOL */
#if defined(MSDOS) || defined(_OSK)
			/* MS-DOS gives cr-lf */
			if(c == '\012')
				continue;
#endif
#ifdef UNIX
			/* These give lf only so we convert */
			if(c == '\012')
				c = '\015';
#endif
			*cp++ = c;
			bp->cnt++;
		}
		cnt -= bp->cnt;

		if (bp->cnt != 0)	/* might happen with a newline at EOF */
			send_nr4(cb,bp);
		else
			free_p(bp) ;
			
		if (c == EOF)
			break ;
	}
	if(cnt != 0){
		/* Error or end-of-file */
#if defined(_OSK) && !defined(_UCC)
		tmpclose(s->upload);
#else
#ifdef SYS5
	/* Note that this area is VERY Unix system-dependent.  See note in
	 * ax25cmd.c for alternate method if your fstat can't detect a pipe.
	 * Note also that I included ionode.h, not types.h.
	 */
		if(fstat(fileno(s->upload),&ss_buf) < 0)
			perror("nr4_tx");
#ifdef COH386
		if ((ss_buf.st_mode & S_IFPIP) == S_IFPIP)
#else
		if((ss_buf.st_mode & IFIFO) == IFIFO)
#endif
			pclose(s->upload);  /* close pipe from dir */
		else
#endif	/* SYS5 */
		fclose(s->upload);
#endif	/* OSK */
		s->upload = NULLFILE;
		free(s->ufile);
		s->ufile = NULLCHAR;
	}
}

struct nr4cb *
simple_getnr(arg)
int arg;
{
	int i,j;
	struct nr4cb *cb;
	
	for(i=0,j=1; i < NR4MAXCIRC; i++){
		if((cb = Nr4circuits[i].ccb) == NULLNR4CB)
			continue ;
		if(j++ == arg)
			return cb;
	}
	return NULLNR4CB;
}

/* Reset a net/rom connection abruptly */

static int
donrreset(argc,argv)
int argc;
char *argv[];
{
	struct nr4cb *cb ;

	if((cb = simple_getnr(atoi(argv[1]))) == NULLNR4CB)
	/* don't think we need to test: nr4valcb(cb) */
		return -1;
	else
		reset_nr4(cb);
	return 0;
}

/* Force retransmission on a net/rom connection */

static int
donrkick(argc,argv)
int argc;
char *argv[];
{
	struct nr4cb *cb ;

	if((cb = simple_getnr(atoi(argv[1]))) == NULLNR4CB || kick_nr4(cb) == -1)
		return -1;
	return 0;
}

/* netrom transport ACK delay timer */

static
donracktime(argc, argv)
int argc ;
char *argv[] ;
{
	long val ;

	if (argc < 2)
		printf("%lu\n", Nr4acktime);
	else	/* note that 3000 is default */
		if((val = atol(argv[1])) < 2000l)	/* could constrain this further */
			return -1;
		else
			Nr4acktime = val ;

	return 0 ;
}

/* netrom transport choke timeout */

static
donrchoketime(argc, argv)
int argc ;
char *argv[] ;
{
	long val ;

	if(argc < 2)
		printf("%lu\n", Nr4choketime);
	else	/* 180,000 is default here */
		if((val = atol(argv[1])) < 60000l)
			return -1;
		else
			Nr4choketime = val;
	return 0 ;
}

/* netrom transport initial round trip time */

static
donrirtt(argc, argv)
int argc ;
char *argv[] ;
{
	long val ;

	if (argc < 2)
		printf("%lu\n", Nr4irtt);
	else
		if((val = atol(argv[1])) < 15000l)	/* arbitrary */
			return -1;
		else
			Nr4irtt = val ;
	return 0 ;
}

/* netrom transport receive queue length limit.  This is the */
/* threshhold at which we will CHOKE the sender. */

static
donrqlimit(argc, argv)
int argc ;
char *argv[] ;
{
	unsigned val ;

	if (argc < 2)
		printf("%u\n", Nr4qlimit);
	else
		if((val = atoi(argv[1])) < 128)	/* arbitrary, little over one line */
			return -1;
		else
			Nr4qlimit = val;
	return 0 ;
}

/* netrom transport maximum window.  This is the largest send and */
/* receive window we may negotiate */

static
donrwindow(argc, argv)
int argc ;
char *argv[] ;
{
	unsigned val ;

	if(argc < 2)
		printf("%u\n", Nr4window);
	else
		if((val = atoi(argv[1])) < 1 || val > NR4MAXWIN)
			return -1;
		else
			Nr4window = val ;
	return 0 ;
}

/* netrom transport maximum retries.  This is used in connect and */
/* disconnect attempts; I haven't decided what to do about actual */
/* data retries yet. */

static
donrretries(argc, argv)
int argc ;
char *argv[] ;
{
	unsigned val ;

	if(argc < 2)
		printf("%u\n", Nr4retries);
	else
		if((val = atoi(argv[1])) < 1 || val > 16)	/* arbitrary */
			return -1;
		else
			Nr4retries = val ;

	return 0 ;
}

/* Display the status of NET/ROM connections */

static
donrstatus(argc, argv)
int argc ;
char *argv[] ;
{
	int i ;
	struct nr4cb *cb ;
	char luser[10], ruser[10], node[10] ;
	void donrdump() ;

	if (argc < 2){
		printf(" CB# Snd-W Snd-Q Rcv-Q     LUser      RUser @Node     State\n");
		for (i=1;;i++){
			if((cb = simple_getnr(i)) == NULLNR4CB)
				break;
			pax25(luser,&cb->luser) ;
			pax25(ruser,&cb->user) ;
			pax25(node,&cb->node) ;
			printf(" %2d%7d%6d%6d%10s%11s %-9s %s\n",
				   i, cb->nbuffered, len_q(cb->txq),
				   len_mbuf(cb->rxq), luser, ruser, node,
				   Nr4states[cb->state]) ;
		}
	}else{
		if((cb = simple_getnr(atoi(argv[1]))) == NULLNR4CB)
		/* don't think we need to check: nr4valcb(cb) */
			return -1;
		else
			donrdump(cb);
	}
	return 0 ;
}

/* Dump one control block */

static void
donrdump(cb)
struct nr4cb *cb ;
{
	char luser[10], ruser[10], node[10] ;
	unsigned seq ;
	struct nr4txbuf *b ;
	struct timer *t ;

	pax25(luser,&cb->luser) ;
	pax25(ruser,&cb->user) ;
	pax25(node, &cb->node) ;

	printf("Local: %s %d/%d Remote: %s @ %s %d/%d State: %s\n",
		   luser, cb->mynum, cb->myid, ruser, node,
		   cb->yournum, cb->yourid, Nr4states[cb->state]) ;

	printf("Window: %-5u Rxpect: %-5u RxNext: %-5u RxQ: %-5d %s\n",
			cb->window, (unsigned)uchar(cb->rxpected),
			(unsigned)uchar(cb->rxpastwin),
		   len_mbuf(cb->rxq), cb->qfull ? "RxCHOKED" : "") ;

	printf(" Unack: %-5u Txpect: %-5u TxNext: %-5u TxQ: %-5d %s\n",
			cb->nbuffered, (unsigned)uchar(cb->ackxpected),
			(unsigned)uchar(cb->nextosend),
		   len_q(cb->txq), cb->choked ? "TxCHOKED" : "") ;

	printf("TACK: ") ;
	if (run_timer(&cb->tack))
		printf("%lu", (cb->tack.start - cb->tack.count) * MSPTICK) ;
	else
		printf("stop") ;
	printf("/%lu ms; ", cb->tack.start * MSPTICK) ;

	printf("TChoke: ") ;
	if (run_timer(&cb->tchoke))
		printf("%lu", (cb->tchoke.start - cb->tchoke.count) * MSPTICK) ;
	else
		printf("stop") ;
	printf("/%lu ms; ", cb->tchoke.start * MSPTICK) ;

	printf("TCD: ") ;
	if (run_timer(&cb->tcd))
		printf("%lu", (cb->tcd.start - cb->tcd.count) * MSPTICK) ;
	else
		printf("stop") ;
	printf("/%lu ms", cb->tcd.start * MSPTICK) ;

	if (run_timer(&cb->tcd))
		printf("; Tries: %u\n", cb->cdtries) ;
	else
		printf("\n") ;

	printf("Backoff Level %u SRTT %ld ms Mean dev %ld ms\n",
		   cb->blevel, cb->srtt, cb->mdev) ;

	/* If we are connected and the send window is open, display */
	/* the status of all the buffers and their timers */
	
	if (cb->state == NR4STCON && cb->nextosend != cb->ackxpected) {

		printf("TxBuffers:  Seq  Size  Tries  Timer\n") ;

		for (seq = cb->ackxpected ;
			 nr4between(cb->ackxpected, seq, cb->nextosend) ;
			 seq = (seq + 1) & NR4SEQMASK) {

			b = &cb->txbufs[seq % cb->window] ;
			t = &b->tretry ;

			printf("%15u%6d%7d  %lu/%lu\n",
				   seq, len_mbuf(b->data), b->retries + 1,
				   (t->start - t->count) * MSPTICK, t->start * MSPTICK) ;
		}

	}

}
#endif	/* NETROM */
