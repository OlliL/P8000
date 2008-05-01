/**************************************************************************
***************************************************************************
 
	W E G A - Quelle
	KERN 3.2	Modul : sys.c
 
	Bearbeiter	: O. Lehmann
	Datum		: 01.05.08
	Version		: 1.0
 
***************************************************************************
**************************************************************************/

#include <sys/param.h>
#include <sys/sysparm.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>

char syswstr[] = "@[$]sys.c		Rev : 4.1 	08/27/83 13:42:32";

/*
 *	indirect driver for controlling tty.
 */

syopen(dev, flag)

{

	if(!(u.u_ttyp)) {
		u.u_error = ENXIO;
		return;
	}
	(*cdevsw[major(u.u_ttyd)].d_open)(u.u_ttyd, flag);
}

syread(dev)
{

	(*cdevsw[major(u.u_ttyd)].d_read)(u.u_ttyd);
}

sywrite(dev)
{

	(*cdevsw[major(u.u_ttyd)].d_write)(u.u_ttyd);
}

sysioctl(dev, cmd, addr, flag)
caddr_t addr;
{

	(*cdevsw[major(u.u_ttyd)].d_ioctl)(u.u_ttyd, cmd, addr, flag);
}
