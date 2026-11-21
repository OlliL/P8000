#include "dos.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#define bufsize 1024


/* dosin
 * Schreibt das MUTOS-File 'in' auf den
 * Datentraeger discp unter dem Namen fn .
 * Ist fn schon vorhanden wird die alte
 * Version geloescht .
 * Liefert 0 falls das uebergeordnete 
 * Verzeichnis nicht existiert ,
 *        -1 bei Lesefehler ,
 *        -2 bei Schreibfehler und
 *        -3 falls fn ein Verzeichnis oder  
 *           schreibgeschuetzt ist .
*/

dosin (in, fn, discp, opt)
char   *in,
       *fn;
struct dosdisc *discp;
int     opt;

{
    struct dostat   fnrec,
                    dirrec;
    int     res,
            z = 1,
            infd,
            i,
            j,
            len,
            io = 1,
            nl,
            mode;
    char   *p,
           *file,
            s[60],
            nam[13];
    char   *strchr ();
    long    adr,
            depos,
            pos;
    unsigned    dnum,
                nxt,
                free,
                follow,
                dosnxt (), fatfree ();
    struct stat instat;
    unsigned char   buf[bufsize],
                    outbuf[bufsize],
                    c;
    struct tm  *sttime,
               *localtime ();
    int     dmax,
            dcnt = 0;

    strcpy (s, fn);
    res = dosopn (fn, discp, &fnrec);
    if (res < 0)
	return (res);
    strcpy (fn, s);
 /* Das DCP-File wird geloescht, falls es bereits existiert */
    if (res > 0) {
	strcpy (fn, s);
	res = dosdel (fn, discp);
	if (res < 1)
	    return (res);
    }

    strcpy (fn, s);
    file = fn;
    while ((p = strchr (file, '/')) != 0)
	file = ++p;

    fn[strlen (fn) - strlen (file) - 1] = '\0';

    if (file == fn) {
	dmax = dnum = discp -> dirnum;
	adr = discp -> maindir;
	nxt = 0;
    }
    else {			/* Das uebergeordnete Verzeichnis ist
				   nicht das * Stammverzeichnis ! */
	res = dosopn (fn, discp, &dirrec);
	if (res < 1)
	    return (res);
	if (dirrec.mode != _SDIR)
	    return (0);
	adr = DOSADR (dirrec.first, discp -> clustsiz) + discp -> data;
    /* Bestimmen der Anzahl von Verzeichniseintraegen */
	nxt = dirrec.first;
	while ((nxt = dosnxt (discp -> fd, nxt, discp -> fat1, discp -> fatelen)) > 0)
	    z++;
	dnum = discp -> clustsiz / 32;
	dmax = z * dnum;
	nxt = dirrec.first;
    }

    pos = lseek (discp -> fd, adr, 0);
 /* Suche des ersten freien Eintrages */
    for (i = 0; i < z; i++) {
	j = 1;
	while (j++ <= dnum && (res = getde (discp -> fd, &fnrec)) > 0) {
	    dcnt++;
	    if ((c = fnrec.name[0]) == 0xe5) {
		i = z;
		depos = lseek (discp -> fd, (long) - 32, 1);
		break;
	    }
	}
	if (res == 0) {
	    depos = lseek (discp -> fd, (long) - 32, 1);
	    break;
	}
	if (nxt != 0) {
      nxt = dosnxt (discp -> fd, nxt, discp -> fat1, discp -> fatelen);
 if (nxt < 1)
		break;
	    else {		/* Naechsten Cluster lesen */
		adr = DOSADR (nxt, discp -> clustsiz) + discp -> data;
		pos = lseek (discp -> fd, adr, 0);
		dirrec.first = nxt;
	    }
	}
    }

    if (res < 0)
	return (res);
    if (fn == file && dcnt >= dmax)
	return (-2);		/* Stammverzeichnis voll */
    if (dcnt >= dmax) {		/* Verzeichniscluster voll */
	free = fatfree (discp)  ;
	if (free < 0)
	    return (free);
	     res = fatput (dirrec.first, free, discp)   ;
	if (res < 0)
	    return (res);
	     res = fatput (free, 0xffff, discp) ;
	if (res < 0)
	    return (res);
	adr = DOSADR (free, discp -> clustsiz) + discp -> data;

	for (i = 0; i < 512; i++)
	    buf[i] = 0x00;
	pos = lseek (discp -> fd, adr, 0);
	z = discp -> clustsiz / 512;
	for (i = 0; i < z; i++)
	    if (write (discp -> fd, buf, 512) != 512)
		return (-2);
    /* depos zeigt auf den zu belegenden Eintrag */
	depos = lseek (discp -> fd, adr, 0);
    }


 /* Oeffnen des MUTOS-Files */
    if ((infd = open (in, 0)) < 0)
	return (-1);
    if (fstat (infd, &instat) < 0)
	return (-1);
    mode = instat.st_mode;
    if ((mode & S_IFMT) == S_IFDIR)
	return (-1);

 /* fnrec wird mit Werten von infd belegt */
    if ((p = strchr (file, '.')) != 0) {
	p++;

	file[strlen (file) - strlen (p) - 1] = '\0';
	for (i = 0; i < 8; i++)
	    if (i >= strlen (file))
		fnrec.name[i] = 0x20;
	    else
		fnrec.name[i] = file[i];
	for (i = 8; i < 11; i++)
	    if (i - 7 > strlen (p))
		fnrec.name[i] = 0x20;
	    else
		fnrec.name[i] = p[i - 8];
    }
    else
	for (i = 0; i < 11; i++)
	    if (i >= strlen (file) || i > 7)
		fnrec.name[i] = 0x20;
	    else
		fnrec.name[i] = file[i];

    strcpy (nam, fnrec.name);
    fnrec.name = nam;


    fnrec.mode = _FILE;
    fnrec.flag = RW;
    sttime = localtime (&(instat.st_mtime));

    fnrec.time.sec = sttime -> tm_sec;
    fnrec.time.min = sttime -> tm_min;
    fnrec.time.hour = sttime -> tm_hour;

    fnrec.date.day = sttime -> tm_mday;
    fnrec.date.month = sttime -> tm_mon + 1;
    fnrec.date.year = sttime -> tm_year;
    free = fatfree (discp);
    if (free < 0)
	return (free);
    fnrec.first = free;
    fatput (free, 0xffff, discp);
    fnrec.dim = instat.st_size;

 /* Uebertragen der Daten */
    adr = DOSADR (free, discp -> clustsiz) + discp -> data;
    pos = lseek (discp -> fd, adr, 0);

    j = nl = z = 0;
/*   for( len=read(infd,buf,bufsize); len >0 ; len=read(infd,buf,bufsize) ) {*/
  while ((len = read (infd, buf, bufsize)) > 0 && io) { 
	for (i = 0; i < len && io; i++) {
	    if (opt)		/* Newlinekonvertierung */
		if (buf[i] == 0x0a) {/* CR einfuegen */
		    outbuf[j++] = 0x0d;
		    nl++;
		    z++;
	    if (z == discp -> clustsiz) {       /*  dg  */
		if (write (discp -> fd, outbuf, j) < j)       /*  dg  */
		    io--;       /*  dg  */
		     follow = fatfree (discp)   ;       /*  dg  */
		if ( follow < 0)       /*  dg  */
		    io--;       /*  dg  */
		else {       /*  dg  */
		    fatput (free, follow, discp);       /*  dg  */
		    fatput (follow, 0xffff, discp);       /*  dg  */
		    free = follow;       /*  dg  */
		    j = 0;       /*  dg  */
		    adr = DOSADR (free, discp -> clustsiz) + discp -> data;       /*  dg  */
		    pos = lseek (discp -> fd, adr, 0);       /*  dg  */
		    z = 0;       /*  dg  */
		}       /*  dg  */
	    }       /*  dg  */
       /*  dg  */
	    if (j == bufsize && io) {       /*  dg  */
		if (write (discp -> fd, outbuf, bufsize) < bufsize)       /*  dg  */
		    io--;       /*  dg  */
		j = 0;       /*  dg  */
	    }       /*  dg  */
		}
	    outbuf[j] = buf[i];
	    j++;
	    z++;
	    if (z == discp -> clustsiz) {
		if (write (discp -> fd, outbuf, j) < j)
		    io--;
		     follow = fatfree (discp)   ;
		if ( follow < 0)
		    io--;
		else {
		    fatput (free, follow, discp);
		    fatput (follow, 0xffff, discp);
		    free = follow;
		    j = 0;
		    adr = DOSADR (free, discp -> clustsiz) + discp -> data;
		    pos = lseek (discp -> fd, adr, 0);
		    z = 0;
		}
	    }

	    if (j == bufsize && io) {
		if (write (discp -> fd, outbuf, bufsize) < bufsize)
		    io--;
		j = 0;
	    }
	}			/* for */
	if (z != 0 && j != 0)
	    if (write (discp -> fd, outbuf, j) < j)
		io--;
	j = 0;
    }				/* while */


    fnrec.dim += nl;

    if (!io) {
	fatdel (&fnrec, discp);
	return (-2);
    }


 /* Screiben eines Verzeichniseintrages */
    depos = lseek (discp -> fd, depos, 0);
    if (putde (&fnrec, discp) < 0)
	return (-1);

    return (1);
}




