char		*m_view;
char		*m_korr;

f_view(frame)
register char		*frame;
{
   if (MMshell(m_view, 0, 0, "./view"))
	MMerror(" view nicht ausfuehrbar ");
}

f_korr(frame)
register char		*frame;
{
   if (MMshell(m_korr, 0, 0, "./korr"))
	MMerror(" korr nicht ausfuehrbar ");
}

main()
{
	register char		*form;
	char			*frame;
	extern char		*MMalloc();

   MMinit_form(1);

   frame = (char *) 0;
   form = MMalloc(&frame, 4, 5, 71, (char *) 0);
   MMout_field(form, "----------------------------------------------------- D E M O   F O R M");

   m_view = form = MMalloc(&frame, 8, 5, 21, (char *) 0);
   MMmen_field(form, f_view, (int (*)()) 0, " ANSICHT DER DB DT");

   m_korr = form = MMalloc(&frame, 10, 5, 21, (char *) 0);
   MMmen_field(form, f_korr, (int (*)()) 0, " KORREKTUR DER DB DT");

   MMform(frame);

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
