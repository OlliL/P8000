#include <stdio.h>
#include "cpmio.h"
extern int 	flp_fmt;
extern int	n_systrk;
disk()
{
int i;
printf("\n Disk status information:\n");
printf("\tblocksize %d\t\ttracks %d (%d system)\n",
	blksiz, tracks, n_systrk);
i = maxdir * 32;
i /= 1024;
printf("\tdirectory entrys %d (%d K)\n",
	maxdir,i);
printf ("\t%d sectors per track\n",
	sectrk);
printf("\tsectorlength %d\n",
	seclth);
printf("\tsectors/group:\t%d\t%d\n",
	16/flp_fmt, flp_fmt);
}
