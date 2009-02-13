/* 3/1/92 removed unnecessary & from (near) lines 206, 239, 268 - K5JB
 * 3/16/92 removed scanf() and made other cleanups - experimenting
 * with the parser in cmdparse.c to replace gettwo() in here
 */

#include "options.h"	/*K5JB*/
#ifdef MULPORT

#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "iface.h"
#include "ax25.h"
#include "cmdparse.h"
#include <ctype.h>
#define MULLINE 80	/* make this easier to change.  Was hardcoded at 256
								in one place, 257 in the other */
#define PNAMLEN 5 /* might as well make array size an even nr */
#define CALLLEN 9
#define MAXLIST 10 /* maximum in exlist and digilist */

/**************************************************************************
* The struct mulrep holds calls and interface names for multiport         *
* repeating                                                               *
* port[] holds the interface name passed back to hack in ax25.c           *
* (And it needs to be bigger than it was - K5JB									  *
**************************************************************************/
typedef struct {
     struct    ax25_addr dcall;    /* digipeater callsign */
	  char port[PNAMLEN + 1];    /* port name - could have dr0a - K5JB */
} mulrep;


#ifndef TRUE	/* K5JB */
#define TRUE 1
#define FALSE 0
#endif

/* the net saving from splitting lineparse out of cmdparse is only 32 bytes
 * but removing unnecessary code from it saved a bunch.
 */

int lineparse();

static int
gettwo(str,tcall,tport,errtype)
char *str;
char *tcall;
char *tport;
char *errtype;
{
	int argc;
	char *argv[NARG];	/* NARG is 20 in cmdparse.h */

	tcall[0] = tport[0] = '\0';
	if(!lineparse(&argc,argv,str) && argc > 1){
		strncpy(tcall,argv[0],CALLLEN);
		tcall[CALLLEN] = '\0';

		strncpy(tport,argv[1],PNAMLEN);
		tport[PNAMLEN] = '\0';
	}
	if (tcall[0] == '\0' || tport[0] == '\0'){
		printf("Invalid record in %s encountered, record ignored\n",errtype);
		return(1);
	}
	return(0);

}

/****************************************************************************
* Multiport system globals                                                  *
* mport is the multiport flag initialized to false until mulport is called. *
****************************************************************************/
int       mport = FALSE;
mulrep    destrpt[MAXLIST], extbl[MAXLIST], lan;

