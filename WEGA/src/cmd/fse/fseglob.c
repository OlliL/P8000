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




void options(opt)
char far *opt;
{ char c;

  if (opt != (char *)0)
  { while (c= *(opt++))
    { switch (c)  {

      case 'a': margin  ^= 66; break;
      case 'm': longmem ^= 1;  break;
      case 'y': yz      ^= 1;  break;
      case 'r': restart ^= 1;  break;
      case 'b': backup  ^= 1;  break;
      case 'n': lfcr    ^= 1;  break;
      case 'c': copyr   ^= 1;  break;
      case 'e': errfile ^= 1;  break;
      case 'k': metakey ^= 1;  break;
      case 'M': longmem  = 2;  break;
      case 'Y': yz       = 2;  break;
      case 'R': restart  = 2;  break;
      case 'B': backup   = 2;  break;
      case 'N': lfcr     = 2;  break;
      case 'C': copyr    = 2;  break;
      case 'E': errfile  = 2;  break;
      case 'K': metakey  = 2;  break;
      default : if (c>='0' && c<='9') strtline= strtline*10 +(c-'0');
      }
    }
  }
}




/* im WEGA existiert keine Standardfunktion 'memcpy' */

memcpy(destin,source,length)
register byte far *destin;
register byte far *source;
register integer length;
{
   if  (source<destin)
   { source+= length;
     destin+= length;
     while (length--) *(--destin)= *(--source);
   }
   else  while (length--)  *(destin++)= *(source++);
}



void shift(nbyte)      /* Achtung : Ein Aufruf mit nbyte=0 ist statthaft und */
long nbyte;            /*           fuehrt zum erwarteten Resultat !!!       */
{ register byte far *source;
  register byte far *destin;
  register integer cnt;
  register byte    b;
  register byte    bytemask;

  bytemask= fcb.bytemask;
  if (nbyte != 0)
  { if (nbyte>0)            /* Kursor in Richtung Dateiende verschieben */
    { source= fcb.bpos;
      destin= fcb.fpos;
      cnt   = (integer) nbyte;
      if (source<=fcb.blkbegin  &&  fcb.blkbegin<source+cnt) fcb.blkbegin -= (integer)(source-destin);
      if (source<=fcb.blkend    &&  fcb.blkend  <source+cnt) fcb.blkend   -= (integer)(source-destin);
      do
      { if ( (b= ((*(destin++)= *(source++)) & bytemask))>'\f'  ||  b<'\n'  ||  b==CNTRL('K')) continue;
        ++lndiff;
        if (b=='\n')  ++fcb.linecnt;
      } while (--cnt);
    }
    else                   /* Kursor in Richtung Dateianfang verschieben */
    { source= fcb.fpos;
      destin= fcb.bpos;
      cnt   = (integer) -nbyte;
      if (source-cnt<fcb.blkbegin  &&  fcb.blkbegin<=source) fcb.blkbegin += (integer)(destin-source);
      if (source-cnt<fcb.blkend    &&  fcb.blkend  <=source) fcb.blkend   += (integer)(destin-source);
      do
      { if ( (b=((*(--destin)= *(--source)) & bytemask))>'\f'  ||  b<'\n'  ||  b==CNTRL('K')) continue;
        --lndiff;
        if (b=='\n')  --fcb.linecnt;
      } while (--cnt);
    }
    fcb.fpos +=nbyte;
    fcb.bpos +=nbyte;
  }
}


void helpln(helpline)
register byte far *helpline;
{ register byte b;
  register int  i;

  i= 1;
  while ((b= *(helpline++)) != 0)
  { if (b> (byte)'\200')
    { b-= (byte)'\177';
      while (--b)
      { ttych(' ');
        ++i;
      }
    }
    else
    { if (b<=(byte)'\032')
      { ttych('^');
        b+= '@';
        ++i;
      }
      ttych( (char)b);
      ++i;
    }
  }
  if (parm._vmod==1)   /* VENIX */
  { while (++i<=TTYCOLUM) ttych(' ');
    ttystr(parm._nvideo);
  }
  else  clreol();
}


short letter(b)
byte b;
{
  b &= fcb.bytemask;
  return ( (b>='A'  &&  b<='Z')  ||
           (b>='a'  &&  b<='z')  ||
           (b>='0'  &&  b<='9')  ||
           (b=='_'            )  );
}

short letter1(b)
byte b;
{
  b &= fcb.bytemask;
  return ( (b>' '  &&  b<=127) );
}

