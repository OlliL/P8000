/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosplit.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "dos.h"

#define STANDARD "/etc/default/dos"

/* dosplit
 * Teilt einen Pfad in die DCP- und
 * WEGA-Bestandteile.
 * Dabei wird das Verzeichnis /etc/default/dos
 * ausgewertet und im DCP-Pfad werden
 * Klein- in Grossbuchstaben umgewandelt.
 * Bei Fehler liefert dosplit -1
 */

dosplit(path,upath,dpath)
register char *path,*upath,*dpath;
{
	char *p,line[LMAX],lw,*strrchr(), *getenv();
	FILE *fp;
	int mlen, found=0;

	strncpy(upath,path,LMAX);
	upath[LMAX-1] = '\0';
	*dpath = '\0';

	strcpy(line, getenv("HOME"));
	strcat(line, "/.dos");

	if ((p=strrchr(path,':')) == '\0')
		/* kein DCP-Pfad */
		return(2); /* keine DCP-Datei angegeben */

	if ((mlen = strlen(path) - strlen(p)) == 1) {
		/* vor ':' ist Laufwerksbuchstabe angegeben */

		if ((fp=fopen(line, "r")) == 0) {
			if ((fp=fopen(STANDARD,"r")) == 0) {
			    fprintf(stderr,"%s nicht eingerichtet\n",STANDARD);
			    return(-1);
			}
		}
		p--;
		lw = toupper(*p);
		p += 2;
		if (*p == '/')
			p++;
		while (fgets(line,LMAX,fp) != 0) {
			if (line[0] == lw && strlen(line) > 3) {
				line[strlen(line)-1] = '\0';
				strcpy(upath,line+2);
				found++;
				break;
			}
		}
		if (!found) {
			fprintf(stderr,"Laufwerk %c: nicht definiert\n",lw);
			return(-1);
		}
	}
	else {
		if (mlen >= LMAX)
			upath[LMAX-1] = '\0';
		else
			upath[mlen] = '\0';
		++p;
		if (*p == '/')
			p++;
	}
	strncpy(dpath,p,LMAX);
	dpath[LMAX-1] = '\n';
	while (*dpath != '\0')
		*dpath++ = islower(*dpath) ? toupper(*dpath) : *dpath;
	/* Der DCP-Pfad enthaelt mindestens das Stammverzeichnis */
	return(1);
}
