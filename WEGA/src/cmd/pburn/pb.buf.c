/************************************************************************
*									*
*		pburn - prom programmer					*
*									*
*	pb2.buf.c	: wega p8000-16-bit-teil			*
*				c-quelle				*
*									*
************************************************************************/


#define true 1
#define false 0

#define con "continue ?"
#define pgr "\nprogram"
#define lis "\nlisting"
#define mod "\nmodify"
#define ver "\nverify"
#define cop "\ncopy"
#define bpr "\nbyteprogram"
#define tcr "\ncrc"
#define fil "file"
#define pro "prom"
#define and "and"
#define to  "to"
#define atm "address to modify ?"
#define atp "address to program ?"
#define era "\n*** the specified address out of address range\n"
#define err "errors\n"
#define sou "source"
#define des "destination"
#define chs "crc_checksum: "

char	d1, e1;

extern int errflag,warning,mofy,answer,qflag;
extern long begadrf, begadre1, lange1, begadre2, lange2, prlang;
extern char progbuf[0xfffe], eprombuf[0xfffe], progffff, epromffff, kffff;
extern char namef[50], namee1[20], namee2[20], cmdbuf[100];
extern char erase, vefy, d, e, zei1, *p_progbuf;
extern long p_begadre, p_lange;
extern int tpe1, tpe2;


/*	program file to prom
	-------------------- */

progprom()
{
	eraseprom (tpe1,begadre1,lange1);
	if (errflag==true) {if (qflag==true)	return;
					else	{errflag=false;
						answer=' ';
						return;}}
	if (warning==true) {answer=' '; warning=false; return;}
	printf ("%s %s %s %s %s %s\n",pgr,fil,namef,to,pro,namee1);
	writeprom (tpe1,p_begadre,p_lange,p_progbuf,&progffff);
	if (errflag==true) {if (qflag==true)	return;
					else	{errflag=false;
						answer=' ';
						return;}}
	promcrc (tpe1,begadre1,lange1);
	if (errflag==true) {if (qflag==true)	return;
					else	{errflag=false;
						answer=' ';
						return;}}
	crcprom(' ');
	if (vefy==0)	{d1=d;
			e1=e;
			crc (' ',progbuf,lange1,&progffff);
			if ((d1!=d) || (e1!=e))	vefy=3;}
	if (vefy!=0)
			{
			warning=14; outwar();
			if (warning==true) {answer=' '; warning=false; return;}
			readprom (tpe1,begadre1,lange1,eprombuf,&epromffff);
			if (errflag==true) {if (qflag==true)	return;
							else	{errflag=false;
								answer=' ';
								return;}}
			compare (progbuf,eprombuf,&progffff,&epromffff,begadre1,begadre1,lange1,namef,namee1);
			}

}

/*	listing filebuffer
	listing eprombuffer
	------------------- */

listbuf (lid,ladr,llang,lbuf,padrffff)

	int lid;
	long ladr, llang;
	char *lbuf, *padrffff;
{
	long i;
	int j, k, l, c;
	char string[16], zei;

k=0; l=0;
	for (i=0,j=0;;i++,j++,lbuf++)
			{if (l==0)	{l=1;
					if (lid=='f')	printf ("%s %s %s\n",lis,fil,namef);
						else	printf ("%s %s %s\n",lis,pro,namee1);
					printf ("%04X:\t",ladr);}
			if (i>=llang)	{printf ("   ");
					string[j]=' ';}

				else	{if (i==0xffffL)	
						lbuf=padrffff;
					zei= *lbuf;
					printf ("%02x ",zei & 0x00ff);
					if (zei<0x21||zei>0x7c) string[j]='.';
						else string[j]=zei;}
			if (j>=15)	{
					j++; string[j]=0x00;
					printf ("    %s \n",string);
					j= -1;
					if (i>=(llang-1)) break;
					ladr=ladr+16L;
					k++;
					if (k>15)	{l=0;
							k=0;
							printf ("%s",con);
							gets (cmdbuf);
							if (cmdbuf[1]!='\0')	c='w';
								else		c=cmdbuf[0];
							if (c!='y') break;
							printf ("\n");}
						else	printf ("%04X:\t",ladr);
					} 

			}

}

/*	listing eprom
	------------- */
listprom()
{
	listbuf ('e',begadre1,lange1,progbuf,&progffff);
}




/*	modify file buffer
	------------------ */
modify()
{
	mofy=true;
	printf ("%s %s %s\n",mod,fil,namef);
	modprog ('m',begadre1,lange1,progbuf,&progffff);
}

/*	modify eprom buffer
	------------------- */
modprom()
{
	mofy=true;
	printf ("%s %s %s\n",mod,pro,namee1);
	modprog ('m',begadre1,lange1,progbuf,&progffff);
}