void makewind()
{ short  i,y;

  if (Qwindow==0)
  { y= fcb.minY+1;
    gotoXY(10,y);
    invers();    i= 44; do ttych(' '); while (--i);     nvideo();
    while (++y <= fcb.minY+5)
    { gotoXY(10,y);
      invers();     ttych(' '); nvideo();
      i= 40;     do ttych(' '); while (--i);
      invers();     ttych(' '); nvideo();
    }
    gotoXY(10,y);
    invers();
    i=44;
    do ttych(' '); while (--i);
    nvideo();
    y= fcb.minY+1;
    do  _display[y]= TRUE;  while(++y<=fcb.minY+6);
  }
}


void putwind(message)
char far *message;
{ short i;
  
  makewind();
  gotoXY(13,fcb.minY+1 + ++Qwindow);
  i= 0;
  while (message[i])  ttych(message[i++]);
  Qcol=i;
  while (++i<=40)  ttych(' ');
}


void gotoWIND()
{ gotoXY(13+Qcol,fcb.minY+1+Qwindow);
}


void getstr(answer,length)
byte far *answer;
short   length;
{ short    i;
  byte     c;
  byte far *p;

  i=0; p=answer;
  length--;             /* Platz fuer abschliessendes '\000' sicherstellen */
  while ((c=getkey()) != RETURN)
  { if ((c==LEFT || c==DELCHRL) && i>0)
    { if (*(--p) < '\040')
      { --i; backstep(); ttych(' '); backstep();
      }
      --i;
      backstep(); ttych(' '); backstep();
    }
    else
    if (c==LINESTRT  ||  c==DOWN)
    { while (i>0)
      { backstep();  ttych(' '); backstep();
        --i;
      }
      p= answer;
    }
    else
    if ((i<length && c>='\040' && c<OPCODE)|| c==ADDCH)
    { if (c == ADDCH) { if ((c=getc())==0) continue; }
      *(p++)= c;
      ++i;
      if (c<'\040')
      { ttych('^'); c+= '@'; ++i;
      }
      ttych(c);
    }
    else bell();
  }
  *p= '\000';
}



void ask(answer,length)
byte far *answer;
int      length;
{
   gotoWIND();
   getstr(answer,length);
}



void shortask(question,answer)
byte far *question;
byte far *answer;
{
  gotoXY(50,fcb.minY-1);
  if (parm._vmod==1)  /* VENIX */  ttystr(parm._video);
  ttystr(question);
  getstr(answer,10);
  gotoXY(50,fcb.minY-1);
  while (*question)
  { ++question;
    ttych(' ');
  }
  while (*answer)
  { ++answer;
    ttych(' ');
  }
  if (parm._vmod==1 /* VENIX */) ttystr(parm._video);
}



void display()      /* bewirkt, dass der Bildschirm voellig neu */
{ short y;          /* ausgeschrieben wird                      */

  _inscnt= _delcnt= 0;
  y= fcb.minY;
  do
  { _display[y]= TRUE;
  } while (++y <= fcb.maxY);
}



void scrollup(cnt)              /* Bildschirm in der Tabelle 'hochrollen' */
short cnt;
{ short source,destin;

  destin= fcb.minY;
  source= fcb.minY +cnt;
  do
  { _display[destin++]= _display[source++];
  } while (source <= fcb.maxY);
  do
  { _display[destin]= FALSE;
  } while (++destin <= fcb.maxY);
}


void scrolldw(cnt)       /* Bildschirm in der Tabelle 'herunterrollen' */
short cnt;
{ short source,destin;

  destin= fcb.maxY;
  source= fcb.maxY -cnt;
  do
  { _display[destin--]= _display[source--];
  } while (source >= fcb.minY);
  do
  { _display[destin]= FALSE;
  } while (--destin >= fcb.minY);
}


void error(nr)
int nr;
{
  switch (nr)
  { case E_SYSTEM:
         switch (errno)
         { case EPERM  : putwind("no owner");             break;
           case ENOENT : putwind("file not found");       break;
           case EIO    : putwind("fatal I/O-error");      break;
           case EACCES : putwind("permission denied");    break;
           case ENODEV : putwind("no such devise");       break;
           case ENOTDIR: putwind("not a directory");      break;
           case EISDIR : putwind("is a directory");       break;
           case ENOSPC : putwind("no space on device");   break;
           case EROFS  : putwind("read-only filesystem"); break;
           default     : putwind("unknown error");
         }
         break;
    case E_TOOBIG:       putwind("file too big, memoryoverflow");
                         break;
    case E_BADFND:       putwind("string not found");
  }
  gotoXY(20,fcb.minY+6);  bell();
  ttystr("Press <ET> to continue: ");
  while (getkey()!=RETURN) bell();
  gotoXY(20,fcb.minY+6);
  nr=24;  do ttych(' '); while (--nr);
  Qwindow= 0;
}