/* fatfree
 * Liefert die naechste freie Clusternummer        
 * auf dem Datentraeger discp .                  
 * Liefert -1 bei Lesefehler und
 *         -3 falls die Disk voll ist   
*/

unsigned    fatfree (discp)
struct dosdisc *discp;

{
    long    pos,
            freeadr;
    unsigned char   n12[3],
                    n16[2];
    unsigned    n1,
                n2,
                fatsiz,
                z = 1;
    int     len;

    fatsiz = (discp -> maindir - discp -> fat1) / discp -> fatnum;
 /* Anzahl  Byte je FAT */

/* pos = lseek(discp->fd,(long)discp->fat1,0);  */

    if (discp -> fatelen == 12) {
	fatsiz -= 3;
	lseek (discp -> fd, (long) discp -> fat1 + 3, 0);
	while (fatsiz / 3 > z++) {
	    len = read (discp -> fd, n12, 3);
	    if (len < 3)
		return (-1);
	    n1 = (n12[1] & 0x0f) * 256 + n12[0];
	    n2 = n12[2] * 16 + n12[1] / 16;

	    if (n1 == 0) {
		pos = lseek (discp -> fd, (long) - 3, 1);
		freeadr = pos - discp -> fat1;
		return (freeadr / 1.5);
	    }
	    if (n2 == 0) {
		pos = lseek (discp -> fd, (long) - 3, 1);
		freeadr = pos - discp -> fat1;
		return (freeadr / 1.5 + 1);
	    }
	}
    }
    else {
	fatsiz -= 4;
	lseek (discp -> fd, (long) discp -> fat1 + 4, 0);
	while (fatsiz / 2 > z++) {
	    len = read (discp -> fd, n16, 2);
	    if (len < 2)
		return (-1);
	    n1 = (n16[1] * 256) + n16[0];
	    if (n1 == 0) {
		pos = lseek (discp -> fd, (long) - 2, 1);
		freeadr = pos - discp -> fat1;
		return (freeadr / 2);
	    }
	}
    }
    return (-3);
}




