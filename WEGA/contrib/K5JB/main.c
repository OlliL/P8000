/* Main metwork program - provides both client and server functions
 * See readme.src for modification notes - K5JB
 * #define CPUSTAT to measure loop performance - adds 768 bytes
 */

#define HOSTNAMELEN 64

#include <stdio.h>
#include <string.h>
#include "options.h"
#include "config.h"
#include "sokname.h"
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "timer.h"
#include "icmp.h"
#include "iface.h"
#include "ip.h"
#include "tcp.h"
#ifdef AX25
#include "ax25.h"
#include "lapb.h"
#endif
#ifdef NETROM
#include "netrom.h"
#include "nr4.h"
#endif
#include "ftp.h"
#include "telnet.h"
#include "remote.h"
#ifdef _FINGER
#include "finger.h"
#endif
#include "session.h"
#include "cmdparse.h"

#if defined(MSDOS) || defined(ASY)
#include "asy.h"
#endif

#ifdef  ASY
#include "slip.h"
#endif

#ifdef  NRS
#include "nrs.h"
#endif

#ifdef UNIX             /* BSD or SYS5 */
#include "unix.h"
#include "unixopt.h"
#endif

#ifdef  TRACE
#include "trace.h"
/* Dummy structure for loopback tracing */
extern struct interface loopback;
extern char howtotrace[];
#endif

#if defined(MSDOS) && defined(CUTE_VIDEO)
unsigned int saved_attrib;
unsigned int ourcute = OUR_CUTE;	/* we select default from options.h */
int doattrib();
unsigned int get_attr();
void putca(),putsa();
#endif

#ifdef AX25
extern int16 paclen;
#endif
extern struct interface *ifaces;
extern void version(),fileinit();
extern struct mbuf *loopq;
extern FILE *trfp;
extern char trname[];
int mode,atoi(),exitval;	/* used by doexit() */

extern char *homedir,*mailspool,*startup;

char badhost[] = "Unknown host %s\n";
char hostname[HOSTNAMELEN];
unsigned nsessions = NSESSIONS;
int32 resolve();
char *strncpy(),*make_path();
int16 lport = 1024;
char prompt[] = "NET> ";
char nospace[] = "No space!!\n";	/* Generic malloc fail message */

#ifdef  SYS5
#include <signal.h>
#ifdef USE_QUIT
int backgrd = 0;	/* backgrd is controlled by SIGQUIT */
#endif
#ifdef FORKSHELL
extern int shellpid;
#endif
int background = 0;	/* background is detected on startup */
int io_active = 0;
#endif

/* PC uses F-10 key always */
#if !defined(MSDOS)
char escape = 0x1d;      /* default escape character is ^] */
#endif

/* Enter command mode */
int
cmdmode()
{
	void cooked();
	extern void echo();

#if defined(SYS5) && defined(FORKSHELL)
	if(shellpid)
		return 0;
#endif
	if(mode != CMD_MODE){
		mode = CMD_MODE;
		cooked();
#ifdef CUTE_FTP
		echo();	/* one in a million chance... */
#endif
	}
	printf(prompt);	/* note change to always print a prompt */
	fflush(stdout);
	return 0;
}

int	/* removed static to make this the only sane exit from program */
doexit()	/* left declaration as int to match cmd list */
{
	void iostop(),exit();
#ifdef TRACE
	if (trfp != stdout)
		fclose(trfp);
#endif
#ifdef  SYS5
	if (!background)
		iostop();	/* this function will exit() */
#else			/* it is also a signal handler */
	iostop();
#endif

	exit(exitval);	/* only rational exit in NET */
}

static
dohostname(argc,argv)
int argc;
char *argv[];
{

	if(argc < 2)
		printf("%s\n",hostname);
	else
		strncpy(hostname,argv[1],HOSTNAMELEN);
	return 0;
}

static
int
dohelp()
{
	extern struct cmds cmds[];
	register struct cmds *cmdp;
	int i;

	printf("Main commands:");	/* first command is "" and will do \n */
	for(i=0,cmdp = cmds;cmdp->name != NULLCHAR;cmdp++,i++){
		printf("%-15s",cmdp->name);
		if(!(i % 5))
			printf("\n");
	}
	printf("\n");
	return 0;
}

int subcmd();

/* Attach an interface
 * Syntax: attach <hw type> <I/O address> <vector> <mode> <label> <bufsize>
		<speed>
 */
