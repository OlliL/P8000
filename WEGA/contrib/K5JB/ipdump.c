#include "config.h"
#include "options.h"
#ifdef TRACE
#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "internet.h"
#include "timer.h"
#include "iface.h"
#include "ip.h"
#include "trace.h"
#include "netuser.h"

extern FILE *trfp;

void
ip_dump(bpp,check)
struct mbuf **bpp;
int check;
{
	void tcp_dump(),udp_dump(),icmp_dump(),trim_mbuf();
	struct ip ip;
	int16 ip_len;
	int16 offset;
	int16 length;
	int16 csum,cksum();
#ifndef NO_INFO_CMD
	extern int info_only;
	extern char axtrhdr[];
#endif

	if(bpp == NULLBUFP || *bpp == NULLBUF)
		return;

#ifndef NO_INFO_CMD	/* missed this one for quite a while */
	if(axtrhdr[0] != 0xff)
		fprintf(trfp,"%sIP: ",axtrhdr);
#else
	fprintf(trfp,"IP:");
#endif
	/* Sneak peek at IP header and find length */
	ip_len = ((*bpp)->data[0] & 0xf) << 2;
	if(ip_len < IPLEN){
		fprintf(trfp," bad header\n");
		return;
	}
	if(check)
		csum = cksum(NULLHEADER,*bpp,ip_len);
	else
		csum = 0;

	ntohip(&ip,bpp);	/* Can't fail, we've already checked ihl */

	/* Trim data segment if necessary. */
	length = ip.length - ip_len;	/* Length of data portion */
	trim_mbuf(bpp,length);
	fprintf(trfp," len %u %s->",ip.length,inet_ntoa(ip.source));
	fprintf(trfp,"%s ihl %u ttl %u",inet_ntoa(ip.dest),ip_len,uchar(ip.ttl));
	if(ip.tos != 0)
		fprintf(trfp," tos %u",uchar(ip.tos));
	offset = (ip.fl_offs & F_OFFSET) << 3;
	if(offset != 0 || (ip.fl_offs & MF))
		fprintf(trfp," id %u offs %u",ip.id,offset);
	if(ip.fl_offs & DF)
		fprintf(trfp," DF");
	if(ip.fl_offs & MF){
		fprintf(trfp," MF");
		check = 0;	/* Bypass host-level checksum verify */
	}
	if(csum != 0)
		fprintf(trfp," CHECKSUM ERROR (%u)",csum);

	if(offset != 0){
		fprintf(trfp,"\n");
		return;
	}
	switch(uchar(ip.protocol)){
	case TCP_PTCL:
		fprintf(trfp," prot TCP dat %u\n",ip.length - 40);
		tcp_dump(bpp,ip.source,ip.dest,check);
		break;
	case UDP_PTCL:
		fprintf(trfp," prot UDP\n");
		udp_dump(bpp,ip.source,ip.dest,check);
		break;
	case ICMP_PTCL:
		fprintf(trfp," prot ICMP\n");
		icmp_dump(bpp,ip.source,ip.dest,check);
		break;
	default:
		fprintf(trfp," prot %u\n",uchar(ip.protocol));
		break;
	}
}
#endif /* TRACE */
