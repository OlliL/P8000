/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1989
	Programm: dosformat.c
 
	Bearbeiter	: P. Hoge
	Datum		: 6.1.89
	Version		: 1.1
 
***************************************************************************

	Formatieren von DCP-Disketten
	Syntax: dosformat [[-96ds15|96ds9|48ds9] dosdev]

***************************************************************************
**************************************************************************/


#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dos.h"

char *arg0;
int fd;
char upath[LMAX],dpath[LMAX];
char buf[512];

/*----------------------------------------------------------------
device	format	sectors	sides	cluster	blocks	dirs	FAT-secs
------------------------------------------------------------------
48ss8	0xfe	8	1	1	320	64	1
48ss9	0xfc	9	1	1	360	64	1
48ds8	0xff	8	2	2	640	112	2
48ds9	0xfd	9	2	2	720	112	2
96ds9	0xf9	9	2	2	1440	112	3
96ds15	0xf9	15	2	1	2400	224	7
-----------------------------------------------------------------*/

char f48ss9[31] = {
	0xeb, 0x34, 0x90,		/* JMP BOOT */
	'W','E','G','A','-','D','O','S',/* System id */
	00, 2,				/* bytes per sector */
	2,				/* sectors per cluster */
	01, 0,				/* reserved sectors at beginning */
	2,				/* fat-copies */
	64, 0,				/* directory entries */
	360%256, 360/256,		/* number of all sectors */
	0xfc,				/* format id */
	2,0,				/* sectors per fat */
	9,0,				/* sectors per track */
	1,0,				/* number of sides */
	0,0,				/* special reserved sectors */
	0xf0,				/* 1. byte fat */
};
char f48ds9[31] = {
	0xeb, 0x34, 0x90,		/* JMP BOOT */
	'W','E','G','A','-','D','O','S',/* System id */
	00, 2,				/* bytes per sector */
	2,				/* sectors per cluster */
	01, 0,				/* reserved sectors at beginning */
	2,				/* fat-copies */
	112, 0,				/* directory entries */
	720%256, 720/256,		/* number of all sectors */
	0xfd,				/* format id */
	2,0,				/* sectors per fat */
	9,0,				/* sectors per track */
	2,0,				/* number of sides */
	0,0,				/* special reserved sectors */
	0xf9				/* 1. byte fat */
};
char f96ds9[31] = {
	0xeb, 0x34, 0x90,		/* JMP BOOT */
	'W','E','G','A','-','D','O','S',/* System id */
	00, 2,				/* bytes per sector */
	2,				/* sectors per cluster */
	01, 0,				/* reserved sectors at beginning */
	2,				/* fat-copies */
	112, 0,				/* directory entries */
	1440%256, 1440/256,		/* number of all sectors */
	0xf9,				/* format id */
	3,0,				/* sectors per fat */
	9,0,				/* sectors per track */
	2,0,				/* number of sides */
	0,0,				/* special reserved sectors */
	0xf9				/* 1. byte fat */
};
char f96ds15[31] = {
	0xeb, 0x34, 0x90,		/* JMP BOOT */
	'W','E','G','A','-','D','O','S',/* System id */
	00, 2,				/* bytes per sector */
	1,				/* sectors per cluster */
	01, 0,				/* reserved sectors at beginning */
	2,				/* fat-copies */
	224, 0,				/* directory entries */
	2400%256, 2400/256,		/* number of all sectors */
	0xf9,				/* format id */
	7,0,				/* sectors per fat */
	15,0,				/* sectors per track */
	2,0,				/* number of sides */
	0,0,				/* special reserved sectors */
	0xf9				/* 1. byte fat */
};


main(argc,argv)
register char **argv;
{
 register char *fdname;
 register char *fdtyp;
 int dev = 0;
 struct stat mstat;
 char *pboot;

 arg0 = argv[0];
 switch(argc) {
 case 1:
	fdtyp = "-96ds9";
 	fdname = "/dev/rfd196ds9";
	break;
 case 2:
	fdtyp = "-96ds9";
	fdname = argv[1];
	break;
 case 3:
	fdtyp = argv[1];
	fdname = argv[2];
	break;
 default:
 error:
	fprintf(stderr,"usage: dosformat [[-96ds15|-96ds9|-48ds9] dosdev]\n");
	exit(1);
 }
 if (fdtyp[0] != '-' || fdname[0] == '-')
	goto error;

 if (dosplit(fdname,upath,dpath) < 0)
	exit(1);

 if (stat(upath, &mstat) == 0) {
	if (mstat.st_mode & (S_IFBLK | S_IFCHR)) {
		dev++;		/* echte Floppy */
		fdtyp = upath;	/* Option ignorieren */
	}
 }
 if (cmp(upath, "/dev/") && dev == 0)
	goto error;
 if (cmp(fdtyp, "48ss9") != 0 && dev)
	pboot = f48ss9;
 else if (cmp(fdtyp, "48ds9") != 0)
	pboot = f48ds9;
 else if (cmp(fdtyp, "96ds9") != 0)
	pboot = f96ds9;
 else if (cmp(fdtyp, "96ds15") != 0)
	pboot = f96ds15;
 else if (dev && cmp(fdtyp, "ss") == 0 && cmp(fdtyp, "ds") == 0) {
	pboot = f96ds9;
 }
 else {
	fprintf(stderr, "%s: falsches DCP-Format\n", arg0);
	exit(1);
 }

 if (dev) {
	strcpy(buf, "format ");		/* physisch Formatieren */
	strcat(buf, upath);
	if (system(buf))
		exit(1);
 }

 if ((fd = open(upath, O_RDWR|O_CREAT|O_TRUNC, 0664)) < 0) {
	fprintf(stderr, "%s: kann %s nicht oeffnen\n", arg0, upath);
	exit(1);
 }
 putdata(pboot);
 exit(0);
}

putsect()
{
 if (write(fd, buf, 512) != 512) {
	fprintf(stderr, "%s: Schreibfehler in %s\n", arg0, upath);
	exit(1);
 }
}

putdata(p)
register char *p;
{
 register i, j;

 clear_buf();			/* write boot sector */
 for (i = 0; i < 30; i++)
	buf[i] = p[i];
 buf[0x2f]  = p[24];	/* sectors/track */
 buf[0x1fe] = 0x55;
 buf[0x1ff] = 0xaa;
 putsect();
				/* write fat's */
 i = p[16];			/* number of fats */
 while (i--) {
	clear_buf();
	buf[0] = p[30];		/* 1. byte fat */
	buf[1] = 0xff;
	buf[2] = 0xff;
	j = p[22];		/* number of fat sectors */
	while (j--) {
		putsect();
		clear_buf();
	}
 }
				/* write directory */
 i = ((p[17]&0x00ff) * 32)/512;	/* number of directory sectors */
 while (i--)
	putsect();
}

clear_buf()
{
	register i = sizeof(buf);
	register char *s = buf;

	while (i--)
		*s++ = 0;
}

cmp(s1, s2)
register char *s1, *s2;
{
	register char *s3;

	s3 = s2;
	while (1) {
		if (*s1 == 0)
			return(0);
		if (*s1 != *s2) {
			s1++;
			continue;
		}
	  loop:
		if (*s2 == 0)
			return(1);
		if (*s1 != *s2) {
			s2 = s3;
			continue;
		}
		s1++;
		s2++;
		goto loop;
	}
}
