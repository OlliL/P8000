/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
	KERN 3.2		  Modul: fio.c
 
 
	Bearbeiter:	
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
******************************************************************************/
 
char fiowstr[] = "@[$]fio.c		Rev : 4.1 	08/27/83 11:54:29";
 
#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/filsys.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <sys/inode.h>
#include <sys/acct.h>
#include <sys/mmu.h>
#include <sys/mount.h>
#include <sys/s.out.h>
#include <sys/user.h>
extern int Nfile;
extern int Nmount;

/*------------------------------------------ Funktionen ---------------------*/
 
/*---------------------------------------------------------------------------*/
/*
 * getf
 * Konvertiert einen vom Nutzer gelieferten  Dateidescriptor in der
 * Nutzer-Dateidescriptortabelle (u.u_ofile) in einen Zeiger auf eine
 * file-Struktur in der Dateitabelle (file[])
 */
/*---------------------------------------------------------------------------*/
struct file *
getf(f)
register int f;
{
	register struct file *fp;

	if(0 <= f && f < NOFILE) {
		fp = u.u_ofile[f];
		if(fp != NULL)
			return(fp);
	}
	u.u_error = EBADF;
	return(NULL);
}

/*---------------------------------------------------------------------------*/
/*
 * closef
 * - interne Form von close
 * - dekrementiert den Datei-Referenzzaehler der file-Struktur
 * - sichert, dass das pipe-Protokoll nicht verstopft ist
 * 
 * - dekrementiert den Inode-Referenzszaehler, um die Freigabe der
 *   zugehoerigen Filestruktur zu verfolgen
 * - ruft den Device-Handler beim letzten close auf
 * Beachte:  Der Device-Handler wird bei jedem Open aber nur beim letzten
 *	     Close aufgerufen
 */
/*---------------------------------------------------------------------------*/
closef(fp)
register struct file *fp;
{
	register struct inode *ip;
	struct mount *mp;
	int flag, fmt;
	dev_t dev;
	int (*cfunc)();

	if (fp == NULL)
		return;
	unlock(fp->f_inode);
	if ((unsigned)fp->f_count > 1) {
		fp->f_count--;
		return;
	}
	ip = fp->f_inode;
	plock(ip);
	flag = fp->f_flag;
	dev = (dev_t)ip->i_rdev;
	fmt = ip->i_mode&IFMT;
	fp->f_count = 0;
	switch(fmt) {

	case IFCHR:
		cfunc = cdevsw[major(dev)].d_close;
		break;

	case IFBLK:
		cfunc = bdevsw[major(dev)].d_close;
		break;

	case IFIFO:
		closep(ip, flag);

	default:
		iput(ip);
		return;
	}
	for (fp = file; fp < &file[Nfile]; fp++) {
		register struct inode *tip;

		if (fp->f_count) {
			tip = fp->f_inode;
			if (tip->i_rdev == dev &&
			  (tip->i_mode&IFMT) == fmt)
				goto out;
		}
	}
	if (fmt == IFBLK) {
		for (mp = mount; mp < &mount[Nmount]; mp++)
			if (mp->m_flags == MINUSE && mp->m_dev == dev)
				goto out;
		bflush(dev);
		(*cfunc)(dev, flag);
		binval(dev);
	} else {
		prele(ip);
		(*cfunc)(dev, flag);
	}
out:
	iput(ip);
}

/*---------------------------------------------------------------------------*/
/* openi
 * Ausfuehrung OPEN-Aktion
 * wird aufgerufen, um dem Handler von special files die Moeglichkeit zur
 * Initialisierung und Validisierung zu geben, bevor aktuelle I/O folgen
 */
