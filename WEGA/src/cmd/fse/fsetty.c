/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 2.1     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Termcap       */
/*                                                   */
/*       Stand: 14.01.89                             */
/*                                                   */
/*****************************************************/


#include "fse.h"





byte      _ttybuff[TTYBUFF];
byte far *_ttybp;
int       _sgflags;



void ttyflush()
{ if (_ttybp != _ttybuff)
  { write(tty,_ttybuff,(integer)(_ttybp -_ttybuff));
    _ttybp= _ttybuff;
  }
}


void ttych(ch)
char ch;
{ if (_ttybp == _ttybuff+TTYBUFF) ttyflush();
  *(_ttybp++)= ch;
}


void ttystr(str)
byte far *str;
{ while (*str)
  { if (_ttybp == _ttybuff+TTYBUFF) ttyflush();
    *(_ttybp++)= *(str++);
  }
}



void clrscr()
{ int i;

  ttystr(parm._clrscr);
  if (parm.clrwait)
  {
    ttyflush();
#ifdef WEGA
    i= parm.clrwait;
    do write(tty,"",1); while (--i);
#else
    sleep(1);
#endif
  }
}


void gotoXY(x,y)
short x,y;
{ byte  far *p;
  byte  c;

  p= parm._gotoXY;
  while ((c= *(p++)) != GOTOEND)
  { switch (c) {
    case GOTOINCR: /* increment */
                   x++; y++; break;
    case GOTOCOUT: /* increse and output (char) */
                   if (y>=0)
                   { ttych((char)(y+ *(p++))); y= -10; }
                   else
                   { ttych((char)(x+ *(p++)));        }
                   break;
    case GOTOSOUT: /* output (string) */
                   if (y>=0)
                   { ttych((char)((y/10) +'0'));
                     ttych((char)((y%10) +'0'));
                     y= -10;
                   }
                   else
                   { ttych((char)((x/10) +'0'));
                     ttych((char)((x%10) +'0'));
                   }
                   break;
    default:       ttych((char)(c));
    }
  }
  if (c= parm.gotowait)
  { ttyflush();
    do write(tty,"",1); while (--c);   /* diese Art zu warten muss nicht immer gut sein */
  }
}


char	getoc()  /* Zeichen original von der Tastatur nehmem */
{ char c;

  c= 0;
  if (kbdp!= (unsigned char far *)0)  c= *(kbdp++);
  if (c==0)
  { kbdr= 0;
    kbdp= (unsigned char far *)0;
    ttyflush();
    read(tty,&c,1);
  }
  return c;
}


char getc()
{ byte    c;

  c= getoc();
  if ( !yz) return c;  /* Umwandlung  Y<=>Z ? */
  if (c=='Y')        return 'Z';
  if (c=='y')        return 'z';
  if (c==CNTRL('Y')) return CNTRL('Z');
  if (c=='Z')        return 'Y';
  if (c=='z')        return 'y';
  if (c==CNTRL('Z')) return CNTRL('Y');
  return c;
}