doattach(argc,argv)
int argc;
char *argv[];
{
	extern struct cmds attab[];

	return subcmd(attab,argc,argv);
}

#ifdef MSDOS
unsigned heapsize;	/* used only on pc with grabcore (see pc.c) */
#endif

#ifdef CPUSTAT
/* cpu performance statistics - This was added to instrument code that
 * is either on a busy or slow machine, or using a processor intensive
 * driver like the DRSI thing.  It will show if the program is running
 * fast enough to service interface chains effectively.
 * syntax: cpu [time], where time, in seconds, would change from the
 * default of 600 second intervals for cycle accumulation.
 * This will not be documented in the user documentation.  Joe, K5JB
 */
time_t cycles[7];	/* cycle accumulators, ten minutes each */
time_t next_update;
int last_cycle;
time_t update_interval = 600L;

void
cycle(first)
int first;
{
	int i;
	time_t curr_time;

	/* first count will be short on account of time() granularity */
	if(first){
		for(i=0;i<7;i++)
			cycles[i] = 0L;
		time(&next_update);
		next_update += update_interval;
		return;
	}

	/* every 16th time through, check time */
	if((cycles[0]++ & 0x0F) == 0x0F){
		time(&curr_time);
		if(curr_time < next_update)
			return;
	} else
		return;

	/* Do it this way, rather than absolute time, to detect dormant periods */
	next_update += update_interval;
	for(i=6;i>0;i--)
		cycles[i] = cycles[i-1] == 16 ? 0 : cycles[i-1]; /* "16" when dormant */
	cycles[0] = 0L;
}

int
report_cycles(argc,argv)
int argc;
char **argv;
{
	time_t now;
	int i;
	extern time_t next_update, update_interval;
	extern time_t cycles[];
#ifdef COMBIOS	/* this is temporary */
	extern long combios_stat[];
#endif
	if(argc == 2 && (i = atoi(argv[1])) != 0){
		update_interval = (time_t)i;
		cycle(1);
#ifdef COMBIOS	/* this is temporary */
		combios_stat[0] = combios_stat[1] = 0;
#endif
		printf("Update interval changed to %ld sec.\n",update_interval);
		return 0;
	}
	now = update_interval/60L; /* use this temporarily */
	printf("Cycles per %ld %s. through main loop, least to most recent:\n",
	    now > 0 ? now : update_interval, now > 0 ? "min" : "sec");
	for(i=6;i>=1;i--)	/* don't report current accumulater */
		printf("%8ld\n",cycles[i]);
	time(&now);
	if(next_update > now)	/* prevent showing when late */
		printf("%d seconds before next update\n",(int)(next_update - now));
#ifdef COMBIOS	/* more temporary */
	printf("Combios 1 = %lu, Combios 2 - %lu\n",combios_stat[0],combios_stat[1]);
#endif
	return 0;	/* to be consistent with other commands */
}
#endif	/* CPUSTAT */

#define PARAM_MTU
/* Manipulate I/O device parameters */
doparam(argc,argv)
int argc;
char *argv[];
{
	register struct interface *ifp;

	for(ifp=ifaces;ifp != NULLIF;ifp = ifp->next){
		if(strcmp(argv[1],ifp->name) == 0)
			break;
	}
	if(ifp == NULLIF){
		printf("Interface \"%s\" unknown\n",argv[1]);
		return 1;
	}
#if defined(AX25) && defined(PARAM_MTU)
	/* note that there is no error msg if you use a letter other than "m" */
	/* and ioctl will get some astonishing, but benign, numbers k33 */
	if(argv[2][0] == 'm'){	/* "mtu" (or "mut", heh!) */
		if(argc == 4){
			ifp->mtu = (int16)atoi(argv[3]);
			if(!ifp->mtu)	/* bad argument, we'll do it for him */
				ifp->mtu = paclen;
		}
		printf("%s mtu is %d\n",argv[1],ifp->mtu);
		return 0;
	}
#endif

	if(ifp->ioctl == NULLFP){	/* e.g. netrom interface */
		printf("No ioctl\n");
		return 1;
	}

	/* Pass rest of args to device-specific code */
	return (*ifp->ioctl)(ifp,argc-2,argv+2);
}