/*---------------------------------------------------------------------------*/
openi(ip, flag)
register struct inode *ip;
{
	dev_t dev;
	register unsigned int maj;

	dev = (dev_t)ip->i_rdev;
	maj = major(dev);
	switch(ip->i_mode&IFMT) {

	case IFCHR:
		if (maj >= nchrdev)
			goto bad;
		if (u.u_ttyp == NULL)
			u.u_ttyd = dev;
		(*cdevsw[maj].d_open)(dev, flag);
		break;

	case IFBLK:
		if (maj >= nblkdev)
			goto bad;
		(*bdevsw[maj].d_open)(dev, flag);
		break;

	case IFIFO:
		openp(ip, flag);
		break;
	}
	return;

bad:
	u.u_error = ENXIO;
}

/*-----------------------------------------------------------*/
/* access
 * Kontrolle Zugriffsrechte auf einen Inode Pointer
 * Mode ist READ, WRITE oder EXEC
 * WRITE: Read-only Status des Files wird gecheckt;
 *        reine Textsegmente koennen nicht geschrieben werden
 * Der Mode wird verschoben, um die owner/group/other-Felder
 * zu selektieren
 * Der Superuser hat alle Zugriffsrechte
 */
/*-----------------------------------------------------------*/
access(ip, mode)
register struct inode *ip;
{
	register m;

	m = mode;
	if (m == IWRITE) {
		if (getfs(ip->i_dev)->s_ronly) {
			u.u_error = EROFS;
			return(1);
		}
		if (ip->i_flag&ITEXT)
			xrele(ip);
		if (ip->i_flag & ITEXT) {
			u.u_error = ETXTBSY;
			return(1);
		}
	}
	if (u.u_uid == 0)
		return(0);
	if (u.u_uid != ip->i_uid) {
		m >>= 3;
		if (u.u_gid != ip->i_gid)
			m >>= 3;
	}
	if ((ip->i_mode&m) != 0)
		return(0);

	u.u_error = EACCES;
	return(1);
}

/*-------------------------------------------------------------------*/
/* owner
 * Absteigen Pfadnamen und Test, ob die resultierende Inode dem
 * aktuellen Nutzer gehoert. Wenn nicht, Test auf Superuser
 * Ist Zugriff erlaubt, Rueckgabee des Inode-Pointers
 */
/*-------------------------------------------------------------------*/
struct inode *
owner()
{
	register struct inode *ip;

	ip = namei(uchar, 0);
	if (ip == NULL)
		return(NULL);
	if (u.u_uid == ip->i_uid )
		return(ip);
	if( suser())
		return(ip);
	iput(ip);
	return(NULL);
}

/*---------------------------------------------------------*/
/*
 * Test, ob der aktuelle Nutzer der Superuser ist
 */
/*---------------------------------------------------------*/
suser()
{

	if (u.u_uid == 0) {
		u.u_acflag |= ASU;
		return(1);
	}
	u.u_error = EPERM;
	return(0);
}

/*----------------------------------------------*/
/*
 * Zuweisung eines User Filedescriptors
 */
/*----------------------------------------------*/
ufalloc(i)
register i;
{

	for(; i<NOFILE; i++)
		if (u.u_ofile[i] == NULL) {
			u.u_r.r_val.r_val1 = i;
			u.u_pofile[i] = 0;
			return(i);
		}
	u.u_error = EMFILE;
	return(-1);
}

/*----------------------------------------------------------*/
/* falloc
 * Zuweisung eines User-Filedescriptors und einer Filestruktur
 * Initialisierung des Descriptors, so dass er auf die File-
 * struktur verweist
 *
 * (kein File,wenn keine Filestruktur verfuegbar)
 */
/*----------------------------------------------------------*/
struct file *
falloc()
{
	register struct file *fp;
	struct file *fp1;		/* nicht notwendig;==> */
					/* BYTE-KOMPATIBILITAET*/
	register i;

	i = ufalloc(0);
	if (i < 0)
		return(NULL);
	for(fp = &file[0]; fp < &file[Nfile]; fp++)
		if (fp->f_count == 0) {
			u.u_ofile[i] = fp;
			fp->f_count++;
			fp->f_offset = 0;
			return(fp);
		}
	printf("no file\n");
	/*syserr.fileovf++;*/
	u.u_error = ENFILE;
	return(NULL);
}
