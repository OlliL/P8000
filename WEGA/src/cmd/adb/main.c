static char sccsid[] = "@[$]main.c\t3.3  09/28/83 11:49:39 - 87wega3.2";

#include "defs.h"


MSG		NOEOR;
MSG		PROMPT;



INT		symfn();


L_INT	round(a,b)
L_INT		a, b;
{
	L_INT		w;

	w = ((a+b-1)/b)*b;
	return(w);
}

/* error handling */

chkerr()
{
	IF errflg ORF mkfault
	THEN	error(errflg);
	FI
}

error(n)
	STRING		n;
{
	errflg=n;
	iclose(); oclose();
	longret(erradb,1);
}

fault(a)
{
	signal(a,fault);
	lseek(infile,0L,2);
	mkfault++;
}

/* set up files and initial address mappings */

main(argc, argv)
REG INT		argc;
REG STRING	*argv;
{
	maxfile=0x7fffffffL; maxstor=1L<<16;

	gtty(0,&adbtty);
	gtty(0,&usrtty);
	WHILE argc>1
	DO	IF eqstr("-w",argv[1])
		THEN	wtflag=2; argc--; argv++;
		ELIF eqstr("-k",argv[1])
		THEN	kflag=1;
			symfil=kernfil;
			corfil=kmemfil;
			argc--; argv++;
		ELSE	break;
		FI
	OD

	IF argc>1 THEN symfil = argv[1]; FI
	IF argc>2 THEN corfil = argv[2]; FI
	IF argc>3
	THEN	printf("usage: adb [-w] [-k] [objectfile] [corefile]\n");
		exit(1);
	FI
	argcount=argc;
	prints(version);
	setsym(); setcor(argcount);
	disinit(get, symfn, segflg);

	/* set up variables for user */
	maxoff=MAXOFF; maxpos=MAXPOS;
	var[VARB] = datbas;
	var[VARD] = datsiz;
	itolws.L = entrypt;
	svar[3].a_segno = (itolws.I[0]>>8)&0177;
	svar[3].a_offset = entrypt&0xffff;
	var[VARM] = magic&0xffffL;
	var[VARS] = stksiz;
	var[VART] = txtsiz;

	IF (sigint=signal(SIGINT,01))!=01
	THEN	sigint=fault; signal(SIGINT,fault);
	FI
	sigqit=signal(SIGQUIT,1);
	setret(erradb);
	IF executing THEN delbp(); FI
	executing=FALSE;

	LOOP	flushbuf();
		IF errflg
		THEN printf("%s\n",errflg);
		     exitflg=1;
		     errflg=0;
		FI
		IF mkfault
		THEN	mkfault=0; printc2(EOR, 0); prints(DBNAME);
		FI
		write(outfile, PROMPT, 2);
		lp=0; rdc(); lp--;
		IF eof
		THEN	IF infile
			THEN	iclose(); eof=0; longret(erradb,1);
			ELSE	done();
			FI
		ELSE	exitflg=0;
		FI
		command(0,lastcom);
		IF lp ANDF lastc!=EOR THEN error(NOEOR); FI
	POOL
}

done()
{
	endpcs();
	exit(exitflg);
}
