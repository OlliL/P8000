/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Modul sys.c
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:15:32
	Version:	1.2
 
*******************************************************************************
******************************************************************************/
 

 
#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <saio.h>

/*------------------------------------------ Funktionen ---------------------*/
 
 
/*---------------------------------------------------------------------------*/
/*
 *openi
 * inode Block in Puffer einlesen, entsprechende inode
 * auswaehlen, Uebertragen von Teilen der Disk inode (dinode),
 * das sind inumber imode isize und die Pointer auf die
 * Daten- und indirekten Pointerbloecke (mit Konvertierung
 * 3 byte Integer in long integer), in den iob.
 * Parameter:	n = inumber
 *		io = Pointer auf iob
 */
/*---------------------------------------------------------------------------*/
static
openi(n,io)
register struct iob *io;
register n;
{
	register struct dinode *dp;

	io->i_offset = 0;
	io->i_bn = (daddr_t)((n+15)/INOPB) + io->i_boff;
	io->i_cc = 512;
	io->i_ma = io->i_buf;
	devread(io);

	dp = (struct dinode *)io->i_buf;
	dp = &dp[(n-1)%INOPB];
	io->i_ino.i_number = n;
	io->i_ino.i_mode = dp->di_mode;
	io->i_ino.i_size = dp->di_size;
	l3tol((char *)io->i_ino.i_addr,(char *)dp->di_addr,NADDR);
}

/*---------------------------------------------------------------------------*/
/*
 *find
 * Suchen einer file mit dem angegebenen path name in der (den)
 * directories
 * Parameter:	path - Pointer auf pathname string
 *		file - Pointer auf iob
 * Return:	inumber der file wenn gefunden
 *		0 wenn nicht gefunden
 */
/*---------------------------------------------------------------------------*/
static
find(path, file)
register char *path;
register struct iob *file;
{
	register char *q;
	register char c;
	register n;
	ino_t dlook();

	if (path==0 || *path=='\0') {
		printf("null path\n");
		return(0);
	}

	openi((ino_t) 2, file);
	while (*path) {
		while (*path == '/')
			path++;
		q = path;
		while(*q != '/' && *q != '\0')
			q++;
		c = *q;
		*q = '\0';

		if ((n=dlook(path, file))!=0) {
			if (c=='\0')
				break;
			openi(n, file);
			*q = c;
			path = q;
			continue;
		} else {
			printf("%s not found\n",path);
			return(0);
		}
	}
	return(n);
}

/*---------------------------------------------------------------------------*/
/*
 *sbmap
 * Ermittlung der Blocknummer im Filesystem fuer
 * einen gewueschten logischen Datenblock
 *
 * Parameter:	io - Pointer auf iob
 *		bn - Blocknummer (logische, n.Datenblock der file
 * Return:	Blocknummer im Filesystem
 *		0 bei Fehler
 */
/*---------------------------------------------------------------------------*/
static daddr_t
sbmap(io, bn)
register struct iob *io;
register daddr_t bn;
{
	register i;
	register struct inode *ip;
	register j, sh;
	register daddr_t nb, *bap;

	ip = &io->i_ino;;
	if(bn < 0) {
		printf("bn negative\n");
		return((daddr_t)0);
	}

	/*
	 * blocks 0..NADDR-4 sind direct blocks
	 */
	if(bn < NADDR-3) {
		i = bn;
		nb = ip->i_addr[i];
		return(nb);
	}

	/*
	 * Adressen NADDR-3, NADDR-2 und NADDR-1
	 * sind single, double, triple indirect blocks.
	 * der erste Schritt ist die Ermittlung
	 * des levels der indirection.
	 */
	sh = 0;
	nb = 1;
	bn -= NADDR-3;
	for(j=3; j>0; j--) {
		sh += NSHIFT;
		nb <<= NSHIFT;
		if(bn < nb)
			break;
		bn -= nb;
	}
	if(j == 0) {
		printf("bn ovf %D\n",bn);
		return((daddr_t)0);
	}

	/*
	 * fetch die Adresse von der inode
	 */
	nb = ip->i_addr[NADDR-j];
	if(nb == 0) {
		printf("bn void %D\n",bn);
		return((daddr_t)0);
	}

	/*
	 * fetch durch die indirect blocks
	 */
	for(; j<=3; j++) {
		if (blknos[j] != nb) {
			io->i_bn = nb + io->i_boff;
			io->i_ma = b[j];
			io->i_cc = 512;
			devread(io);
			blknos[j] = nb;
		}
		bap = (daddr_t *)b[j];
		sh -= NSHIFT;
		i = (bn>>sh) & NMASK;
		nb = bap[i];
		if(nb == 0) {
			printf("bn void %D\n",bn);
			return((daddr_t)0);
		}
	}

	return(nb);
}