/*************************************************************************
* mulport fills the array of structures of digi calls to which packets   *
* may  be repeated and sets the global flag mport TRUE. It reads from    *
* the file DIGILIST which must be either in NETHOME or in the ROOT dir-  *
* ectory and is formatted:  <callsign+ssid> <interface_name>				 *
* example:                                                               *
* KE4ZV-1 ax0   Note: These calls would be after ours in addr - K5JB     *
* KD4NC-1 ax1                                                            *
* mulport also builds an array of destination calls from the file EXLIST *
* in the root directory for destinations not fitting the general rules.  *
* This is the function called when the command "mulport on" is given.    *
* (Calls in exlist would match destinations in address lists - K5JB      *
*************************************************************************/
int mulport(argc,argv)
int argc;
char *argv[];
{
FILE *fp, *fopen();
int       x;
char tcall[CALLLEN + 1];
char str[MULLINE]; /* cut down by a bunch - was 257 */
extern char *mdigilist, *mexlist; /*K5JB*/
/* char *strcpy();  K5JB */
int setcall();

	if(argv[1][1] == 'n'){	/* avoid function call - only looking for "on"
										K5JB */
		if ((fp = fopen(mdigilist,"rt")) == 0) { /* K5JB */
			 printf("Could not open file %s\n",mdigilist);
          return(FALSE);
      }
		setcall(&(lan.dcall),"lan");
		lan.port[0] = '\0';	/* avoid another function call */
		printf("\nDigi Call       Interface\n");

		for (x=0; (x < MAXLIST) && (fgets(str, MULLINE, fp)); x++) {
       /* make sure we clear out the whole line to the \n   */
       /* otherwise, any other  whitespace after the port  */
       /* designator will fubar the parser                 */

	/* ill formed tcall and tport can fubar this code too!  Caution notes
	 * added to the documentation so as to possibly not surprise the
	 * unsuspecting user - (much safer with gettwo() instead of scanf) K5JB
	 */
/*			 tmp = sscanf(str, "%s %s", tcall, tport); This sucker takes almost
 * 2k of code!
 */
			if(gettwo(str,tcall,destrpt[x].port,"DIGILIST") == 1)
            continue;

			printf("%9.9s          %6.6s\n",tcall,destrpt[x].port);

/*  we really need to verify that the port  exists before we put it into the
    table or direct a stream to it. Current default behavior is to send the
    packet back out the port it came in on if there is no match.
*/
         setcall(&(destrpt[x].dcall),tcall);
/*			strcpy(destrpt[x].port,tport);	don't need this */
#ifdef MULBUG
         printf("%9.9s      %6.6s\n",destrpt[x].dcall.call,destrpt[x].port);
#endif
      }
      fclose(fp);
/*finish out the array with null entries */
		tcall[0] = '\0';
/*		tport[0] = '\0'; */

/* K5JB - Leaving this to show what not to do:
		strcpy(tcall,'\0');
		strcpy(tport,'\0');
		In fact, it would be better to do the following without messing
		with tcall and tport...
*/
		for(;x < MAXLIST;x++){
			setcall(&(destrpt[x].dcall),tcall);
			destrpt[x].port[0] = '\0';
/*			strcpy(destrpt[x].port,tport);*/
      }
      /*****************************************************************
      *    This code builds the exception list extbl[]                 *
      *    This list handles destination calls who need behavior that  *
      *    does not follow the mulport rules ie: a user station on the *
		*    high speed trunk.  													  *
      *****************************************************************/
		if ((fp = fopen(mexlist,"rt")) == 0) { /* environmental stuff - K5JB */
			printf("Could not open file %s\n",mexlist);
         return(FALSE);
      }
		printf("\nException Call  Interface\n");
		for (x=0; (x < MAXLIST) && (fgets(str, MULLINE, fp)); x++) {
/*			tmp = sscanf(str, "%s %s", tcall, tport); - shudder! */
			/* make sure we don't do a partial entry */
			if(gettwo(str,tcall,extbl[x].port,"EXLIST") == 1)
            continue;

			printf("%9.9s          %6.6s\n",tcall,extbl[x].port);
         setcall(&(extbl[x].dcall),tcall);
#ifdef MULBUG
         printf("%9.9s      %6.6s\n",extbl[x].dcall.call,extbl[x].port);
#endif
      }
      fclose(fp);

/*finish out the array with null entries - I think this is what is wanted */
		tcall[0] = '\0';
		for(;x < MAXLIST;x++){
         setcall(&(extbl[x].dcall),tcall);
			extbl[x].port[0] = '\0';
      }
      mport = TRUE;
      return(TRUE);
   } else {
      mport = FALSE;
      return(FALSE);
   }

}

/**************************************************************************
* Here is the repeater hack called from ax_recv in module ax25.c.         *
* Repeater searches the array of digi calls created by mulport for the    *
* digi call following ours in the packet header. If a match occurs the    *
* corresponding interface name is found by comparing the string in        *
* port[] to the names assigned to interfaces with the attach command.     *
* A pointer to the interface is returned by repeater. Default behavior on *
* match failure is to return a pointer to the interface the packet came   *
* in on. If our call is the last digi call in the header, the destination *
* call is compared to the array of exception calls and, if a match, the   *
* corresponding interface is returned by repeater. Default behavior on    *
* match failure is to return a pointer to the interface referenced by the *
* pseudo call "lan" in digilist.                                          *
* If no matches at all are found, default behavior is to return a pointer *
* to the interface the packet came in on.                                 *
**************************************************************************/
struct interface *
repeater(ap,interface,hdr)
struct ax25_addr *ap;
struct interface *interface;
struct ax25 *hdr;
{
struct interface *intport;
int x, flg, match;
int strcmp(), addreq();

