/****************************************************************
*								*
*		pburn - eprom programmer			*
*								*
*	pb1.cmdline.c		: wega p8000/16-bit-teil	*
*								*
****************************************************************/	


#include <stdio.h>

#define of1 "filename [-w] [-D] [-os hex] : "
#define of2 "filename [-w] [-D] [-ose hex]: "
#define op1 "promtype [-ol hex]           : "
#define op2 "promtype : "
#define des "   destination "
#define sou "   source      "

#define true 1
#define false 0

#define rdy "\nready to "
#define rpr "\007program:\n"
#define rli "list:\n"
#define rte "crc:\n"
#define rby "bytemode:\n"
#define rfi "file:\n"
#define rco "\007copy:\n"
#define eprom "promtyp "
#define fle "file    :"
#define start "start"
#define end "end"
#define segment "segment"
#define entry "entry"
#define byte " byte"
#define lob " low byte"
#define hib " high byte"

#define err1 "invalid options\n"
#define err2 "multiple defined\n"
#define err3 "can't open file\n"
#define err4 "end of file\n"
#define err5 "not valid object file\n"
#define err6 "segment not found in file\n"
#define err7 "file not separate I & D\n"
#define err8 "file read error\n"
#define err9 "file write error\n"
#define err10 "bytemode prom e2708 not possible\n"
#define err11 "break 'dev/ud' (16-bit/8-bit)\n"
#define err12 "invalid prom type\n"
#define err13 "\007short-circuit prom\n"

#define war1 "specified address range contains no code\n"
#define war2 "specified address range contains no data\n"
#define war3 "specified address range contains no code or data\n"
#define war4 "break byte-oriented files\n"
#define war5 "break word-oriented files\n"
#define war6 "file already exist\n"
#define war7 "code top or bottom rounded to 0x"
#define war8 "data top or bottom rounded to 0x"
#define war9 "prom not erased\n"
#define war10 "is prom really 2732, not 2732a ?\n"
#define war11 "set address to 0x0000 and set length to 0x0400 for prom e2708\n"
#define war12 "end of segment address\n"

extern int command, qflag, errflag, answer, mofy, offset256;
extern char cmdbuf[100];

/* global vereinbarungen */

int error;		/* identifier error */
int warning;		/* identifier warnings */
int eofseg;		/* end of segment */

/*	identifier file
	--------------- */
char namef[80];		/* file name */
char bre;		/* break wordorientierte files
				s-standard (byteorientiert)
				l-low byte
				h-high byte */
char bbre;		/* back bre */
char sect;		/* i/d-files
				d-data
				i-code */
unsigned segnr;		/* segment number */
unsigned begadrf;		/* begin-adr. file */
unsigned langf;		/* laenge file */
unsigned entryf;		/* entry-address file */

/*	identifier eprom
	---------------- */
char namee1[20], namee2[20];	/* eprom name */
int tpe1, tpe2;			/* type eprom
					0-e2708
					1-e2716
					7-e2732
					8-e2732a
					9-e2764 */
unsigned begadre1, begadre2;		/* begin-adr. eprom */
unsigned lange1, lange2;		/* laenge eprom */


char bufopt[100];
char *ptr;
unsigned *pint;
unsigned prlang;
unsigned bbegadrf, bbegadre1, blangf, blange1;
char e2708[] ="e2708";
char e2716[] ="e2716";
char e2732[] ="e2732";
char e2764[] ="e2764";
char e2732a[]="e2732a";


/*	ausgabe "filename [options]:"
	----------------------------- */

outoptf()
{	if (command=='p') printf ("%s",of1);
		else	  printf ("%s",of2);
}

/*	ausgabe "epromtype [options]:"
	------------------------------ */

outopte()
{	
	if (command=='b')	printf ("%s",op2);
	 	else		printf ("%s",op1);
}


/*	eingabe filename [options]:	" . . . "
	----------------------------------------- */