/*	program bytemode
	---------------- */
byte()
{
	printf ("%s %s %s\n",bpr,pro,namee1);
	modprog ('p',0L,prlang,progbuf,&progffff);
	promcrc (tpe1,0L,prlang);
	if (errflag==true)	{if (qflag==true)	return;
					else		{errflag=false;
							answer=' ';
							return;}}
	crcprom(' ');
}


/*	address to modify ?
	address to program ?
	-------------------- */

modprog (id,ladr,llang,zadr,padrffff)

int id;
long ladr, llang;
char *zadr, *padrffff;
{
int i;
long madr;
char bufadr[50];
char zei, *bzadr;

	errflag=false;
	bzadr=zadr;
do
{
	do
	{
		do
		{if (errflag==true) {printf ("%s",era); errflag=false;}
		qflag=false;
		if (id=='m')	printf ("%s",atm);
			else	printf ("%s",atp);
		gets(bufadr);
		if (bufadr[0]=='\0') {errflag=true;continue;}
			for (i=0;i<4;i++)	
				{if (bufadr[i]=='q') return;
				if (bufadr[i]=='\0') break;
				if (bufadr[i]<'0') {errflag=true; break;}
				if (bufadr[i]>'9')
					{if (bufadr[i]<'A') {errflag=true;break;}
					if (bufadr[i]>'F')
						{if (bufadr[i]<'a'||bufadr[i]>'f')
							{errflag=true; break;}}}}
		}while (errflag==true||bufadr[i]!='\0');
	sscanf(bufadr,"%X",&madr);
	if (madr<ladr||madr>=(ladr+llang)) errflag=true;
	}while (errflag==true);
		zadr=bzadr+(madr-ladr);
		do
		{errflag=false;

		/*behandlung adr ffff prom e27512*/
		if ((madr-ladr)==0xffffL)	zadr=padrffff;

		if (id=='m')	printf ("%04X:\t%02x  ",madr,*zadr & 0x00ff);
			else	{lange1=1; begadre1=madr-ladr;
				readprom (tpe1,begadre1,lange1,eprombuf,&epromffff);
				if (errflag==true) {if (qflag==true)	return;
								else	{errflag=false;
									answer=' ';
									return;}}
				printf ("%04X:\t%02x  ",madr,eprombuf[0] & 0x00ff);}

		gets (bufadr);
		i=0;
		if (bufadr[i]!='\0')
			{for (i=0;i<2;i++)
				{if (bufadr[i]=='q') {qflag=true; break;}
				if (bufadr[i]=='\0') break;
				if (bufadr[i]<'0') {errflag=true; break;}
				if (bufadr[i]>'9')
					{if (bufadr[i]<'A') {errflag=true; break;}
					if (bufadr[i]>'F')
						{if (bufadr[i]<'a'||bufadr[i]>'f') {errflag=true; break;}}}}
				if (errflag==false&&qflag==false&&bufadr[i]=='\0')
					{sscanf (bufadr,"%x",&zei); 
					if (id=='m')	*zadr= zei;
						else	{eprombuf[0]=zei;
							writeprom (tpe1,begadre1,lange1,eprombuf,&epromffff);
							if ((errflag==true)||(zei1=='n')) {if (qflag==true)	return;
													else	{errflag=false;
														answer=' ';
														return;}}}}}

		if (errflag==false&&qflag==false&&bufadr[i]=='\0') {madr++; zadr++;}
		if (madr>=(ladr+llang)) {errflag=true; qflag=true;}
		}while (qflag==false);
}while (qflag==true);	
			
}

/*	verify eprombuffer and filebuffer
	-------------------------------- */
verify()
{
	readprom(tpe1,begadre1,lange1,eprombuf,&epromffff);
	if (errflag==true) {if (qflag==true) 	return;
				else		{errflag=false;
						answer=' ';
						return;}}
	printf ("%s %s %s %s\n",ver,namee1,and,namef);
	compare (progbuf,eprombuf,&progffff,&epromffff,begadre1,begadre1,lange1,namef,namee1);
}

/*	verify eprom1 and eprom2
	------------------------ */

verifyprom()
{
	readprom (tpe2,begadre2,lange2,eprombuf,&epromffff);
	if (errflag==true) return;
	printf ("%s %s %s %s %s %s\n",ver,pro,namee1,and,pro,namee2);
	compare (progbuf,eprombuf,&progffff,&epromffff,begadre1,begadre2,lange1,namee1,namee2);
}

/*	compare buffer1 and buffer2
	--------------------------- */
compare (pbuf1,pbuf2,p1ffff,p2ffff,adr1,adr2,lang,psou,pdes)

