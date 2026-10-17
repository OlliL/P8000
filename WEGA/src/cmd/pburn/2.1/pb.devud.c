/************************************************************************
*									*
*			pburn - eprom programmer			*
*									*
*	pb2.devud.c		: wega p8000/16-bit-teil		*
*									*
************************************************************************/

#define nit "\nnot interruption, execution of read or write prom, please wait\n"
#define true 1
#define false 0

extern int warning, errflag, answer, error, fd, fd1, cfd, cfd1;
extern char eprombuf[0xfffe], Vpp, kennS;

char zei, zei1, *pbufepr1, *pbufepr2;
char erase, vefy, d, e;
int fd2, k, kffff;
long lang, adr, size;


/*	read prom ueber "dev/ud" (16-bit/8-bit-schnittstelle p8000)
	----------------------------------------------------------- */

readprom(tpe,adrepr,lepr,pbufepr,padrffff) 
int tpe;
long adrepr, lepr;
char *pbufepr, *padrffff;

{


	pbufepr1=pbufepr;
	pbufepr2=pbufepr;
	adr=adrepr;
	if (lepr==0x10000L)	{lepr--;
				kffff=1;}
		else		kffff=0;

	do
	{
		do
		{
		set_del();
		if (lepr>0x4000L)	size=0x4000L;
			else		size=lepr;
	
		header (1,tpe,adr,size);
		if (errflag==true) {res_del(); return;}
		read (fd2,&zei,1);
		switch (zei)	{
				case 1: error=12; printf("\n"); outerr(); close (fd2); res_del(); return;
				case 2: error=13; printf("\n"); outerr(); close (fd2); res_del(); return;
				}
		r_devud ();
		lepr=lepr-0x4000L;
		adr=adr+0x4000L;
		close (fd2);
		res_del();
		} while (lepr>0);

	/*behandlung adr ffff prom e27512*/
	if (kffff==1)	{pbufepr1=padrffff;
			pbufepr2=padrffff;
			lepr=1L;
			kffff=0;}
	} while (lepr>0);

}


/* uebertragung dev/ud-schnittstelle max. 16-K (0x4000) bloecken 
   ------------------------------------------------------------- */

r_devud ()

{
	lang=size;
	do
		{
		for (k=64;(lang>0) && (k>0);lang--,k--,pbufepr1++)
					read (fd2,pbufepr1,1);
		zei='q';
		write (fd2, &zei, 1);
		}while (lang>0);

}


/*	write prom ueber "dev/ud/ (16-bit/8-bit-schnittstelle p8000)
	------------------------------------------------------------ */

writeprom  (tpe,adrepr,lepr,pbufepr,padrffff)
int tpe;
long adrepr, lepr;
char *pbufepr, *padrffff;
{
	vefy=0;
	pbufepr2=pbufepr;
	adr=adrepr;
	if (lepr==0x10000L)	{lepr--;
				kffff=1;}
		else		kffff=0;

	do
	{
		do
		{
		set_del();
		if (lepr>0x4000L)	size=0x4000L;
			else		size=lepr;
		header (2,tpe,adr,size);
		if (errflag==true) {res_del(); return;}
		w_devud ();
			do
			{
			read (fd2,&zei,1);
			switch (zei)	{
					case 1: error=12; printf("\n"); outerr(); close (fd2); res_del(); return;
					case 2: error=13; printf("\n"); outerr(); close (fd2); res_del(); return;
					case 3: if (kennS==1)	warning=16;
							else	warning=15;
						outwar();
						vefy=1;
						if (warning==true)	{zei1='n';
									write (fd2,&zei1,1);
									close (fd2);
									res_del();
									answer=' ';
									warning=false;
									return;}
							else		{zei1='y';
									write (fd2,&zei1,1);
									break;}
					}
			}while (zei!=0);

			if (kennS==1)	{	/* Standard Speed Program Mode */
					read (fd2,&zei,1);
					switch (zei)	{
							case 1: error=12; printf ("\n"); outerr(); close (fd2); res_del(); return;
							case 2: error=13; printf ("\n"); outerr(); close (fd2); res_del(); return;
							}
					if (vefy==0) vefy=zei;
					}	
		lepr=lepr-0x4000L;
		adr=adr+0x4000L;
		close (fd2);
		res_del();
		} while (lepr>0);

	/*behandlung adr ffff prom e27512*/
	if (kffff==1)	{pbufepr2=padrffff;
			lepr=1;
			kffff=0;}
	} while (lepr>0);

}

/*uebertragung dev/ud-schnittstelle von max. 16-K (0x4000) bloecken
  -----------------------------------------------------------------*/

w_devud ()
{
	lang=size;
	do
		{
		for (k=64;(lang>0) && (k>0);lang--,k--,pbufepr2++)
					write (fd2,pbufepr2,1);
		read (fd2, &zei, 1);
		}while (lang>0);
}

/* erase prom ueber "dev/ud" (16-bit/8-bit-schnittstelle 
   ----------------------------------------------------- */

eraseprom (tpe,adrepr,lepr)

int tpe;
long adrepr, lepr;
{
	set_del();
	header (3,tpe,adrepr,lepr);
	if (errflag==true) {res_del(); return;}
	read (fd2,&erase,1);
	switch (erase)	{
			case 1: error=12; printf ("\n"); outerr(); close (fd2); res_del(); return;
			case 2: error=13; printf ("\n"); outerr(); close (fd2); res_del(); return;
			}
	if (erase!=0) {warning=9; outwar(); close (fd2); res_del(); return;}

	close (fd2);
	res_del();
}

/* promcrc ueber "dev/ud" (16-bit/8-bit-schnittstelle p8000)
   --------------------------------------------------------- */

promcrc (tpe,adrepr,lepr)

int tpe;
long adrepr, lepr;

{
	set_del();
	header (4,tpe,adrepr,lepr);
	if (errflag==true) {res_del(); return;}
	read (fd2,&erase,1);
	switch (erase)	{
			case 1: error=12; printf ("\n"); outerr(); close (fd2); res_del(); return;
			case 2: error=13; printf ("\n"); outerr(); close (fd2); res_del(); return;
			}	
	read (fd2,&d,1);
	read (fd2,&e,1);
	close (fd2);
	res_del();
}

/* output header (read/write; typ prom; begadr prom; laenge prom 
   ------------------------------------------------------------- */

header (rw,tpe,adre,lange)
 
char rw;
int  tpe;
long adre, lange;
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
	adre=adre & 0x00ffL;
	zei=adre;
	write (fd2,&zei,1);			/* begadr prom high */
	lang=lang & 0x00ffL;
	zei=lang;
	write (fd2,&zei,1);			/*	       low */

	lang=lange;
	lange=lange>>8;
	lange=lange & 0x00ffL;
	zei=lange;
	write (fd2,&zei,1);			/* laenge prom high */
	lang=lang & 0x00ffL;
	zei=lang;
	write (fd2,&zei,1);			/*	       low */
	write (fd2,&Vpp,1);			/* programmierspannung Vpp*/
	write (fd2,&kennS,1);			/* program mode*/

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


