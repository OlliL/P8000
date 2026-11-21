/* fsepar: fseinst (ID=1) stripped to the setting of the parameter block parm.
 * fseinst carries the program packed in pgm1/pgm2 (made by mkfseinst), unpacks it, fills parm
 * from the directory /bin and termcap, encrypts it and writes /bin/fse. fsepar does the same on
 * the linked, not installed program FILE (parm is found by 1,2,3,4,5,6,7 at the head of _gotoXY)
 * and writes parm back in place:
 *
 *	fsepar FILE
 *
 * The code from "fse -Eintrag im Directory suchen" to the encryption is that of fseinst. What
 * fseinst took from the machine it ran on is that machine's here, as far as the shipped /bin/fse
 * shows it (its block decrypted):
 *  - the directory /bin, read into buff: fse (inode 0x160) at byte 0x6a0 gives inode and inodepos;
 *    ps (0x7b) at 1040 and pstat (0x7c) at 1056 are what tgetstr does not overwrite behind its
 *    strings at buff+1024, and fseinst copies 9 bytes of each string
 *  - the environment (getenv below): TERM=P8 and the string after it, TZ=MEZ (fseinst copies 9 bytes
 *    from getenv("TERM") to par.TERM); the termcap entry P8 is that of /etc/termcap of WEGA 3.1 with
 *    ic=\EQ, which the shipped block holds (given as TERMCAP: tgetent takes the entry from there)
 *  - time(): mask= time % 250 + 2 = 0xd5, incr= (time % 1024) / 6 + 5 = 0xaa
 */
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

#define writestr(X) { static char text[]= X; write(2,text,sizeof(text)-1); }


/* Aktionszeichen in gotoXY -Zeichenkette */
#define GOTOINCR  255 /* x,y -Werte um 1 erhoehen                         */
#define GOTOCOUT  254 /* y bzw x -Wert um folgenden Wert erhoehen und als */
                      /*          Zeichen ausgeben                        */
#define GOTOSOUT  253 /* y bzw x -Wert als Zeichenfolge (2) ausgeben      */
#define GOTOEND   252 /* Ende der Zeichenfolge                            */

#include "fsetab.h"

#define MASK	0xd5
#define INCR	0xaa

/* the environment of the installation: TERM=P8 followed by TZ=MEZ */
char envterm[]= "P8\0TZ=MEZ";
char envtermcap[]= "P8|P8000-A|P8000-Terminal kompatibel ADM31:\
al=\\EE:am:bs:ce=\\ET:cm=\\E=%+ %+ :cl=50\\E*^^:co#80:dc=\\EW:dl=\\ER:\
cd=\\EY:ho=^^:li#24:ma=^Z^L^K^P^L j^Jk^Pl :\
kl=^H:kd=^J:ku=^K:kr=^L:kh=^^:\
NM=\\E(\\EG0:NB=\\E(\\EG2:NR=\\E(\\EG4:NS=\\E(\\EG6:\
AL=\\E)\\EG0:AB=\\E)\\EG2:AR=\\E)\\EG4:AS=\\E)\\EG6:OV#1:\
mi:nd=^L:up=^K:ic=\\EQ:\
so=\\EG4:se=\\EG0:sg#1:us=\\EGC:ue=\\EG0:ug#1:";

/* the entries of the directory /bin of the installation that the block shows */
struct bindir {
	unsigned pos;
	int	ino;
	char	*name;
} bindir[]= {
	1040,	0x7b,	"ps",
	1056,	0x7c,	"pstat",
	0x6a0,	0x160,	"fse",
	0,	0,	0
};

extern	unsigned char	*tgetstr();
extern	int		tgetnum();

struct parms *parm,par;
unsigned char buff[8000];      /* reicht fuer 500 Eintraege in /bin */
unsigned char scratch[1050];
#define PGMSIZ 50000
unsigned char pgm[PGMSIZ];	/* the program */

unsigned char	*p, *pp, *g;



unsigned char *getenv(name)
char *name;
{ if (strcmp(name,"TERM")==0)     return((unsigned char *)envterm);
  if (strcmp(name,"TERMCAP")==0)  return((unsigned char *)envtermcap);
  return((unsigned char *)0);
}



void memcpy(dest, source, cnt)
unsigned char *dest;
unsigned char *source;
int  cnt;
{ while (cnt--)  *(dest++)= *(source++);
}



main(argc,argv)
int argc;
char *argv[];
{ int          mask, incr;
  int          f,ii,jj;
  unsigned int i,len;
  long         ll;
  struct bindir *d;

  if (argc != 2)
  { writestr("usage: fsepar file\n");
    exit(1);
  }
  if ((f= open(argv[1],O_RDWR)) < 0)
  { writestr("can't open file\n");
    exit(1);
  }
  len= read(f, pgm, PGMSIZ);
  if (len == 0 || len == (unsigned)-1 || len >= PGMSIZ)
  { writestr("can't read file\n");
    exit(1);
  }

  /* parm -Structur finden */
  i= 0;
  do
  { parm= (struct parms *)(pgm+i);
  } while ( ++i < (len-sizeof(par))    &&
            strncmp(parm->_gotoXY,"\001\002\003\004\005\006\007",7));

  if ( i >= (len-sizeof(par)) )       /* nicht gefunden */
  { writestr("internal error (1)\n");
    exit(1);
  }

  memcpy(&par,parm,sizeof(par));  /* Abzug von parm -Structur */

  /* /bin -Verzeichnis der Installation */
  for (d= bindir; d->name; d++)
  { buff[d->pos]= d->ino >> 8;
    buff[d->pos+1]= d->ino;
    strncpy(buff+d->pos+2, d->name, 14);
  }
  jj= sizeof(buff);

                  /* fse -Eintrag im Directory suchen            */
  ii= 0;          /* beim Suchen ganze Directories ueberspringen */
  while (ii<jj && strncmp(buff+2+ii,"fse",4)) ii+= 16;
  if (ii>=jj)
  { writestr("internal error (2)\n");
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


  mask= MASK;
  incr= INCR;
  par.incr= incr ^ 0xe5;
  par.mask= mask ^ 0xe5;
  i= sizeof(par) -sizeof(par.incr) -sizeof(par.mask);

  g= (unsigned char *)(par._gotoXY);
  
  while (i--)
  { *(g++) ^= mask;
    mask= (mask+incr) % 256;
  }

  memcpy(parm,&par,sizeof(par));

  if (lseek(f, (long)(unsigned)((unsigned char *)parm - pgm), 0) < 0 ||
      write(f, parm, sizeof(par)) != sizeof(par))
  { writestr("can't write file\n");
    exit(1);
  }
  close(f);
  exit(0);
}
