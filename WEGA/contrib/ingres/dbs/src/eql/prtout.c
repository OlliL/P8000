# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

/*
**  PRTOUT.C -- output routines
**
**	Output routines for non-specific data structures
**	(i.e. w_display is in[display.c])
**
**	Defines:
**		w_con()
**		w_var()
**		w_key()
**		w_new()
**		begin_quote()
**		end_quote()
**		equate_lines()
**		w_sync()
**		w_flush()
**		w_file()
**		w_string()
**		w_raw()
**		Charcnt - number of characters already output
**			on current line (used in[yylex.c])
**
**
*/

short	Fillcnt =	FILLCNT;

/*
**  W_CON -- write out a constant
**	Writes out a constant of type 'type'
**	pointed to by 'string'.
*/
w_con(type, string)
register int	type;
register char	*string;
{
	if (type == Tokens.sp_sconst)
		w_string(string, 1);
	else
		w_key(string);
}


/*
**  W_VAR -- writes out code to send the
**	     value of a C variable down to
**	     the QL scanner.
**
**	Conserves the state of In_quote.
*/
w_var(disp, type)
int				type;
register struct display		*disp;
{
	register int			savestat;
	extern char			*AA_iocv();

	savestat = In_quote;

	if (type != opIDSTRING)
	{
		w_new("IIcvar(");
		if (type != opSTRING)
			w_raw("&");
	}
	else
		w_new("IIwrite(");
	w_display(disp);
	switch (type)
	{
	  case opINT :
		w_raw(", ");
		w_raw(AA_iocv(opINT));
		w_raw(", ");
		w_raw(AA_iocv(sizeof (short)));
		w_raw(");");
		break;

# ifndef NO_F4
	  case opFLT :
		w_raw(", ");
		w_raw(AA_iocv(opFLT));
		w_raw(", ");
		w_raw(AA_iocv(sizeof (float)));
		w_raw(");");
		break;
# endif

	  case opSTRING :
		w_raw(", ");
		w_raw(AA_iocv(opSTRING));
		w_raw(", 0);");
		break;

	  case opDOUBLE :
		w_raw(", ");
		w_raw(AA_iocv(opDOUBLE));
		w_raw(", ");
		w_raw(AA_iocv(sizeof (double)));
		w_raw(");");
		break;

	  case opCHAR :
		w_raw(", ");
		w_raw(AA_iocv(opCHAR));
		w_raw(", ");
		w_raw(AA_iocv(sizeof (char)));
		w_raw(");");
		break;

	  case opLONG :
		w_raw(", ");
		w_raw(AA_iocv(opLONG));
		w_raw(", ");
		w_raw(AA_iocv(sizeof (long)));
		w_raw(");");
		break;

	  case opIDSTRING :
		w_raw(");");
		break;

	  default :
		AAsyserr(13003, type);
	}
	if (savestat)
		begin_quote();
}


/*
**  W_KEY -- write out a string needing a blank to
**	     separate it from other keywords.
*/
w_key(string)
register char	*string;
{
	w_raw(" ");
	w_raw(string);
	w_raw(" ");
}


/*
**  W_NEW -- write out a string after getting out of
**	     any pending IIwrite's.
*/
w_new(string)
register char	*string;
{
	end_quote();
	w_raw(string);
}


/*
**  BEGIN_QUOTE -- Issue an IIwrite("
*/
begin_quote()
{
	In_string = 1;
	In_quote = 1;
	Fillmode = 1;
	w_raw("IIwrite(\"");
}


/*
**  END_QUOTE -- End any pending IIwrite("
*/
end_quote()
{
	In_string = 0;
	if (In_quote)
		w_raw("\");");
	In_quote = 0;
}


/*
**  EQUATE_LINES -- Make subsequent lines be output on the
**		    same line they were read (lines of C_CODE only).
**
**	Note: Because of the algorithm used, it is possible that
**		the correct line in the output has already been passed,
**		in which case equate_lines does nothing.
*/
equate_lines()
{
	Fillmode = 0;
	while (Lineout < yyline)
		w_raw("\n");
}


