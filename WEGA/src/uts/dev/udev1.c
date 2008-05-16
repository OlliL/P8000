char udev1wstr[] = "@[$]udev1.c		Rev : 1.1 " ;

/*
 * This file contains all the dummy device driver routines included
 * in the bdevsw and cdevsw tables in conf.c. 
 * Each of these dummy routines is equivalent to nodev() (subr.c).
 */

#include	"../h/param.h"
#include	"../h/sysinfo.h"
#include	"../h/systm.h"
#include	"../h/dir.h"
#include	"../h/state.h"
#include	"../h/mmu.h"
#include	"../h/s.out.h"
#include	"../h/user.h"

usr1int (s) 
register struct state *s;
{
	prstate (s);
	panic ("usr1: unexpected interrupt");
	evi ();
}

/*
 ***  block i/o devices  ***
 */
usr1open () 
{
	u.u_error = ENODEV ;
} 

usr1close ()
{
	u.u_error = ENODEV ;
} 

usr1strategy ()
{
	u.u_error = ENODEV ;
} 

/*
 ***  character i/o devices  ***
 */
u1open () 
{
	u.u_error = ENODEV ;
}

u1close ()
{
	u.u_error = ENODEV ;
}

u1read () 
{
	u.u_error = ENODEV ;
}

u1write () 
{
	u.u_error = ENODEV ;
}

u1ioctl () 
{
	u.u_error = ENODEV ;
}
