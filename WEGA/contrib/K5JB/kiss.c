#include "options.h"
#include "config.h"
#ifdef AX25
#include "global.h"
#include "mbuf.h"
#include "iface.h"
#include "kiss.h"
#include "trace.h"
#ifdef COH386
#undef __ARGS /* avoid a redefinition */
#endif
#include <stdio.h>
#include <string.h>

#if defined(MSDOS) && defined(SERIALTEST)
#include "8250.h"
#endif

#ifndef MDKISSPORT
/* Send raw data packet on KISS TNC */
void
kiss_raw(interface,data)
struct interface *interface;
struct mbuf *data;
{
	register struct mbuf *bp;
	void dump();
	int slip_raw();

	dump(interface,IF_TRACE_OUT,TRACE_AX25,data);

	/* Put type field for KISS TNC on front */
	if((bp = pushdown(data,1)) == NULLBUF){
		free_p(data);
		return;
	}
	bp->data[0] = KISS_DATA;

	slip_raw(interface,bp);
}

/* Process incoming KISS TNC frame */
void
kiss_recv(interface,bp)
struct interface *interface;
struct mbuf *bp;
{
	char kisstype;
	int ax_recv();
	void dump();
	extern int16 freeps;	/* defined in ipcmd.c */

#if defined(MSDOS) && defined(SERIALTEST)
/* Also done in slip.c, kpc.c and nrs.c to clear error flag possibly
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

	switch(lonibble(kisstype)){
	case KISS_DATA:
		dump(interface,IF_TRACE_IN,TRACE_AX25,bp);
		ax_recv(interface,bp);
		break;
	default:
		free_p(bp);	/* probably no good */
		freeps++;	/* remove after test - K5JB */
	}
}
#endif /* MDKISSPORT */

/* Perform device control on KISS TNC by sending control messages */
kiss_ioctl(interface,argc,argv)
struct interface *interface;
int argc;
char *argv[];
{
	struct mbuf *hbp;
	register int i;
	register char *cp;
	int atoi();

	if(argc < 1){
		printf("Data field missing\n");
		return 1;
	}

	/* Allocate space for arg bytes */
	if((hbp = alloc_mbuf((int16)argc)) == NULLBUF){
		free_p(hbp);
		return 0;
	}

	hbp->cnt = argc;
	hbp->next = NULLBUF;
	cp = hbp->data;
	*cp = (char)atoi(argv[0]);

#ifdef KPCPORT
	if (!strncmp(interface->name,"kp", 2) &&
		interface->name[strlen(interface->name) - 1] == 'b')
			*cp |= 0x10;
#endif
#ifdef MDKISSPORT	/* watch out for sign extensions here if we start using */
						/* upper parts of this number */
	if(interface->kissport)
		*cp |= interface->kissport << 4;
#endif
	for(i = 1,cp++;i < argc;)
		*cp++ = (char)atoi(argv[i++]);

	slip_raw(interface,hbp);    /* Even more "raw" than kiss_raw */

	return 0;
}
#endif	/* AX25 */