byte getkey()      /* Funktionstaste einlesen */
{ int i,j,c;
  char f[4];

  klau= 0;
  c= getc();
  for (;;) {
  if (c<CNTRL('A') || (c>CNTRL('Z') && c<' ') || c>= 0x7f)
  { switch (c)  {
    case 0x1b:  /* mit ESC beginnende Sequenzen */
               c=getc();
               if (c>='a' && c<='z')
               { kbdp= kbd.k[c-'a'];
                 c= *(kbdp++);
                 if (++kbdr==27)
                 { kbdp= (unsigned char far *)0;
                   c   = 0;
                   kbdr= 0;
                 }
                 break;
               }
               switch (c) {
               case '[': switch (getoc()) {
                         case 'A': c= UP;    break;    /* vt100, XENIX-ANSI */
                         case 'B': c= DOWN;  break;    /* vt100, XENIX-ANSI */
                         case 'C': c= RIGHT; break;    /* vt100, XENIX-ANSI */
                         case 'D': c= LEFT;  break;    /* vt100, XENIX-ANSI */
                         case 'Z': c= WRDLEFT; break;  /* vt100, XENIX-ANSI */
                         default : c= 0; }
                         break;
               case 'H': c= UP;        break;    /* VENIX auf Schneider -PC */    
               case 'K': c= LEFT;      break;    /* VENIX auf Schneider -PC */
               case 'M': c= RIGHT;     break;    /* VENIX auf Schneider -PC */
               case 'P': c= DOWN;      break;    /* VENIX auf Schneider -PC */
               case 'I': if (parm._vmod) c= PAGESTRT;         /* VENIX auf Schneider -PC */    
                         else            c= WRDLEFT;  break;  /* |<- auf P8000/ADM3a     */
               case 'Q': if (parm._vmod) c= PAGEEND;          /* VENIX auf Schneider -PC */
                         else            c= HELP;     break;  /* F9  auf P8000/ADM3a     */
               case 'G': c= WRDLEFT;   break;    /* VENIX auf Schneider -PC */    
               case 'O': c= WRDRIGHT;  break;    /* VENIX auf Schneider -PC */    
               case ';': c= HELP;      break;    /* VENIX auf Schneider -PC  1 */
               case '<': c= POSITION;  break;    /* VENIX auf Schneider -PC  2 */
               case '=': c= NEXTERR;   break;    /* VENIX auf Schneider -PC  3 */
               case '>': c= CHWIND;    break;    /* VENIX auf Schneider -PC  4 */
               case '?': c= VIEWDIR;   break;    /* VENIX auf Schneider -PC  5 */
               case '@': c= HELP;      break;    /* VENIX auf Schneider -PC  6 */
               case 'A': c= FINDOLD;   break;    /* VENIX auf Schneider -PC  7 */
               case 'B': c= LASTERR;   break;    /* VENIX auf Schneider -PC  8 */
               case 'C': c= HELP;      break;    /* VENIX auf Schneider -PC  9 */
               case 'D': c= FILEQUIT;  break;    /* VENIX auf Schneider -PC 10 */
               case 'S': c= DELCHRR;   break;    /* VENIX auf Schneider -PC (Del) */
               case 'T': c= FINDOLD;   break;    /* P7  auf P8000/ADM3a     */
               case 'E': c= LASTERR;   break;    /* F8  auf P8000/ADM3a     */
               case 'R': c= NEXTERR;   break;    /* F13 auf P8000/ADM3a     */
               case 'Y': c= POSITION;  break;    /* F12 auf P8000/ADM3a     */
               case 'W': c= CHWIND;    break;    /* F14 auf P8000/ADM3a     */
               case 'J': c= PAGESTRT;  break;    /* PgUp auf XENIX/wy60     */
               default : c= 0; }
               break;
    case 0x00: if ((c=getoc())==0) c= FILEQUIT;  /* F10 auf P8000/ADM3a     */
               break;   /* ansonsten bleibt c so, wie es war  */
    case 0x01:
              switch  (getoc()) {
              case 0x1b:
                         switch (getoc()) {
                         case '@': c= HELP;     break;  /* F1  auf XENIX/wy60 */
                         case 'A': c= VIEWDIR;  break;  /* F2  auf XENIX/wy60 */
                         case 'B': c= REJCLOAD; break;  /* F3  auf XENIX/wy60 */
                         case 'C': c= FIND;     break;  /* F4  auf XENIX/wy60 */
                         case 'D': c= FINDOLD;  break;  /* F5  auf XENIX/wy60 */
                         case 'E': c= LASTERR;  break;  /* F6  auf XENIX/wy60 */
                         case 'F': c= NEXTERR;  break;  /* F7  auf XENIX/wy60 */
                         case 'G': c= FILEQUIT; break;  /* F8  auf XENIX/wy60 */
                         case 'H': c= POSITION; break;  /* F9  auf XENIX/wy60 */
                         case 'I': c= CHWIND;   break;  /* F10 auf XENIX/wy60 */
                         default : c= 0; }
                         getoc();  /* nachfolgendes ^M bei XENIX/wy60 ueberlesen */
                         break;
               default:  c= 0; }
               break;
               
    case 0x7f: c= DELCHRL; break;   /* DEL auf WEGA/ADM3a, DEL auf XENIX/wy60 */
    case 0x1e: if (parm._vmod) c= LINESTRT;       /* HOME auf XENIX/wy60 */
               else            c= PAGESTRT;       /* <-\ auf P8000/ADM3a */
               break;
    default  : c= 0; }
  }
  if (c>=CNTRL('A') && c<=CNTRL('Z'))  c= parm.cntrl[c-CNTRL('A')];
  if (c==CNTRL1 || c==CNTRL2)
  { i= getc();
    if (i>='a' && i<='z')  i-= ('a'-'A');
    if (i>='A' && i<='Z')  i-= '@';
    if (i>=CNTRL('A') && i<=CNTRL('Z'))
    { if (c==CNTRL1)  c= parm.cntrl1[i-CNTRL('A')];
      else            c= parm.cntrl2[i-CNTRL('A')];
    }
    else c= 0;
  }
  if (c)  break;
  bell();
  c= getc();
  }
  return(c);
}