int kennfo,kennfe,kennw,kennD,kenns;
inoptf()
{
	bre='s';sect='i';segnr=begadrf=entryf=0;	
	qflag=false;
	errflag=false;
	bre='s';sect='i';segnr=0;begadrf=0;
	kennw=0;kennD=0;kennfo=0;kenns=0;kennfe=0;
	gets(bufopt);
	for (ptr=bufopt;*ptr==' ';ptr++);
	if (*ptr=='\0'||*ptr=='-') {error=1; outerr(); return;}
	if (*ptr=='q') {qflag=true;return;}
	sscanf (ptr,"%s",namef);
	for (;*ptr!=' '&&*ptr!='\0';ptr++);
	if (*ptr=='\0') return;
  do
  {
  do	{
	for (;*ptr==' ';ptr++);
	if (*ptr=='\0') {plpfil(); return;}
	if (*ptr!='-') {error=1; outerr(); return;}
	ptr++;
	if (*ptr!='w'&&*ptr!='D'&&*ptr!='o'&&*ptr!='s'&&*ptr!='e') {error=1;outerr();return;}
	if (*ptr=='w')	{if (kennw==0) {kennw=1;bre='l';}
				else {error=2;outerr();return;}
			}
	if (*ptr=='D')	{if (kennD==0) {kennD=1;sect='d';}
				else {error=2;outerr();return;}
			}
	if (*ptr=='o')	{if (kennfo==0) {pint= &begadrf;kennfo=1;}
				else {error=2;outerr();return;}
			}
	if (*ptr=='s')	{if (kenns==0) {pint= &segnr;kenns=1;}
				else {error=2;outerr();return;}
			}
	if (*ptr=='e')	{if (kennfe==0) {pint= &entryf;kennfe=1;}
				else {error=2;outerr();return;}
			}
	ptr++;
	} while (*(ptr-1)=='w'||*(ptr-1)=='D');
	if (*ptr!=' ') {error=1;outerr();return;}
	for (;*ptr==' ';ptr++);
	if (*ptr=='\0') {error=1;outerr();return;}
	sscanf (ptr,"%x",pint);
	for (;*ptr!=' '&&*ptr!='\0';ptr++);
  }while (*ptr==' ');
}



/*	plausibilitaetspruefung eingabe file [options]
	---------------------------------------------- */
plpfil()
{
	if (command!='f')	{if (kennfe==1) {error=1;outerr();return;}}
		else if (command=='b')
				{if (kennfe==1||kennfo==1||kennD==1||kenns==1||kennw==1)
					{error=1;outerr();return;}}
	if (segnr>=0x80) {error=1; outerr(); return;}
}



/*	eingabe epromtype [options]:	" . . . "
	----------------------------------------- */
int kenneo,kennel;
inopte()
{
	begadre1=lange1=0;
	qflag=errflag=false;
	kenneo=kennel=0;
	gets(bufopt);
	for (ptr=bufopt;*ptr==' ';ptr++);
	if (*ptr=='\0'||*ptr=='-') {error=1; outerr(); return;}
	if (*ptr=='q') {qflag=true; return;}
	sscanf (ptr,"%s",namee1);
	for (;*ptr!=' '&&*ptr!='\0';ptr++);
	if (*ptr=='\0') {plpepr(); return;}
  do	{
	for (;*ptr==' ';ptr++);
	if (*ptr=='\0') {plpepr();return;}
	if (*ptr!='-') {error=1; outerr(); return;}
	ptr++;
	if (*ptr!='o'&&*ptr!='l') {error=1; outerr(); return;}
	if (*ptr=='o')	{if (kenneo==0) {pint= &begadre1; kenneo=1;}
				else {error=2; outerr(); return;}
			}
	if (*ptr=='l')	{if (kennel==0) {pint= &lange1; kennel=1;}
				else {error=2; outerr(); return;}
			}
	ptr++;
	if (*ptr!=' ') {error=1; outerr(); return;}
	for (;*ptr==' ';ptr++);
	if (*ptr=='\0') {error=1; outerr(); return;}
	sscanf (ptr,"%x",pint);
	for (;*ptr!=' '&&*ptr!='\0';ptr++);
	}while (*ptr==' ');
	plpepr();
}


/*	plausibilitaetstest eingabe epromtype [options]:	?
	--------------------------------------------------------- */
	
