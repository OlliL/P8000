#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

#define writestr(X) { static char text[]= X; write(1,text,sizeof(text)-1); }


/* Aktionszeichen in gotoXY -Zeichenkette */
#define GOTOINCR  255 /* x,y -Werte um 1 erhoehen                         */
#define GOTOCOUT  254 /* y bzw x -Wert um folgenden Wert erhoehen und als */
                      /*          Zeichen ausgeben                        */
#define GOTOSOUT  253 /* y bzw x -Wert als Zeichenfolge (2) ausgeben      */
#define GOTOEND   252 /* Ende der Zeichenfolge                            */

#include "fsetab.h"
#include "fseinst.h"

extern	unsigned char	*getenv();
extern	unsigned char	*tgetstr();
extern	int		tgetnum();
extern	long		time();

struct parms *parm,par;
unsigned char buff[8000];      /* reicht fuer 500 Eintraege in /bin */
unsigned char scratch[1050];

unsigned char	*p, *pp, *g;



void memcpy(dest, source, cnt)
unsigned char *dest;
unsigned char *source;
int  cnt;
{ while (cnt--)  *(dest++)= *(source++);
}



main(argc,argv)
int argc;
char *argv[];
{ int          nmask,mask, nincr,incr;
  int          f,f2,ii,jj;
  unsigned int i;
  long         ll;

  /* Entpacken von pgm1 und pgm2 */
  mask= pgm1[5] ^ 0xe5;
  incr= pgm1[6] ^ 0xe5;
  i= 9;
  while (i < (unsigned)sizeof(pgm1))
  { pgm1[i++] ^= mask;
    mask += incr;
  }
  mask= pgm2[5] ^ 0xe5;
  incr= pgm2[6] ^ 0xe5;
  i= 9;
  while (i < (unsigned)sizeof(pgm2))
  { pgm2[i++] ^= mask;
    mask+= incr;
  }

  /* parm -Structur finden */
  i= 9;
  do
  { parm= (struct parms *)(pgm2+i);
  } while ( ++i < (unsigned)(sizeof(pgm2)-20)    &&
            strncmp(parm->_gotoXY,"\001\002\003\004\005\006\007",7));

  if ( i >= (unsigned)(sizeof(pgm2)-20) )       /* nicht gefunden */
  { writestr("internal error (1)\n");
    exit(1);
  }

  memcpy(&par,parm,sizeof(par));  /* Abzug von parm -Structur */

  if ((f= creat("/bin/fse",0333))<=0)     /* fse -Datei eroeffnen     */
  { writestr("can't access /bin/fse\n");
    exit(1);
  }
  f2= open("/bin",O_RDONLY);              /* /bin -Verzeichnis einlesen */
  jj= read(f2, buff, sizeof(buff));
  close(f2);

                  /* fse -Eintrag im Directory suchen            */
  ii= 0;          /* beim Suchen ganze Directories ueberspringen */
  while (ii<jj && strncmp(buff+2+ii,"fse",4)) ii+= 16;
  if (ii>=jj)
  { close(f);
    writestr("internal error (2)\n");
    exit(1);
  }
  memcpy( /* dest   */  &(par.inode),
          /* source */  buff+ii,
          /* cnt    */  sizeof(ii)     );

  ll= ii; /* muss in eine long -Variable geladen werden */

  memcpy( /* dest   */ &(par.inodepos),
          /* source */ &ll,
          /* cnt    */ sizeof(ll)      );

  /*  Termcap verarbeiten */
  /************************/

  memcpy(par.TERM,p=getenv("TERM"),9);
  if (tgetent(buff,p) != 1)
  { writestr("error in termcap\n");
    exit(1);
  }
  par.gotowait= 0;
  par.clrwait = 0;
  p= buff+1024;
  ii= tgetnum("sg");
  if (ii<0)  ii= tgetnum("ug");
  par._vmod= (ii<=0);   /*  1 --> 0 (P8000)           */
                        /*  0 --> 1 (VENIX)           */
                        /* -1 --> 1 (VENIX = default) */
  if ((pp=tgetstr("cm",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9')
                           { par.gotowait= 10*(par.gotowait) + *(pp++) -'0';
                           }
                           memcpy(scratch,pp,30); }
                           else scratch[0]= 0;
  if ((pp=tgetstr("cl",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9')
                           { par.clrwait= 10*(par.clrwait) + *(pp++) -'0';
                           }
                           if (*pp=='*') ++pp;
                           memcpy(par._clrscr ,pp,9); }
                           else par._clrscr[0]= 0;
  if ((pp=tgetstr("ce",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9') ++pp;
                           memcpy(par._clreol ,pp,9); }
                           else par._clreol[0] = 0;
  if ((pp=tgetstr("al",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9') ++pp;
                           memcpy(par._insline,pp,9); }
                           else par._insline[0]= 0;
  if ((pp=tgetstr("dl",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9') ++pp;
                           memcpy(par._delline,pp,9); }
                           else par._delline[0]= 0;
  if ((pp=tgetstr("ic",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9') ++pp;
                           memcpy(par._ins_ch ,pp,9); }
                           else par._ins_ch[0] = 0;
  if ((pp=tgetstr("so",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9') ++pp;
                           memcpy(par._video  ,pp,9); } 
                           else par._video[0]= 0;

  if ((pp=tgetstr("se",&p)) != (byte *)0)  {
                           while (*pp>='0' && *pp<='9') ++pp;
                           memcpy(par._nvideo ,pp,9); }
                           else par._video[0]= 0;

  if (par._video[0]==0)  /* Keine Videoattribute */
  { par._video[0]= par._nvideo[0]= ' ';
    par._video[1]= par._nvideo[1]= 0;
    par._vmod= 0;  /* P8000 */
  }

  g= (byte *)(par._gotoXY);
  p= scratch;
  while (*p)
  { if (p[0]=='\\' && p[1]=='E')
    { *(g++)= '\033';
      p+= 2; continue;
    }
    if (p[0]=='%' && p[1]=='i')
    { *(g++)= GOTOINCR;
      p+=2; continue;
    }
    if (p[0]=='%' && p[1]=='+')
    { p+= 2;
      *(g++)= GOTOCOUT;
      if (p[0]=='\\')
      { *(g++)= (p[1]-'0')*64 +
                (p[2]-'0')*8  +
                (p[3]    );
        p+= 4; continue;
      }
      *(g++)= *(p++);
      continue;
    }
    if (p[0]=='%' && p[1]=='d')
    { p+= 2;
      *(g++)= GOTOSOUT;
      continue;
    }
    if (p[0]=='%' && p[1]=='2')
    { p+= 2;
      if (*p == 'd') ++p;
      *(g++)= GOTOSOUT;
      continue;
    }
    if (p[0]=='\\')
    { *(g++)= (p[1]-'0')*64 +
              (p[2]-'0')*8  +
              (p[3]    );
      p+= 4; continue;
    }
    *(g++)= *(p++);
    if (p> scratch+20)
    { par._gotoXY[0]= 0;
      break;
    }
  }
  *g= GOTOEND;
   
  if (par._delline[0]==0)  par._insline[0]= 0;
  
  if (par._clrscr[0] ==0 ||
      par._gotoXY[0] ==0 ||
      par._clreol[0] ==0 )
  { writestr("can't work with this terminal\n");
    exit(1);
  }


  mask= time((long)0) % 250 +2;
  incr= (time((long)0) % 1024) / 6  +5;
  par.incr= incr ^ 0xe5;
  par.mask= mask ^ 0xe5;
  i= sizeof(par) -sizeof(par.incr) -sizeof(par.mask);

  g= (unsigned char *)(par._gotoXY);
  
  while (i--)
  { *(g++) ^= mask;
    mask= (mask+incr) % 256;
  }

  memcpy(parm,&par,sizeof(par));
  
  write(f, pgm1+9, sizeof(pgm1)-9);
  write(f, pgm2+9, sizeof(pgm2)-9);
  close(f);

writestr("\n\n\n\
fse installed\n\
\n\n\
If you have problems with fse/fsc or\n\
special wishes or suggestions,\n\
please reffer to the author\n\n\
\tJens -Peter Redlich\n\n\
\tinformation processing group\n\
\tHumboldt -University of Berlin\n\
\tDepartment of Mathemathics\n\
\tGDR 1086 Berlin\n\
\tP. B. 1297\n\n");
execl("/bin/rm","/bin/rm",argv[0],(char *)0);
}

  
  
