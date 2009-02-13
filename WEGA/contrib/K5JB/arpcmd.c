/* 7/29/91 Corrected placement of ifdef NETROM in arpadd() */
/* 11/14/93 Added support for an IP only address - K5JB */
#include "config.h"
#if defined(ETHER) || defined(AX25)
#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "timer.h"
#ifdef ETHER
#include "enet.h"
#endif
#include "iface.h"
#include "ax25.h"
#include "arp.h"
#include "cmdparse.h"

#ifdef VCIP_SSID
extern struct ax25_addr ip_call;
#endif

extern char badhost[];
extern char nospace[];

int doarpadd(),doarpdrop();
struct cmds arpcmds[] = {
	"add", doarpadd, 4,
#ifdef NETROM
#ifdef ETHER
	"arp add <hostid> ether|ax25|vax25|netrom <ether addr|callsign>",
#else
	"arp add <hostid> ax25|vax25|netrom <callsign>",
#endif	/* ETHER */
#else	/* NETROM */
	"arp add <hostid> ax25|vax25 <callsign>",
#endif	/* NETROM */

	"arp add failed",

	"drop", doarpdrop, 3,
#ifdef NETROM
#ifdef ETHER
	"arp drop <hostid> ether|ax25|netrom|vax25",
#else
	"arp drop <hostid> ax25|netrom|vax25",
#endif	/* ETHER */
#else	/* NETROM */
	"arp drop <hostid> ax25|vax25",
#endif	/* NETROM */

	"not in table",

	"publish", doarpadd, 4,
#ifdef NETROM
#ifdef ETHER
	"arp publish <hostid> ether|ax25|netrom|vax25 <ether addr|callsign>",
#else
	"arp publish <hostid> ax25|netrom|vax25 <callsign>",
#endif /* ETHER */
#else	/* NETROM */
	"arp publish <hostid> ax25|vax25 <callsign>",
#endif	/* NETROM */
	"arp publish failed",

	NULLCHAR, NULLFP, 0,
	"arp subcommands: add, drop, publish",
	NULLCHAR
};
char *arptypes[] = {	/* little bit of wasted space here - K5JB */
	"NET/ROM",			/* but arp.h would have to be also modified */
	"10 Mb Ethernet",
	"3 Mb Ethernet",
	"AX.25",
	"Pronet",
	"Chaos",
	"Blank",
	"Arcnet",
	"Appletalk",
	"VAX.25"
};

int
doarp(argc,argv)
int argc;
char *argv[];
{
	int subcmd(),dumparp();

	if(argc < 2){
		dumparp();
		return 0;
	}
	return subcmd(arpcmds,argc,argv);
}

#include <ctype.h>
static
doarpadd(argc,argv)
int argc;
char *argv[];
{
	int16 hwalen,hardware,naddr;
	int32 addr,resolve();
	void free();
	int stop_timer();
	char *hwaddr;
	struct arp_tab *ap;
	struct arp_type *at;
	void arp_drop();
	int pub = 0;

	if(argv[0][0] == 'p')	/* Is this entry published? */
		pub = 1;
	if((addr = resolve(argv[1])) == 0){
		printf(badhost,argv[1]);
		return 1;
	}
	/* This is a kludge. It really ought to be table driven */
	switch(tolower(argv[2][0])){
#ifdef NETROM
	case 'n':	/* Net/Rom pseudo-type */
		hardware = ARP_NETROM;
		naddr = argc - 3 ;
		if (naddr != 1) {
			printf(
			"No digipeaters in NET/ROM arp entries - Use netrom route add\n") ;
			return 1 ;
		}
		break;
#endif
#ifdef ETHER
	case 'e':	/* "ether" */
		hardware = ARP_ETHER;
		naddr = 1;
		break;
#endif
	case 'a':	/* "ax25" */
		hardware = ARP_AX25;
		naddr = argc - 3;
		break;
	case 'v':	/* VC AX.25 */
#ifdef VCIP_SSID
		if(ip_call.call[0] == '\0'){
			printf("Set vcipcall first\n");
			return -1;
		}
#endif
		hardware = ARP_VAX25;
		naddr = argc - 3;
		break;
	default:
		printf("unknown hardware type \"%s\"\n",argv[2]);
		return -1;
	}
	/* If an entry already exists, clear it */
	if((ap = arp_lookup(hardware,addr)) != NULLARP)
		arp_drop(ap);
	if(hardware == ARP_VAX25)
		at = &arp_type[ARP_AX25];
	else
		at = &arp_type[hardware];
	if(at->scan == NULLFP){
		printf("Attach device first\n");
		return 1;
	}
	/* Allocate buffer for hardware address and fill with remaining args */
	hwalen = at->hwalen * naddr;
	if((hwaddr = malloc(hwalen)) == NULLCHAR){
		printf(nospace);
		return 0;
	}
	/* Destination address */
	(*at->scan)(hwaddr,&argv[3],argc - 3);
	ap = arp_add(addr,hardware,hwaddr,hwalen,pub);	/* Put in table */
	free(hwaddr);					/* Clean up */
	stop_timer(&ap->timer);			/* Make entry permanent */
	ap->timer.count = ap->timer.start = 0;
	return 0;
}