	if (++ap < &hdr->digis[hdr->ndigis]){	/* first check addr after ours */
#ifdef MULBUG
		printf("mport && there is a call after ours\n");
#endif
		for (x=0, flg=0; (!flg) && (x < MAXLIST) &&
				destrpt[x].dcall.call != '\0'; x++){
					/* Normally 10 mports max */
/* (strcmp('\0',&(destrpt[x].dcall.call))!=0); x++){} '\0' is not a string */
#ifdef MULBUG
			printf("stepping thru destrpt at %d %9.9s   %9.9s\n",
					x,ap->call,&(destrpt[x].dcall.call));
#endif
			if (addreq(ap,&(destrpt[x].dcall))){
#ifdef MULBUG
				printf("dcall match\n");
#endif
				for (intport=ifaces; !flg && (intport!=NULLIF);
						intport=intport->next){
#ifdef MULBUG
					printf("stepping thru interfaces %9.9s  %9.9s\n",
							intport->name,&(destrpt[x].port));
#endif
					if (strcmp(intport->name,destrpt[x].port) == 0){
#ifdef MULBUG
						printf("interface match on %s\n",&(destrpt[x].port));
#endif
						interface=intport;
						flg = 1;
					}
				}
			}
		}
		ap--;	/* not necessary, we only modified our copy - K5JB */
	}
	else{
		for (match=0,x=0, flg=0; (!flg) && (x < MAXLIST) &&
				extbl[x].dcall.call != '\0'; x++){          /* K5JB */
											  /* MAXLIST mports max */

/*		(strcmp('\0',&(extbl[x].dcall.call))!=0); x++){}*/
#ifdef MULBUG
			printf("scanning for hdr.dest %9.9s  %9.9s\n",
					&(hdr->dest.call),&(extbl[x].dcall.call));
#endif
			if (addreq(&(hdr->dest),&(extbl[x].dcall))){
#ifdef MULBUG
				printf("dest match\n");
#endif
				for (intport=ifaces; !flg && (intport!=NULLIF);
						intport=intport->next){
#ifdef MULBUG
					printf("stepping thru interfaces %9.9s  %9.9s\n",
							intport->name,&(extbl[x].port));
#endif
					if (strcmp(intport->name,extbl[x].port) == 0){
#ifdef MULBUG
						printf("interface match\n");
#endif
						interface=intport;
						flg = 1;
						match=1;
					}
				}
			}
		}
		if (!match){
			for (x=0, flg=0; (!flg) && (x < MAXLIST) &&
					destrpt[x].dcall.call != '\0'; x++){          /* K5JB */
							/* 10 mports max */
/*					(strcmp('\0',&(destrpt[x].dcall.call))!=0); x++){} */
#ifdef MULBUG
				printf("scanning for lan.dcall %9.9s  %9.9s\n",
						&(lan.dcall.call),&(destrpt[x].dcall.call));
#endif
				if (addreq(&(lan.dcall),&(destrpt[x].dcall))){
#ifdef MULBUG
					printf("dcall match\n");
#endif
					for (intport=ifaces; !flg && (intport!=NULLIF);
							intport=intport->next){
#ifdef MULBUG
						printf("stepping thru interfaces %9.9s  %9.9s\n",
								intport->name,&(destrpt[x].port));
#endif
							if (strcmp(intport->name,destrpt[x].port) == 0){
#ifdef MULBUG
								printf("interface match\n");
#endif
								interface=intport;
								flg = 1;
							}
						}
					}
				}
			}
			ap--;	/* also not necessary - K5JB */
	}
	return(interface);
}
#endif