/*
**  W_SYNC -- Put out an IIsync() call
*/
w_sync()
{
	w_new("IIsync(");
	w_file();
	w_raw(");");
}


/*
**  W_FLUSH -- Put out an IIflush_tup() call
*/
w_flush()
{
	w_new("IIflushtup(");
	w_file();
	w_raw(");");
}


/*
**  W_FILE -- Writes out the name and line number of the
**	      input file if Rtdb is specified, else a 0.
*/
w_file()
{
	extern char	*AA_iocv();

	if (Rtdb)
	{
		w_string(Input_file_name, 0);
		w_raw(", ");
		w_key(AA_iocv(yyline));
	}
	else
		w_key("(char *) 0");
}


/*
**  W_STRING -- Writes out a string
**
**	String is output as a string constant if type == 0
**	otherwise writes out string inside an IIwrite(
*/
w_string(s, type)
register char	*s;
register int	type;
{
	register char	*t;

	if (type)
	{
		if (!In_quote)
			begin_quote();
		w_raw("\\");
	}
	In_string++;
	w_raw("\"");
	for (t = s; *t; )
	{
		if (*t == '\\')
		{
			if (t[1] == '\n')
			{
				*t = '\0';
				w_raw(s);
				s = t = &t[2];
				w_raw("\\\n");
			}
			else
			{
				*t++ = '\0';
				w_raw(s);
				s = t;
				/* note that this call must be atomic,
				 * as w_raw would feel free to put newlines
				 * in if not.
				 */
				if (type)
					w_raw("\\\\");
				else
					w_raw("\\");
			}
		}
		else if (*t == '"')
		{
			w_raw("\\\"");
			s = ++t;
		}
		else
			t++;
	}
	w_raw(s);
	--In_string;
	if (type)
		w_raw("\\\"");
	else
		w_raw("\"");
}


/*
**  W_RAW -- Lowest level output character routine
**
**	Output_string string depending on Fillcnt and In_quote
**	and In_string and Fillmode.
**	When not in Fillmode does straight output.
**	When on Fillmode, fills lines to Fillmode.
**
**	NOTE : w_raw will feel free to output a newline after
**		'string' if the string causes more than Fillcnt
**		characters to be output.
**		Inside strings (In_string != 0) w_raw will put
**		a '\\' before the newline issued.
**		When In_quote != 0 when the fillcnt is exceeded,
**		the IIwrite( is ended an continued on the next line
**		so that the query string won't overflow the C
**		pre-processor's line buffer.
*/
w_raw(string)
register char	*string;
{
	register char	*s;
	register int	charcnt;

	charcnt = 0;
	for (s = string; *s; s++)
	{
		if (*s != '\n')
		{
			putc(*s, Out_file);
			charcnt++;
		}
		else
		{
			if (!Fillmode || Charcnt + charcnt > Fillcnt || In_string)
			{
				putc(*s, Out_file);
				Lineout++;
				charcnt = 0;
				Charcnt = 0;
			}
			else
			{
				putc(' ', Out_file);
				charcnt++;
			}
		}
	}
	if ((Charcnt += charcnt) > Fillcnt && Fillmode == 1)
	{
		if (In_string)
		{
			if (In_quote)
			{
				put_string("\");\nIIwrite(\"", Out_file);
				Charcnt = 9;
			}
			else
			{
				put_string("\\\n", Out_file);
				Charcnt = 0;
			}
		}
		else
		{
			putc('\n', Out_file);
			Charcnt = 0;
		}
		Lineout++;
	}
}


/*
**  PUTS -- put a string on an output file using putc()
*/
put_string(s, f)
register char	*s;
register FILE	*f;
{
	for ( ; *s; s++)
		putc(*s, f);
}
