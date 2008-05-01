/**************************************************************************
***************************************************************************
 
	W E G A - Quelle
	KERN 3.2	Modul : mem.c
 
	Bearbeiter	: O. Lehmann
	Datum		: 01.05.08
	Version		: 1.0
 
***************************************************************************
**************************************************************************/

#include <sys/param.h>
#include <sys/tty.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>

char memwstr[] = "@[$]mem.c		Rev: 4.1	08/27/83 13:39:47";

/*
 *	Memory special file
 *	minor device 0 is physical memory
 *	minor device 1 is kernel memory
 *		(physical address := virtual address + contents of kernel mapping register )
 *	minor device 2 is EOF/RATHOLE
 */


mmread(dev)
{
	unsigned char c;
	switch (minor(dev)){
		case 0:	do {
			/* physical memory */
			if (fpbyte(u.u_offset, &c)){
				u.u_error = ENXIO;
				return;
			}
			if ((passc(c)) == -1)
				return;
		} while (1); 
		case 1:	 do {
			/* kernel memory */
			if (fkbyte(u.u_offset, &c)){
				u.u_error = ENXIO;
				return;
			}
			if ((passc(c)) == -1)
				return;
		} while (1); 
	} 
} 

mmwrite(dev)
{
	register int c;

	switch (minor (dev)) {
	case 0: 		/* physical memory */
			do {
			if ((c = cpass()) < 0) return;
			if (spbyte(u.u_offset-1, c)){
				u.u_error = ENXIO;
				return;
			}
		} 
		while (1);
		break;
	case 1: 		/* kernel memory */
		do {
			if ((c = cpass()) < 0) return;
			if (skbyte(u.u_offset-1, c)){
				u.u_error = ENXIO;
				return;
			}
		} 
		while (1);
		break;
	case 2: 
		u.u_count = 0;
	}
}