void ttyinit()
{ struct sgttyb sgtty;


  kbdr= 0;
  kbdp= (unsigned char far *)0;
  tty= open("/dev/tty",O_RDWR);
  gtty(tty,&sgtty);
  _sgflags= sgtty.sg_flags;
  sgtty.sg_flags |= RAW;
  sgtty.sg_flags &= ~(ECHO | CRMOD | TANDEM | XTABS);
  stty(tty,&sgtty);
  _ttybp= _ttybuff;
  clrscr();
  ttyflush();
}

void ttydrop()
{ struct sgttyb sgtty;

  clrscr();
  ttyflush();
  gtty(tty,&sgtty);
  sgtty.sg_flags= _sgflags;
  stty(tty,&sgtty);
  close(tty);
}

void delins(del,ins)  /* dient zum Rollen eines Bildschirmbereiches */
int del, ins;
{
  if (parm._insline[0]==0) display();
  else
  { gotoXY(0,del);  ttystr(parm._delline);
    gotoXY(0,ins);  ttystr(parm._insline);
  }
}


void invers()
{
  if (parm._vmod == 0) /* P8000 */
  { ttystr(parm._video);
  }
  else
  { ttych(' ');
    ttystr(parm._video);
  }
}

void nvideo()
{
  if (parm._vmod == 0) /* P8000 */
  { ttystr(parm._nvideo);
  }
  else
  { ttych(' ');
    ttystr(parm._nvideo);
  }
}


void bell()
{ write(1,"\007",1);
}

char getletter()
{ char c;

  c= getc();
  if (CNTRL('A')<=c  &&  c<=CNTRL('Z')) c+= ('A' -CNTRL('A'));
  else
  if ('a' <= c  &&  c <= 'z') c+= ('A' -'a');
  else
  if ('A' > c   ||  c > 'Z' ) c= ' ';
  return(c);
}


char normch(b)
byte b;
{
  if ((b &=fcb.bytemask) <'\040' || b>='\177')  b= (fcb.cntrmode==1)? ' ' : '.';
  return( (char) b);
}


byte numeral(c)
byte c;
{ char ch;

  if (c<'\177')
  { ch= (char)c;
    if (ch>='0'  &&  ch<='9') { c-= '0';                          } else
    if (ch>='a'  &&  ch<='f') { c-= ('a'-'\012'); ch+= ('A'-'a'); } else
    if (ch>='A'  &&  ch<='F') { c-= ('A'-'\012');                 } else
    goto  numer_1;
    ttych(ch);
  }
  else
  { numer_1:
    c= '\020';
  }
  return(c);
}


void hex2out(b)
byte b;
{ static char hexnum[]= { '0','1','2','3','4','5','6','7','8','9',
                          'A','B','C','D','E','F' };
  ttych( hexnum[ b>>4     ]);
  ttych( hexnum[ b & 0x0f ]);
}

void decout(i)
integer i;
{ static char _scratch[]= "12345";
  char   *_p;

  _p  = _scratch+4;
  do
  { *(_p--)= '0'+ (i%10);
    i/= 10;
  } while (i);
  while (_p >= _scratch)  *(_p--)= ' ';
  if (parm._vmod==1)  /* VENIX */
  { ttystr(parm._video);
    ttystr(_scratch);
    ttystr(parm._nvideo);
  }
  else  ttystr(_scratch);
}