plpepr()
{
int i, j;
char *vptr;
	j=0;
	do
	{	j++;
		ptr=namee1;
		switch (j) {
			case 1: for (prlang=0x400,tpe1=0,vptr=e2708,i=5;*ptr==*vptr&&i>0;ptr++,vptr++,i--); break;
			case 2: for (prlang=0x800,tpe1=1,vptr=e2716,i=5;*ptr==*vptr&&i>0;ptr++,vptr++,i--); break;
			case 3: for (prlang=0x1000,tpe1=8,vptr=e2732a,i=6;*ptr==*vptr&&i>0;ptr++,vptr++,i--); break;
			case 4: for (prlang=0x1000,tpe1=7,vptr=e2732,i=5;*ptr==*vptr&&i>0;ptr++,vptr++,i--); break;
			case 5: for (prlang=0x2000,tpe1=9,vptr=e2764,i=5;*ptr==*vptr&&i>0;ptr++,vptr++,i--); break;
			case 6: i=0; error=1; outerr(); return;
			   }
	}while (i!=0 || *ptr != '\0');
	if (tpe1==7)	{warning=10; outwar();
			if (warning==true)	{printf ("\n"); errflag=true; return;}}
	if (command=='b' && tpe1==0) {error=10; outerr(); return;}
	if (begadre1>=prlang) {error=1; outerr(); return;}
	if ((begadre1+lange1)>prlang) {error=1; outerr(); return;}
	if (lange1==0) lange1=prlang-begadre1;
	if (command=='p' && tpe1==0 && (begadre1 != 0 || lange1 != 0x400))	
								{warning=11; outwar();
								begadre1=0; lange1=0x400;}
	langf=lange1;
	if (bre!='s') langf=langf+langf;
	if (command=='b')	{if (kenneo==1||kennel==1)
					{error=1; outerr(); return;}}
	bbegadrf=begadrf; bbegadre1=begadre1;
	blangf=langf; blange1=lange1; bbre=bre;							
}	

/*	ausgabe "ready: . . . " (program,test,list)
	------------------------------------------- */

ready()
{
unsigned ende, endf;

	do
	{
	if (eofseg==1 && (answer=='y' || answer=='s'))	{qflag=true; return;}
	if (answer=='r')	{begadrf=bbegadrf; begadre1=bbegadre1;
				langf=blangf; lange1=blange1; bre=bbre;
				mofy=false;
				endf=begadrf+langf;
				ende=begadre1+lange1;
				if (ende>prlang)
					{lange1=prlang-begadre1;
					if (bre=='s')	endf=begadrf+lange1;
						else	endf=begadrf+lange1+lange1;
					langf=endf-begadrf;
					ende=lange1+begadre1;}}
		else	{
			if (answer==' '||answer=='l'||answer=='m'||answer=='t')
					{ende=begadre1+lange1; endf=begadrf+langf;}
				else	{mofy=false;
					bbre=bre;
					switch (bre)
						{
						case 's': break;
						case 'l': bre='h'; break;
						case 'h': bre='l'; break;
						}
					bbegadrf=begadrf; bbegadre1=begadre1;
					blangf=langf; blange1=lange1;
					if (bre!='h')	{begadrf=begadrf+langf;
							begadre1=begadre1+lange1;
							if (begadre1>=prlang)	{begadre1=0;
										lange1=prlang;
										if (bre=='s')	langf=lange1;
											else	langf=lange1+lange1;}}	
 
					endf=begadrf+langf;
					ende=begadre1+lange1;
					if (ende>prlang)
						 {lange1=prlang-begadre1;
						 if (bre=='s')	endf=begadrf+lange1;
							else	endf=begadrf+lange1+lange1;
						 langf=endf-begadrf; 
						 ende=lange1+begadre1;}
					}
			if (command=='p')
				{if (endf<begadrf)	{langf=0xffff - begadrf;
							if (bre != 's')	lange1=langf/2;
								else	lange1=langf;
							endf=begadrf+langf;
							ende=begadre1+lange1;}
				if (endf==0xffff && bre != 'l')
							{eofseg=1;
							warning=12;
							outwar();}}
			}
	do
	{
	printf ("%s",rdy);
	switch (command) {
		case 'p': printf ("%s",rpr); break;
		case 'l': printf ("%s",rli); break;
		case 't': printf ("%s",rte); break;}
	
	if (command=='p') {
		printf ("%s %s %s %s %x %s %x %s %x",sou,fle,namef,segment,segnr,start,begadrf,end,(endf-1));
		if (bre=='l') printf ("%s \n",lob);
			else if (bre=='h') printf ("%s \n",hib);
			else printf ("%s \n",byte);	
		printf ("%s %s: %s %s %x %s %x %s",des,eprom,namee1,start,begadre1,end,(ende-1),byte);}
	else	printf ("%s %s: %s %s %x %s %x %s",sou,eprom,namee1,start,begadre1,end,(ende-1),byte);

	if (command=='p') printf ("  ?(y/r/s/m/v/l/t/q)");
		else printf ("  ?(y/r/s/q)");

	gets (cmdbuf);
	if (cmdbuf[1]!='\0')	answer='w';
		else		answer=cmdbuf[0];
	switch (answer){
		case 'q': qflag=true; break;
		case 'y':
		case 's':
		case 'r': break;
		case 'l':
		case 't':
		case 'm':
		case 'v': if (command!='p') answer='w';
			  break;
		default: answer='w';
		}
	}while (answer=='w');
	}while (answer=='s' || answer=='r');

}

