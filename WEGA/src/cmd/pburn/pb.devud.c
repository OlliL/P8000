/************************************************************************
*									*
*			pburn - eprom programmer			*
*									*
*	pb1.devud.c		: wega p8000/16-bit-teil		*
*									*
************************************************************************/

#define nit "not interruption, execution of read or write prom, please wait\n"
#define true 1

extern int warning, errflag, error, fd, fd1, cfd, cfd1;
extern char eprombuf[0x4000];

char zei;
int fd2, k;
unsigned lang;


/*	read prom ueber "dev/ud" (16-bit/8-bit-schnittstelle p8000)
	----------------------------------------------------------- */

readprom(test,tpe,adrepr,lepr,pbufepr) 
int tpe;
unsigned adrepr, lepr;
char test;
char *pbufepr;
{
char *bpbufepr;

	set_del();
	bpbufepr=pbufepr;
	header (1,tpe,adrepr,lepr);
	if (errflag==true) {res_del(); return;}
	read (fd2,&zei,1);
	switch (zei)	{
			case 0: break;
			case 1: error=12; printf("\n"); outerr(); close (fd2); res_del(); return;
			case 2: error=13; printf("\n"); outerr(); close (fd2); res_del(); return;
			}
	lang=lepr;
	do
		{
		for (k=64;(lang>0) && (k>0);lang--,k--,bpbufepr++)
					read (fd2,bpbufepr,1);
		zei='q';
		write (fd2, &zei, 1);
		}while (lang>0);

	if (test!= ' ')	{for (warning=0;lepr>0;lepr--,pbufepr++)
					{if (*pbufepr != 0xff)	{warning=9; 
								if (test=='t')	{outwar(); break;}
									else	break;}}}
	close (fd2);
	res_del();
}


/*	write prom ueber "dev/ud/ (16-bit/8-bit-schnittstelle p8000)
	------------------------------------------------------------ */

writeprom  (tpe,adrepr,lepr,pbufepr)
int tpe;
unsigned adrepr, lepr;
char *pbufepr;
{
	set_del();
	header (2,tpe,adrepr,lepr);
	if (errflag==true) {res_del(); return;}
	lang=lepr;
	do
		{
		for (k=64;(lang>0) && (k>0);lang--,k--,pbufepr++)
					write (fd2,pbufepr,1);
		read (fd2, &zei, 1);
		}while (lang>0);
	
	read (fd2,&zei,1);
	switch (zei)	{
			case 0: break;
			case 1: error=12; printf("\n"); outerr(); break;
			case 2: error=13; printf("\n"); outerr(); break;
			}
	close (fd2);
	res_del();
}


/* output header (read/write; typ prom; begadr prom; laenge prom */

header (rw,tpe,adre,lange)
 
char rw;
int  tpe;
unsigned adre, lange;
{
	fd2=open ("/dev/ud",2);
	if (fd2==-1)	{error=11; printf("\n"); outerr(); return;}
	zei='P';
	write (fd2,&zei,1);			/* P - programmer */
	write (fd2,&rw,1);			/* read or write */
	zei=tpe & 0x00ff;
	write (fd2,&zei,1);			/* typ prom */
	lang=adre;
	adre=adre>>8;
	adre=adre & 0x00ff;
	zei=adre;
	write (fd2,&zei,1);			/* begadr prom high */
	lang=lang & 0x00ff;
	zei=lang;
	write (fd2,&zei,1);			/*	       low */
	lang=lange;
	lange=lange>>8;
	lange=lange & 0x00ff;
	zei=lange;
	write (fd2,&zei,1);			/* laenge prom high */
	lang=lang & 0x00ff;
	zei=lang;
	write (fd2,&zei,1);			/*	       low */

	read (fd2, &zei, 1);			/* acknowledge */

}

#include <signal.h>

int del = 0;
int del_pro();

set_del()
{
	signal (SIGINT ,del_pro);		/* int-signal del-taste einfangen */
	signal (SIGHUP ,SIG_IGN);		/* weitere signale ignorieren */

	signal (SIGQUIT,SIG_IGN);
	signal (SIGILL ,SIG_IGN);
	signal (SIGTRAP,SIG_IGN);
	signal (SIGIOT ,SIG_IGN);
	signal (SIGSEGV,SIG_IGN);
	signal (SIGSYS ,SIG_IGN);
	signal (SIGPIPE,SIG_IGN);
	signal (SIGALRM,SIG_IGN);
	signal (SIGTERM,SIG_IGN);
}

res_del()
{
	signal (SIGINT ,SIG_DFL);		/* int-signal del-taste freigeben */
	signal (SIGHUP ,SIG_DFL);
	signal (SIGQUIT,SIG_DFL);
	signal (SIGILL ,SIG_DFL);
	signal (SIGTRAP,SIG_DFL);
	signal (SIGIOT ,SIG_DFL);
	signal (SIGSEGV,SIG_DFL);
	signal (SIGSYS ,SIG_DFL);
	signal (SIGPIPE,SIG_DFL);
	signal (SIGALRM,SIG_DFL);
	signal (SIGTERM,SIG_DFL);

	if (del==1)	{if (cfd==1) close (fd);
			if (cfd1==1) close (fd1);
			exit (0);}
}

del_pro()
{
	signal (SIGINT, SIG_IGN);
	del=1;
	printf ("%s",nit);
}


