/************************************************************************
*									*
*			pburn - prom programmer				*
*									*
*	pb1.buf.c	: wega p8000-16-bit-teil			*
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

extern int errflag,warning,mofy,answer,qflag;
extern unsigned begadrf, begadre1, lange1, begadre2, lange2, prlang;
extern char progbuf[0x2000], eprombuf[0x4000];
extern char namef[50], namee1[20], namee2[20], cmdbuf[100];
extern int tpe1, tpe2;


/*	program file to prom
	-------------------- */

progprom()
{
	readprom ('t',tpe1,begadre1,lange1,eprombuf);
	if (errflag==true) {if (qflag==true)	return;
					else	{errflag=false;
						answer=' ';
						return;}}
	if (warning==true) {answer=' '; return;}
	printf ("%s %s %s %s %s %s\n",pgr,fil,namef,to,pro,namee1);
	writeprom (tpe1,begadre1,lange1,progbuf);
	if (errflag==true) {if (qflag==true)	return;
					else	{errflag=false;
						answer=' ';
						return;}}
	readprom (' ',tpe1,0,prlang,eprombuf);
	if (errflag==true) {if (qflag==true)	return;
					else	{errflag=false;
						answer=' ';
						return;}}
	crc (' ',eprombuf,prlang);

	compare (progbuf, &eprombuf[begadre1],begadre1,begadre1,lange1,namef,namee1);

}

/*	listing filebuffer
	listing eprombuffer
	------------------- */

listbuf (lid,ladr,llang,lbuf)

	int lid;
	unsigned ladr, llang;
	char *lbuf;
{
	int i, j, k, l, c;
	char string[16], zei;

k=0; l=0;
	for (i=0,j=0;;i++,j++,lbuf++)
			{if (l==0)	{l=1;
					if (lid=='f')	printf ("%s %s %s\n",lis,fil,namef);
						else	printf ("%s %s %s\n",lis,pro,namee1);
					printf ("%04x:\t",ladr);}
			if (i>=llang)	{printf ("   ");
					string[j]=' ';}

				else	{zei= *lbuf;
					printf ("%02x ",zei & 0x00ff);
					if (zei<0x21||zei>0x7c) string[j]='.';
						else string[j]=zei;}
			if (j>=15)	{
					j++; string[j]=0x00;
					printf ("    %s \n",string);
					j= -1;
					if (i>=(llang-1)) break;
					ladr=ladr+16;
					k++;
					if (k>15)	{l=0;
							k=0;
							printf ("%s",con);
							gets (cmdbuf);
							if (cmdbuf[1]!='\0')	c='w';
								else		c=cmdbuf[0];
							if (c!='y') break;
							printf ("\n");}
						else	printf ("%04x:\t",ladr);
					} 

			}

}

/*	listing eprom
	------------- */
listprom()
{
	listbuf ('e',begadre1,lange1,progbuf);
}




/*	modify file buffer
	------------------ */
modify()
{
	mofy=true;
	printf ("%s %s %s\n",mod,fil,namef);
	modprog ('m',begadre1,lange1,progbuf);
}

/*	modify eprom buffer
	------------------- */
modprom()
{
	mofy=true;
	printf ("%s %s %s\n",mod,pro,namee1);
	modprog ('m',begadre1,lange1,progbuf);
}


/*	program bytemode
	---------------- */
byte()
{
	printf ("%s %s %s\n",bpr,pro,namee1);
	modprog ('p',0,prlang,progbuf);
	readprom (' ',tpe1,0,prlang,eprombuf);
	if (errflag==true)	{if (qflag==true)	return;
					else		{errflag=false;
							answer=' ';
							return;}}
	crc (' ',eprombuf,prlang);
}


/*	address to modify ?
	address to program ?
	-------------------- */

modprog (id,ladr,llang,zadr)

