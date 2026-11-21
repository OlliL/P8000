/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 2.0     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Termcap       */
/*                                                   */
/*       Stand: 16.01.89                             */
/*                                                   */
/*****************************************************/

#include "fse.h"
#include "fseextern.c"


/*
 * Das Hauptprogramm
 * *****************
 */

main(argc,argv)
int  argc;
char **argv;
{
  long              kurspos,cnt;
  int               mode,pf,pagelen;
  int	            i,j,col,linecnt,z,s, rcode;
  byte              b;
  byte     far      *newbpos;
  unsigned char far *ap, *g, *p, *pp, c;
  static   char     termerr[]= "error in termcap\n";
  static   char     errmsg[] = "error in memory\n";
  

  /* Entschluesselung der parm -Structur */
  g= (unsigned char *)(parm._gotoXY);
  i= parm.mask ^ 0xe5;
  j= parm.incr ^ 0xe5;
  s= sizeof(parm) -sizeof(parm.mask) -sizeof(parm.incr);
  while (s--)
  { *(g++) ^= i;
    i= (i+j) % 256;
  }


  /* minimalen Arbeitsspeicher anfordern */
  buff= (byte far *)malloc(BUFFSIZE);
  if (buff == (byte far *)0)
  { write(1,errmsg,sizeof(errmsg)-1);
    exit(1);
  }
  fcb.membegin= buff+1;
  fcb.memend  = fcb.membegin+ (BUFFSIZE-2);

#include "fsetermcap.c"     /* notfalls die Termcap einlesen */

  klau= 2;   /* Kopierschutz einschalten */

  ttyinit();

  /* Standardwerte fuer Optionen */
  strtline= 0;
  longmem = FALSE;
  lfcr    = FALSE;
  backup  = TRUE;
  copyr   = TRUE;
  yz      = FALSE;
  restart = TRUE;
  metakey = FALSE;
  errfile = 0;
  options(getenv("FSE"));
  ap= (unsigned char far *) *(++argv);
  if (ap!=(unsigned char far *)0  &&  *ap=='-')
  { options(ap+1);
    ap= (unsigned char far *) *(++argv);
  }

  if (errfile)
  { errfile= open(ERRFILE,O_RDONLY);
    if (errfile>0)
    { errpos= 0;
      while (errpos<MAXERR)
      { err[errpos].line=
        err[errpos].column=
        err[errpos].text[0]= 0;   /* zur Sicherheit */
        ++errpos;
      }
      errpos= (read(errfile,err,sizeof(err))>=sizeof(err[0]));
      close(errfile);
      unlink(ERRFILE);
      errfile= errpos;
      copyr= FALSE;
    }
    else errfile= 0;
  }
  errline= errpos= -1;
  openfile= 0;
  if ( metakey  ||
       (ap==(unsigned char far *)0   &&  restart) ) /* wenn Filename spezifiziert kein Restart */
  { s= open(fsememory,O_RDONLY);
    if (s>0)
    { j= (metakey)? sizeof(kbd): sizeof(kbd)-sizeof(kbd.k);
      read(s,&kbd,j);
      close(s);
      if (restart && ap==(unsigned char far *)0)
      { openfile= open(kbd.fname, O_RDONLY);
        if (openfile > 0)
        { if (strtline==0) strtline= kbd.strtline;
          ap     = (unsigned char far *)kbd.fname;
          longmem= kbd.longmem;
        }
        else openfile= 0;
      }
    }
  }
 
  /*  Eroeffnungsausschrift */
  Qwindow = 0;
  fcb.minY= 1;
  fcb.maxY= TTYLINES-2;
  if (copyr)
  { putwind("   << FullScreenEditor  V2.0  >>");
    putwind("   Autor:     Jens-Peter Redlich");
    gotoXY(10,TTYLINES-7);
    ttystr("Hit ET to continue: ");
    ttyflush();
    getkey();
    gotoXY(10,TTYLINES-7);
    ttystr("                   ");
    gotoXY(0,TTYLINES-1);
  }
  /* eventuell angegebenen Filenamen uebernehmen  */

  Qwindow= 0;
  if (*ap == 0)    ap= (unsigned char far *)0;
  if (ap != (unsigned char far *)0)
  { memcpy((byte far *)fcb.fname, (byte far *)ap,(integer)sizeof(fcb.fname)-1);
    fcb.fname[FNAMELEN-1]= '\000'; /* zur Sicherheit */
  }
  else
  { if ( !getfname(fcb.fname)) goto ed_exit;
  }
  *muster= 0;
  *repl  = 0;
  if (longmem)
  { fcb2.membegin= (byte far *)malloc(BUFFSIZE)+1;
    if (fcb2.membegin == (byte far *)0)
    { static char errmsg[]= "error in extention memory\n";
      write(1,errmsg,sizeof(errmsg)-1);
      ttydrop();
      exit(1);
    }
    fcb2.memend  = fcb2.membegin+ (BUFFSIZE-2);
  }
  *fcb2.fname= '\000';
  loadfile();
  if (fcb.mode==TEXTMODE)
  { if (fcb.errfile && err[0].line)  gotoLine((long)err[0].line);
    else
    if (strtline) gotoLine(strtline);
  }

  while (fcb.mode != QUITMODE)
  { Qwindow= 0;
    if (fcb.errfile && fcb.mode==TEXTMODE)
    { if (errpos<0 || err[errpos].line!=fcb.linecnt)
      { errpos= 0;
        while (errpos<MAXERR && err[errpos].line != fcb.linecnt) ++errpos;
      }
      if (errpos<MAXERR)
      { if (errline != errpos)
        { gotoXY(0,TTYLINES-1);
          invers();
          ap= (unsigned char far *)err[errpos].text;
          i= TTYCOLUM-1;
          while (--i)
          { if (*ap)  ttych(*(ap++));
            else      ttych(' ');
          }
          nvideo();
          errline= errpos;
        }
        goto nohlpln;
      }
      else  errpos= -1;
    }

    if (errline>=0)
    { errline= -1;
      helpmode= 20;
    }

    if (helpmode != fcb.mode)
    { gotoXY(0,TTYLINES-1);
      invers();
      if (fcb.mode==TEXTMODE)
      { if (fcb.insert) helpln(txtihelp);
        else            helpln(txtohelp);
      }
      else              helpln(dmphelp);
      if (parm._vmod==1 /* VENIX */)    ttystr(parm._nvideo);
      helpmode= fcb.mode;
    }

    nohlpln:
    if (fcb.mode != fcb.oldmode)
    { if ((fcb.oldmode= fcb.mode) == TEXTMODE) newscr();
      else fcb.dumpstrt= (long) ((integer)(fcb.fpos-fcb.membegin) & ~0x007f);
      display();
    }
    if (fcb.oldlncnt != fcb.linecnt)
    { fcb.oldlncnt= fcb.linecnt;
      gotoXY(TTYCOLUM-6,fcb.minY-1);
      decout(fcb.linecnt);
    }

    if (fcb.mode == TEXTMODE)  buildscr();
    else
    { linecnt= fcb.maxY -fcb.minY;
      pagelen= linecnt *16;
      kurspos= (long) ((integer)(fcb.fpos-fcb.membegin));
      if ((cnt= kurspos -fcb.dumpstrt)<0) cnt -=15;
      if ( (cnt/=16) < 0)    /* Bild rollt nach unten */
      { if (cnt < -M_SCROLL)
        { display();
          fcb.dumpstrt= kurspos & ~0x000f;
        }
        else
        { scrolldw( -(short)cnt) ;
          do
          { delins(fcb.maxY /* del */, fcb.minY /* ins */);
            dumpline(fcb.dumpstrt -=16);
          } while (++cnt < 0);
        }
      }
      else                   /* Bild rollt nach oben  */
      if ((cnt -=linecnt) > 0)
      { if (cnt > M_SCROLL)
        { display();
          fcb.dumpstrt= (kurspos & ~0x000f) -pagelen;
        }
        else
        { scrollup( (short) cnt);
          do
          { delins( fcb.minY /* del */, fcb.maxY /* ins */);
            dumpline( (fcb.dumpstrt +=16) +pagelen);
          } while ( --cnt > 0);
        }
      }
      Y  = fcb.minY +(kurspos -fcb.dumpstrt)/16;
      col= ( (short) (kurspos -fcb.dumpstrt) ) & 0x0f;
      /* beachte offset 1 bei Bildschirmausgabe !!! */
      X= (fcb.dmpascii)? 59+col : 7+col+col+col;
      dumppage();
    }
    gotoXY(X+1,Y);  /* die 1. Spalte ist fuer Steuerzeichen freizuhalten */
    errno=  0;
    lndiff= 0;
    _insy= _dely= 0;

    switch  ((b=getkey()))
    { case DELSPACE: delspace(); break;

      case FILECSAV: delspace();

      case FILESAVE: if (fcb.modifyed) savefile();
                     break;

      case FILECQUT: delspace();

      case FILEQUIT: if (fcb.modifyed && !savefile()) break;
                     delwind();
                     rcode= 0;
                     break;

      case FILEREJC: if (reject()) delwind();
                     rcode= 0;
                     break;

      case COMPILE:  fcb.mode= QUITMODE;
                     rcode= 2;
                     break;

      case SAVCLOAD: delspace();

      case SAVELOAD: if (!fcb.modifyed  ||  savefile())
                     { if (getfname(fcb.fname)) loadfile();
                     } break;

      case REJCLOAD: if (reject())
                     { if (getfname(fcb.fname)) loadfile();
                     } break;

      case LEARN:    putwind(" MetaKeyDefinition (^D= end)");
                     putwind(" key: "); gotoWIND();
                     if ((c=getc())>='a' && c<='z') c-= 'a'-'A';
                     if (c<'A' || c>'Z')
                     { bell();
                       break;
                     }
                     ttych(c);
                     putwind(" : ");  gotoWIND();
                     i= 0; ap= kbd.k[c-'A'];
                     while ((c= getc())!=CNTRL('D'))
                     { if (c==CNTRL('P')) c=getc();
                       if (i>=14) bell();
                       else
                       { *(ap++)= c;
                         i++;
                         if (c<32)
                         { ttych('^');
                           c+= '@';
                         }
                         ttych(c);
                       }
                     }
                     *ap= 0;
                     break;

     case LEARNHLP: i= 0;
                     do
                     { gotoXY(0,i+fcb.minY);
                       _display[fcb.minY+i]= TRUE;
                       clreol();
                     } while (++i<10);
                     gotoXY(30,fcb.minY); ttystr(">> MetaKey's <<");
                     i= 0;
                     do
                     { gotoXY( (i%3)*(TTYCOLUM/3), fcb.minY+1+i/3);
                       ttych('a'+i);  ttystr(": ");
                       ap= kbd.k[i];
                       j= 0;
                       while ((c= *(ap++)) && j<(TTYCOLUM/3-6))
                       { if (c<32)
                         { ttych('^');
                           c+= '@';
                           ++j;
                         }
                         ttych(c);
                         ++j;
                       }
                       if (c) ttych('-');
                     } while (++i < 26);
                     gotoXY(0,0);
                     getc();
                     break;

      case MRKLN  :  fcb.mrkln= fcb.linecnt;
                     break;

      case BLKKB  :  if (fcb.blkend==(byte *)0) fcb.blkend  = fcb.bpos;
                     else                       fcb.blkbegin= fcb.bpos;
                     goto blkm01;

      case BLKKK  :  fcb.blkend= fcb.bpos;
                     blkm01:
                     if (fcb.blkend<fcb.blkbegin)
                     { newbpos      = fcb.blkbegin;
                       fcb.blkbegin = fcb.blkend;
                       fcb.blkend   = newbpos;
                     }
                     if (fcb.blkbegin==fcb.blkend)
                     { fcb.blkbegin= fcb.blkend= (byte *) 0;
                     }
                     if (fcb.blkend   != (byte *)0  &&
                         fcb.blkbegin == (byte *)0  )
                     { newbpos      = fcb.blkbegin;  /* Wenn Block unvollst. definiert, */
                       fcb.blkbegin = fcb.blkend;    /* dann blkend==0, blkbegin= Marke */
                       fcb.blkend   = newbpos;
                     }
                     else display();
                     break;

      case BLKNOMRK: if (fcb.blkend!=(byte *)0) display();
                     fcb.blkbegin= fcb.blkend= (byte far *)0;
                     break;

      case BLKWRITE: if (fcb.blkend == (byte far *)0) break;
                     if ( !getfname(scratch)) break;
                     save(scratch,fcb.blkbegin,fcb.blkend);
                     break;

      case CNGFNAME: if (getfname(fcb.fname))
                     { fcb.modifyed= TRUE;
                       gotoXY(7,fcb.minY-1);
                       if (parm._vmod /* VENIX */) ttystr(parm._video);
                       ttystr(fcb.fname);
                       i= 30;
                       while (--i) ttych(' ');
                       if (parm._vmod /* VENIX */) ttystr(parm._nvideo);
                     }
                     break;
      case CHMOD   : putwind("<Changing Filemode> use * to abort");
                     putwind("File:");  ask(scratch,FNAMELEN);
                     putwind("Mode:");
                     gotoWIND();
                     i=3; mode=0;
                     while (i)
                     { if ((b=getkey())=='*') goto chmod_1;
                       if ('0'<=b  &&  b<='7')
                       { mode <<=3;
                         mode += (b-'0');
                         i--;
                         ttych((char)b);
                       }
                       else bell();
                     }
                     if (chmod(scratch,mode)!=0) error(E_SYSTEM);
                     chmod_1:
                     break;
      case VIEWDIR : { struct direct directory[40] ;

                       pf= open(".",O_RDONLY);
                       lseek(pf,(long)32,0);
                       while ((j= read(pf,directory,640)/16) != 0)
                       { i= fcb.minY;
                         gotoXY(0,fcb.minY);
                         ttystr("      Your current directory:"); clreol();
                         _display[fcb.minY]= TRUE;
                         while (i<fcb.minY+9)
                         { gotoXY(0,++i); clreol();
                           _display[i]= TRUE;
                         }
                         if (fcb.maxY < TTYLINES-2)
                         { _display[fcb.minY+10]= TRUE;
                           gotoXY(0,fcb.minY+10);
                           clreol();
                         }
                         z=fcb.minY+1;
                         s= 2;
                         i= 0;
                         while (i<j)
                         { if (directory[i].d_ino != 0)
                           { gotoXY(s,z);
                             ttystr(directory[i].d_name);
                             if ((s+=15)>=77) { s=2; z++; }
                           }
                           i++;
                         }
                         gotoXY(0,0);
                         if (getkey()==DELCHRL) break;
                       }
                     }
                     close(pf);
                     break;
      case CHDIR   : putwind("< Change Homedirectory >");
                     putwind("");
                     putwind("new Path:");
                     ask(scratch,FNAMELEN);
                     if (chdir(scratch)!=0) error(E_SYSTEM);
                     break;
      case UNLINK  : putwind("< Delete Files >");
                     putwind("");
                     putwind("File:");  ask(scratch,FNAMELEN);
                     if (unlink(scratch)!=0) error(E_SYSTEM);
                     break;
      case CHWIND  : if (*fcb2.fname=='\000')  /* wenn 2. Fenster schon existiert, ist der F'name eingetragen        */
                     { fcb.minY= TTYLINES/2+1; /* Frage nach F'name fuer neues Fenster auf unterer Bildschirmhaelfte */
                       if (getfname(scratch))
                       { memcpy(fcb2.fname,scratch,sizeof(fcb.fname));
                         fcb2.maxY= TTYLINES-2;
                         fcb2.minY= TTYLINES/2+1;
                         fcb.maxY = TTYLINES/2-1;
                         fcb.minY = 1;
                         if ( !longmem)
                         { newbpos= fcb.fpos +((integer)(fcb.bpos-fcb.fpos))/4;
                           memcpy(newbpos,fcb.bpos,(integer)(fcb.memend-fcb.bpos)+1 );
                           /* ein Zeichen mehr uebertragen, damit das abschliessende,          */
                           /* nicht mehr zur Datei gehoerende '\n' auch mit herueber kommt     */
                           if (fcb.blkbegin >= fcb.bpos) fcb.blkbegin -= (integer)(fcb.bpos-newbpos);
                           if (fcb.blkend   >= fcb.bpos) fcb.blkend   -= (integer)(fcb.bpos-newbpos);
                           fcb.memend -= (integer)(fcb.bpos-newbpos);
                           fcb.bpos= newbpos;
                           fcb2.membegin= fcb.memend+1;
                           fcb2.memend  = buff+ (BUFFSIZE-1);
                         }
                         memcpy(scratch,&fcb,(integer)sizeof(fcb));
                         memcpy(&fcb,&fcb2,(integer)sizeof(fcb));
                         memcpy(&fcb2,scratch,(integer)sizeof(fcb));
                         loadfile();
                       }
                       else
                       { fcb.minY= 1;  /* getfname hat doch keinen Namen geliefert */
                         bell();
                       }
                     }
                     else   /* 2. Fenster existiert schon, also einfach fcb's austauschen ! */
                     { memcpy(scratch,&fcb,(integer)sizeof(fcb));
                       memcpy(&fcb,&fcb2,(integer)sizeof(fcb));
                       memcpy(&fcb2,scratch,(integer)sizeof(fcb));
                     }
                     break;
      case HELP:
                     i= 0;
                     Y= fcb.minY;
                     do
                     { gotoXY(0,Y);   _display[Y]= TRUE;
                       helpln(helptext[i++]);
                       ++Y;
                       if (Y>=fcb.maxY+1 || Y==TTYLINES-1)
                       { if (Y==fcb.maxY+1 && fcb.minY!=1)   --helpmode;
                         else
                         { gotoXY(0,0);
                           if (getkey()==DELCHRL) break;
                           Y= fcb.minY;
                           if (i> 15) break;
                         }
                       }
                     } while (1);
                     break;
      case BYTEMASK: gotoXY(1,TTYLINES-1);
                     helpln("bytemask  1) 01111111  2) 11111111");
                     gotoXY(0,0);
                     while((b=getkey())!='1'  &&  b!='2') bell();
                     if ((b= (b=='1')? (byte)'\177' : (byte)'\377') != fcb.bytemask) display();
                     fcb.bytemask= b;
                     --helpmode;
                     break;
      case BACKUP  : gotoXY(1,TTYLINES-1);
                     ttystr("backup: (y/n)   current:");
                     if (backup) ttych('y'); else ttych('n');
                     clreol();
                     gotoXY(0,0);
                     while ((b=getkey())!='y' && b!='n') bell();
                     backup= (b=='y');
                     --helpmode;
                     break;
      case CNTRMODE: gotoXY(1,TTYLINES-1); clreol();
                     helpln("1) no display  2)space  3)point  4)as control");
                     gotoXY(0,0);
                     while ((b=getkey())<'1'  ||  b>'4')  bell();
                     if ((i= b-'1') != fcb.cntrmode)  display();
                     fcb.cntrmode= i;
                     --helpmode;
                     break;
      case TEXTDUMP: fcb.mode= (fcb.mode==DUMPMODE)? TEXTMODE : DUMPMODE;
                     break;

      default      : if (fcb.mode == DUMPMODE) dump_ed(b);
                     else                      text_ed(b);
    }
  }
  ed_exit:
  ttydrop();
  exit(rcode);
}
 