/* Command lookup and branch table */
int go(),doax25(),doconnect(),dotelnet(),doclose(),doreset(),dotcp(),
dotrace(),doescape(),doroute(),doip(),doarp(),dosession(),doftp(),
dostart(),dostop(),dosmtp(),doudp(),dodump(),dorecord(),doupload(),
dokick(),domode(),doshell(),dodir(),dohelp1(),dolog(),domail(),
#ifdef AXMBX
dombox(),
#endif
#ifdef FORWARD
doforward(),
#endif
#ifdef MULPORT
mulport(),
#endif
#if defined(REWRITE) && defined(REWRITECMD)
dorewrite(),
#endif
docd(),doatstat(),doping(),doremote();

#ifdef NETROM
int donetrom();
#ifdef NRS
int donrstat();
#endif
#endif
#ifndef UNIX
int memstat();
#endif

#ifdef ETHER
int doetherstat();
#endif
#ifdef DRSI
int dodrstat();
#endif
#ifdef _FINGER
int dofinger();
#endif
#ifdef _TELNET
int doecho();
int doeol();
#endif
#ifdef SOKNAME
int issok = 1;

/* reverse state of socket switch, this command is an exception in always
 * being verbose.
 */
int
flipsok()	/* int to be consistent with rest of commands */
{
	issok = !issok;
	printf("Socket names are %s\n",issok ? "on" : "off");
	return(0);
}
#endif

#if defined(TN_MOTD) || defined(AX_MOTD)
int motd();
#endif
#ifdef AX25
extern char badcall[];
#endif

struct cmds cmds[] = {
	/* The "go" command must be first */
	"",             go,             0, NULLCHAR,    NULLCHAR,
#ifdef SHELL
	"!",            doshell,        0, NULLCHAR,    NULLCHAR,
#endif
#if     (defined(AX25) || defined(ETHER)) /* K5JB */
	"arp",          doarp,          0, NULLCHAR,    NULLCHAR,
#endif
#ifdef  AX25
	"ax25",         doax25,         0, NULLCHAR,    NULLCHAR,
#endif
#ifdef AX_MOTD
	"axmotd",	motd,	0,	NULLCHAR,	NULLCHAR,
#endif
	"attach",       doattach,       2,
	"attach <hardware> <hw specific options>", NULLCHAR,
	/* This one is out of alpabetical order to allow abbreviation to "c" */
#ifdef  AX25
	"connect",      doconnect,      3,"connect <interface> <callsign> [digipeaters]",
	badcall,
#endif
	"cd",           docd,           0, NULLCHAR,    NULLCHAR,
	"close",        doclose,        0, NULLCHAR,    NULLCHAR,
#ifdef CPUSTAT
	"cpustat",	report_cycles,	0, NULLCHAR, NULLCHAR,
#endif
#ifdef AX25
	"disconnect",   doclose,        0, NULLCHAR,    NULLCHAR,
#endif
	"dir",          dodir,          0, NULLCHAR,    NULLCHAR,
#ifdef	DRSI
	"drsistat",		 dodrstat,       0, NULLCHAR,    NULLCHAR,
#endif
#ifdef _TELNET
	"echo",         doecho,         0, NULLCHAR,    "echo [refuse|accept]",
	"eol",          doeol,          0, NULLCHAR,
	"eol options: unix, standard",
#endif
#if !defined(MSDOS)
	"escape",       doescape,       0, NULLCHAR,    NULLCHAR,
#endif
#ifdef  PC_EC
	"etherstat",    doetherstat,    0, NULLCHAR,    NULLCHAR,
#endif  /* PC_EC */
	"exit",         doexit,         2, "exit please",    NULLCHAR,
#ifdef _FINGER
	"finger",       dofinger,       0, NULLCHAR, NULLCHAR,
#endif
#ifdef FORWARD
	"forward",      doforward,      0, NULLCHAR,    NULLCHAR,
#endif
	"ftp",          doftp,          2, "ftp <address>",     NULLCHAR,
	"help",         dohelp1,        0, NULLCHAR,    NULLCHAR,
	"hostname",     dohostname,     0, NULLCHAR,    NULLCHAR,
	"ip",           doip,           0, NULLCHAR,    NULLCHAR,
	"kick",         dokick,         0, NULLCHAR,    NULLCHAR,
	"log",          dolog,          0, NULLCHAR,    NULLCHAR,
#ifndef UNIX
	"memstat",      memstat,        0, NULLCHAR,    NULLCHAR,
#endif
	"mailscan",		domail,         0, NULLCHAR, "Usage: mailscan [<mailfile>] [<skip pages>]",
#ifdef  AX25
#ifdef AXMBX
	"mbox",         dombox,         0, NULLCHAR,    NULLCHAR,
#endif
	"mode",         domode,         2, "mode <interface>",  NULLCHAR,
#ifdef TN_MOTD
	"motd",			 motd,			  0, NULLCHAR,	   NULLCHAR,
#endif
#ifdef MULPORT
	"mulport",      mulport,        2, "mulport <on|off>",  NULLCHAR,
#endif
#ifdef  NETROM
	"netrom",       donetrom,       0, NULLCHAR,    NULLCHAR,
#ifdef  NRS
	"nrstat",       donrstat,       0, NULLCHAR,    NULLCHAR,
#endif
#endif /* NETROM */
#endif /* AX25 */
#ifdef PARAM_MTU
	"param",        doparam,        2, "param <interface> mtu | <kiss data>", NULLCHAR,
#else
	"param",        doparam,        2, "param <interface> mtu", NULLCHAR,
#endif
	"ping",         doping,         0, NULLCHAR,    NULLCHAR,
#ifdef UNIX /* BSD or SYS5 */
	"pwd",          docd,           0, NULLCHAR,    NULLCHAR,
#endif
	"record",       dorecord,       0, NULLCHAR,    NULLCHAR,
	"remote",       doremote,       4, "remote <address> <port> <command>",
	NULLCHAR,
	"reset",        doreset,        0, NULLCHAR,    NULLCHAR,
	"route",        doroute,        0, NULLCHAR,    NULLCHAR,
#if defined(REWRITE) && defined(REWRITECMD)
	"rewrite",	dorewrite,	2,	"rewrite <to>",	NULLCHAR,
#endif
	"session",      dosession,      0, NULLCHAR,    NULLCHAR,
#ifdef SHELL
	"shell",        doshell,        0, NULLCHAR,    NULLCHAR,
#endif
	"smtp",         dosmtp,         0, NULLCHAR,    NULLCHAR,
#ifdef SOKNAME
	"sokname",      flipsok,        0, NULLCHAR,    NULLCHAR,
#endif
#ifdef  SERVERS
	"start",        dostart,        2, "start <servername>",NULLCHAR,
	"stop",         dostop,         2, "stop <servername>", NULLCHAR,
#endif
	"tcp",          dotcp,          0, NULLCHAR,    NULLCHAR,
#ifdef _TELNET
	"telnet",       dotelnet,       2, "telnet <address>",  NULLCHAR,
#endif
#ifdef  TRACE
	"trace",        dotrace,        0, NULLCHAR, howtotrace,
#endif
	"udp",          doudp,          0, NULLCHAR,    NULLCHAR,
#if defined(_TELNET) || defined(AX25)
	"upload",       doupload,       0, NULLCHAR,    NULLCHAR,
#endif
#if defined(MSDOS) && defined(CUTE_VIDEO)
	"vattrib",    doattrib,       0,      NULLCHAR,       NULLCHAR,
#endif
	"?",            dohelp,         0, NULLCHAR,    NULLCHAR,
	NULLCHAR,       NULLFP,         0,
	"Unknown command; type \"?\" for list",   NULLCHAR
};

