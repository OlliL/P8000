static char sccsid[] = "@[$]dosys.c\t2.1 11/18/83 13:41:26 - 87wega3.2";
# include "defs"

extern char Makecall;


dosys(comstring, nohalt)
register CHARSTAR comstring;
int nohalt;
{
	register CHARSTAR p;
	register int i;
	int status;

	p = comstring;
	while(	*p == BLANK ||
		*p == TAB ||
		*p == AT ||
		*p == MINUS ||
		*p == NULL) p++;

	if(IS_ON(NOEX) && Makecall == NO)
		return(0);

	if(metas(comstring))
		status = doshell(comstring,nohalt);
	else
		status = doexec(comstring);

	return(status);
}



metas(s)   /* Are there are any  Shell meta-characters? */
register CHARSTAR s;
{
	while(*s)
		if( funny[*s++] & META)
			return(YES);

	return(NO);
}

doshell(comstring,nohalt)
register CHARSTAR comstring;
register int nohalt;
{
	register CHARSTAR shell;
	register CHARSTAR opt;

	if((waitpid = fork()) == 0)
	{
		enbint(0);
		doclose();

		setenv();
		shell = varptr("SHELL")->varval;
		opt = nohalt ? "-c" : "-ce";
		if(shell == 0 || shell[0] == CNULL)
			shell = SHELLCOM;
		else if(equal(shell, "/bin/csh"))	/* WEGA: csh without its .cshrc */
			opt = nohalt ? "-cf" : "-cfe";
		execl(shell, "sh", opt, comstring, 0);
		fatal("Couldn't load Shell");
	}

	return( await() );
}




await()
{
	int intrupt();
	int status;
	int pid;

	enbint(intrupt);
	while( (pid = wait(&status)) != waitpid)
		if(pid == -1)
			fatal("bad wait code");
	waitpid = 0;
	return(status);
}






doclose()	/* Close open directory files before exec'ing */
{
	register OPENDIR od;

	for (od = firstod; od != 0; od = od->nextopendir)
		if (od->dirfc != NULL)
			fclose(od->dirfc);
}





doexec(str)
register CHARSTAR str;
{
	register CHARSTAR t;
	register CHARSTAR *p;
	CHARSTAR argv[200];
	int status;

	while( *str==BLANK || *str==TAB )
		++str;
	if( *str == CNULL )
		return(-1);	/* no command */

	p = argv;
	for(t = str ; *t ; )
	{
		*p++ = t;
		while(*t!=BLANK && *t!=TAB && *t!=CNULL)
			++t;
		if(*t)
			for( *t++ = CNULL ; *t==BLANK || *t==TAB  ; ++t);
	}

	*p = NULL;

	if((waitpid = fork()) == 0)
	{
		enbint(0);
		doclose();
		setenv();
		execvp(str, argv);
		fatal1("Cannot load %s",str);
	}

	return( await() );
}

touch(s)
register CHARSTAR s;
{
	if(junkname[0] == 0)
		sprintf(junkname,  "MAKEJUNK%d", getpid() );

	link(s, junkname);
	unlink(junkname);
	sleep(1);
}
