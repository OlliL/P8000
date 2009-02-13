/* Miscellaneous format conversion subroutines */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "sokname.h"	/* for SOKNAME - N5OWK */
#include "global.h"
#include "netuser.h"

#ifdef UNIX
#include "unix.h"
#endif /* UNIX */

int net_error;

#define LINELEN 80

/* Convert Internet address in ascii dotted-decimal format (44.0.0.1) to
 * binary IP address
 */
int32
aton(s)
register char *s;
{
	int32 n;
	int atoi();
	register int i;

	n = 0;
	for(i=24;i>=0;i -= 8){
		n |= (int32)atoi(s) << i;
		if((s = index(s,'.')) == NULLCHAR)
		break;
		s++;
	}
	return n;
}

/* Resolve a host name into an IP address. IP addresses in dotted-decimal
 * notation are distinguished from domain names by enclosing them in
 * brackets, e.g., [44.64.0.1]
 */
int32
resolve(host)
char *host;
{
	register char *cp;
	int32 addr;
	char hostent[LINELEN];
	FILE *sfile;
	static struct {
		char *name;
		int32 address;
	} cache;
	void rip(), free();

	if(*host == '['){
		/* Brackets indicate IP address in dotted-decimal form */
		return aton(host + 1);
	}
	if(isdigit(*host))
		return aton(host);
	if(cache.name != NULLCHAR && strcmp(cache.name,host) == 0)
		return cache.address;

	/* Not a numerical IP address, search the host table */
	if((sfile = fopen(hosts,"r")) == NULLFILE){
		return 0;
	}
	while (!feof(sfile)){
		fgets(hostent,LINELEN,sfile);
		rip(hostent);
		cp = hostent;
		if(*cp == '#' || !isdigit(*cp))
			continue;	/* Comment or invalid line */
		cp = strtok(cp," \t");
		addr = aton(cp);
		while(cp != NULLCHAR){
			cp = strtok(NULLCHAR," \t");
			if(strcmp(host,cp) == 0){
				/* Found it, put in cache */
				fclose(sfile);
				if(cache.name != NULLCHAR)
					free(cache.name);
				cache.name = malloc((unsigned)strlen(host)+1);
				strcpy(cache.name,host);
				cache.address = addr;
				return cache.address;
			}
			/* That one didn't match, try the next one */
		}
	}
	/* No address found */
	fclose(sfile);
	return 0;
}

/* Convert an internet address (in host byte order) to a dotted decimal ascii
 * string, e.g., 255.255.255.255\0
 */
char *
inet_ntoa(a)
int32 a;
{
	static char buf[16];

	sprintf(buf,"%u.%u.%u.%u",
		hibyte(hiword(a)),
		lobyte(hiword(a)),
		hibyte(loword(a)),
		lobyte(loword(a)) );
	return buf;
}
/* Convert a socket (address + port) to an ascii string of the form
 * aaa.aaa.aaa.aaa:ppppp
 */
char *
psocket(s)
struct socket *s;
{
	static char buf[30];

	sprintf(buf,"%s:%u",inet_ntoa(s->address),s->port);
	return buf;
}

#ifdef SOKNAME
/* Convert a socket (address + port) to an ascii string of the form
 * n5owk.okla.ampr:pppppppp
 */

#define POSITION 2	/* position of returned label in hosts.net file */
char *
puname(s)
struct socket *s;
{
	register int i;
	static char buf[25];			/* printf in caller only will do 24 chars */
	char	*cp, *cpt,*psocket();  	/* but that would leave no space between */
	char	hostent[LINELEN];		/* port and next field so shortened return */
	FILE	*sfile;              /* string by one character below */
	char *tcp_port();
	extern int issok;
	void rip();

	if(issok){	/* it may be turned off by sokname command */
		strncpy(buf,inet_ntoa(s->address),16);
		i = strlen(buf);
		if ((sfile = fopen(hosts,"r")) == NULLFILE)
			return psocket(s);

		while (!feof(sfile)){
			fgets(hostent,LINELEN,sfile);

			if(hostent[0] == '#' || !isdigit(hostent[0]))
				continue;	/* Comment or invalid line */
			if(strncmp(buf,hostent,i) != 0)	/* this requires IP address be */
				continue;			/* left justified in file */
			fclose(sfile);
			rip(hostent);
			cp = hostent;
			for(i = 0,cp = strtok(cp," \t");cp != NULLCHAR && i < POSITION;i++){
				cpt = cp;
				cp = strtok(NULLCHAR," \t");
			}
			if(cp == NULLCHAR)
				cp = cpt;
			cp[15] = '\0';	/* in case cp was gross - size restricted for */
			sprintf(buf,"%s:%s", cp, tcp_port(s->port)); /* screen cosmetics */
			return buf;		/* in tcpcmd.c, port is max of 8 chars */
		}

		/* No address found */
		fclose(sfile);
	}
	return psocket(s);	/* punt, never heard of em - or sokname may be off */
}
#endif

/* Convert hex-ascii string to long integer */
long
htol(s)
char *s;
{
	long ret;
	char c;

	ret = 0;
	while((c = *s++) != '\0'){
#if	(!ATARI_ST && !LATTICE)	/* DG2KK "ik versta er heelemal niets van!" */
		c &= 0x7f;
#endif
		if(c >= '0' && c <= '9')
			ret = ret*16 + (c - '0');
		else if(c >= 'a' && c <= 'f')
			ret = ret*16 + (10 + c - 'a');
		else if(c >= 'A' && c <= 'F')
			ret = ret*16 + (10 + c - 'A');
		else
			break;
	}

	return ret;
}