#ifdef  SERVERS
/* "start" and "stop" subcommands */
int dis1(),echo1(),ftp1(),smtp1(),tn1(),rem1();

#if defined(UNIX) && defined(TELUNIX)
int tnix0(),tnix1();
#endif

#if defined(UNIX) && defined(TELSERV)
int tserv0(),tserv1();
#endif

#ifdef _FINGER
int finger1();
#endif

static struct cmds startcmds[] = {
	"discard",      dis1,           0, NULLCHAR, NULLCHAR,
	"echo",         echo1,          0, NULLCHAR, NULLCHAR,
#ifdef _FINGER
	"finger",       finger1,        0, NULLCHAR, NULLCHAR,
#endif
	"ftp",          ftp1,           0, NULLCHAR, NULLCHAR,
	"smtp",         smtp1,          0, NULLCHAR, NULLCHAR,
#ifdef _TELNET
	"telnet",       tn1,            0, NULLCHAR, NULLCHAR,
#endif
#if defined(UNIX) && defined(TELUNIX)
	"telunix",      tnix1,          0, NULLCHAR,
	"Could not start telunix - no pty?",
#endif
#if defined(UNIX) && defined(TELSERV)
	"telserv",      tserv1,          0, NULLCHAR,
	"Could not start telserv - no server?",
#endif
	"remote",       rem1,           0, NULLCHAR, NULLCHAR,
	NULLCHAR,       NULLFP,         0,
	STARTOPT,	NULLCHAR		/* macro defined in config.h */
};

