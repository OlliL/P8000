/* corrected stray characters to stdout when trace file active - K5JB */
/* 8/19/91 added ROSE and formating changes per N5OWK */
#include <stdio.h>
#include "options.h"
#include "config.h"
#ifdef AX25
#ifdef TRACE
#include "global.h"
#include "mbuf.h"
#include "ax25.h"
#include "timer.h"
#include "iface.h"	/* K5JB */
#include "lapb.h"
#include "trace.h"
#ifdef MSDOS
#include <mem.h>
#endif

extern FILE *trfp;
int ntohax25(),pax25(),arp_dump(),ip_dump();	/* latter 3 should have been a void - K5JB */
void netrom_dump();

/* Dump an AX.25 packet header */
void
ax25_dump(bpp,unused)
struct mbuf **bpp;
int unused;
{
	char *decode_type();
	char tmp[20];
	unsigned char control,pid;
	int16 type,ftype();
	struct ax25 hdr;
	struct ax25_addr *hp;
#ifndef NO_INFO_CMD
	extern int info_only;
	extern char axtrhdr[];
#endif

	/* Extract the address header */
	if(ntohax25(&hdr,bpp) < 0){
		/* Something wrong with the header */
#ifndef NO_INFO_CMD
		fprintf(trfp,"%sAX25: bad header!\n",axtrhdr);
#else
		fprintf(trfp,"AX25: bad header!\n");
#endif
		return;
	}
#ifndef NO_INFO_CMD
#ifdef CRAP
	if(!pullup(bpp,&control,1) == 1 &&
		(!((type = ftype(control)) == UI || type == I) && info_only)){
		axtrhdr[0] = '\0';	/* flag to not print anything further */
		return;
	}
#endif

	if(pullup(bpp,&control,1) != 1){
		axtrhdr[0] = '\0';
		return;
	}
	if(!((type = ftype(control)) == UI || type == I))
		if(info_only){
			axtrhdr[0] = '\0';	/* flag to not print anything further */
			return;
		}

	fprintf(trfp,"%sAX25: ",axtrhdr);
	axtrhdr[0] = 0xff;	/* flag for IP not to print header again */
#else
	if(pullup(bpp,&control,1) != 1){
		fprintf(trfp,"\n");
		return;
	}
	type = ftype(control);
#endif

	pax25(tmp,&hdr.source);
	fprintf(trfp,"%s",tmp);
	pax25(tmp,&hdr.dest);
	fprintf(trfp,"->%s",tmp);
	if(hdr.ndigis > 0){
		fprintf(trfp," v");
		for(hp = &hdr.digis[0]; hp < &hdr.digis[hdr.ndigis]; hp++){
			/* Print digi string */
			pax25(tmp,hp);
			fprintf(trfp," %s%s",tmp,(hp->ssid & REPEATED) ? "*":"");
		}
	}
	/* First bug I fixed!  Space here was send with putchar() - K5JB */
	fprintf(trfp," %s",decode_type(type));
	/* Dump poll/final bit */
	if(control & PF){
		switch(hdr.cmdrsp){
		case COMMAND:
			fprintf(trfp,"(P)");
			break;
		case RESPONSE:
			fprintf(trfp,"(F)");
			break;
		default:
			fprintf(trfp,"(P/F)");
			break;
		}
	}
	/* Dump sequence numbers */
	if((type & 0x3) != U)	/* I or S frame? */
		fprintf(trfp," NR=%d",(control>>5)&7);
	if(type == I || type == UI){
		if(type == I)
			fprintf(trfp," NS=%d",(control>>1)&7);
		/* Decode I field */
		if(pullup(bpp,&pid,1) == 1){	/* Get pid */
			fprintf(trfp," pid=");
			switch(pid){
			case PID_ARP:
				fprintf(trfp,"ARP\n");
				break;
			case PID_NETROM:
				fprintf(trfp,"NET/ROM\n");
				break;
			case PID_IP:
				fprintf(trfp,"IP\n");
				break;
			case PID_NO_L3:
				fprintf(trfp,"Text\n");
				break;
			case PID_ROSE1:
				fprintf(trfp,"ROSE\n");
				break;
#ifdef SEGMENT
			case PID_SEGMENT:
				fprintf(trfp,"Segment\n");
				break;
#endif
			default:
				fprintf(trfp,"0x%02x\n",pid);	/* N5OWK */
			}
			/* Only decode frames that are the first in a
			 * multi-frame sequence
			 */
			switch(pid){
			case PID_ARP:
				arp_dump(bpp);
				break;
			case PID_IP:
				/* Only checksum complete frames */
				ip_dump(bpp,pid);
				break;
			case PID_NETROM:
				netrom_dump(bpp);
				break;
			}
		}
	} else if(type == FRMR && pullup(bpp,tmp,3) == 3){
		fprintf(trfp,": %s",decode_type(ftype(tmp[0])));
		fprintf(trfp," Vr = %d Vs = %d",(tmp[1] >> 5) & MMASK,
			(tmp[1] >> 1) & MMASK);
		if(tmp[2] & W)
			fprintf(trfp," Invalid control field");
		if(tmp[2] & X)
			fprintf(trfp," Illegal I-field");
		if(tmp[2] & Y)
			fprintf(trfp," Too-long I-field");
		if(tmp[2] & Z)
			fprintf(trfp," Invalid seq number");
		fprintf(trfp,"\n");
	} else
		fprintf(trfp,"\n");

	fflush(stdout);
}
/* Display NET/ROM network and transport headers */
static void
netrom_dump(bpp)
struct mbuf **bpp;
{
	struct ax25_addr src,dest;
	char x;
	char tmp[16];
	char thdr[5];
	register i;

