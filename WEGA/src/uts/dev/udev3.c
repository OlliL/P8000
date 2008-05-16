char udev3wstr[] = "@[$]udev3.c		Rev : 1.1 " ;

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

usr3int (s) 
register struct state *s;
{
	prstate (s);
	panic ("usr3: unexpected interrupt");
	evi ();
}

/*
 ***  block i/o devices  ***
 */
usr3open () 
{
	u.u_error = ENODEV ;
} 

usr3close ()
{
	u.u_error = ENODEV ;
} 

usr3strategy ()
{
	u.u_error = ENODEV ;
} 

/*
 ***  character i/o devices  ***
 */
u3open () 
{
	u.u_error = ENODEV ;
}

u3close ()
{
	u.u_error = ENODEV ;
}

u3read () 
{
	u.u_error = ENODEV ;
}

u3write () 
{
	u.u_error = ENODEV ;
}

u3ioctl () 
{
	u.u_error = ENODEV ;
}
