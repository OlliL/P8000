/*************************************************************************
**************************************************************************

	pburn - programmer	(c) zft/keaw abt.: basissoftware wae
				wega p8000/16-bit-teil
				version : 1.4
				datum	: 04.01.1989

	bearbeiter:		r. kuehle

**************************************************************************

	pburn besteht aus c-quellen, plzsys-quellen und u880 (z80) assembler-
	quellen.
	folgende quellen sind erforderlich:

	wega p8000/16-bit-teil:
	-----------------------
	pb1.c				(c)
	pb1.command.c			(c)
	pb1.cmdline.c			(c)
	pb1.file.c			(c)
	pb1.buf.c			(c)
	pb1.devud.c			(c)

	udos p8000/8-bit-teil:
	----------------------
	PB1.PROM.S			(plzsys)
	PB1.P8000.S			(z80)

**************************************************************************
*************************************************************************/

	
/************************************************************************
*									*	
*			pburn - prom programmer				*
*									*
*	pb1.c		: wega p8000 16-bit-teil			*
*				  c-quelle				*
*									*
************************************************************************/




#define dia1 "\n\n\t\t   pburn - prom programmer\n"
#define cpr "\t\t (c) zft/keaw 04.01.1989/v 1.4\n"
#define dia2 "\t\t =============================\n\n"
#define dia3 "menue:\n\tp - program\n\tl - list\n\tt - crc\n\tb - bytemode"
#define dia4 "\n\tf - file\n\tc - copy"
#define dia5 "\n\tq - quit\nselect ?:"

#define selp "\n\np:	programming prom\n"
#define sell "\n\nl:	listing prom\n"
#define selt "\n\nt:	crc-test prom\n"
#define selb "\n\nb:	bytemode prom\n"
#define self "\n\nf:	file prom\n"
#define selc "\n\nc:	copy prom's\n"

#define true 1
#define false 0


/* global definitionen */

int command;		/*kommando aus menue*/
int qflag;		/*qflag=true rueckkehr in menue*/
int errflag;		/*error flag*/
int answer;		/*antwort ready:....	?*/
int eof;		/*end of file*/
int mofy;		/*modify flag*/
int next;		/*next prom	prom to file*/
char cmdbuf[100];

extern int fd, fd1;	/*file descriptoren*/
extern int cfd, cfd1;
extern int eofseg;


main()
{


	do	{
		printf ("%s %s %s %s %s %s",dia1,cpr,dia2,dia3,dia4,dia5);
		cfd=false;
		cfd1=false;
		eofseg=false;
		mofy=false;
		next=false;
		gets (cmdbuf);
		if (cmdbuf[1]!='\0')	command='w';
			else		command=cmdbuf[0];	
		switch (command){
			case 'p': printf ("%s",selp); program(); 
						      if (cfd==1) close (fd); 
					       	      break;
			case 'l': printf ("%s",sell); list_crc(); break;
			case 't': printf ("%s",selt); list_crc(); break;
			case 'b': printf ("%s",selb); bytemode(); break;
			case 'f': printf ("%s",self); file(); 
						      if (cfd1==1) header_file();  
						      break;
			case 'c': printf ("%s",selc); copy(); break;
			case 'q': qflag=false; break;
			default:  qflag=true;  
			}
		}while (qflag==true);
	printf ("\n");


}