/*---------------------------------------------------------------------------*/
/*
 *dlook
 * Lesen einer directory und Durchsuchen nach einer file
 *
 * Parameter:	s - Pointer auf filename string
 *		io - Pointer auf iob
 * Return:	inumber falls file gefunden
 *		0 wenn nicht gefunden bzw bei Fehler
 */
/*---------------------------------------------------------------------------*/
static ino_t
dlook(s, io)
register char *s;
register struct iob *io;
{
	register struct direct *dp;
	register struct inode *ip;
	register daddr_t bn;
	register n,dc;

	if (s==0 || *s=='\0')
		return(0);
	ip = &io->i_ino;
	if ((ip->i_mode&IFMT)!=IFDIR) {
		printf("not a directory\n");
		return(0);
	}

	n = ip->i_size/sizeof(struct direct);

	if (n==0) {
		printf("zero length directory\n");
		return(0);
	}

	dc = 512;
	bn = (daddr_t)0;
	while(n--) {
		if (++dc >= 512/sizeof(struct direct)) {
			io->i_bn = sbmap(io, bn++) + io->i_boff;
			io->i_ma = io->i_buf;
			io->i_cc = 512;
			devread(io);
			dp = (struct direct *)io->i_buf;
			dc = 0;
		}

		if (match(s, dp->d_name))
			return(dp->d_ino);
		dp++;
	}
	return(0);
}

/*---------------------------------------------------------------------------*/
/*
 *match
 * Vergleich zweier Strings (device filename)
 *
 * Parameter:	s1, s2 - Pointer auf filename string
 * Return:	1 - Strings sind gleich
 *		0 - Strings sind ungleich
 */
/*---------------------------------------------------------------------------*/
static
match(s1,s2)
register char *s1,*s2;
{
	register cc;

	cc = DIRSIZ;
	while (cc--) {
		if (*s1 != *s2)
			return(0);
		if (*s1++ && *s2++)
			continue; else
			return(1);
	}
	return(1);
}

/*---------------------------------------------------------------------------*/
/*
 *lseek
 * Positionierung des Filepointers (in iob)
 *
 * Parameter:	fdesc - Nummer des Filedescriptors
 *		addr  - gewueschter Offset im file
 *		ptr   - nur 0 = offset von Beginn der file zulaessig
 * Return:	0  - ok
 *		-1 - bei Fehler
 */
