/* 10/20/90 changed printfs to fprintf to correct trace dumping to file.
 * Pointed out by N5OWK - K5JB
 */
#include "config.h"
#ifdef TRACE
#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "internet.h"
#include "udp.h"

extern FILE *trfp;

/* Dump a UDP header */
void
udp_dump(bpp,source,dest,check)
struct mbuf **bpp;
int32 source,dest;
int check;		/* If 0, bypass checksum verify */
{
	struct udp udp;
	struct pseudo_header ph;
	int16 csum,cksum();
	void ntohudp();

	if(bpp == NULLBUFP || *bpp == NULLBUF)
		return;

 /* Can't believe I didn't find this until 7/7/91 - K5JB */
	/* Compute checksum */
	ph.source = source;
	ph.dest = dest;
	ph.protocol = UDP_PTCL;
	ph.length = len_mbuf(*bpp);
	csum = cksum(&ph,*bpp,ph.length);

	ntohudp(&udp,bpp);	/* will set udp.checksum */
	fprintf(trfp,"UDP: %u->%u len %u",udp.source,udp.dest,udp.length);

	/* This part is not well thought out - K5JB */
	if(csum || udp.checksum)
		fprintf(trfp," CHECKSUM ERROR (%u)",check ? csum : udp.checksum);

	fprintf(trfp,"\n");
}
#endif /* TRACE */