/* putde
 * Schreibt fuer das File statp auf dem
 * Datentraeger discp einen 32-Byte
 * Verzeichniseintrag.
 * Liefert -2 bei Schreibfehler.
*/

putde (statp, discp)
struct dostat  *statp;
struct dosdisc *discp;

{
    int     res,
            i;
    unsigned char   attrib = 0x20,
                   *h,
                    buf[32];
    long    pos;
    unsigned    tv;

    for (i = 0; i < 11; i++)
	buf[i] = statp -> name[i];
    buf[11] = attrib;
    for (i = 12; i < 22; i++)
	buf[i] = '\0';

    tv = statp -> time.hour * 2045 + statp -> time.min * 32 + statp -> time.sec / 2;
/*
 h = &tv;
 buf[22] = *h++;
 buf[23] = *h;   
*/
    buf[22] = tv % 256;
    buf[23] = tv / 256;
    tv = (statp -> date.year - 80) * 512 + statp -> date.month * 32 + statp -> date.day;
/*
 h = &tv;
 buf[24] = *h++;
 buf[25] = *h;
*/
    buf[24] = tv % 256;
    buf[25] = tv / 256;
/*
 h = &(statp->first);
 buf[26] = *h++;
 buf[27] = *h;
*/
    buf[26] = statp -> first % 256;
    buf[27] = statp -> first / 256;
/*
 h = &(statp->dim);
 buf[28] = *h++;
 buf[29] = *h++;
 buf[30] = *h++;
 buf[31] = *h; 
*/
    buf[28] = statp -> dim % 256;
    buf[29] = (statp -> dim % 65536) / 256;
    buf[30] = (statp -> dim % 16777216) / 65536;
    buf[31] = statp -> dim / 16777216;
    if (write (discp -> fd, buf, 32) != 32)
	return (-2);

    return (1);
}