/*	ausgabe "ready to ........" (bytemode, file, copy)
	-------------------------------------------------- */


ready1() 
{
unsigned ende1, ende2, endf;

	if (eofseg==1)	{qflag=true; return;}
	
	langf=prlang;
	if (bre != 's')	langf=langf+langf;

	ende1=begadre1+lange1; ende2=begadre2+lange2;
	if (command=='f' && answer=='y') {if (bre == 's') begadrf=begadrf+langf;}
	if (command=='f')	{endf=begadrf+langf;
				if (endf<begadrf)	{lange1=0xffff - begadrf;

							lange1=lange1+1;
							eofseg=1;
							warning=12; outwar();
							endf=begadrf+lange1;
							ende1=begadre1+lange1;}
				}
	do
	{printf ("%s",rdy);
	if (command=='b')		{printf ("%s",rby);
					printf ("%s %s: %s",sou,eprom,namee1);
					printf ("  ?(y/q)");}
		else if (command=='f')	{printf ("%s",rfi);
					printf ("%s %s: %s %s %x %s %x %s",sou,eprom,namee1,start,begadre1,end,(ende1-1),byte);
					switch (bre)
						{
						case 's': 	printf ("\n"); break;
						case 'l':	printf (" low_addess\n"); break;
						case 'h':	printf (" high_address\n"); break;
						}
					printf ("%s %s %s %s %x %s %x %s %x %s %x",des,fle,namef,segment,segnr,entry,entryf,start,begadrf,end,(endf-1));
					printf ("  ?(y/m/l/q)");}
		else if (command=='c')	{printf ("%s",rco);
					printf ("%s %s: %s %s %x %s %x %s\n",sou,eprom,namee1,start,begadre1,end,(ende1-1),byte);
					printf ("%s %s: %s %s %x %s %x %s",des,eprom,namee2,start,begadre2,end,(ende2-1),byte);
					printf ("  ?(y/l/m/v/r/q)");}
	gets (cmdbuf);
	if (cmdbuf[1]!='\0')	answer='w';
		else		answer=cmdbuf[0];
	switch (answer) {
		case 'q':	qflag=true; break;
		case 'y':	break;
		case 'm':	if (command=='b') answer='w'; break;
		case 't':
		case 'l':	if (command=='b') answer='w'; break;
		case 'v':
		case 'r':	if (command=='b'||command=='f') answer='w'; break;
		default :	answer='w';
		}
	}while (answer=='w');
}


/*	ausgabe errors
	-------------- */

outerr()
{
	errflag=true;
	printf ("*** error - ");
	switch(error){
		case 1: printf ("%s",err1); break;
		case 2: printf ("%s",err2); break;
		case 3: printf ("%s",err3); break;
		case 4: printf ("%s",err4); break;
		case 5: printf ("%s",err5); break;
		case 6: printf ("%s",err6); break;
		case 7: printf ("%s",err7); break;
		case 8: printf ("%s",err8); break;
		case 9: printf ("%s",err9); break;
		case 10: printf ("%s",err10); break;
		case 11: printf ("%s",err11); qflag=true; break;
		case 12: printf ("%s",err12); break;
		case 13: printf ("%s",err13); break;
		}
}

/*	ausgabe warnings
	---------------- */

outwar()
{
int c;
	printf ("\n*** warning - ");
	switch (warning)
		{
		case 1: printf ("%s",war1); break;
		case 2: printf ("%s",war2); break;
		case 3: printf ("%s",war3); break;
		case 4: printf ("%s",war4); break;
		case 5: printf ("%s",war5); break;
		case 6: printf ("%s",war6); break;
		case 7: printf ("%s%x\n",war7,offset256); warning=false; return;
		case 8: printf ("%s%x\n",war8,offset256); warning=false; return;
		case 9: printf ("%s",war9); break;
		case 10: printf ("%s",war10); break;
		case 11: printf ("%s",war11); warning=false; return;
		case 12: printf ("%s",war12); warning=false; return;
		}
	printf ("\007continue ?");
	gets (cmdbuf);
	if (cmdbuf[1]!='\0')	c='w';
		else		c=cmdbuf[0];
	if (c=='y') 	warning=false;
		else	warning=true;
}