short getfname(filename)
byte far *filename;
{ char   fname[FNAMELEN];
  byte far *p;
  
  putwind("Please enter Filename !");
  putwind("File: ");
  ask(fname,FNAMELEN);
  if (*fname)
  { memcpy(filename,fname,(integer)sizeof(fname));
    p= filename;
    while (*p)
    { if (*p==' ') *p= '_';
      ++p;
    }
  }
  return( (short) *fname);
}


void delspace()
{ register byte far *oldp;
  register byte far *newp;
  register byte   b;
  register byte   bytemask;
  short           modifyed;

  bytemask= fcb.bytemask;
  oldp=newp= fcb.memend;
  modifyed= FALSE;
  while (oldp >= fcb.membegin)
  { b= (*newp= *oldp) & bytemask;
    if (oldp == fcb.blkbegin) fcb.blkbegin= newp;
    if (oldp == fcb.blkend  ) fcb.blkend  = newp;
    if (oldp == fcb.bpos    ) oldp= fcb.fpos;
    --oldp;
    --newp;
    if (b=='\n'  ||  b=='\f')
    { while ((b= *oldp & bytemask)==' '  ||  b==CNTRL('I'))
      { if (oldp == fcb.blkbegin) fcb.blkbegin= newp;
        if (oldp == fcb.blkend  ) fcb.blkend  = newp;
        if (oldp == fcb.bpos    ) oldp= fcb.fpos;
        --oldp;
        modifyed= TRUE;
      }
    }
  }
  fcb.bpos= newp+1;
  fcb.fpos= fcb.membegin;
  fcb.linecnt= 1;
  fcb.oldmode= ~fcb.mode;  /* sichert Neuaufbau des Bildschirms */
  if (modifyed) fcb.modifyed= TRUE;
}


short save(filename,buffstrt,buffend)
byte far *filename;
byte far *buffstrt;
byte far *buffend;
{
  short    work_ok;
  byte far *begin, *end;
  integer  nbyte;
  int      destin;
  char     backname[FNAMELEN+5];
  struct   stat statbuff;
  int      savemode;
  
  putwind("Saving File: ");
  gotoWIND(); ttystr(filename);
  gotoXY(0,0); ttyflush();
  work_ok= TRUE;
  if (klau) return;
  if (stat(filename,&statbuff)== -1)  savemode= 0666;
  else                                savemode= statbuff.st_mode & 0777;
  if (backup)
  { nbyte=0;
    while (filename[nbyte] && nbyte<FNAMELEN-1)
    { backname[nbyte]= filename[nbyte];
      ++nbyte;
    }
    backname[nbyte]= '~';
    backname[nbyte+1]= 0;
    unlink(backname);
    link(filename,backname);
    unlink(filename);
  }
  errno= 0;
  if ((destin= creat(filename, savemode)) <0)
  { work_ok= FALSE; error(E_SYSTEM);
  }
  else
  { do
    { if (buffstrt == fcb.fpos) buffstrt= fcb.bpos;
      if (buffend  == fcb.bpos) buffend = fcb.fpos;
      begin= buffstrt;
      end  = (buffstrt<fcb.fpos  &&  fcb.fpos<=buffend)? fcb.fpos : buffend;
      if (end > begin)
      { nbyte=(integer)(end-begin);
        if (write(destin,begin,nbyte) != nbyte  ||  errno!=0)
        { work_ok= FALSE;
          error(E_SYSTEM);
          break;
        }
      }
      buffstrt= fcb.bpos;
    } while (end != buffend);
    close(destin);
  }
  Qwindow= 0;
  return(work_ok);
}



short savefile()
{ short result;

  result= FALSE;
  if (save(fcb.fname,fcb.membegin,fcb.memend))
  { result= TRUE;
    fcb.modifyed= FALSE;
  }
  return(result);
}