int ftp_stop(),smtp_stop(),echo_stop(),dis_stop(),tn_stop();
int dis0(),echo0(),ftp0(),smtp0(),tn0(),rem0();

#ifdef _FINGER
int finger0();
#endif

static struct cmds stopcmds[] = {
	"discard",      dis0,           0, NULLCHAR, NULLCHAR,
	"echo",         echo0,          0, NULLCHAR, NULLCHAR,
#ifdef _FINGER
	"finger",       finger0,        0, NULLCHAR, NULLCHAR,
#endif
	"ftp",          ftp0,           0, NULLCHAR, NULLCHAR,
	"smtp",         smtp0,          0, NULLCHAR, NULLCHAR,
#ifdef _TELNET
	"telnet",       tn0,            0, NULLCHAR, NULLCHAR,
#endif
#if defined(UNIX) && defined(TELUNIX)
	"telunix",      tnix0,          0, NULLCHAR,
	"Stop telunix failed, no server running",
#endif
#if defined(UNIX) && defined(TELSERV)
	"telserv",      tserv0,          0, NULLCHAR,
	"Stop telserv failed, no server running",
#endif
	"remote",       rem0,           0, NULLCHAR, NULLCHAR,
	NULLCHAR,       NULLFP,         0,
	STOPOPT, NULLCHAR		/* macro in config.h */
};
#endif /* SERVERS */

/* Standard commands called from main */

/* Configuration-dependent code */

/* List of supported hardware devices */
int asy_attach(),pc_attach();
#ifdef NETROM
int nr_attach();
#endif

#ifdef MSDOS
#ifdef DRSI
int dr_attach();
#endif
#ifdef  PC_EC
int ec_attach();
#endif
#ifdef COMBIOS
int combios_attach();
#endif
#endif /* MSDOS */

#ifdef MDKISSPORT
int mdasy_attach();
#endif

#ifdef  PACKET
int pk_attach();
#endif

struct cmds attab[] = {
#if defined(PC_EC) && defined(MSDOS)
	/* 3-Com Ethernet interface */
	"3c500", ec_attach, 7,
	"attach 3c500 <address> <vector> arpa <label> <buffers> <mtu>",
	"Could not attach 3c500",
#endif
#ifdef ASY
	/* Ordinary (or extended) PC asynchronous adaptor */
	"asy", asy_attach, 8,
	ATTACHOPT,	/* defined in options.h */
	"Could not attach asy",
#endif /* ASY */

#if defined(DRSI) && defined(MSDOS)
	"drsi", dr_attach, 8,
	"attach drsi <address> <vector> ax25 dr0|dr1 <buffers> <mtu> <speed>",
	"Could not attach DRSI",
#endif

#if defined(COMBIOS) && defined(MSDOS)
	"combios", combios_attach, 7,
	"attach combios <com#> ax25 <label> <buffers> <mtu> <speed>",
	"Couldn't attach combios",
#endif

#ifdef NETROM
	/* fake netrom interface */
	"netrom", nr_attach, 1,
	"attach netrom",
	"Could not attach netrom",
#endif
#ifdef  PACKET
	/* FTP Software's packet driver spec */
	"packet", pk_attach, 4,
#ifdef PACKET_POLL
	"attach packet <int#> <label> <buffers> <mtu> [-p]",
#else
	"attach packet <int#> <label> <buffers> <mtu>",
#endif
	"Could not attach packet driver",
#endif
	NULLCHAR, NULLFP, 0,  "Unknown device",  NULLCHAR
};

#ifdef  ASY

/* Attach a serial interface to the system
 * argv[0]: hardware type, must be "asy"
 * argv[1]: I/O address, e.g., "0x3f8"
 * argv[2]: vector, e.g., "4"
 * argv[3]: mode, may be:
 *          "slip" (point-to-point SLIP)
 *          "ax25" (AX.25 frame format in SLIP for raw TNC)
 *          "nrs" (NET/ROM format serial protocol)
 * argv[4]: interface label, e.g., "sl0"
 * argv[5]: receiver ring buffer size in bytes
 * argv[6]: maximum transmission unit, bytes
 * argv[7]: interface speed, e.g, "9600"
 * argv[8]: optional ax.25 callsign (NRS only)
 */