int id;
unsigned ladr, llang;
char *zadr;
{
int i;
unsigned madr;
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
	sscanf(bufadr,"%x",&madr);
	if (madr<ladr||madr>=(ladr+llang)) errflag=true;
	}while (errflag==true);
		zadr=bzadr+(madr-ladr);
		do
		{errflag=false;
		if (id=='m')	printf ("%04x:\t%02x  ",madr,*zadr & 0x00ff);
			else	{lange1=1; begadre1=madr-ladr;
				readprom (' ',tpe1,begadre1,lange1,eprombuf);
				if (errflag==true) {if (qflag==true)	return;
								else	{errflag=false;
									answer=' ';
									return;}}
				printf ("%04x:\t%02x  ",madr,eprombuf[0] & 0x00ff);}

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
							writeprom (tpe1,begadre1,lange1,eprombuf);
							if (errflag==true) {if (qflag==true)	return;
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
	readprom(' ',tpe1,begadre1,lange1,eprombuf);
	if (errflag==true) {if (qflag==true) 	return;
				else		{errflag=false;
						answer=' ';
						return;}}
	printf ("%s %s %s %s\n",ver,namee1,and,namef);
	compare (progbuf,eprombuf,begadre1,begadre1,lange1,namef,namee1);
}

/*	verify eprom1 and eprom2
	------------------------ */

verifyprom()
{
	readprom (' ',tpe2,begadre2,lange2,eprombuf);
	if (errflag==true) return;
	printf ("%s %s %s %s %s %s\n",ver,pro,namee1,and,pro,namee2);
	compare (progbuf,eprombuf,begadre1,begadre2,lange1,namee1,namee2);
}

/*	compare buffer1 and buffer2
	--------------------------- */
compare (pbuf1,pbuf2,adr1,adr2,lang,psou,pdes)

char *pbuf1, *pbuf2, *psou, *pdes;
unsigned adr1, adr2, lang;
{
char zei1, zei2;
int k, c, i;

	k=0; i=0;
	for (;lang>0;pbuf1++,pbuf2++,adr1++,adr2++,lang--)
		{if (*pbuf1 != *pbuf2)
			{if (i==0)	{i=1;
					printf ("%s%s\t\t\t%s\n%s\t\t\t%s\n\n",err,sou,des,psou,pdes);}
			zei1= *pbuf1; zei2= *pbuf2;
			printf ("%04x:  %02x\t\t%04x:  %02x\n",adr1,zei1 &0x00ff,adr2,zei2 & 0x00ff);
			k++;
			if (k>15)	{k=0; i=0;
					printf ("%s",con);
					gets (cmdbuf);
					if (cmdbuf[1]!='\0')	c='w';
						else		c=cmdbuf[0];
					if (c!='y') break;
					printf ("\n");}}}
}

/*	crc-berechnung prom
	------------------- */

crcprom()
{
	crc ('e',progbuf,lange1);
}
 
/*	crc-berechnung adr buffer; laenge buffer
	---------------------------------------- */
crc (cid,pbuf,lang)
char *pbuf;
int cid;
unsigned lang;
{
char d, e, a1, a2;
int i;

	if (cid=='e') printf ("%s %s %s\n",tcr,pro,namee1);
		else  if (cid=='f') printf ("%s %s %s\n",tcr,fil,namef);
		else  printf ("\n");
	for (d=0xff,e=0xff;lang>0;pbuf++,lang--)
		{a1= *pbuf;
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
		e=a1;}

	printf ("%s %02x%02x\n",chs,d & 0xff,e & 0xff);
}
		

/*	copy eprom1 and eprom2
	---------------------- */

copyprom()
{
	readprom ('t',tpe2,begadre2,lange2,eprombuf);
	if (errflag==true) 	return;
	if (warning==true) return;
	printf ("%s %s %s %s %s %s\n",cop,pro,namee1,to,pro,namee2);
	writeprom (tpe2,begadre2,lange2,progbuf);
	if (errflag==true) 	return;
	readprom (' ',tpe2,0,prlang,eprombuf);
	if (errflag==true)	return;
	crc (' ',eprombuf,prlang);
	compare (progbuf, &eprombuf[begadre2],begadre1,begadre2,lange1,namee1,namee2);
}