short load(filename)
byte far *filename;
{ long     length;
  int      source;
  short    work_ok;
  byte far *pp;

  work_ok= TRUE;
  if (copyr)
  { putwind("Loading File ");
    gotoWIND(); ttystr(filename);
    ttyflush();
  }
  copyr= TRUE;
  errno=0;
  source= openfile;
  openfile= 0;
  if (!source &&  (source= open(filename,O_RDONLY))<0 )
  { if (errno != ENOENT) work_ok= FALSE;
    error(E_SYSTEM);
  }
  else      /*  Hinweis: lseek(fd,0,2) positioniert Filepointer auf Dateiende */
            /*  und gib dessen Wert (also Dateilaenge) als long zurueck       */
  { length= lseek(source,(long)0,2);
    if ( length >= (long) ((integer)(fcb.bpos-fcb.fpos)) )
    { error(E_TOOBIG); work_ok= FALSE;
    }
    else
    { lseek(source,(long)0,0);  /* Dateipointer wieder auf Dateianfang setzen */
      if ((integer)length != read(source,fcb.bpos-length, (integer)length)  ||  errno!=0)
      { error(E_SYSTEM);
        work_ok= FALSE;
      }
      else
      { pp= fcb.bpos-1;
	fcb.bpos -=length;
        fcb.modifyed= TRUE;
      }
    }
    close(source);
  }
  return(work_ok);
}


void loadfile()
{ int i;
  
  gotoXY(0,fcb.minY-1);
  invers();
  if (parm._vmod==1)  /* VENIX */            /* fuer VENIX  (besondere Behandlung  */
  { gotoXY(1,fcb.minY-1);                     /*           der Bildschirmattribute) */
    ttystr("File: ");
    for (i=0;++i<TTYCOLUMN-17;ttych(' '));
    ttystr("Line:      ");
    gotoXY(7,fcb.minY-1);
    ttystr(fcb.fname);
    ttystr(parm._nvideo);
  }
  else
  { clreol();                                 /* fuer WEGA/XENIX */
    gotoXY(1,fcb.minY-1);
    ttystr("File: ");
    ttystr(fcb.fname);
    gotoXY(TTYCOLUM-11,fcb.minY-1);
    ttystr("Line:");
  }
  fcb.bytemask= parm.bytemask;
  fcb.offset  = 0;
  fcb.linecnt = 1;
  fcb.oldlncnt= 0;          /* Nichtuebereinstimmung von linecnt und oldlncnt bewirkt */
                            /* das physische Ausschreiben der Zeilennummer            */
  fcb.blkbegin= fcb.blkend  = (byte far *) 0;
  fcb.cntrmode= parm.cntrmode;
  fcb.dmpascii= parm.dmpascii;
  fcb.insert  = parm.insert;
  fcb.autotab = parm.autotab;
  fcb.oldfpos = fcb.oldbpos= (byte far *)0;
  fcb.bpos    = fcb.memend;
  fcb.fpos    = fcb.membegin;
  fcb.mrkln   = 0;
  fcb.linelen = margin;
  *fcb.memend      = '\n';
  *(fcb.membegin-1)= '\n';
  if (load(fcb.fname))
  { if  (fcb.modifyed == -1)    fcb.modifyed= TRUE;
    else fcb.modifyed= FALSE;
    fcb.mode   = (*fcb.bpos > '\177')? DUMPMODE : TEXTMODE;
    fcb.oldmode= fcb.mode+1;
    if (errfile)    /* Fehlerbehandlung kann nur im ersten File erfolgen */
    { errfile= 0;
      fcb.errfile= 1;
    }
    else fcb.errfile= 0;
    helpmode= 30;    /* Helpzeile wird garantiert aktualisiert */
    /* Das Nichtuebereinstimmen von mode und oldmode bewirkt:
     * - die Neuersputwindung des Bildschirms
     * - die Wertzuweisung an fcb. pagestrt/linestrt/y/dumpstrt
     * - die physische Aktualisierung eds Bildschirms
     */
  }
  else delwind();
}



short reject()
{ short result;
  char  c;

  result= 1;
  if (fcb.modifyed)
  { putwind("Your Workfile has been modified");
    putwind("Save before quit ?  (y/n/c) ");
    result= 2; /* also weder TRUE noch FALSE */
    do
    { gotoXY(0,0);
      if ((c= getletter()) == 'C') result= FALSE;
      else
      if (c=='Y'  ||  c=='J') result= savefile();
      else
      if (c=='N') result= TRUE;
      else bell();
    } while (result == 2);
  }
  Qwindow= 0;
  return(result);
}



