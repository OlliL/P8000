/****************************************************************
*								*	
*		pburn - eprom programmer			*
*								*
*	pb2.command.c		: wega p8000/16-bit-teil	*
*								*
****************************************************************/


#define EOF "\n\n*** end of file\n"
#define re1 "\n\nread source prom"
#define cte "\n\nchange to destination prom"
#define rtr "\nready to read?"
#define rtc "\nready to copy?"
#define rtv "\nready to verify?"
#define npr "change to next prom?"
#define npl "change to next prom low_address?"
#define nph "change to next prom high_address?"
#define per "prom erased\n"
#define pne "prom not erased\n"
#define true 1
#define false 0

extern int qflag, errflag, warning, mofy, answer, command, eof, next;
extern int tpe1, tpe2;
extern char bre, progbuf[0xfffe], progffff, erase, board;
extern char namee1[20], namee2[20], cmdbuf[100];
extern long begadre1, begadre2, lange1, lange2, begadrf;

/*	p:	programming eprom
	------------------------- */


program()
{
	do	{ do	{printf ("\n");
			 outoptf();		/*ausgabe filename [options]:*/
			 inoptf();		/*eingabe filename [options]*/
			 if (qflag==true) return;
			}while (errflag==true);
		openfile();
		}while (errflag==true);
		  do
			{ 			/*ausgabe eprom [options]*/
			if (board=='1')	outopte();
				else	outopte1();
			 inopte();		/*eingabe eprom [options]*/
			 if (qflag==true) return;
	          }while (errflag==true);
			answer=' ';
		  do	
			 {ready();		/*Ready: .....*/
			 if (qflag==true) return;
			 if (mofy==false) readfile();
			 if (errflag==true) {if (qflag==true) 	return;
							else	{errflag=false;
								answer=' ';
								continue;}}
			 if (warning==true) {qflag=true; return;}
			 if (eof==true) {qflag=true; printf ("%s",EOF); return;}
			 if (answer=='y') 	      progprom();
				else if (answer=='v') verify();
				else if (answer=='m') modify();
				else if (answer=='l') {listbuf('f',begadre1,lange1,progbuf,&progffff);
						      mofy=true;}
				else if (answer=='t') {crc ('t',progbuf,lange1,&progffff);
						      mofy=true;}
		  }while (errflag==false);
} 

/*	l:	listing eprom
	t:	crc-test eprom
	---------------------- */

list_crc()
{
	bre='s';
	do
	{printf ("\n");
	outopte();
	inopte();
	if (qflag==true) return;
	}while (errflag==true);
	answer=' ';
	do
	{ready();
	if (qflag==true) return;
	if (command=='l')
		{
		readprom(tpe1,begadre1,lange1,progbuf,&progffff);
		if (errflag==true) {if (qflag==true)	return;
					else		{errflag=false;
							answer=' ';
							continue;}}
		listprom();
		}
	else	{
		promcrc (tpe1,begadre1,lange1);
		if (errflag==true) {if (qflag==true)	return;
					else		{errflag=false;
							answer=' ';
							continue;}}
		crcprom('e');
			if (erase==0)	printf ("%s",per);
				else	printf ("%s",pne);
		}
	}while (errflag==false);
}



/*	f:	eprom to file
	--------------------- */

file()
{
	bre='s';
	do
	{
	if (next==true)	{switch (bre)
				{
				case 's': printf ("\n%s",npr); break;
				case 'l': printf ("\n%s",npl); break;
				case 'h': printf ("\n%s",nph); break;
				}	
			gets (cmdbuf);
			if (cmdbuf[0]  != 'y' || cmdbuf[1] != '\0') {qflag=true;
								    return;}}
	do
	{printf ("\n");
	outopte();
	inopte();
	if (qflag==true) return;
	}while (errflag==true);
	if (next==false)	{
				do
				{
				outoptf();
				inoptf();
				if (qflag==true) return;
				}while (errflag==true);
				answer=' ';
				}
	do
	{ready1();
	if (qflag==true) return;
	if (mofy==false)		{readprom (tpe1,begadre1,lange1,progbuf,&progffff);
					if (errflag==true) {if (qflag==true)	return;
								else		{errflag=false;
										answer=' ';
										continue;}}}
	if (answer=='y')		{promfile(); mofy=false;
					if (warning==true) {qflag=true; return;}
					next=true;}
		else if (answer=='l')	{listprom(); mofy=true;}
		else if (answer=='m')	modprom();
	}while ((errflag==false && next==false) || mofy==true);
	}while (errflag==false && next==true);
}



/*	b:	bytemode eprom
	---------------------- */

bytemode()
{
	bre='s';
	do
	{printf ("\n");
	if (board=='1')	outopte();
		else	outopte1();
	inopte();
	if (qflag==true) return;
	}while (errflag==true);
	answer=' ';
	do
	{ready1();
	if (qflag==true) return;
	byte();
	}while (errflag==false);
}

/*	c:	copy eprom's
	-------------------- */

copy()
{
int answer1;
long c;

	bre='s';
	do
	{printf ("\n");
	outopte();
	inopte();
	if (qflag==true) return;
	}while (errflag==true);
	tpe2=tpe1;
	lange2=lange1;
	begadre2=begadre1;
	for (c=0;c<20;c++) namee2[c]=namee1[c];
	do
	{
	if (board=='1')	outopte();
		else	outopte1();
	inopte();
	if (tpe1==0 && (begadre1 != 0 || lange1 != 0x400))	{warning=11; outwar();
								begadre1=0; lange1=0x400;}
	if (qflag==true) return;
	}while (errflag==true);

	if (lange1<lange2)	lange2=lange1;
		else		lange1=lange2;

	c=tpe1; tpe1=tpe2; tpe2=c;
	c=lange1; lange1=lange2; lange2=c;
	c=begadre1; begadre1=begadre2; begadre2=c;
	for (c=0;c<20;c++)	{answer=namee1[c];
				namee1[c]=namee2[c];
				namee2[c]=answer;}
	answer=' ';
	answer1=' ';
  do{
	ready1();
	if (qflag==true) return;
	if (mofy==false)	{if (answer!='r')
					{printf ("%s %s",re1,rtr);
					gets (cmdbuf);
					if (cmdbuf[1]!='\0')	continue;
						else		c=cmdbuf[0];
					if (c!='y') continue;
					readprom (tpe1,begadre1,lange1,progbuf,&progffff);
					if (errflag==true) {if (qflag==true)	return;
								else		{errflag=false;
										answer=' ';
										continue;}}}}

	if (answer=='m')		{modprom(); continue;}
	if (answer=='l')		{listprom(); mofy=true; continue;}
	if (answer=='r') 		{if (answer1==' ') continue;} 
    do{
	errflag=false;
	printf ("%s",cte);
	switch (answer) {
		case 'y':	printf ("%s",rtc); answer1=answer; break;
		case 'v':	printf ("%s",rtv); answer1=answer; break;
		case 'r':	if (answer1=='y')		printf ("%s",rtc);
					else if (answer1=='v')	printf ("%s",rtv);
				break;
		default :	break;
			}
	gets (cmdbuf);
	if (cmdbuf[1]!='\0')	continue;
		else		c=cmdbuf[0];
	if (c!='y') continue;
	if (answer=='y')		{copyprom(); mofy=false;}
		else if (answer=='v')	{verifyprom(); mofy=false;}
		else if (answer=='r')	{if (answer1=='y')		copyprom();
						else if (answer1=='v')	verifyprom();}
      }while (errflag==true);
    }while (errflag==false);
}