int
asy_attach(argc,argv)
int argc;
char *argv[];
{
	register struct interface *if_asy;
	extern struct interface *ifaces;
	int16 dev;
	int mode,asy_init(),asy_send(),asy_ioctl(),asy_stop(),ax_send(),ax_output();
	int kiss_raw(),kiss_ioctl(),slip_send(),slip_raw(),atoi();
	void kiss_recv(),doslip(),slip_recv(),asy_speed(),free();

#ifdef  AX25
	extern int16 tcp_mss;
#ifdef NRS
	struct ax25_addr addr ;
#endif
	void axarp();
#endif
	int ax_send(),ax_output(),nrs_raw(),asy_ioctl();
	void nrs_recv();

	if(nasy >= ASY_MAX){
		printf("Too many asynch controllers\n");
		return -1;
	}
	if(strcmp(argv[3],"slip") == 0)
		mode = SLIP_MODE;
#ifdef AX25
	else if(strcmp(argv[3],"ax25") == 0)
#ifdef MDKISSPORT
		return mdasy_attach(argc,argv);	/* See mdkiss.c - we're done */
#else
	mode = AX25_MODE;
#endif
#endif
#ifdef NRS
	else if(strcmp(argv[3],"nrs") == 0)
		mode = NRS_MODE;
#endif
	else{
		printf("Mode %s unknown for interface %s\n",argv[3],argv[4]);
		return(-1);
	}

	dev = nasy++;

	/* Create interface structure and fill in details */
	if_asy = (struct interface *)calloc(1,sizeof(struct interface));
	if_asy->name = malloc((unsigned)strlen(argv[4])+1);
	strcpy(if_asy->name,argv[4]);
	if_asy->mtu = (int16)atoi(argv[6]);
#if defined(AX25) && !defined(SEGMENT)
	/* If we aren't going to do segmentation, enforce mtu size - K5JB k33 */
	if(mode == AX25_MODE && if_asy->mtu > paclen)
		if_asy->mtu = paclen;
#endif
	if_asy->dev = dev;
	if_asy->recv = doslip;
	if_asy->stop = asy_stop;

	switch(mode){
#ifdef  SLIP
		case SLIP_MODE:
			if_asy->ioctl = asy_ioctl;
			if_asy->send = slip_send;
			if_asy->output = NULLFP;        /* ARP isn't used */
			if_asy->raw = slip_raw;
			if_asy->flags = 0;
			slip[dev].recv = slip_recv;
			break;
#endif
#if defined(AX25) && !defined(MDKISSPORT)
		case AX25_MODE:  /* Set up a SLIP link to use AX.25 */
			axarp();
			if(mycall.call[0] == '\0'){
				printf("set mycall first\n");
				free(if_asy->name);
				free((char *)if_asy);
				nasy--;
				return -1;
			}
			if_asy->ioctl = kiss_ioctl;
			if_asy->send = ax_send;
			if_asy->output = ax_output;
			if_asy->raw = kiss_raw;
			if(if_asy->hwaddr == NULLCHAR)
				if_asy->hwaddr = malloc(sizeof(mycall));
			memcpy(if_asy->hwaddr,(char *)&mycall,sizeof(mycall));
			slip[dev].recv = kiss_recv;
			if(tcp_mss == DEF_MSS)	/* here we change a default - K5JB k34 */
				tcp_mss = 216;
			break;
#endif /* AX25 && MDKISSPORT */
#ifdef NRS
		case NRS_MODE: /* Set up a net/rom serial interface */
			if(argc < 9){
				/* no call supplied? */
				if(mycall.call[0] == '\0'){
					/* try to use default */
					printf("set mycall first or specify in attach statement\n");
					return -1;
				} else
					addr = mycall;
			} else {
				/* callsign supplied on attach line */
				if(setcall(&addr,argv[8]) == -1){
					printf ("bad callsign on attach line\n");
					free(if_asy->name);
					free((char *)if_asy);
					nasy--;
					return -1;
				}
			}
			if_asy->recv = nrs_recv;
			if_asy->ioctl = asy_ioctl;
			if_asy->send = ax_send;
			if_asy->output = ax_output;
			if_asy->raw = nrs_raw;
			if(if_asy->hwaddr == NULLCHAR)
				if_asy->hwaddr = malloc(sizeof(addr));
			memcpy(if_asy->hwaddr,(char *)&addr,sizeof(addr));
			nrs[dev].iface = if_asy;
			if_asy->mtu -= 20;	/* k33 */
			if(if_asy->mtu > 236 || if_asy->mtu < 44)
				if_asy->mtu = 236;	/* Stomp! */
			break;
#endif /* NRS */
	}	/* switch */
	if_asy->next = ifaces;
	ifaces = if_asy;
	asy_init(dev,argv[1],argv[2],(unsigned)atoi(argv[5]));
	asy_speed(dev,atoi(argv[7]));
	return 0;
}
#endif /* ASY */