/*---------------------------------------------------------------------------*/
lseek(fdesc, addr, ptr)
register fdesc;
register off_t addr;
register ptr;
{
	register struct iob *io;

	if (ptr != 0) {
		printf("Seek not from beginning of file\n");
		return(-1);
	}
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((io = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	io->i_offset = addr;
	io->i_bn = addr/512 + io->i_boff;
	io->i_cc = 0;
	return(0);
}

/*---------------------------------------------------------------------------*/
/*
 *getc
 * 1 Zeichen (Byte) der eroeffneten file holen, Filepointer
 * incrementieren
 * bei fdesc < 3 Zeichen von Console (getchar) einlesen
 * open ist dafuer nicht moeglich bzw erforderlich
 *
 * Parameter:	fdesc - Nummer Filedescriptor
 * Return:	Zeichen falls ok
 *		-1 bei Fehler
 */
/*---------------------------------------------------------------------------*/
getc(fdesc)
register fdesc;
{
	register struct iob *io;
	register char *p;
	register c;
	register off;


	if (fdesc >= 0 && fdesc <= 2)
		return(getchar());
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((io = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	p = io->i_ma;
	if (io->i_cc <= 0) {
		io->i_bn = io->i_offset/(off_t)512;
		if (io->i_flgs&F_FILE)
			io->i_bn = sbmap(io, io->i_bn) + io->i_boff;
		io->i_ma = io->i_buf;
		io->i_cc = 512;
		devread(io);
		if (io->i_flgs&F_FILE) {
			off = io->i_offset % (off_t)512;
			if (io->i_offset+(512-off) >= io->i_ino.i_size)
				io->i_cc = io->i_ino.i_size - io->i_offset + off;
			io->i_cc -= off;
			if (io->i_cc <= 0)
				return(-1);
		} else
			off = 0;
		p = &io->i_buf[off];
	}
	io->i_cc--;
	io->i_offset++;
	c = (unsigned char)*p++;
	io->i_ma = p;
	return(c);
}
 
/*---------------------------------------------------------------------------*/
/*
 *getw
 * 1 Word der eroeffneten file holen, Filepointer
 * incrementieren
 * bei fdesc < 3 word von Console (getchar) einlesen
 * open ist dafuer nicht moeglich bzw erforderlich
 *
 * Parameter:	fdesc - Nummer Filedescriptor
 * Return:	word falls ok
 *		-1 bei Fehler
 */
/*---------------------------------------------------------------------------*/
getw(fdesc)
register fdesc;
{
	register w,i;
	register char *cp;
	int val;

	for (i = 0, val = 0, cp = (char *)&val; i < sizeof(val); i++) {
		w = getc(fdesc);
		if (w < 0) {
			if (i == 0)
				return(-1);
			else
				return(val);
		}
		*cp++ = w;
	}
	return(val);
}

/*---------------------------------------------------------------------------*/
/*
 *read
 * 1 Block (bzw eine Anzahl von Bytes) ab aktuellem Filepointer
 * der eroeffneten file lesen,
 * bei fdesc < 3 von Console (getchar) einlesen, open
 * fuer diesen Fall nicht moeglich
 *
 * Parameter:	fdesc - Nummer Filedescriptor
 *		buf - Pointer auf Puffer
 *		count - Anzahl zu lesender Bytes
 * Return:	Anzahl gelesener Bytes fall ok
 *		-1 bei Fehler
 *		0 bei negativem count
 */
/*---------------------------------------------------------------------------*/
read(fdesc, buf, count)
register fdesc;
register char *buf;
register count;
{
	register i;
	register struct iob *file;

	if (fdesc >= 0 & fdesc <= 2) {
		i = count;
		do {
			*buf = getchar();
		} while (--i && *buf++ != '\n');
		return(count - i);
	}
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	if ((file->i_flgs&F_READ) == 0)
		return(-1);
	if ((file->i_flgs&F_FILE) == 0) {
		file->i_cc = count;
		file->i_ma = buf;
		i = devread(file);
		file->i_bn++;
		return(i);
	}
	else {
		if (file->i_offset+count > file->i_ino.i_size)
			count = file->i_ino.i_size - file->i_offset;
		if ((i = count) <= 0)
			return(0);
		do {
			*buf++ = getc(fdesc+3);
		} while (--i);
		return(count);
	}
}

/*---------------------------------------------------------------------------*/
/*
 *write
 * 1 Block (bzw eine Anzahl von Bytes) ab aktuellem Filepointer
 * der eroeffneten file schreiben,
 * bei fdesc < 3 an Console (putchar) ausgeben, open
 * fuer diesen Fall nicht moeglich
 *
 * Parameter:	fdesc - Nummer Filedescriptor
 *		buf - Pointer auf Puffer
 *		count - Anzahl zu schreibender Bytes
 * Return:	Anzahl geschriebener Bytes fall ok
 *		-1 bei Fehler
 */
/*---------------------------------------------------------------------------*/
write(fdesc, buf, count)
register fdesc;
register char *buf;
register count;
{
	register i;
	register struct iob *file;

	if (fdesc >= 0 && fdesc <= 2) {
		i = count;
		while (i--)
			putchar(*buf++);
		return(count);
	}
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	if ((file->i_flgs&F_WRITE) == 0)
		return(-1);
	file->i_cc = count;
	file->i_ma = buf;
	i = devwrite(file);
	file->i_bn++;
	return(i);
}

/*---------------------------------------------------------------------------*/
/*
 *open
 * device / file eroeffnen
 * 4 files / devices koennen gleichzeitig open sein
 *
 * Parameter:	str - Pointer auf device / filename string
 *			zB zd(0,13200)wega
 *			zd - devicename
 *			0  - Nummer des physischen drives
 *			13200 - offset
 *				bei Disk: Beginn des logischen
 *				Filesystems
 *				bei Tape: n. file auf Tape
 *				(filename nicht moeglich)
			wega - filename
 *				bei einem Disk-Device:
				auch vollstaendiger pathname moeglich,
 *				nur Lesen einer file moeglich !
 *				ist kein filename angegeben,wird
 *				aber das device fuer Zugriffe
 *				auf Blockebene eroeffnet.
 *				bei rm device (Kopplung mit local
 *				System ueber V24):
 *				drivenummer und offset werden nicht
 *				benutzt, mueesen aber angegeben
 *				werden (zB 0), nur filename moeglich.
 *		how - 0 = open fuer read
 *		      1 = open fuer write (falls moeglich)
 * Return:	Nummer des filedescriptors wenn ok
 *		-1 bei Fehler
 */
/*---------------------------------------------------------------------------*/
open(str, how)
register char *str;
register how;
{
	register char *cp;
	register i;
	register struct iob *file;
	register struct devsw *dp;
	register fdesc;
	static first = 1;
	long	atol();

	if (first) {
		for (i = 0; i < NFILES; i++)
			iob[i].i_flgs = 0;
		first = 0;
	}

	for (fdesc = 0; fdesc < NFILES; fdesc++)
		if (iob[fdesc].i_flgs == 0)
			goto gotfile;
	_stop("No more file slots");
gotfile:
	(file = &iob[fdesc])->i_flgs |= F_ALLOC;

	for (cp = str; *cp && *cp != '('; cp++)
			;
	if (*cp != '(') {
		printf("Bad device\n");
		file->i_flgs = 0;
		return(-1);
	}
	*cp++ = '\0';
	for (dp = devsw; dp->dv_name; dp++) {
		if (match(str, dp->dv_name))
			goto gotdev;
	}
	printf("Unknown device\n");
	file->i_flgs = 0;
	return(-1);
gotdev:
	*(cp-1) = '(';
	file->i_ino.i_dev = dp-devsw;
	file->i_unit = *cp++ - '0';
	if (file->i_unit < 0 || file->i_unit > 7) {
		printf("Bad unit specifier\n");
		file->i_flgs = 0;
		return(-1);
	}
	if (*cp++ != ',') {
badoff:
		printf("Missing offset specification\n");
		file->i_flgs = 0;
		return(-1);
	}
	file->i_boff = atol(cp);
	for (;;) {
		if (*cp == ')')
			break;
		if (*cp++)
			continue;
		goto badoff;
	}
	cp++;
	file->i_ino.i_number = (unsigned)cp;
	if (devopen(file) == -1) {
		file->i_flgs = 0;
		return(-1);
	}
	if (file->i_unit == 'r' || *cp == '\0') {
		file->i_flgs |= how+1;
		file->i_cc = 0;
		file->i_offset = 0;
		return(fdesc+3);
	}
	if ((i = find(cp, file)) == 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if (how != 0) {
		printf("Can't write files yet.. Sorry\n");
		file->i_flgs = 0;
		return(-1);
	}
	openi(i, file);
	file->i_offset = 0;
	file->i_cc = 0;
	file->i_flgs |= F_FILE | (how+1);
	return(fdesc+3);
}

/*---------------------------------------------------------------------------*/
/*
 *close
 * file schliessen und iob des files freigeben
 *
 * Parameter:	fdesc - Nummer filedescriptor
 * Return:	0 wenn ok
 *		-1 bei Fehler
 */
/*---------------------------------------------------------------------------*/
close(fdesc)
register fdesc;
{
	register struct iob *file;

	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	if ((file->i_flgs&F_FILE) == 0)
		devclose(file);
	file->i_flgs = 0;
	return(0);
}

/*---------------------------------------------------------------------------*/
/*
 *exit
 * Beendigung des laufenden Programms, Wiedereintrit in
 * boot Programm, schliessen aller offenen files
 */
/*---------------------------------------------------------------------------*/
exit()
{
register fdesc;
 
	for (fdesc=3; fdesc < NFILES+3; fdesc++)
		close(fdesc);
	_stop("Exit called");
}

/*---------------------------------------------------------------------------*/
/*
 *stop
 * Return zu boot (von exit aus)
 */
/*---------------------------------------------------------------------------*/
_stop(s)
register char *s;
{
	printf("%s\n", s);
	_ret();
}
