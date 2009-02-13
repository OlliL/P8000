#include "config.h"
#ifdef TRACE
#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "internet.h"
#include "timer.h"
#include "tcp.h"
#include "trace.h"

#define DODECIMAL /* experimental aid in tracing */

extern FILE *trfp;

/* TCP segment header flags */
char *tcpflags[] = {
	"FIN",	/* 0x01 */
	"SYN",	/* 0x02 */
	"RST",	/* 0x04 */
	"PSH",	/* 0x08 */
	"ACK",	/* 0x10 */
	"URG"	/* 0x20 */
};

/* Dump a TCP segment header. Assumed to be in network byte order */
void
tcp_dump(bpp,source,dest,check)
struct mbuf **bpp;
int32 source,dest;	/* IP source and dest addresses */
int check;		/* 0 if checksum test is to be bypassed */
{
	int i,ntohtcp();
	struct tcp seg;
	struct pseudo_header ph;
	int16 csum,cksum();

	if(bpp == NULLBUFP || *bpp == NULLBUF)
		return;

	/* Verify checksum */
	ph.source = source;
	ph.dest = dest;
	ph.protocol = TCP_PTCL;
	ph.length = len_mbuf(*bpp);
	csum = cksum(&ph,*bpp,ph.length);

	ntohtcp(&seg,bpp);

#ifdef DODECIMAL
	fprintf(trfp,"TCP: %u->%u Seq x%lx (...%lu)",seg.source,seg.dest,seg.seq,
		(unsigned long)seg.seq - ((unsigned long)seg.seq/1000L)*1000l);
#else
	fprintf(trfp,"TCP: %u->%u Seq x%lx",seg.source,seg.dest,seg.seq);
#endif
	if(seg.flags & ACK)
#ifdef DODECIMAL
		fprintf(trfp," Ack x%lx (...%lu)",seg.ack,(unsigned long)seg.ack - ((unsigned long)seg.ack/1000l)*1000l);
#else
		fprintf(trfp," Ack x%lx",seg.ack);
#endif
	for(i=0;i<6;i++)
		if(seg.flags & (1 << i))	/* compiler thot possibly ambiguous */
			fprintf(trfp," %s",tcpflags[i]);
	fprintf(trfp," Wnd %u",seg.wnd);
	if(seg.flags & URG)
		fprintf(trfp," UP x%x",seg.up);
	/* Print options, if any */
	if(seg.mss != 0)
		fprintf(trfp," MSS %u",seg.mss);
	if(check && csum != 0)
		fprintf(trfp," CHECKSUM ERROR (%u)",csum);	/* was i, didn't tell */
										/* us anything, always 6 */
	fprintf(trfp,"\n");
}
#endif /* TRACE */
