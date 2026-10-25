/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosout.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

#include "dos.h"

/* dosout
 * Gibt die Datei, welche durch 'file'
 * gekennzeichnet wird, in das File 'out' aus.
 * 'conv' zeigt Newlinekonvertierung an.
 * dosout liefert -1 bei Lesefehler und
 *		  -2 bei Schreibfehler
 */
dosout(file,out,dp,conv)
struct dostat *file;
int out;
register struct dosdisc *dp;
int conv;
{
 register clustsiz = dp->clustsiz;
 register i, j, len;
 long adr, z;
 unsigned next = file->first;
 char last = 0;
 char *buf, *outbuf, *malloc();

 outbuf = malloc(clustsiz+1);
 buf = &outbuf[1];
 adr = DOSADR(next,clustsiz) + dp->data;
 lseek(dp->fd,adr,0);
 z = 0;

 while (next) {
 	len = clustsiz;
	if ((long)len > file->dim - z)
		len = file->dim - z;
	if (len == 0)
		break;

	if (read(dp->fd,buf,clustsiz) != clustsiz)
		return(-1);
	
	if (conv) { /* CR LF -> LF */
		for (i=0, j=0; i<len; i++) {
			if (last && buf[i] != 0x0a)
				outbuf[j++] = last;
			last = 0;
			if (buf[i] == 0x1a)
				break;	/* Dateiende */
			if (buf[i] == 0x0d) {
				last++;
				continue;
			}
			outbuf[j++] = buf[i];
		}
		if (write(out, outbuf, j) != j)
			return(-2);
	}
	else { /* Binary-Copy */
		if (write(out, buf, len) != len)
			return(-2);
	}
	z += len;
	if (z >= file->dim)
		break;

	next = dosnxt(dp->fd,next,dp->fat1,dp->fatlen);
	adr = DOSADR(next,clustsiz) + dp->data;
	lseek(dp->fd,adr,0);
 }
 return(1);
}
