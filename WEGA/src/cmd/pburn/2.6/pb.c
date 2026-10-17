/*************************************************************************
**************************************************************************

	pburn - programmer	(c) zft/keaw abt.: basissoftware wae
				wega p8000/16-bit-teil
				version : 2.6
					  (prom programmer)
				datum	: 09.02.1990


	bearbeiter:		r. kuehle

**************************************************************************

	pburn besteht aus c-quellen, plzsys-quellen und u880 (z80) assembler-
	quellen.
	folgende quellen sind erforderlich:

	wega p8000/16-bit-teil:
	-----------------------
	pb2.c				(c)
	pb2.command.c			(c)
	pb2.cmdline.c			(c)
	pb2.file.c			(c)
	pb2.buf.c			(c)
	pb2.devud.c			(c)

	udos p8000/8-bit-teil:
	----------------------
	K.PROM.S			(plzsys)
	K.P8000.S			(z80)

**************************************************************************
*************************************************************************/

	
/************************************************************************
*									*	
*			pburn - prom programmer				*
*									*
*	pb2.c		: wega p8000 16-bit-teil			*
*				  c-quelle				*
*									*
************************************************************************/




#define dia1 "\n\n\t\t      pburn - prom programmer board"
#define cpr  "\t\t         (c) zft/keaw  version 2.6\n"
#define dia2 "\t\t      ===============================\n\n"
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
char board;		/*'1' eprom programmer board 1
			  '2' eprom programmer board 2*/

extern int fd, fd1, fd2;	/*file descriptoren*/
extern int cfd, cfd1, error;
extern int eofseg;


main ()
{
	/* prom programmer board */
	set_del();
	header (5,0,0L,0L);
	if (errflag==true) {res_del(); return;}
	read (fd2,&board,1);
	close (fd2);
	res_del();

	do	{
		printf ("%s %c\n%s %s %s %s %s",dia1,board,cpr,dia2,dia3,dia4,dia5);
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
