/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	boot.c (secundary boot)
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:15:20
	Version:	1.1
 
*******************************************************************************
******************************************************************************/
 

 
#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/z.out.h>
#include <saio.h>

extern unsigned codemmu, datammu, stakmmu;
extern unsigned myseg, hisseg, srcseg;
extern union entry {
	long lentry;
	int ientry[2];
	} entry;
extern char dfline[];
 
unsigned manboot;
unsigned currclk;
unsigned isseg;
int fd;

#define NCHAR 100
char line[NCHAR];
char buffer[0x200];
 
union header {
	struct s_exec s_out;
	struct z_exec z_out;
	} header;
struct segt segtble[128];
 
 
/*---------------------------------------------------------------------------*/
/*
 *main
 * main Funktion von boot
 *
 * Meldung Boot Programm, Eingabe des zu ladenen Programms
 * bei manuellem boot bzw interner Name des Programms
 * - zd(x,yyyy)wega - bei automatischem boot. Open file,
 * bei Fehler Wiederholung der Eingabe, wenn open ok
 * laden des files durch loadit. Return in mch Module,
 * dort erfolgt Start des geladenen Programms
 */
/*---------------------------------------------------------------------------*/
main()
{
	printf("\n\n\nBoot\n");
	do {
		line[0] = 0;
		printf(": ");
		if (manboot)
			gets(line, NCHAR);
		if (line[0] == 0) {
			printf(dfline);
			putchar('\n');
			fd = open(dfline, 0);
		}else
			fd = open(line, 0);
	} while (fd < 0);

	loadit(fd);
	putchar('\n');
}
 
/*---------------------------------------------------------------------------*/
/*
 *loadit
 * Laden eines executable files in den RAM
 * Folgende files koennen geladen werden:
 *	s.out, segmented, separate instruction & data (Magic E611)
 *	s.out, segmented, executable (Magic E607)
 *	s.out, nonsegmented, separate instruction & data (Magic E711)
 *	s.out, nonsegmented, executable (Magic E707)
 *	z.out, nonsegmented, separate instruction & data (Magic E811)
 *	z.out, nonsegmeted, executable (Magic E807)
 *
 * Segmentierte (standalone) Programme werden in die Segmente geladen
 * fuer die sie gelinkt wurden (Segment 62 und 63 fuer boot reserviert)
 * nichtsegmentierte Programme werden in Segment 0 geladen.
 * Der BSS Bereich wird mit 0 initialisiert.
 */
/*---------------------------------------------------------------------------*/
loadit(io)
register io;
{
	register unsigned sepid;
	register unsigned nsegt;
	register struct s_exec *psh = &header.s_out;
	register struct z_exec *pzh = (struct z_exec *)psh;
	register struct segt *psegt = segtble;
	register unsigned csize;
	unsigned dsize;
	unsigned bsize;
 
	isseg = 0;		/* nonsegmented init */
	sepid = 0;		/* executable init */
	fd = io;
	lseek(fd, (off_t)0, 0);
	psh->s_magic = getw(io);
 
	switch(psh->s_magic) {
 
	case 0xE611:		/* s.out segmented seperate I&D */
		sepid = 1;
	case 0xE607:		/* s.out segmented executable */
		isseg++;
		goto E707;
 
	case 0xE711:		/* s.out nonseg separate I&D */
		sepid = 1;
	case 0xE707:		/* s.out nonseg executable */
 
E707:				/* fuer alle s.out Formate */
 
		getn(io, (char *)&header.s_out.s_imsize, sizeof(struct s_exec)-sizeof(int));
		entry.lentry = psh->s_entry;
		getn(io, (char *)psegt, psh->s_segt);
		nsegt = psh->s_segt / sizeof(struct segt);
 
		for (; nsegt != 0; nsegt--) {
			if (psegt->sg_atr & (SG_OCODE | SG_ODATA | SG_OBSS)) {
				close(io);
				printf("Cannot load offset segs yet\n");
				exit(1);
			}
			csize=0;
			dsize=0;
			bsize=0;
			if (psegt->sg_atr & SG_CODE)
				csize = psegt->sg_code;
			if (psegt->sg_atr & SG_DATA)
				dsize = psegt->sg_data;
			if (psegt->sg_atr & SG_BSS)
				bsize = psegt->sg_bss;
			loadseg((unsigned char)psegt->sg_segno, csize, dsize, bsize, sepid);
			psegt++;
		}
		break;
 
	case 0xE811:		/* z.out nonseg sep I&D */
		sepid = 1;
	case 0xE807:		/*z.out nonseg executable */
 
		getn(io, (char *)&header.z_out.z_code, sizeof(struct z_exec)-sizeof(int));
		entry.ientry[1] = pzh->z_entry;
		loadseg(hisseg, pzh->z_code, pzh->z_data, pzh->z_bss, sepid);
		break;
 
	default:
		close(fd);
		printf("Cannot load 0x%x files\n", psh->s_magic);
		exit(1);
		return;
	}
 
	close(io);
	if (isseg == 0)
		entry.ientry[0] = hisseg * 256;
}
 