	if(bpp == NULLBUFP || *bpp == NULLBUF)
		return;
	/* See if it is a routing broadcast */
	if(uchar(*(*bpp)->data) == 0xff) {
		pullup(bpp,tmp,1);		/* Signature */
		pullup(bpp,tmp,ALEN);
		tmp[ALEN] = '\0';
		fprintf(trfp,"NET/ROM Routing: %s\n",tmp);
		for(i = 0;i < 11;i++) {
			if (pullup(bpp,tmp,AXALEN) < AXALEN)
				break;
			memcpy(src.call,tmp,ALEN);
			src.ssid = tmp[ALEN];
			pax25(tmp,&src);
			fprintf(trfp,"        %12s",tmp);
			pullup(bpp,tmp,ALEN);
			tmp[ALEN] = '\0';
			fprintf(trfp,"%8s",tmp);
			pullup(bpp,tmp,AXALEN);
			memcpy(src.call,tmp, ALEN);
			src.ssid = tmp[ALEN];
			pax25(tmp,&src);
			fprintf(trfp,"    %12s", tmp);
			pullup(bpp,tmp,1);
			fprintf(trfp,"    %3u\n", (unsigned)uchar(tmp[0]));
		}
		return;
	}
	/* Decode network layer */
	pullup(bpp,tmp,AXALEN);
	memcpy(src.call,tmp,ALEN);
	src.ssid = tmp[ALEN];
	pax25(tmp,&src);
	fprintf(trfp,"NET/ROM: %s",tmp);

	pullup(bpp,tmp,AXALEN);
	memcpy(dest.call,tmp,ALEN);
	dest.ssid = tmp[ALEN];
	pax25(tmp,&dest);
	fprintf(trfp,"->%s",tmp);

	pullup(bpp,&x,1);
	fprintf(trfp," ttl %u\n",(unsigned)uchar(x));

	i = 0; /* reuse this as a flag */

	/* Read first five bytes of "transport" header */
	pullup(bpp,thdr,5);
	switch(thdr[4] & 0xf){
	case 0:	/* network PID extension */
		if(thdr[0] == (PID_IP & 0x0f) && thdr[1] == (PID_IP & 0x0f)){
			i = 1;	/* suppress an extra linefeed */
			ip_dump(bpp,1) ;
		}else
			fprintf(trfp,"         protocol family %x, proto %x",
					uchar(thdr[0]), uchar(thdr[1])) ;
		break ;
	case 1:	/* Connect request */
		fprintf(trfp,"         conn rqst: ckt %u/%u",(unsigned)uchar(thdr[0]),
			(unsigned)uchar(thdr[1]));
		pullup(bpp,&x,1);
		fprintf(trfp," wnd %d",x);
		pullup(bpp,(char *)&src,sizeof(struct ax25_addr));
		pax25(tmp,&src);
		fprintf(trfp," %s",tmp);
		pullup(bpp,(char *)&dest,sizeof(struct ax25_addr));
		pax25(tmp,&dest);
		fprintf(trfp,"@%s",tmp);
		break;
	case 2:	/* Connect acknowledgement */
		fprintf(trfp,"         conn ack: ur ckt %u/%u my ckt %u/%u",
			(unsigned)uchar(thdr[0]), (unsigned)uchar(thdr[1]),
			(unsigned)uchar(thdr[2]),(unsigned)uchar(thdr[3]));
		pullup(bpp,&x,1);
		fprintf(trfp," wnd %d",x);
		break;
	case 3:	/* Disconnect request */
		fprintf(trfp,"         disc: ckt %u/%u",
			(unsigned)uchar(thdr[0]),(unsigned)uchar(thdr[1]));
		break;
	case 4:	/* Disconnect acknowledgement */
		fprintf(trfp,"         disc ack: ckt %u/%u",
			(unsigned)uchar(thdr[0]),(unsigned)uchar(thdr[1]));
		break;
	case 5:	/* Information (data) */
		fprintf(trfp,"         info: ckt %u/%u txseq %u rxseq %u",
			(unsigned)uchar(thdr[0]),(unsigned)uchar(thdr[1]),
			(unsigned)uchar(thdr[2]),(unsigned)uchar(thdr[3]));
		break;
	case 6:	/* Information acknowledgement */
		fprintf(trfp,"         info ack: ckt %u/%u rxseq %u",
			(unsigned)uchar(thdr[0]),(unsigned)uchar(thdr[1]),
			(unsigned)uchar(thdr[3]));
		break;
	default:
		fprintf(trfp,"         unknown transport type %d", thdr[4] & 0x0f) ;
		break;
	}
	if(!i)
		fprintf(trfp,"%s\n",	thdr[4] & 0x80 ? " CHOKE" :
			thdr[4] & 0x40 ? " NAK" : "");
}
char *
decode_type(type)
int16 type;
{
	switch(uchar(type)){
	case I:
		return "I";
	case SABM:
		return "SABM";
	case DISC:
		return "DISC";
	case DM:
		return "DM";
	case UA:
		return "UA";
	case RR:
		return "RR";
	case RNR:
		return "RNR";
	case REJ:
		return "REJ";
	case FRMR:
		return "FRMR";
	case UI:
		return "UI";
	default:
		return "[invalid]";
	}
}
#endif /* TRACE */
#endif /* AX25 */
