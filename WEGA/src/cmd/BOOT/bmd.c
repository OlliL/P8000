/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Modul md.c
 
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:15:13
	Version:	1.1
 
*******************************************************************************
 
	Device Treiber md fuer Disk
	Der physische Treiber befindet sich im Monitor
 
*******************************************************************************
******************************************************************************/
 

 
#include <sys/param.h>
#include <sys/inode.h>
#include <saio.h>
 
extern long bsbase;
 
 
 
mdstrategy(io, func)
register struct iob *io;
{
	register unsigned errcount;
	register daddr_t bn;
	register daddr_t addr;
	register unsigned count;
	unsigned size;

	bn = io->i_bn;
	count = io->i_cc;
	addr = (daddr_t)io->i_ma + bsbase;
 
	for (; count != 0; count -= size) {
		errcount = 0;
		size = min(0x200, count);
		for (; disk(func, io->i_unit, bn, addr, size);) {
			if (errcount == 0)
				printf("md: io error\n");
			if (++errcount >= 10) {
				printf ("md: fatal error\n");
				return(-1);
			}
		}
		addr += size;
		bn++;
	}
	return(io->i_cc);
}
