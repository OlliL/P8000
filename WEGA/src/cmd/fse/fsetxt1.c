/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 1.1     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Termcap       */
/*                                                   */
/*       Stand: 03.01.89                             */
/*                                                   */
/*****************************************************/

#include "fse.h"


byte far  *last_ln(curr_pos)    /* es muss sichergestellt sein, dass 'curr_pos' niemals */
register byte far *curr_pos;    /* von bpos nach fpos ueberspringen muss                */
{ register byte b;
  register byte bytemask;

  bytemask= fcb.bytemask;
  while ((b= *(--curr_pos) & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K'));
  return(curr_pos);        /* dies ist die Adresse des Zeilentrennzeichens */
}

byte far *next_ln(curr_pos)    /* es muss sichergestellt werden, dass 'curr_pos' niemals */
register byte far *curr_pos;   /* von fpos nach bpos ueberspringen muss                  */
{ register byte b;
  register byte bytemask;

  bytemask= fcb.bytemask;
  --curr_pos;
  while ((b= *(++curr_pos) & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K'));
  return(curr_pos);        /* dies ist die Adresse des Zeilentrennzeichens */
}

void count_ln(begin,end)   /* beachte: Das Zeichen '*end' wird nicht mitgezaehlt */
register byte far *begin;
register byte far *end;
{ register byte b;
  register byte bytemask;

  nl_cnt= 0;
  ff_cnt= 0;
  bytemask= fcb.bytemask;
  while (begin < end)
  { if  ((b= *(begin++) & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K')) continue;
    if  (b=='\n')  { ++nl_cnt;  continue; }
    ++ff_cnt;   /* da nur noch der Fall b=='\f' uebrigbleibt */
  }
}

void gotoLine(lineno)
long lineno;
{ register byte far *fpos;
  register byte far *bpos;
  register byte b;
  register byte bytemask;
           byte far *oldfpos,
                    *oldbpos;
           long ln;

  if (lineno == 0)
  { shortask("Line: ",scratch);
    fpos= scratch;
    ln= 0;
    while ((b= *(fpos++))!='\000')
    { if (b>='0'  &&  b<='9') ln= ln*10 +b-'0';
    }
  }
  else ln= lineno;
  if (ln==0) ln= fcb.mrkln;
  if (ln==0) return;
  bytemask= fcb.bytemask;
  oldbpos= bpos= fcb.bpos;
  oldfpos= fpos= fcb.fpos;

  if (ln>fcb.linecnt)
  { do
    { goto1_ln:
      while ((b=(*(fpos++)= *(bpos++)) & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K'));
      if (bpos > fcb.memend)
      { --bpos;  --fpos;
        break;
      }
      ++lndiff;
      if (b=='\f') goto goto1_ln;
    } while (++fcb.linecnt < ln);
    if (oldbpos<=fcb.blkbegin  &&  fcb.blkbegin<bpos) fcb.blkbegin-= (integer)(bpos-fpos);
    if (oldbpos<=fcb.blkend    &&  fcb.blkend  <bpos) fcb.blkend  -= (integer)(bpos-fpos);
  }
  else
  { do
    { goto2_ln:
      while ((b=(*(--bpos)= *(--fpos)) & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K'));
      --lndiff;
      if (b=='\f') goto goto2_ln;
    } while (--fcb.linecnt >= ln);
    if (fpos<=fcb.blkbegin  &&  fcb.blkbegin<oldfpos) fcb.blkbegin+= (integer)(bpos-fpos);
    if (fpos<=fcb.blkend    &&  fcb.blkend  <oldfpos) fcb.blkend  += (integer)(bpos-fpos);
    fcb.linecnt= ln;
    ++fpos;
    ++bpos;
    ++lndiff;
  }
  fcb.fpos= fpos;
  fcb.bpos= bpos;
}

long gotoByte(adr)
byte far *adr;
{ if (fcb.fpos>adr) return(-(long)((integer)(fcb.fpos-adr)));
  else              return( (long)((integer)(adr-fcb.bpos)));
}

byte far *atCol(curr_pos)     /* 'atCol' ermittelt den Wert den 'fpos' haben muss, damit der Kursor */
register byte far *curr_pos;  /* in Spalte 'Col' und in der mit dem Zeilentrennzeichen auf Adresse  */
{ register byte    b;         /* 'curr_pos' beginnnenden Zeile  steht.                              */
  register long    x;
  register byte    bytemask;

  bytemask= fcb.bytemask;
  x= 0;
  do
  { if ((b= *(++curr_pos) & bytemask) < '\040')
    { if (b=='\n'  ||  b=='\f')  break;
      if (b=='\t')  {x= (x+8) & ~0x07; continue; }
      if ( !fcb.cntrmode) continue;
      if (b=='\r' && (*(curr_pos+1)  & bytemask)=='\n') break;
      if (fcb.cntrmode==3) ++x;
    }
    ++x;
  } while (x<=Col);
  return(curr_pos);
}

long getCol()                   /* Col   := Spaltennummer des Kursors ohne Beruecksichtigung der Blockmarken */
{ register byte far *curr_pos;  /* BlkCol:= Anzahl der Blockmarkierungen in der Zeile                        */
  register byte   b;            /* getCol:= effektive Spaltennummer des Kursors                              */
  register long   x;
  register byte   bytemask;
           byte   oldch;
           
  BlkCol= 0;
  oldch= *fcb.fpos;
  *fcb.fpos= '\n';
  bytemask= fcb.bytemask;
  x= 0;
  curr_pos= fcb.linestrt;  /* 'fcb.linestrt' ist die Adresse des Zeilentrennzeichens */
  while (1)
  { if ((b= *(++curr_pos) & bytemask) == '\n') curr_pos= fcb.bpos;
    if (curr_pos == fcb.blkbegin && fcb.blkend!=(byte far *)0) ++BlkCol;
    if (curr_pos == fcb.blkend                           ) ++BlkCol;
    if (b<'\040')
    { if (b=='\n')
      { *fcb.fpos= oldch;
        return( (Col=x)+BlkCol);
      }
      if (b=='\t')
      { x= (x+8) & ~0x07; continue;
      }
      if (fcb.cntrmode==0) continue;
      if (b=='\r'  &&  (*fcb.bpos & bytemask)=='\n') continue;
      if (fcb.cntrmode==3) ++x;
    }
    ++x;
  }
}

void i_tab()      /* intelligenter Tabulator */
{ register byte      b;
  register byte far  *curr_pos;
  register byte      bytemask;
           short     inword;

  if ((curr_pos= fcb.linestrt) >= fcb.membegin)
  { curr_pos       = atCol(last_ln(curr_pos));
    inword         = TRUE;
    fcb.modifyed   = TRUE;
    _display[fcb.y]= TRUE;
    bytemask       = fcb.bytemask;
    while ((b= *(curr_pos++) & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K'))
    { if ( !letter(b))
      { inword= FALSE;
      }
      else
      { if (!inword) break;
      }
      if (b!='\t') b= ' ';
      if (fcb.fpos == fcb.bpos)
      { error(E_TOOBIG);
        break;
      }
      *(fcb.fpos++)= b;
    }
  }
}


void a_tab()      /* auto -Tabulator */
{ register byte      b;
  register byte far *curr_pos;
  register byte      bytemask;

  curr_pos       = last_ln(fcb.linestrt);
  bytemask       = fcb.bytemask;
  while ((b= *(++curr_pos) & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K'))
  { if (b!=' '  &&  b!='{'  &&  b!=CNTRL('I')) break;
    if (b != '\t') b= ' ';
    if (fcb.fpos == fcb.bpos)
    { error(E_TOOBIG);
      break;
    }
    *(fcb.fpos++)= b;
  }
}

void newline(nl)
byte nl;
{ register byte far *p;
  register byte      b;

  p= fcb.fpos-1;
  while ((b= *p & fcb.bytemask)==' ' || b=='\t') --p;
  fcb.fpos= p+1;
  if (fcb.fpos == fcb.bpos)  error(E_TOOBIG);
  else
  { fcb.modifyed= TRUE;
    if (lfcr  &&  nl=='\n')
    { *(fcb.fpos++)= '\r';
      if (fcb.fpos == fcb.bpos) error(E_TOOBIG);
    }
    fcb.linestrt= fcb.fpos;
    if ((*(fcb.fpos++)= nl) == '\n')
    { aline(++fcb.linecnt, 1);
    }
    clreol();
    if (fcb.y >= fcb.maxY-2)
    { _dely= fcb.minY;
      _delcnt= 1;
      fcb.pagestrt= next_ln(fcb.pagestrt+1);
    }
    else _display[++fcb.y]= TRUE;
    _insy= fcb.y;
    _inscnt= 1;
    if (fcb.autotab)
    { a_tab();
    }
    p= fcb.bpos;
    while ((b= *p & fcb.bytemask)==' ' || b=='\t') ++p;
    if (b=='\n' || b=='\f') fcb.bpos= p;
  }
}


void textpage()
{ register byte far *curr_pos;
  register byte     b;
  register byte     bytemask;
  register byte far *fpos;
           short    y,isty;

  curr_pos= fcb.pagestrt;
  bytemask= fcb.bytemask;
  fpos    = fcb.fpos;
  y= isty= fcb.minY;
  do
  { while ( !_display[y])
      if (++y>fcb.maxY) return;
    while (isty<y  &&  curr_pos!=fcb.memend)
    { do
      { if (++curr_pos == fpos) curr_pos= fcb.bpos;
      } while ((b= *curr_pos & bytemask)>'\f'  ||  b<'\n'  ||  b==CNTRL('K'));
      ++isty;
    }
    _display[y]= FALSE;
    gotoXY(0,y);
    if (curr_pos == fcb.memend)
    { ttych('~'); clreol();  --curr_pos;
    }
    else textline(curr_pos);
  } while (1);
}

void newscr()
{
  fcb.y= fcb.minY;
  lndiff= 0;
  fcb.pagestrt= fcb.linestrt= last_ln(fcb.fpos);
  while (fcb.pagestrt>=fcb.membegin  &&  fcb.y< fcb.minY+(TTYLINES/4-1) )
  { ++fcb.y;
    fcb.pagestrt= last_ln(fcb.pagestrt);
  }
  fcb.offset= 0;   /* vor der Bildausgabe wird offset ohnehin noch gesetzt */
  display();
}

void buildscr()
{ short pagelen,my,i;
  long  longx,newy;

  if (fcb.y > fcb.maxY) {newscr(); lndiff= 0;}
  newy= fcb.y +lndiff -fcb.minY;
  pagelen= fcb.maxY+1 -fcb.minY;
  if (lndiff!=0)
  { if (newy <= -pagelen) newscr();
    else
    if (newy < 0)
    { if (newy > -M_SCROLL)  /* Bildschirm abwaerts rollen lassen */
      { _insy  = fcb.minY;
        _inscnt= -newy;
      }
      else display();
      fcb.pagestrt= fcb.linestrt= last_ln(fcb.fpos);
      fcb.y= fcb.minY;
    }
    else
    if (newy < pagelen)
    { fcb.linestrt= last_ln(fcb.fpos);
      fcb.y= newy+fcb.minY;
    }
    else
    if (newy <= 2*pagelen)
    { if (newy < pagelen+M_SCROLL)  /* Bildschirn aufwaerts rollen lassen */
      { _dely  = fcb.minY;
        _delcnt= newy -pagelen +1;
      }
      else display();
      fcb.y= fcb.maxY;
      fcb.pagestrt= fcb.linestrt= last_ln(fcb.fpos);
      i= pagelen;
      while (--i) fcb.pagestrt= last_ln(fcb.pagestrt);
    }
    else newscr();
  }
  longx= getCol();
  if (longx < fcb.offset  ||  longx>=fcb.offset+(TTYCOLUM-1))
  { display();
    if ((fcb.offset= longx -(TTYCOLUM-16))<0) fcb.offset=0;
  }
  X= (short) (longx-fcb.offset);
  Y= fcb.y;

  if (_delcnt  ||  _inscnt)
  { my= fcb.maxY+1 -_dely;          /* max moegl. Anz. zu loeschender Zeilen auf BS ab _dely   */
    if (_delcnt > my) _delcnt= my;  /* Falls mehr Loeschungen angefordert wurden, wird deren   */
                                    /* deren Anzahl auf den max. moeglichen Wert zurueckgesetz */
    my= fcb.maxY+1 -_insy;          /* max. moegl. Anz. einzufuegender Zeilen auf BS ab _insy  */
    if (_dely < _insy) my+= _delcnt;/* Falls jedoch ueber _insy die delcnt Zeilen geloescht    */
                                    /* werden, vergroessert sich my entsprechend.              */
    if (_inscnt > my) _inscnt= my;  /* Falls mehr Einfuegungen als moeglich angefordert werden,*/
                                    /* wird deren Anzahl auf einen sinnvollen Wert herabgesetzt*/
    if ((my= _delcnt) < _inscnt)    /* my := Anzahl der Rollungen, die noetig sind, um den     */
         my= _inscnt;               /* Bildschirm zu aktualisieren                             */
    if ((longx= _inscnt) < _delcnt)  longx=_delcnt;
    if (longx > pagelen-2)          /* wenn Rollen uneffektiv ist, Bildschirm neu aufbauen     */
    { _delcnt= _inscnt= 0;
      if (_insy < _dely) _dely= _insy;
      do _display[_dely]= TRUE;  while (++_dely<=fcb.maxY);
    }

    while (_inscnt)
    { if (_delcnt) { my= _dely; --_delcnt; if (_insy<_dely) ++_dely;  }
      else         { my= fcb.maxY;                                    }
      if (my != _insy) delins(/* del */ my, /* ins */ _insy);
      --_inscnt;
      if (my > _insy) memcpy(_display+_insy  ,_display+_insy-1   ,(integer)(my-_insy+1));
      else            memcpy(_display+my     ,_display+1+my      ,(integer)(_insy-my+1));
      _display[_insy]= TRUE;
    }
    while (_delcnt)
    { if (_dely != fcb.maxY) delins(_dely /* del */, fcb.maxY /* ins */);
      --_delcnt;
      memcpy(_display+_dely,_display+1+_dely,(integer)(fcb.maxY-_dely));
      _display[fcb.maxY]= TRUE;
    }
  }
  lndiff= 0;
  textpage();
}


short step()
{
  if (fw_opt)
  { if (fcb.bpos == fcb.memend)  return FALSE;
    shift( (long) 1);
  }
  else
  { if (fcb.fpos == fcb.membegin)  return FALSE;
    shift( (long)-1 );
  }
  return TRUE;
}


short findfw()
{ register byte far *bpos;
  register byte     c;
  register byte     muster1;
  register byte far *fpos;
  register byte far *p;
  register byte far *m;
           short    result;

  fpos   = fcb.fpos;
  bpos   = fcb.bpos;
  muster1= *muster;

  while (1)
  { c= *bpos & fcb.bytemask;
    if (c==muster1)
    { p= bpos+1;
      m= muster+1;
      while ((*p & fcb.bytemask) == *m)
      { ++p;  ++m;
      }
      if (! *m)  {result= TRUE; break; }
    }
    if (c<='\f' && c>='\n' && c!= CNTRL('K'))
    { if (bpos==fcb.memend)
      { result= FALSE; break; }
      ++lndiff;
      if (c=='\n') ++fcb.linecnt;
    }
    *(fpos++)= *(bpos++);
  }
  if (result==TRUE && bpos+ml>fcb.memend)   result= FALSE;
  if (bpos>fcb.blkbegin  &&  fcb.blkbegin>=fcb.bpos) fcb.blkbegin-= (integer)(bpos-fpos);
  if (bpos>fcb.blkend    &&  fcb.blkend  >=fcb.bpos) fcb.blkend  -= (integer)(bpos-fpos);
  fcb.bpos= bpos;
  fcb.fpos= fpos;
  return  result;
}


short findfwu()
{ register byte far *bpos;
  register byte     c;
  register byte     muster1;
  register byte far *fpos;
  register byte far *p;
  register byte far *m;
  register byte     cc;
           short    result;

  fpos   = fcb.fpos;
  bpos   = fcb.bpos;
  muster1= *muster;

  while (1)
  { c= *bpos & fcb.bytemask;
    if (c>='a' && c<='z')  c-= 'a'-'A';
    if (c==muster1)
    { p= bpos+1;
      m= muster+1;
      while (1)
      { cc= *p & fcb.bytemask;
        if (cc>='a' && cc<='z') cc-= 'a'-'A';
        if (cc != *m) break;
        ++p;  ++m;
      }
      if (! *m)  {result= TRUE; break; }
    }
    if (c<='\f' && c>='\n' && c!= CNTRL('K'))
    { if (bpos==fcb.memend)
      { result= FALSE; break; }
      ++lndiff;
      if (c=='\n') ++fcb.linecnt;
    }
    *(fpos++)= *(bpos++);
  }
  if (result==TRUE && bpos+ml>fcb.memend)   result= FALSE;
  if (bpos>fcb.blkbegin  &&  fcb.blkbegin>=fcb.bpos) fcb.blkbegin-= (integer)(bpos-fpos);
  if (bpos>fcb.blkend    &&  fcb.blkend  >=fcb.bpos) fcb.blkend  -= (integer)(bpos-fpos);
  fcb.bpos= bpos;
  fcb.fpos= fpos;
  return  result;
}


short findbw()
{ register byte far *bpos;
  register byte     c;
  register byte     muster1;
  register byte far *fpos;
  register byte far *p;
  register byte far *m;
           short    result;

  fpos   = fcb.fpos;
  bpos   = fcb.bpos;
  muster1= *muster;

  c= *bpos & fcb.bytemask;

  while (1)
  { if (c==muster1)
    { p= bpos+1;
      m= muster+1;
      while ((*p & fcb.bytemask) == *m)
      { ++p;  ++m;
      }
      if (! *m)
      { if (bpos+ml <= fcb.memend)
        { result= TRUE; break;
        }
      }
    }
    c= ( *(--bpos)= *(--fpos)) & fcb.bytemask;
    if (c<='\f' && c>='\n' && c!= CNTRL('K'))
    { if (fpos<fcb.membegin)
      { result= FALSE;
        ++fpos; ++bpos; break;
      }
      --lndiff;
      if (c=='\n') --fcb.linecnt;
    }
  }
  if (fpos<=fcb.blkbegin  &&  fcb.blkbegin<fcb.fpos) fcb.blkbegin+= (integer)(bpos-fpos);
  if (fpos<=fcb.blkend    &&  fcb.blkend  <fcb.fpos) fcb.blkend  += (integer)(bpos-fpos);
  fcb.bpos= bpos;
  fcb.fpos= fpos;
  return  result;
}


short findbwu()
{ register byte far *bpos;
  register byte     c;
  register byte     muster1;
  register byte far *fpos;
  register byte far *p;
  register byte far *m;
  register byte     cc;
           short    result;

  fpos   = fcb.fpos;
  bpos   = fcb.bpos;
  muster1= *muster;

  c= *bpos & fcb.bytemask;

  while (1)
  { if (c>='a' && c<='z')  c-= 'a'-'A';
    if (c==muster1)
    { p= bpos+1;
      m= muster+1;
      while (1)
      { cc= *p & fcb.bytemask;
        if (cc>='a' && cc<='z')  cc-= 'a'-'A';
        if (cc != *m) break;
        ++p;
        ++m;
      }
      if (! *m)
      { if (bpos+ml <= fcb.memend)
        { result= TRUE; break;
        }
      }
    }
    c= ( *(--bpos)= *(--fpos)) & fcb.bytemask;
    if (c<='\f' && c>='\n' && c!= CNTRL('K'))
    { if (fpos<fcb.membegin)
      { result= FALSE;
        ++fpos; ++bpos; break;
      }
      --lndiff;
      if (c=='\n') --fcb.linecnt;
    }
  }
  if (fpos<=fcb.blkbegin  &&  fcb.blkbegin<fcb.fpos) fcb.blkbegin+= (integer)(bpos-fpos);
  if (fpos<=fcb.blkend    &&  fcb.blkend  <fcb.fpos) fcb.blkend  += (integer)(bpos-fpos);
  fcb.bpos= bpos;
  fcb.fpos= fpos;
  return  result;
}


int wwrap()              /* automatischer Zeilenumbruch */
{ byte *bp, *p2, *p;
  int  c, i, j, b, l, z;

  p= fcb.fpos -1;
  i= 2;
  do
  { while ((c= (*p & fcb.bytemask))!='\n' && c!=' ' && c!='\f') --p;
    while ((c= (*p & fcb.bytemask)) == ' ') --p;
    if (c=='\n'  ||  c=='\f') return FALSE;   /* ganze Zeile hat nur ein Wort */
  } while (--i);

  _display[fcb.y]= TRUE;
  p2= fcb.bpos;

  /* vor angefangenes Wort gehen */

  z= 0;
  while (((*(--fcb.bpos)= *(--fcb.fpos)) & fcb.bytemask) != ' ') ++z;
  fcb.bpos++;        /* trennende Leerzeichen loeschen */
  while ((*(fcb.fpos-1) & fcb.bytemask) == ' ')  --fcb.fpos;
  p= fcb.fpos -1;
  while ((c=(*p & fcb.bytemask))!='\n'  &&  c!='\f') --p;
  if (   (fcb.blkbegin<=p2 && fcb.blkbegin>=p) ||
         (fcb.blkend  <=p2 && fcb.blkend >= p)  )
  { display();
    fcb.blkbegin= fcb.blkend= (byte *) 0;
  }
  l= fcb.linelen;
  b= j= 0;
  while ((*(++p) & fcb.bytemask)==' ') --l;
  if (l<3)
  { *(fcb.fpos++)= ' ';
    goto shiftback;
  }
  bp= p2= p;             /* Adr 1. Zeichen der Zeile */
  while (p2 < fcb.fpos)  /* Leerzeichen loeschen     */
  { if ((c= (*p2 & fcb.bytemask))=='\t' || c=='\r') *p2= c= ' ';
    *p= *(p2++);
    if (c==' ')
    { if (p[-1]!=' ') { ++b; ++j; *p= ' '; }
      else            { --p;               }
    }
    else  if (c >= ' ') ++j;
    ++p;
  }
              /* b: Anzahl der Leerzeichen in der Zeile         */
              /* j: Anzahl der Zeichen der Zeile (+Leerzeichen) */
              /* l: Zeilenlaenge                                */
  j= l-j;             /* Anzahl der zusaetzlichen Leerzeichen   */
  while (j<0)
  { *(--fcb.bpos)= ' '; --b;
    do
    { ++z;  ++j;
    } while ((*(--fcb.bpos)= *(--p)) != ' ');
    ++fcb.bpos;
  }
  fcb.fpos= (p2= p+j);           /* neue fpos -Position         */
  /* Leerzeichen aufteilen */
  while (p >= bp)
  { if ((*(--p2)= *(--p)) == ' ')
    { i= j/(b--);
      j -= i;
      while (i)
      { --i;
        *(--p2)= ' ';
      }
    }
  }
  if (fcb.fpos >= fcb.bpos)
  { fcb.fpos= fcb.bpos-1;
    error(E_TOOBIG);
    return TRUE;
  }
  newline('\n');
  shiftback:
  while (--z >= 0)  *(fcb.fpos++)= *(fcb.bpos++);
  return TRUE;
}



void format()
{ byte *bp, *p2, *p;
  int i, j, b, c,l;

  if ( !(l=fcb.linelen)) { bell(); return; }   /* kein Rand gesetzt          */
  fcb.modifyed= TRUE;
  fcb.errfile= FALSE;                      /* Fehlerverwaltung entfaellt */
  if (fcb.blkbegin != (byte *)0 || fcb.blkend != (byte *)0)
  { fcb.blkbegin= fcb.blkend= (byte *)0;
    i= fcb.minY;
  }
  else i= fcb.y;
  while (i<=fcb.maxY) _display[i++]= TRUE;
  while ((c=(*(fcb.fpos-1) & fcb.bytemask))!='\n' && c!='\f')
    *(--fcb.bpos)= *(--fcb.fpos);                 /* an den Zeilenanfang   */
  while (l>2 && (*fcb.bpos & fcb.bytemask)==' ')
  { --l;
    *(fcb.fpos++)= *(fcb.bpos++);
  }
  p= fcb.bpos;                                    /* Suche nach Absatzende */
  while (p < fcb.memend)
  { if ((c= (*p & fcb.bytemask)) == '\n')
    { *p= ' ';
      if ( !letter1(p[1]))
      { ++p;
        break;
      }
    }
    if (c=='\f') break;
    if (c=='\t' || c=='\r')  *p= ' ';
    ++p;
  }
  bp= p2= p;   /* bp: Adr des ersten nicht mehr zu nehmenden Zeichens */
  /*   Leerzeichen streichen */
  while (--p2 >= fcb.bpos)
  { if (((*(--p)= *p2) & fcb.bytemask) == ' ')
    { if (p[1]== ' ') ++p;
      else            *p= ' ';
    }
  }
  fcb.bpos= p;
  while (fcb.bpos<bp)
  { if (*fcb.bpos == ' ')  ++fcb.bpos;
    p= fcb.bpos;
    b= -1;
    i= j= 0;
    while (l>= i)
    { if (p==bp)
      { while (fcb.bpos<bp)  { *(fcb.fpos++)= *(fcb.bpos++); }
        b= 0;
        break;
      }
      if (*p == ' ')
      { b++;
        j= i;
        p2= p;
      }
      if (*p>=' ')  ++i;
      ++p;
    }
    if (b<=0)  /* Woerter zu lang */
    { while (fcb.bpos<bp && *fcb.bpos!= ' ')
      { *(fcb.fpos++)= *(fcb.bpos++);
      }
      if (*(fcb.fpos-1) == ' ')  --fcb.fpos;
    }
    else  /* Leerzeichen einfuegen */
    { i= l-j;
      while (fcb.bpos<p2)
      { if ((c=(*(fcb.fpos++)= *(fcb.bpos++))) == ' ')
        { j= i/(b--);
          i-= j;
          while (j)
          { --j;
            *(fcb.fpos++)= ' ';
          }
        }
      }
    }
    l= fcb.linelen;
    if (lfcr) *(fcb.fpos++)= '\r';
    *(fcb.fpos++)= '\n';
    ++lndiff;
    ++fcb.linecnt;
  }
}