void delwind()
{ long  diff;
  int   i,j;

  if (metakey  ||  (*fcb2.fname==0  &&  restart))
  { i= creat(fsememory,0666);
    if (i>0)
    { memcpy(kbd.fname, fcb.fname, sizeof (fcb.fname));
      kbd.longmem= longmem;
      kbd.strtline= fcb.linecnt;
      j= (metakey)? sizeof(kbd) : sizeof(kbd)-sizeof(kbd.k);
      write(i,&kbd,j);
      close(i);
    }
  }
  memcpy(scratch,&fcb,(integer)sizeof(fcb));
  memcpy(&fcb,&fcb2,(integer)sizeof(fcb));
  memcpy(&fcb2,scratch,(integer)sizeof(fcb));
  *fcb2.fname= '\000';
  if (*fcb.fname == '\000')  fcb.mode= QUITMODE;
  else
  { if (fcb.minY!=1)
    { gotoXY(7,0);
      if (parm._vmod==1 /* VENIX */)
      { ttystr(parm._video);
        ttystr(fcb.fname);
        i= 30;
        while (--i) ttych(' ');
        ttystr(parm._nvideo);
      }
      else
      { ttystr(fcb.fname);
        i= 30;
        while (--i) ttych(' ');
      }
      fcb.oldlncnt= 0;
    }
    fcb.minY= 1;
    fcb.maxY= TTYLINES-2;
    fcb.oldmode= (fcb.mode==TEXTMODE)? DUMPMODE : TEXTMODE;
    /* Das Nichtuebereinstimmen von oldmode und mode bewirkt:
     *   - die Neuerstellung des Bildschirminhaltes
     *   - die Neuermittlung von fcb. pagestrt/linestrt/y/dumpstrt
     *   - die physische Aktualisierung des Bildschirminhaltes
     */
    if ( !longmem)
    { if (fcb.membegin == buff+1)
      { diff= fcb.memend-fcb.bpos;  /* Anzahl der Zeichen im hinteren Pufferbereich */
        memcpy(buff+BUFFSIZE-1-diff,fcb.memend-diff,(integer)diff);
        diff= (long) ((integer)(buff+BUFFSIZE-1 -fcb.memend));
        if (fcb.blkbegin >= fcb.bpos) fcb.blkbegin += diff;
        if (fcb.blkend   >= fcb.bpos) fcb.blkend   += diff;
        fcb.bpos += diff;
        fcb.memend= buff+BUFFSIZE-1;
      }
      else
      { memcpy(buff+1,fcb.membegin,(integer)(fcb.fpos-fcb.membegin) );
        diff= (long) ((integer)(fcb.membegin-(buff+1)));
        if (fcb.blkbegin>=fcb.membegin  &&  fcb.blkbegin<fcb.fpos) fcb.blkbegin -=diff;
        if (fcb.blkend  >=fcb.membegin  &&  fcb.blkend  <fcb.fpos) fcb.blkend   -=diff;  
        fcb.fpos -= diff;
        fcb.membegin= buff+1;
      }
    }
  }
}


void dline(ln, cnt)    /*  ln: Nummer der ersten geloeschen Zeile */
long ln;               /* cnt: Anzahl der geloeschten Zeilen      */
int cnt;
{ int i,j,k;

  if ( !fcb.errfile) return;
  i= 0;   /* i: erste geloeschte Zeile     */
  while (err[i].line<ln)  if (++i==MAXERR) return;
  j= 0;   /* j: erste noch bleibende Zeile */
  while (err[j].line  &&  err[j].line<ln+cnt  &&  ++j<MAXERR);
  k= j;
  while (k<MAXERR  &&  err[k].line)
    err[k++].line -= cnt;
  if ((k= MAXERR-j) > 0)
  { memcpy( /* dest   */ err+i,
            /* source */ err+j,
            /* cnt    */ k*sizeof(err[0]));
    i += j;
    while (i<MAXERR) err[i++].line= 0;
  }
  if (errline != -1)
  { errline = -1;  /* neue Helpzeile erzwingen */
    helpmode= -1;
  }
}


void aline(ln,cnt)
long  ln;
int   cnt;
{ int i;

  if (fcb.errfile)
  { i= MAXERR;
    while (--i>=0)
    { if (err[i].line >= ln)  err[i].line += cnt;
    }
    if (errline != -1)
    { errline = -1;  /* neue Helpzeile erzwingen */
      helpmode= -1;
    }
  }
}
  
void clrerr()
{ int i;

  if (fcb.errfile)
  { if (errline != -1)
    { errline = -1;   /* neue Helpzeile erzwingen */
      helpmode= -1;
    }
    i= MAXERR;
    while (--i>0) err[i].line= 0;
  }
}