/*---------------------------------------------------------------------------*/
/*
 *getn
 * Laden von n Bytes einer eroeffnetten file in einen Puffer
 */
/*---------------------------------------------------------------------------*/
getn(io, buf, n)
register io;
register char *buf;
register unsigned n;
{
	for (; n != 0; n--)
		*buf++ = getc(io);
}
 
/*---------------------------------------------------------------------------*/
/*
 *getsh
 * Laden des Headers einer s.out file
 *
 * (wird innerhalb des boot Programms nicht verwendet)
 */
/*---------------------------------------------------------------------------*/
struct s_exec *
getsh(io, magic)
{
	header.s_magic = magic;
	getn(io, (char *)&header.s_out.s_imsize, sizeof(struct s_exec)-sizeof(int));
	return(&header.s_out);
}
 
/*---------------------------------------------------------------------------*/
/*
 *getzh
 * Laden des Headers einer z.out file
 *
 * (wird innerhalb des boot Programms nicht verwendet)
 */
/*---------------------------------------------------------------------------*/
struct z_exec *
getzh(io, magic)
{
	header.z_out.z_magic = magic;
	getn(io, (char *)&header.z_out.z_code, sizeof(struct z_exec)-sizeof(int));
	return(&header.z_out);
}
 
/*---------------------------------------------------------------------------*/
/*
 *getst
 * Lader der Segment table einer s.out file
 *
 * (wird innerhalb des boot Programms nicht verwendet)
 */
/*---------------------------------------------------------------------------*/
struct segt *
getst(io, segt)
register io;
register unsigned segt;
{
	getn(io, (char *)segtble, segt);
	return(segtble);
}
 
/*---------------------------------------------------------------------------*/
/*
 *loadseg
 * Laden eines Segmentes des Programms
 *
 * Jedes im Programm vorhandene Segment ist in der Segment table
 * beschrieben (siehe s.out.h)
 * Die MMU's werden fuer dieses Segment gesetzt.
 * Fuer executable Programme wird Code MMU = Data MMU gesetzt,
 * Stack MMU wird = Data MMU gesetzt, bei nichtsegmentierten
 * Programmen wird die Laenge fuer die Stack MMU auf 64K gesetzt
 * (Stack liegt von F800 abwaerts).
 */