char *pbuf1, *pbuf2, *p1ffff, *p2ffff, *psou, *pdes;
long adr1, adr2, lang;
{
char zei1, zei2;
int k, c, i;

	if (lang==0x10000L)	{lang--;
				kffff=1;}
		else		{if ((adr2+lang)==0x10000L)	{lang--;
								kffff=2;}
					else			kffff=0;}
	k=0; i=0;

	do
	{
		do
		{
		if (*pbuf1 != *pbuf2)
			{if (i==0)	{i=1;
					printf ("%s%s\t\t\t%s\n%s\t\t\t%s\n\n",err,sou,des,psou,pdes);}
			zei1= *pbuf1; zei2= *pbuf2;
			printf ("%04X:  %02x\t\t%04X:  %02x\n",adr1,zei1 &0x00ff,adr2,zei2 & 0x00ff);
			k++;
			if (k>15)	{k=0; i=0;
					printf ("%s",con);
					gets (cmdbuf);
					if (cmdbuf[1]!='\0')	c='w';
						else		c=cmdbuf[0];
					if (c!='y') return;
					printf ("\n");}}
		pbuf1++;
		pbuf2++;
		adr1++;
		adr2++;
		lang--;
		} while (lang>0);
	/*behandlung adr ffff prom e27512*/
	if (kffff==1)	{pbuf1=p1ffff;
			pbuf2=p2ffff;
			lang=1L;
			kffff=0;}
		else	{if (kffff==2)	{pbuf2=p2ffff;
					lang=1L;
					kffff=0;}}
	} while (lang>0);

}

/*	crc-berechnung prom
	------------------- */

crcprom (cid)

char cid;
{
	if (cid=='e') printf ("%s %s %s\n",tcr,pro,namee1);
		else  printf ("\n");
	printf ("%s %02x%02x\n",chs,d &0xff,e & 0xff);
}
 
/*	crc-berechnung adr buffer; laenge buffer
	---------------------------------------- */
crc (cid,pbuf,lang,padrffff)

char cid;
char *pbuf, *padrffff;
long lang;

{
char  a1, a2;
int i;

	if (cid=='t')	printf ("%s %s %s\n",tcr,fil,namef);

	d=0xff; e=0xff;

	if (lang==0x10000L)	{lang--;
				kffff=1;}
		else		kffff=0;

	do {

		do
		{
		a1= *pbuf;
		a1=a1 ^ d;
		d=a1;
		for (i=0;i<4;i++)
			{if ((a1 & 1)> 0)	{a1=a1 >> 1; a1=a1 | 0x80;}
				else		{a1=a1 >> 1; a1=a1 & 0x7f;}}
		a1=a1 & 0x0f;
		a1=a1 ^ d;
		d=a1;
		for (i=0;i<3;i++)
			{if ((a1 & 1)>0)	{a1=a1 >> 1; a1=a1 | 0x80;}
				else		{a1=a1 >> 1; a1=a1 & 0x7f;}}
		a2=a1;
		a1=a1 & 0x1f;
		a1=a1 ^ e;
		e=a1;
		a1=a2;
		if ((a1 & 1)>0)	{a1=a1 >> 1; a1=a1 | 0x80;}
			else	{a1=a1 >> 1; a1=a1 & 0x7f;}
		a1=a1 & 0xf0;
		a1=a1 ^ e;
		e=a1;
		a1=a2;
		a1=a1 & 0xe0;
		a1=a1 ^ d;
		d=e;
		e=a1;
		pbuf++;
		lang--;
		} while (lang>0);

	/*behandlung adr ffff e27512*/
	if (kffff==1)	{pbuf=padrffff;
			lang=1L;
			kffff=0;}

	} while (lang>0);


	if (cid=='t')	printf ("%s %02x%02x\n",chs,d & 0xff,e & 0xff);
}
		

/*	copy eprom1 and eprom2
	---------------------- */

copyprom()
{
	eraseprom (tpe2,begadre2,lange2);
	if (errflag==true)  	return;
	if (warning==true) 	{errflag=true; warning=false; return;}
	printf ("%s %s %s %s %s %s\n",cop,pro,namee1,to,pro,namee2);
	writeprom (tpe2,begadre2,lange2,progbuf,&progffff);
	if (errflag==true)  	return;
	promcrc (tpe2,begadre2,lange2);
	crcprom (' ');
	if (vefy==0)	{d1=d;
			e1=e;
			crc (' ',progbuf,lange2,&progffff);
			if ((d1!=d) || (e1!=e))	vefy=3;}
	if (errflag==true)	return;
	if (vefy!=0)
		{
		warning=14; outwar();
		if (warning==true) {answer=' '; return;}
		readprom (tpe2,begadre2,lange2,eprombuf,&epromffff);
		if (errflag==true)	return;
		compare (progbuf,eprombuf,&progffff,&epromffff,begadre1,begadre2,lange1,namee1,namee2);
		}
}