/* Remove an ARP entry */
static
doarpdrop(argc,argv)
int argc;
char *argv[];
{
	int16 hardware;
	int32 addr,resolve();
	struct arp_tab *ap;
	void arp_drop();

	if((addr = resolve(argv[1])) == 0){
		printf(badhost,argv[1]);
		return 1;
	}
	/* This is a kludge. It really ought to be table driven */
	switch(tolower(argv[2][0])){
#ifdef NETROM
	case 'n':
		hardware = ARP_NETROM;
		break;
#endif
#ifdef ETHER
	case 'e':	/* "ether" */
		hardware = ARP_ETHER;
		break;
#endif
	case 'a':	/* "ax25" */
		hardware = ARP_AX25;
		break;
	case 'v':	/* VC AX.25 */
		hardware = ARP_VAX25;
		break;
#ifdef APPLETALK
	case 'm':	/* "mac appletalk" */
		hardware = ARP_APPLETALK;
		break;
#endif
	default:
		hardware = 0;
		break;
	}
	if((ap = arp_lookup(hardware,addr)) == NULLARP)
		return -1;
	arp_drop(ap);
	return 0;
}
/* Dump ARP table */
static int
dumparp()
{
	register int i;
	extern struct arp_stat arp_stat;
	register struct arp_tab *ap;
	char e[128];
	char *inet_ntoa();

	printf(
	"received %u badtype %u bogus addr %u reqst in %u replies %u reqst out %u\n",
	 arp_stat.recv,arp_stat.badtype,arp_stat.badaddr,arp_stat.inreq,
	 arp_stat.replies,arp_stat.outreq);

	printf("IP addr         Type           Time Q Addr\n");
	for(i=0;i<ARPSIZE;i++){
		for(ap = arp_tab[i];ap != (struct arp_tab *)0;ap = ap->next){
			printf("%-16s%-15s%-5ld",inet_ntoa(ap->ip_addr),
				arptypes[ap->hardware],ap->timer.count*(long)MSPTICK/1000);
			if(ap->state == ARP_PENDING)
				printf("%-2u",len_q(ap->pending));
			else
				printf("  ");
			if(ap->state == ARP_VALID){
				if(ap->hardware == ARP_VAX25){
					if(arp_type[ARP_AX25].format != NULLFP)
						(*arp_type[ARP_AX25].format)(e,ap->hw_addr);
				}else	if(arp_type[ap->hardware].format != NULLFP){
					(*arp_type[ap->hardware].format)(e,ap->hw_addr);
				} else {
					e[0] = '\0';
				}
				printf("%s",e);
			} else {
				printf("[unknown]");
			}
			if(ap->pub)
				printf(" (published)");
			printf("\n");
		}
	}
	return 0;
}
#endif /* ETHER || AX25 */