#ifdef NOTUSED
#ifndef NETROM
#ifdef  AX25
struct ax25_addr nr_nodebc;
#endif
nr_route(bp)
struct mbuf *bp;
{
	free_p(bp);
}

nr_nodercv(bp)
{
	free_p(bp);
}
#endif /* NETROM */
#endif /* NOTUSED */


/* Display or set IP interface control flags */
domode(argc,argv)
int argc;
char *argv[];
{
	register struct interface *ifp;

	for(ifp=ifaces;ifp != NULLIF;ifp = ifp->next){
		if(strcmp(argv[1],ifp->name) == 0)
			break;
	}
	if(ifp == NULLIF){
		printf("Interface \"%s\" unknown\n",argv[1]);
		return 1;
	}
	if(argc < 3){
		printf("%s: %s\n",ifp->name,
		    (ifp->flags & CONNECT_MODE) ? "VC mode" : "Datagram mode");
		return 0;
	}
	switch(argv[2][0]){
	case 'v':
	case 'c':
	case 'V':
	case 'C':
		ifp->flags = CONNECT_MODE;
		break;
	case 'd':
	case 'D':
		ifp->flags = DATAGRAM_MODE;
		break;
	default:
		printf("Usage: %s [vc | datagram]\n",argv[0]);
		return 1;
	}
	return 0;
}

#ifdef SERVERS
dostart(argc,argv)
int argc;
char *argv[];
{
	return subcmd(startcmds,argc,argv);
}
dostop(argc,argv)
int argc;
char *argv[];
{
	return subcmd(stopcmds,argc,argv);
}
#endif /* SERVERS */


#ifndef MSDOS
static
int
doescape(argc,argv)
int argc;
char *argv[];
{
	if(argc < 2)
		printf("0x%x\n",escape);
	else
		escape = *argv[1];
	return 0;
}
#endif /* MSDOS */

static
doremote(argc,argv)
int argc;
char *argv[];
{
	struct socket fsock,lsock;
	struct mbuf *bp;
	void send_udp();

	lsock.address = ip_addr;
	fsock.address = resolve(argv[1]);
	lsock.port = fsock.port = (int16)atoi(argv[2]);
	bp = alloc_mbuf(1);
	if(strcmp(argv[3],"reset") == 0)
		*bp->data = SYS_RESET;	/* only if enabled in pc.c */
		/* in Unix, this will exit(2) */
#ifdef NOTUSED    /* crap, cost 88 bytes to do this */
	else if(strcmp(argv[3],"restart") == 0)
		*bp->data = SYS_RESTART;	/* will exit(1) */
#endif
	else if(strcmp(argv[3],"exit") == 0)
		*bp->data = SYS_EXIT;		/* will exit(1) which will distinguish */
	else {                        /* from a normal exit */
		printf("Unknown command %s\n",argv[3]);
		return 1;
	}
	bp->cnt = 1;
	send_udp(&lsock,&fsock,0,0,bp,0,0,0);
	return 0;
}

void
keep_things_going()
{
	void check_time(), ip_recv(),ntohip();
	struct interface *ifp;
	struct mbuf *bp;
#if defined(MSDOS)
	void giveup();
#endif
#if defined(UNIX) && defined(TELUNIX)
	void tnix_scan();
#endif
#if defined(UNIX) && defined(TELSERV)
	void tserv_scan();
#endif
#ifdef TRACE
	void dump();
#endif

	/* Service the loopback queue */
	if((bp = dequeue(&loopq)) != NULLBUF){
		struct ip ip;
#ifdef  TRACE
		dump(&loopback,IF_TRACE_IN,TRACE_IP,bp);
#endif
		/* Extract IP header */
		ntohip(&ip,&bp);
		ip_recv(&ip,bp,0);
	}
	/* Service the interfaces */
#ifdef  SYS5
	do {
		io_active = 0;
#endif
		for(ifp = ifaces; ifp != NULLIF; ifp = ifp->next){
			if(ifp->recv != NULLVFP)
				(*ifp->recv)(ifp);
		}
#ifdef  SYS5
	} while(io_active);
#endif

	/* Service the clock if it has ticked */
	check_time();

#if defined(MSDOS)
	/* Tell DoubleDos or Desqview to let the other task run for awhile.
	 * If if neither are active, this is a no-op
	 */
	giveup();
#endif /* MSDOS */

#if defined(UNIX) && defined(TELUNIX)
	tnix_scan();	/* this is what eihalt() calls */
#endif
#if defined(UNIX) && defined(TELSERV)
	tserv_scan();	/* check on the telnet server(s) */
#endif
}

