/*
** ---  RC.C ------------ BASIC DECLARATIONS  ---------------------------------
**
**	Report Compiler Definitions
**
**		contains the  global  structures and  variable
**		declarations  needed by the  lexical analyzer.
**		This includes  Manifest  constants and certain
**		variables for internal communication purposes.
**		Therefore,  extreme  care  should be exercised
**		when modifying this file.
**
**	Version: @(#)rc.c		4.0	02/05/89
**
*/

# include	"rc.h"

/* PROCEDURE handling */
char		*Proc[MAX_PROCS];

/* CASE HANDLING */
int		Case_no;
char		*Case[MAX_CASES];

/* MACRO HANDLING */
int		Macro_no;
struct macro	Macro[MAX_MACROS];

/* DECLARATIONS */
struct desc_t	Desc[MAX_DOM];		/* domfrmt = 0			*/
					/* domfrml = 0			*/
					/* domoff  = 0			*/
int		Line;			/* source line counter		*/
int		Lcase = TRUE;		/* UPPER->lower conversion flag */
int		Compile = TRUE;		/* FALSE for only compilation	*/
int		Iflag;			/* read from standard input	*/
int		Oflag;			/* write to file "r_code"	*/
int		Sflag = TRUE;		/* be silent if FALSE		*/
int		S_error;		/* # of errors found		*/
char		*S_name;		/* name of R_CODE file		*/
char		*S_desc;		/* name of DESC file		*/


RC_BYTE(byte)
register int	byte;
{
# ifdef RC_TRACE
	if (TR_R_CODE)
# ifdef MSDOS
		printf("RC_BYTE: %d (%Fp)\n", byte, R_rc);
# else
		printf("RC_BYTE: %d (%08x)\n", byte, R_rc);
# endif
# endif
	*R_rc++ = byte;
}


RC_WORD(word)
register unsigned int		word;
{
	register char		*rc;
	register unsigned short	w;

	rc = R_rc;
        w = word;
# ifdef RC_TRACE
	if (TR_R_CODE)
# ifdef MSDOS
		printf("RC_WORD: %d (%Fp)\n", word, rc);
# else
		printf("RC_WORD: %d (%08x)\n", word, rc);
# endif
# endif
	*rc++ = w >> BYTE_LOG2;
	*rc++ = w & BYTE_MASK;
	R_rc = rc;
}


