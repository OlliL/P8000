/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
 
	Standalone	Modul misc.c
 
	Bearbeiter:	P. Hoge
	Datum:		04/01/87 11:15:15
	Version:	1.1
 
*******************************************************************************
 
Dieses Programm kann fuer alle Standalone Programme benutzt werden,
die unter dem Wega boot laufen.
Funktionen:
		atol	ASCII zu long integer
		atoi	ASCII zu integer
		printfv
		gets
*******************************************************************************
******************************************************************************/
 

 
/*------------------------------------------ Funktionen ---------------------*/
 
 
/*---------------------------------------------------------------------------*/
/*
 *atol
 * Konvertierung ASCII in long integer
 *
 * es werden Tabs und Space, falls vor Dezimalzahl vorhanden, uebergangen
 * es wird auf ein optionales Vorzeichen getestet
 * die Dezimalziffern werden so lange konvertiert bis ein
 * Zeichen ungleich einer Dezimalzahl gefunden wird
 */
/*---------------------------------------------------------------------------*/
long atol(s)
register char *s;
{
	register sign;
	register long n;
 
	for(sign=0;; s++) {
		switch(*s) {
		case '\t':
		case ' ':
		continue;
 
		case '-':
			sign++;
		case '+':
			s++;
		}
		break;
	}
 
	for (n = 0; *s >= '0' && *s <= '9'; s++)
		n = 10 * n + *s - '0';
	if (sign)
		return(0-n);
	return(n);
}
 
/*---------------------------------------------------------------------------*/
/*
 *atoi
 * Konvertierung ASCII string in integer
 
 * es wird atol aufgerufen und long in int konvertiert
 */
/*---------------------------------------------------------------------------*/
atoi(s)
char *s;
{
	return(atol(s));
}

/*---------------------------------------------------------------------------*/
/*
 *ltol3
 * Konvertierung long integer in 3 byte integer
 *
 * Konvertiert eine Liste von long integer (lp zeigt darauf)
 * in eine Liste von 3 byte integer (cp zeigt darauf)
 * n = Anzahl der Zahlen
 */
/*---------------------------------------------------------------------------*/
ltol3(cp, lp, n)
register unsigned char *cp;
register unsigned char *lp;
register n;
{
	for(; n != 0; n--) {
		lp++;
		*(cp++) = *(lp++);
		*(cp++) = *(lp++);
		*(cp++) = *(lp++);
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *l3tol
 * Konvertierung 3 byte integer in long integer
 *
 * Konvertiert eine Liste von 3 byte integer (cp zeigt darauf)
 * in eine Liste von long integer (lp zeigt darauf)
 * n = Anzahl der Zahlen
 */
/*---------------------------------------------------------------------------*/
l3tol(lp, cp, n)
register unsigned char *lp;
register unsigned char *cp;
register n;
{
	for(; n != 0; n--) {
		*(lp++) = 0;
		*(lp++) = *(cp++);
		*(lp++) = *(cp++);
		*(lp++) = *(cp++);
	}
}
 
/*---------------------------------------------------------------------------*/
/*
 *lswap
 * Vertauschen low und high von long integer
 */
/*---------------------------------------------------------------------------*/
long lswap(l)
long l;
{
	return(l<<16 | l>>16);
}
 
/*---------------------------------------------------------------------------*/
/*
 *printf
 * Abgeruestete Version des C Library printf.
 * Nur %s %u %d (==%u) %o %x %D %X sind realisiert.
 * Ausgabe erfolgt auf Console.
 * Sollte nicht fuer "chit-chat" benutzt werden.
 *
 * Der eigentliche Entry Point muss in Assembler
 * geschrieben werden. Das Programm muss wie folgt
 * aussehen:
 *		_printf procedure
*		  entry
 *			ex	r2, @r15
 *			push	@r15, r3
 *			push	@r15, r4
 *			push	@r15, r5
 *			push	@r15, r6
 *			ld	r6, r15
 *			push	@r15, r2
 *			call	_printfv
 *			ld	r2, @r15
 *			inc	r15, #12
 *			jp	@r2
 *		end _printf
 */
/*---------------------------------------------------------------------------*/
printfv(fmt, x1)
register char *fmt;
register unsigned *x1;
{
	register c;
	register unsigned int *adx;
	register char *s;

	adx = x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn((long)*adx, c=='o'? 8: (c=='x'? 16:10));
	else if(c == 's') {
		s = (char *)*adx;
		while(c = *s++)
			putchar(c);
	} else if (c == 'D' || c == 'X' ) {
		for(; adx-x1 < 5; adx++) {
			if ((adx-x1)%2 == 0)
				continue;
			printn(lswap(*(long *)adx), c=='D' ? 10 : 16);
			goto end;
		}
		printn(*(long *)adx, c=='D' ? 10 : 16 );
end:
		adx += (sizeof(long) / sizeof(int)) - 1;
	}
	adx++;
	goto loop;
}

/*---------------------------------------------------------------------------*/
/*
 *printn
 * Print ein unsigned integer zur Basis b
 */
/*---------------------------------------------------------------------------*/
printn(n, b)
register long n;
register b;
{
	register long a;

	if (n<0) {	/* shouldn't happen */
		putchar('-');
		n = -n;
	}
	if(a = n/b)
		printn(a, b);
	putchar("0123456789ABCDEF"[(int)(n%b)]);
}

/*---------------------------------------------------------------------------*/
/*
 *gets
 * Eingabe einer Zeichenkette von der Console
 *
 * in einen vorgegebenen Puffer bis CR oder LF
 * oder bis der Eingabepuffer gefuellt ist
 * Das letzte Zeichen im Puffer ist 0x00. CR und LF werden
 * nicht in den Puffer geschrieben
 */
/*---------------------------------------------------------------------------*/
char *gets(buf, buflen)
register char *buf;
register unsigned buflen;
{
	register char *lp;
	register c;

	lp = buf;
	for (;;) {
		c = getchar() & 0177;
		switch(c) {
		case '\n':
		case '\r':
			*lp = '\0';
			return(buf);
		case '\b':
			putchar(' ');
			putchar('\b');
			lp--;
			if (lp < buf)
				lp = buf;
			continue;
		case 0x18:
		case 0x7f:
			lp = buf;
			putchar('\n');
			continue;
		default:
			if (lp>=buf+buflen-1) {
				*lp = '\0';
				return(buf);
			}
			*lp++ = c;
		}
	}
}