main(argc,argv)
int argc;
char *argv[];
{
	char *indent = "     ";
	static char inbuf[BUFSIZ];      /* keep it off the stack */
	char *fgets();
	int cmdparse();
	void check_time(),ip_recv(),ioinit(),check_kbd();
	FILE *fp;
#ifdef  UNIX
	char *getenv();
#endif
#ifdef MSDOS
	void chktasker();
#endif

#ifdef CPUSTAT
	void cycle();
#endif

#ifndef NOCOOKIE
	extern char CMARK[];
	char *vp = CMARK;
	static char *cookie = "!_ (DY]";
	while(*cookie){
		if(!*vp)
			exit(1);
		*vp++ = *cookie++ ^ 'j';
	}
	if(*vp)
		*vp = '\0';
#endif
#ifdef CUTE_VIDEO
#ifdef UNIX	/* Unix or others with curses */
	/* BKW -- W5GFE  - this to let reverse video option work */
	setupterm((char *) 0,1,(int *) 0);
#else
	saved_attrib = get_attr();	/* MSDOS */
#endif
#endif

#ifdef  SYS5
	if (signal(SIGINT, SIG_IGN) == SIG_IGN) {
		background++;
		/*    daemon(); interferes with nohup.out  K5JB  */
	} else
		ioinit();

#else
	ioinit();
#endif

#if defined(MSDOS)
	chktasker();
#endif

#ifdef  SYS5
	if (!background) {
#endif
		printf("%s%sInternet Protocol Package, (C) 1988 by Phil Karn, KA9Q\n",
		    indent,indent);

#ifdef NETROM
		printf("%s%s%sNET/ROM Support (C) 1989 by Dan Frank, W9NK\n\n",
		    indent,indent,indent) ;
#endif
#ifndef AX25
		printf("%s%s%sNote: This version lacks AX.25 functions!\n\n",
		    indent,indent,indent);
#endif

#if defined(MSDOS) && defined(CUTE_VIDEO)
		printf("%s%s%s%s",indent,indent,indent,indent);
		fflush(stdout);
		putsa(" This puppy was housebroken by \n",ourcute);
		printf("%s%s  ",indent,indent);
		fflush(stdout);
		putsa(" Joe Buswell, K5JB, 44.78.0.2, Midwest City OK \n\n",ourcute);
#else
		printf("%s%s%s%s   This puppy was housebroken by\n",
		    indent,indent,indent,indent);
		printf("%s%s%sJoe Buswell, K5JB, 44.78.0.2, Midwest City OK\n\n",
		    indent,indent,indent);
#endif
#ifdef MSDOS
		printf("%s%s%sDESQview or Windows users: Heap = %u\n",indent,indent,
		    indent,heapsize);
#endif /* MSDOS */
#ifdef _OSK
		printf("%sOS9-68k Port, Robert E. Brose II, N0QBJ, 44.94.249.29, St. Paul, MN\n\n",
		    indent);
#endif

		version();  /* do formatting in version.c */

		/* version will include options like Unix, DRSI, MULPORT, etc. */

#ifdef  SYS5
	}
#endif
	fflush(stdout);
	sessions = (struct session *)calloc(nsessions,sizeof(struct session));
	fileinit(argv[1]);	/* added to permit environmental control K5JB */

	/* if SKIP_FSCHECK not defined we could skip this */
	fp = fopen(startup,"r");	/* and permit alternative startup files */
	if(fp != NULLFILE){
		while(fgets(inbuf,BUFSIZ,fp) != NULLCHAR){
#ifdef MSDOS
			if(inbuf[0] == 0x1a)	/* Ctrl-Z - prevent error message */
				break;
#endif
			cmdparse(cmds,inbuf);
		}
		fclose(fp);
	}
	cmdmode(1);

#ifdef CPUSTAT
	cycle(1);	/* initialize cycle counter */
#endif
	/* Main commutator loop */
	for(;;){
#ifdef CPUSTAT
		cycle(0);
#endif
		check_kbd();
		keep_things_going();
	}
}
