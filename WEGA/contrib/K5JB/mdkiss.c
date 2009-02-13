/* Multidrop KISS asy attach, previously called extended KISS but changed to
 * avoid conflict with other uses of word "extended".
 */
/* Extended KISS asy attach.  Backward compatible to old attach asy command,
 * permits multiple, currently 16, virtual kiss ports by adding a number
 * after the speed arg in the attach command.  If a number is given, other
 * than 0, will make a label name consisting of specified label and a number
 * e.g. ax00, ax01, ax02, or simply ax0 ax1 ax2 ...
 * Joe, K5JB, 12 Dec 93
 */

#include "options.h"
#include "config.h"

#ifdef MDKISSPORT
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "mbuf.h"
#include "ax25.h"
#include "slip.h"
#include "asy.h"
#include "iface.h"
#include "kiss.h"
#include "trace.h"
#include "netuser.h" /* needed for tcp.h */
#include "timer.h" /* ditto */
#include "tcp.h"
#if defined(MSDOS) && defined(SERIALTEST)
#include "8250.h"
#endif

#define MAX_PORTS 16	/* can expand this later maybe */
#define MAX_NAMESIZE 3 /* want to protect some fragile code */

void	mdkiss_recv(), doslip(),dump();
int	mdkiss_raw();
int	asy_stop(), ax_send(), ax_output(), kiss_ioctl();
extern int16 paclen,tcp_mss;

/* this is satisfactory for 16 virtual ports */
static char
ntoh(n)
int n;
{
	static char hset[] = "0123456789abcdef";

	if(n > -1 && n < MAX_PORTS)	/* may get a boogered control byte */
		return hset[n];
	else
		return '\0';
}

/*
 * Attach a serial interface to a multiport KISS TNC
 * argv[0]: hardware type, must be "asy"
 * argv[1]: I/O address, e.g., "0x3f8"
 * argv[2]: vector, e.g., "4"
 * argv[3]: mode, will be "ax25" but we don't use it
 * argv[4]: interface label, e.g., "ax0"
 * argv[5]: receiver ring buffer size in bytes
 * argv[6]: maximum transmission unit, bytes
 * argv[7]: interface speed, e.g, "4800"
 * argv[8]: optional kissport number, distinguishes from std attach asy.
 */

int
mdasy_attach(argc,argv)
int argc;
char *argv[];
{
	register struct interface *if_pcx;
	extern struct interface *ifaces;
	int16 dev;
	int axarp(),atoi(),asy_init(),asy_speed();
	int i = 0,j,maxports = 0;

	if(nasy >= ASY_MAX)  {
		printf("Too many asy controllers\n");
		return -1;
	}

	dev = nasy++;
	axarp();

	if(mycall.call[0] == '\0'){
		printf("set mycall first\n");
		nasy--;
		return -1;
	}

	/* Create iface structures and fill in details */
	if(argc > 8)
		maxports = atoi(argv[8]);
	maxports = maxports <= MAX_PORTS ? maxports : MAX_PORTS;

	/* we want to attach at least one port */
	for(;;){
		/* note we are assuming this thing is running with enough memory */
		if_pcx = (struct interface *)calloc(1,sizeof(struct interface));

		j = strlen(argv[4]);
		/* here we constrain name length because some code, like mulport, \
		 * chokes on long names.
		 */
		if(j > MAX_NAMESIZE){	/* will do some bashing */
			j = MAX_NAMESIZE;
			argv[4][j+1] = '\0';
		}
		/* extra char space calloced whether we need it or not */
		if_pcx->name = (char *)calloc(1,(unsigned)(j+2));
		strcpy(if_pcx->name,argv[4]);
		/* here is where we get backward compatibility */
		if(maxports) /* Append number to ifaces */
			if_pcx->name[j] = ntoh(i);
		if_pcx->mtu = (int16)atoi(argv[6]);

#ifndef SEGMENT
		/* If we aren't going to do segmentation, enforce mtu size limit - K5JB */
		 */
		if(if_pcx->mtu > paclen)
			if_pcx->mtu = paclen;
#endif

		if_pcx->dev = dev;
		if_pcx->recv = doslip;
		if_pcx->stop = asy_stop;
		if_pcx->ioctl = kiss_ioctl;
		if_pcx->send = ax_send;
		if_pcx->output = ax_output;
		if_pcx->raw = mdkiss_raw;
		if_pcx->hwaddr = (char *)malloc(sizeof(mycall));
		memcpy(if_pcx->hwaddr,(char *)&mycall,sizeof(mycall));
		if_pcx->kissport = i;

		/* Link it into the iface chain */
		if_pcx->next = ifaces;	/* linked to last iface assigned */
		ifaces = if_pcx;	/* last iface pointer now links to this one */
		if(++i >= maxports)
			break;
	}

	slip[dev].recv = mdkiss_recv;
	if(tcp_mss == DEF_MSS)	/* Help 'em out a little */
		tcp_mss = 216;

	asy_init(dev,argv[1],argv[2],(unsigned)atoi(argv[5]));
	asy_speed(dev,atoi(argv[7]));
	return 0;
}

/* doctor up the control byte and send out with slip_raw */
static int
mdkiss_raw(interface,data)
struct interface *interface;
struct mbuf *data;
{
	register struct mbuf *bp;
	int slip_raw();

	dump(interface,IF_TRACE_OUT,TRACE_AX25,data);

	/* Put type field for KISS TNC on front */
	if((bp = pushdown(data,1)) == NULLBUF){
		free_p(data);
		return(0);
	}

	bp->data[0] = KISS_DATA | (interface->kissport << 4);
	slip_raw(interface,bp);
	return(0);
}

/* We make a local copy of the iface name based on information in the
 * upper nybble of the control byte, and find the interface with if_lookup.
 * Note that with multiport KISS, vast majority of packets are "received"
 * on the last attached pseudo interface, so we use the kissport member as
 * a flag to further investigate which pseudo interface this came from.
 */

void
mdkiss_recv(interface,bp)
struct interface *interface;
struct mbuf *bp;
{
	int	i;
	unsigned char kisstype;
	char tmp[8];
	struct interface *if_lookup();
	int ax_recv();
#if defined(MSDOS) && defined(SERIALTEST)
/* Also done in kiss.c, slip.c and nrs.c to clear error flag possibly
 * set in the async handler
 */
	extern int16 serial_err;        /* defined in ipcmd.c */
	extern int32 tot_rframes;
	struct fifo *fp;

	tot_rframes++;
	fp = &asy[interface->dev].fifo;
	if(fp->error){  /* contains 8250 line status register */
		serial_err++;
		fp->error = 0;
		free_p(bp);
		return;
	}
#endif

	kisstype = pullchar(&bp);

	if ((kisstype & 0x0F) == KISS_DATA)  {
		if(interface->kissport){	/* save some time if standard asy */
			strcpy(tmp,interface->name);
			i = strlen(tmp);

			tmp[i-1] = ntoh(kisstype >> 4);
			/* here is one place where maybe we could get a speed increase.
			 * if_lookup does a strcmp() to match iface->name
			 */
			interface = if_lookup(tmp);
		}
		dump(interface,IF_TRACE_IN,TRACE_AX25,bp);
		ax_recv(interface,bp); /* in ax25.c */
	}else{
		extern int16 freeps;
		free_p(bp);	/* probably no good */
		freeps++;
	}
}
#endif /* MDKISSPORT */
