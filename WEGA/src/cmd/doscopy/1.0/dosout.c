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
struct dosdisc *dp;
int conv;
{
 long z;
 int len;
 unsigned i, j;
 long adr, n;
 unsigned next;
 static char buf[1024] = {0};

 z = 0;
 n = dp->clustsiz;
 next = file->first;
 adr = DOSADR(next,dp->clustsiz) + dp->data;
 lseek(dp->fd,adr,0);

 while (next) {
	len = dp->clustsiz;
	if (len > 1024)
		len = 1024;
	if (file->dim - z < (long)len)
		len = file->dim - z;
	if (n - z < (long)len)
		len = n - z;
	if (len == 0)
		break;
	if ((len = read(dp->fd,buf,len)) <= 0)
		return(-1);
	if (conv) { /* CR LF -> LF */
		for (i=0, j=0; ; i++, j++) {
			if (i >= len || buf[i] == 0x1a)
				break;	/* Dateiende */
			if (buf[i] == 0x0d) {
				if (i == len-1) {
					buf[i] = ' ';
					continue;
				}
				if (buf[++i] == 0x0a) {
					buf[j] = buf[i];
					continue;
				}
				else {
					--i;
					buf[j] = buf[i];
					continue;
				}
			}
			buf[j] = buf[i];
		}
	}
	else
		j = len;
	if (write(out,buf,j) != j)
		return(-2);
	z += len;
	if (z >= file->dim)
		break;
	if (z >= n) {
		next = dosnxt(dp->fd,next,dp->fat1,dp->fatlen);
		adr = DOSADR(next,dp->clustsiz) + dp->data;
		lseek(dp->fd,adr,0);
		n += dp->clustsiz;
	}
 }
 return(1);
}
