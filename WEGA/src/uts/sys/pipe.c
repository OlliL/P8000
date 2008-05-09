/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985

	KERN 3.2	Modul: pipe.c
 
 
	Bearbeiter:
	Datum:		$D$
	Version:	$R$
 
*******************************************************************************
******************************************************************************/
 
char pipewstr[] = "@[$]pipe.c		Rev : 4.1 	08/27/83 11:56:31";
 
#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/inode.h>
#include <sys/file.h>

/*------------------------------------------ Funktionen ---------------------*/
 
/*---------------------------------------------------------------------------*/
/*
 * pipe
 * Der Pipe-Systemaufruf.
 * Zuweisen eines Inodes des Wurzelgeraetes (root dev).
 * Zuweisen von zwei File-Strukturen und Verbinden
 * dieser Teile durch Setzen entsprechender Flags.
 */
/*---------------------------------------------------------------------------*/
pipe()
{
	register struct inode *ip;
	register struct file *rf, *wf;
	int r;

	ip = ialloc(pipedev);
	if(ip == NULL)
		return;
	rf = falloc();
	if(rf == NULL) {
		iput(ip);
		return;
	}
	r = u.u_r.r_val1;
	wf = falloc();
	if(wf == NULL) {
		rf->f_count = 0;
		u.u_ofile[r] = NULL;
		iput(ip);
		return;
	}
	u.u_r.r_val2 = u.u_r.r_val1;
	u.u_r.r_val1 = r;
	wf->f_flag = FWRITE;
	wf->f_inode = ip;
	rf->f_flag = FREAD;
	rf->f_inode = ip;
	ip->i_count = 2;
	ip->i_mode = IFIFO;
	ip->i_flag = IACC|IUPD|ICHG;
	ip->i_frcnt=1;
	ip->i_fwcnt=1;
	iupdat(ip,&time,&time);
}


/*----------------------------------------------------------*/
/*
 * openp
 * Eroeffnen einer Pipe
 * Kontrolle Lese-/Schreibgroesse; streichen, wenn notwendig
 */
/*----------------------------------------------------------*/

openp(ip, mode)
register struct inode *ip;
register mode;
{
	if (mode&FREAD) {
		if (ip->i_frcnt++ == 0)
			wakeup((caddr_t)&ip->i_frcnt);
	}
	if (mode&FWRITE) {
		if (mode&FNDELAY && ip->i_frcnt == 0) {
			u.u_error = ENXIO;
			return;
		}
		if (ip->i_fwcnt++ == 0)
			wakeup((caddr_t)&ip->i_fwcnt);
	}
	if (mode&FREAD) {
		while (ip->i_fwcnt == 0) {
			if (mode&FNDELAY || ip->i_size)
				return;
			sleep(&ip->i_fwcnt, PPIPE);
		}
	}
	if (mode&FWRITE) {
		while (ip->i_frcnt == 0)
			sleep(&ip->i_frcnt, PPIPE);
	}
}

/*-------------------------------------------------*/
/*
 * closep
 * Schliessen einer Pipe
 * Update counts and cleanup
 */
/*------------------------------------------------*/

closep(ip, mode)
register struct inode *ip;
register mode;
{
	register i;
	daddr_t bn;

	if (mode&FREAD) {
		if ((--ip->i_frcnt == 0) && (ip->i_fflag&IFIW)) {
			ip->i_fflag &= ~IFIW;
			wakeup((caddr_t)&ip->i_fwcnt);
		}
	}
	if (mode&FWRITE) {
		if ((--ip->i_fwcnt == 0) && (ip->i_fflag&IFIR)) {
			ip->i_fflag &= ~IFIR;
			wakeup((caddr_t)&ip->i_frcnt);
		}
	}
	if ((ip->i_frcnt == 0) && (ip->i_fwcnt == 0)) {
		for (i=NFADDR-1; i>=0; i--) {
			bn = ip->i_faddr[i];
			if (bn == (daddr_t)0)
				continue;
			ip->i_faddr[i] = (daddr_t)0;
			free(ip->i_dev, bn);
		}
		ip->i_size = 0;
		ip->i_frptr = 0;
		ip->i_fwptr = 0;
		ip->i_flag |= IUPD|ICHG;
	}
}
/*---------------------------------------------------------------------------*/
/*
 * plock
 * Blockieren eines Pipekanals.
 * Wenn er bereits blockiert ist, so wird das WANT-Flag gesetzt
 * und auf die Freigabe gewartet.
 */
/*---------------------------------------------------------------------------*/
plock(ip)
register struct inode *ip;
{
int fcw;

	fcw=dvi();
	while(ip->i_flag&ILOCK) {
		ip->i_flag |= IWANT;
		sleep((caddr_t)ip, PINOD);
	}
	ip->i_flag |= ILOCK;
	rvi(fcw);
}

/*---------------------------------------------------------------------------*/
/*
 * prele
 * Aufhebung der Blockierung eines Inodes.
 * Wenn das WANT-Flag gesetzt ist,
 * so Aufwecken des wartenden Prozesses.
 * Diese Routine wird auch allgemein benutzt,
 * um die Blockierung eines Inodes aufzuheben.
 */
/*---------------------------------------------------------------------------*/
prele(ip)
register struct inode *ip;
{
int fcw;

	fcw=dvi();
	ip->i_flag &= ~ILOCK;
	if(ip->i_flag&IWANT) {
		ip->i_flag &= ~IWANT;
		wakeup((caddr_t)ip);
	}
	rvi(fcw);
}