/*---------------------------------------------------------------------------*/
loadseg(segnr, csize, dsize, bsize, sepid)
register unsigned segnr, csize, dsize, bsize, sepid;
{
	register unsigned tsize;
 
	if (sepid == 0) {
		dsize += csize;
		csize = 0;
	}
	if (csize) {
		tsize = ((unsigned long)csize+256)/256;
		setmmu(codemmu, segnr, currclk, tsize-1, 0);
		loadmem(currclk, csize);
		currclk += tsize;
	}
 
	bsize += dsize;
	if (isseg == 0 && bsize == 0)
		bsize++;
 
	if (bsize) {
		bsize = ((unsigned long)bsize+256)/256;
		tsize = bsize;
		tsize--;
		setmmu(datammu, segnr, currclk, tsize, 0);
		if (sepid == 0 || isseg)
			setmmu(codemmu, segnr, currclk, tsize, 0);
		if (isseg == 0 && sepid == 0) {
			bsize = 0x0100; /* nonseg size = 64k */
			tsize = bsize;
			tsize--;
			setmmu(codemmu, segnr, currclk, tsize, 0);
		}
		setmmu(stakmmu, segnr, currclk, tsize, 0);
		clrseg(currclk, bsize); /* data+bss = 0 */
		if (dsize)
			loadmem(currclk, dsize);
 
		currclk += bsize;
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *loadmem
 * Laden des Code oder Data Teil eines Segmentes in den RAM
 * Dafuer wird Segment 62 der MMU's verwendet
 
 */
/*---------------------------------------------------------------------------*/
loadmem(addr, size)
register unsigned addr, size;
{
	register char *bp;
	register unsigned srcadr;
	register unsigned smin, count;
	register unsigned len = 0x100;
 
	setmmu(codemmu, srcseg, addr, 0xFF, 0);
	setmmu(datammu, srcseg, addr, 0xFF, 0);
	setmmu(stakmmu, srcseg, addr, 0xFF, 0);
	for (srcadr = 0; size != 0; size -= smin) {
		smin = min(size, len);
		for (bp = buffer, count = smin; count != 0; count--)
			*bp++ = getc(fd);
		if (smin < len)
			zero(bp, len-smin);
		scopy(srcseg, srcadr, myseg, buffer, len);
		srcadr += len;
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *setmmu
 * ein Segment einer MMU programmieren
 *
 * Die Daten fuer 1 Segment werden in den Stack gebracht
 * und loadsd im Modul mch aufgerufen.
 */
/*---------------------------------------------------------------------------*/
setmmu(mmu, segnr, addr, size, attrib)
unsigned mmu, segnr, addr, size, attrib;
{
	struct segd mmuout;
 
	mmuout.sg_base = addr;
	mmuout.sg_limit = size;
	mmuout.sg_attr = attrib;
	loadsd(mmu, segnr, &mmuout);
}
 
/*---------------------------------------------------------------------------*/
/*
 *clrseg
 * Speicherbereich mit 00 initialisieren
 *
 * Ein Puffer im boot Programm (0x100 Laenge) wird mit
 * 00 beschrieben. Dieser wird entsprechend der vorgegebenen
 * Laenge n mal in den physischen Speicherbereich kopiert
 */
/*---------------------------------------------------------------------------*/
clrseg(addr, size)
register unsigned addr, size;
{
	register unsigned srcadr;
	register unsigned len = 0x100;
 
	setmmu(codemmu, srcseg, addr, size-1, 0);
	setmmu(datammu, srcseg, addr, size-1, 0);
	setmmu(stakmmu, srcseg, addr, size-1, 0);
	srcadr = 0;
	zero(buffer, len);
	for (; size != 0; size--) {
		scopy(srcseg, srcadr, myseg, buffer, len);
		srcadr += len;
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *zero
 * Puffer (im boot) mit 0 beschreiben
 */
/*---------------------------------------------------------------------------*/
zero(buf, n)
register char *buf;
register unsigned n;
{
	for (; n != 0; n--)
		*buf++ = 0;
}
 
/*---------------------------------------------------------------------------*/
/*
 *min
 * Ermittlung der kleineren von 2 Zahlen
 */
/*---------------------------------------------------------------------------*/
min(num1, num2)
unsigned num1, num2;
{
	if (num1 < num2)
		return(num1);
	return(num2);
}