RC_JUMP(jump_to)
register char	*jump_to;
{
	register char		*rc;
	register unsigned short	rel_addr;

	rc = R_rc;
	R_rc += sizeof (short);
	rel_addr = jump_to - R_rc;
# ifdef RC_TRACE
	if (TR_R_CODE)
# ifdef MSDOS
		printf("RC_JUMP: to %Fp from %Fp - 2, rel_addr %d\n",
# else
		printf("RC_JUMP: to %08x from %08x - 2, rel_addr %d\n",
# endif
			jump_to, R_rc, rel_addr);
# endif
	*rc++ = rel_addr >> BYTE_LOG2;
	*rc = rel_addr & BYTE_MASK;
}


char	*RC_INIT_JUMP(code)
register int	code;
{
	register char	*ret;

	RC_BYTE(code);
	ret = R_rc;
	RC_WORD(0);
# ifdef RC_TRACE
	if (TR_R_CODE)
# ifdef MSDOS
		printf("RC_INIT_JUMP: code %d on %Fp\n", code, ret);
# else
		printf("RC_INIT_JUMP: code %d on %08x\n", code, ret);
# endif
# endif
	return (ret);
}


RC_UPDATE(jump_from)
register char	*jump_from;
{
	register unsigned short	rel_addr;

	rel_addr = R_rc - (jump_from + sizeof (short));
# ifdef RC_TRACE
	if (TR_R_CODE)
# ifdef MSDOS
		printf("RC_UPDATE: jump from (%Fp - 2) to %Fp, rel_addr %d\n",
# else
		printf("RC_UPDATE: jump from (%08x - 2) to %08x, rel_addr %d\n",
# endif
			jump_from + sizeof (short), R_rc, rel_addr);
# endif
	*jump_from++ = rel_addr >> BYTE_LOG2;
	*jump_from = rel_addr & BYTE_MASK;
}


RC_SYS_VAR(var, print)
register int	var;
register int	print;
{
# ifdef RC_TRACE
	if (TR_R_CODE)
# ifdef MSDOS
		printf("RC_SYS_VAR: %d %d (%Fp)\n", var, print, R_rc);
# else
		printf("RC_SYS_VAR: %d %d (%08x)\n", var, print, R_rc);
# endif
# endif
	switch (var)
	{
	  case SYS_COL:
		RC_BYTE(print? prCOL: pushCOL);
		break;

	  case SYS_LINE:
		RC_BYTE(print? prLINE: pushLINE);
		break;

	  case SYS_ALL_LINE:
		RC_BYTE(print? prALLINES: pushALLINES);
		break;

	  case SYS_PAGE:
		RC_BYTE(print? prPAGE: pushPAGE);
		break;

	  case SYS_TUPL:
		RC_BYTE(print? prTUPLE: pushTUPLE);
		break;

	  default:
		RC_BYTE(print? prVAR: pushVAR);
		RC_WORD(var);
	}
}


RC_STR(str, len)
register char	*str;
register int	len;
{
	register char	*rc;

	rc = R_rc;
# ifdef RC_TRACE
	if (TR_R_CODE)
# ifdef MSDOS
		printf("RC_STR: len=%d (%Fp)\n\t", len, rc);
# else
		printf("RC_STR: len=%d (%08x)\n\t", len, rc);
# endif
# endif
	while (--len >= 0)
	{
# ifdef RC_TRACE
		if (TR_R_CODE)
		{
			printf("%3o %c\t", *str, *str);
			if (!len)
				printf("\n");
			else if (!(len % 8))
				printf("\n\t");
		}
# endif
		*rc++ = *str++;
	}
	R_rc = rc;
}


RC_IDOM(dom, frmt, frml)
register int	dom;
register int	frmt;
register int	frml;
{
	if (Doms == dom - 1)
		RC_DOM(frmt, frml);
	else
	{
		rc_error(25, (char *) dom, ((char *) (Doms + 1)));
		S_desc = (char *) 0;
	}
}


RC_NDOM(dom, frmt, frml)
register char	*dom;
register int	frmt;
register int	frml;
{
	if (RC_MACRO(dom, Doms + 1))
		RC_DOM(frmt, frml);
	else
		S_desc = (char *) 0;
}


RC_DOM(frmt, frml)
register int	frmt;
register int	frml;
{
	register int	dom;
	if ((dom = Doms) >= MAX_DOM)
	{
		rc_error(26);
		S_desc = (char *) 0;
		return;
	}
	Desc[dom].domfrmt = frmt;
	Desc[dom].domfrml = frml;
	Desc[dom].domoff = Tup_len;
	Tup_len += frml;
	Doms++;		/* new domain */

	if (frml <= 0)
		goto ill_len;
	switch (frmt)
	{
	  case INT:
		switch (frml)
		{
		  case sizeof (char):
		  case sizeof (short):
		  case sizeof (long):
			break;

		  default:
			goto ill_len;
		}
		break;

	  case FLOAT:
		switch (frml)
		{
# ifndef NO_F4
		  case sizeof (float):
# endif
		  case sizeof (double):
			break;

		  default:
			goto ill_len;
		}
		break;

	  case CHAR:
		if (frml > MAX_FIELD)
		{
ill_len:
			rc_error(27);
			S_desc = (char *) 0;
		}
	}
}


RC_MACRO(t, s)
register char	*t;
register int	s;
{
	register int	ovfl;

	if (ovfl = (Macro_no < MAX_MACROS))
	{
		Macro[Macro_no].templt = t;
		Macro[Macro_no].subst = s;
		Macro_no++;
	}
	else
		rc_error(24);
	return (ovfl);
}
