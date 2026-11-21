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


/*
 * Der TEXT -Editor
 * ****************
 */

void text_ed(b)
byte b;
{ long     shiftcnt,longx,longy;
  short    modifyed,found,i;
  byte     b1;
  byte far *oldbpos,
           *oldfpos;
  int      c;

  modifyed= FALSE;
  shiftcnt= 0;
  oldfpos= fcb.oldfpos;
  oldbpos= fcb.oldbpos;
  fcb.oldfpos= fcb.oldbpos= (byte far *)0;
  switch (b)
  { case  MARKWORD: 
          if ( !letter(*fcb.bpos)) break;
          if (fcb.blkend != (byte far *)0) display();
          fcb.blkend= fcb.bpos;
          while (letter(*(++fcb.blkend)));
          if (letter(*(fcb.fpos-1)))
          { fcb.blkbegin= fcb.fpos-1;
            while (letter(*(fcb.blkbegin-1))) --fcb.blkbegin;
          }
          else fcb.blkbegin= fcb.bpos;
          _display[fcb.y]= TRUE;
          break;

    case MARKLINE:
         if (fcb.blkend != (byte far *)0) display();
         if  ((p= last_ln(fcb.fpos)) == (np= next_ln(fcb.bpos)))  break;
         _display[fcb.y]  = TRUE;
         _display[fcb.y+1]= TRUE;
         fcb.blkbegin= p+1;
         fcb.blkend  = np+1;
         break;

    case LASTERR:
         if (fcb.errfile==0) break;
         if (err[errpos].line == fcb.linecnt)
         { if (errpos) --errpos;
         }
         else
         { errpos= 0;
           while (errpos < MAXERR-1     &&
                  err[errpos+1].line!=0 &&
                  err[errpos+1].line<fcb.linecnt) ++errpos;
         }
         goto errdspl;

    case NEXTERR:
         if (fcb.errfile==0) break;
         if (err[errpos].line==fcb.linecnt)
         { if (errpos<MAXERR-1) ++errpos;
         }
         else
         { errpos= MAXERR-1;
           while ( errpos>0  &&
                   (err[errpos].line==0 || err[errpos-1].line > fcb.linecnt) )
           --errpos;
         }
         errdspl:
         if (err[errpos].line==0) break;
         gotoLine((long)err[errpos].line);
         break;

    case RIGHT:
         shiftcnt= 1;
         break;

    case WRDRIGHT:
         p= fcb.bpos;
         if ((*p & fcb.bytemask) == '\n')
         { if (p==fcb.memend) break;
           ++p;
         }
         while (letter(*p)) ++p;
         while ((*p & fcb.bytemask)!='\n' &&  !letter(*p)) ++p;
         shiftcnt= (long) ((integer)(p-fcb.bpos));
         break;

    case LINEEND:
         shiftcnt= (long) ((integer)(next_ln(fcb.bpos+1) -fcb.bpos));
         break;

    case PAGEEND:
         *(p= fcb.bpos-1)= ' ';
         if ((Y= fcb.maxY-fcb.y) == 0)    Y= fcb.maxY-fcb.minY;
         do p= next_ln(p)+1;  while (--Y  &&  p<=fcb.memend);
         shiftcnt= (long) ((integer)(p-fcb.bpos));
         break;

    case FILEEND:
         shiftcnt= (long)((integer)(fcb.memend -fcb.bpos));
         break;

    case LEFT:
         shiftcnt= -1;
         break;

    case WRDLEFT:
         p= fcb.fpos-1;
         while (letter(*p)) --p;
         while (p>=fcb.membegin  &&  !letter(*p) ) --p;
         shiftcnt= -(long) ((integer)(fcb.fpos-1-p));
         break;

    case LINESTRT:
         shiftcnt= -(long) ((integer)(fcb.fpos-1 -fcb.linestrt));
         if (shiftcnt==0) shiftcnt= -1;
         break;

    case PAGESTRT:
         if ((shiftcnt= -(long)((integer)(fcb.fpos-1-fcb.pagestrt))) != 0) break;
         Y= fcb.maxY-fcb.minY;
         p= fcb.fpos;
         do if (p>=fcb.membegin) p=last_ln(p);  while (--Y);
         shiftcnt= -(long)((integer)(fcb.fpos-1-p));
         break;
     
    case FILESTRT:
         shiftcnt= -(long) ((integer)(fcb.fpos-fcb.membegin));
         break;

    case UP:
         if ((p= last_ln(fcb.fpos)) < fcb.membegin)  break;
         shiftcnt= -(long) ((integer)(fcb.fpos- atCol(last_ln(p))));
         break;

    case DOWN:
         if ((p= next_ln(fcb.bpos)) == fcb.memend)  break;
         shiftcnt= (long) ((integer)(atCol(p) -fcb.bpos));
         break;

    case SHIFTKB:
         if (fcb.blkbegin == (byte far *)0)  break;
         shiftcnt= gotoByte(fcb.blkbegin);
         break;

    case SHIFTKK:
         if (fcb.blkend == (byte far *)0)  break;
         shiftcnt= gotoByte(fcb.blkend);
         break;

    case TAB:
         i_tab();
         break;


    case POSITION:
         gotoLine((long)0);
         break;
         
    case EXCHLN:
         longx     = fcb.mrkln;
         fcb.mrkln = fcb.linecnt;
         gotoLine(longx);
         break;

    case RETURN:
         newline( (byte)'\n');
         break;

    case NEWPAGE:
         newline( (byte)'\f');
         break;

    case ADDCH:
         if ((b=getc())=='\n'  ||  b=='\f')
         { newline(b);  break; }
         if (b<'\040'  &&  (fcb.cntrmode==0 || fcb.cntrmode==3 || b=='\t'))
             c= '\000';
         else
         if (b<'\040') c= (fcb.cntrmode==1)? ' ' : '.';
         else c= (char)b;
         goto insch_1;

    default:
         if (b<'\040' || b>=OPCODE) break;
         c= (char)b;
         insch_1:
         if (fcb.fpos == fcb.bpos)
         { error(E_TOOBIG); break; }
         fcb.modifyed= TRUE;
         if (fcb.blkend==fcb.bpos) fcb.blkend= fcb.fpos;
         *(fcb.fpos++)= b;
         if ((b1= *fcb.bpos & fcb.bytemask)=='\n' || b1=='\f' || fcb.insert) b1= '\n';
         else ++fcb.bpos;
         if (fcb.linelen)
         { p= fcb.fpos;
           i= fcb.linelen+7;
           while (i && (b=(*(--p) & fcb.bytemask))!='\n' && b!='\f')
           { if (b>=' ') --i; 
           }
           if (i>0 && i<6 && wwrap())  break;
         }
         p= fcb.bpos;
         while ((b= *(p++) & fcb.bytemask) > '\f'        ||
                                        b  < '\t'        ||
                                        b == CNTRL('K') );
         if ( parm._ins_ch[0]==0   ||
              b=='\t' || c=='\000'  ||
             (b1!='\n' && b1<'\040' && (fcb.cntrmode==0 || fcb.cntrmode==3)))
         { _display[fcb.y]= TRUE; break; }
         if (b1=='\n')  insertch();
         if (parm._vmod /* VENIX */) ttystr(parm._video);
         ttych(c);
         if (parm._vmod /* VENIX */) ttystr(parm._nvideo);
         break;

    case DELCHRL:
         fcb.oldfpos= fcb.fpos;
         fcb.oldbpos= fcb.bpos;
         if (fcb.fpos==fcb.membegin) break;
         fcb.modifyed= TRUE;
         if ( (b= *(--fcb.fpos) & fcb.bytemask)=='\n'  ||  b=='\f')
         { if (b=='\n')
           { if (*(fcb.fpos-1)=='\r') --fcb.fpos;
             dline(fcb.linecnt--,1);
           }
           fcb.linestrt= last_ln(fcb.fpos);
           if (fcb.y==fcb.minY) fcb.pagestrt= fcb.linestrt;
           else
           { _delcnt= 1;
             _dely= fcb.y--;
           }
         }
         _display[fcb.y]= TRUE;
         break;

    case DELCHRR:
         delchr:
         if ((b= (*fcb.bpos & fcb.bytemask))=='\n'  ||  b=='\f')  break;
         fcb.oldfpos= fcb.fpos;
         fcb.oldbpos= fcb.bpos;
         ++fcb.bpos;
         _display[fcb.y]= TRUE;
         fcb.modifyed= TRUE;
         break;

    case DELWRD:
         fcb.oldfpos= fcb.fpos;
         fcb.oldbpos= fcb.bpos;
         if ((*fcb.bpos & fcb.bytemask)==' ')
         { while ((*fcb.bpos & fcb.bytemask)==' ')
           { modifyed= TRUE;  ++fcb.bpos;}
           goto deleol1;
         }
         if ( !letter(*fcb.bpos)) goto delchr;
         while (letter(*(fcb.fpos-1))) {--fcb.fpos; modifyed= TRUE; }
         while (letter(*(fcb.bpos  ))) {++fcb.bpos; modifyed= TRUE; }
         goto deleol1 ;

    case FORMAT:
         if ((*fcb.bpos & fcb.bytemask)== '\n') shiftcnt= 1;
         else format();
         break;

    case MARGIN:
         shortask("linelen:  ",scratch);
         fcb.linelen= 0;
         oldbpos= scratch;
         while (*oldbpos) fcb.linelen= fcb.linelen*10 + *(oldbpos++) -'0';
         if (fcb.linelen < 5)
         { bell();
           fcb.linelen= 0;
         }
         oldbpos= (byte *)0; /* zur Sicherheit auf Null setzen */
         break;

    case DELSOL:
         fcb.oldfpos= fcb.fpos;
         fcb.oldbpos= fcb.bpos;
         if ((p=last_ln(fcb.fpos)) != fcb.fpos-1) { fcb.fpos= p+1; modifyed= TRUE; }
         goto deleol1 ;

    case DELEOL:
         fcb.oldfpos= fcb.fpos;
         fcb.oldbpos= fcb.bpos;
         del0eol :
         if ((p=next_ln(fcb.bpos))!=fcb.bpos)   { fcb.bpos=p;    modifyed= TRUE; }
         deleol1 :
         if (modifyed)
         { fcb.modifyed= TRUE;
           _display[fcb.y]= TRUE;
         }
         break;

    case DELLINE:
         fcb.oldfpos= fcb.fpos;
         fcb.oldbpos= fcb.bpos;
         fcb.fpos= last_ln(fcb.fpos)+1;
         if ((fcb.bpos= next_ln(fcb.bpos)) == fcb.memend)
         { _display[fcb.y]= TRUE;
         }
         else
         { ++fcb.bpos;
           _dely= fcb.y;
           _delcnt= 1;
         }
         dline(fcb.linecnt,1);
         fcb.modifyed= TRUE;
         break;

    case WNDCOPY:
         if (*fcb2.fname == '\000') break;
         if (fcb2.blkend  ==  (byte far *)0    ) break;
         longx= (long) ((integer)(fcb2.blkend -fcb2.blkbegin));
         p= fcb.bpos;
         if (fcb2.blkbegin<fcb2.fpos  &&  fcb2.bpos <= fcb2.blkend)
         { longx-= (long)((integer)(fcb2.bpos-fcb2.fpos));
           np= fcb2.fpos;
         }
         else np= fcb2.blkend;
         if (longx >= (long)((integer)(fcb.bpos-fcb.fpos)))  {error(E_TOOBIG); break; }
         if (np==fcb2.fpos)
         { memcpy(fcb.bpos-=(integer)(fcb2.blkend -fcb2.bpos),
                   fcb2.bpos,
                   (integer)(fcb2.blkend -fcb2.bpos) );
         }
         memcpy(fcb.bpos-=(integer)(np -fcb2.blkbegin),
                 fcb2.blkbegin,
                 (integer)(np -fcb2.blkbegin) );
         goto blkcpy_1;

    case BLKREAD:
         if ( !getfname(scratch)) break;
         p= fcb.bpos;
         if (load(scratch)) goto blkcpy_1;
         break;

    case BLKMOVE:
    case BLKCOPY:
         if (fcb.blkend==(byte far *)0) break;
         if (fcb.blkbegin<fcb.fpos   &&   fcb.bpos<fcb.blkend) break;
         p= (fcb.blkend==fcb.bpos)? fcb.fpos: fcb.blkend;
         if ((longx= (long)((integer)(p -fcb.blkbegin))) >=
                     (long)((integer)(fcb.bpos -fcb.fpos)) )  {error(E_TOOBIG);  break; }
         p= fcb.bpos;
         memcpy(fcb.bpos-=longx, fcb.blkbegin, (integer)longx);
         if (fcb.blkend==p)  fcb.blkend-= longx;
         blkcpy_1:
         fcb.modifyed   = TRUE;
         _display[fcb.y]= TRUE;
         count_ln(fcb.bpos,p);
         _insy= fcb.y;
         _inscnt= nl_cnt + ff_cnt;
         aline(fcb.linecnt,(int)_inscnt);
         if (b != BLKMOVE) break;

    case BLKDEL:
         if (fcb.blkend == (byte far *)0) break;   /* longx: Adr. der 1. zu loeschenden Zeile */
         if (fcb.blkbegin >= fcb.bpos)             /* longy: Anz. der zu loeschenden Zeilen   */
         { count_ln(fcb.blkbegin,fcb.blkend);
           longy= nl_cnt+ff_cnt;
           count_ln(fcb.bpos,fcb.blkbegin);
           memcpy((byte far *)(fcb.bpos+(unsigned)(fcb.blkend-fcb.blkbegin)),
                  (byte far *)(fcb.bpos),
                  (integer)(fcb.blkbegin-fcb.bpos)  );
           fcb.bpos+= (integer)(fcb.blkend -fcb.blkbegin);
           longx= fcb.y+nl_cnt+ff_cnt-_inscnt;
         }
         else
         if (fcb.blkend < fcb.fpos)
         { count_ln(fcb.blkbegin,fcb.blkend);
           fcb.linecnt-= nl_cnt;
           longy= nl_cnt +ff_cnt;
           count_ln(fcb.blkend,fcb.fpos);
           memcpy(fcb.blkbegin,fcb.blkend,(integer)(fcb.fpos-fcb.blkend));
           fcb.fpos-= (integer)(fcb.blkend -fcb.blkbegin);
           longx= fcb.y -(nl_cnt+ff_cnt) -longy;
         }
         else  /* d.h.  (fcb.blkbegin<fcb.fpos  &&  fcb.bpos<fcb.blkend)   */
         { count_ln(fcb.blkbegin,fcb.fpos);
           longx= nl_cnt +ff_cnt;
           fcb.linecnt-= nl_cnt;
           count_ln(fcb.bpos,fcb.blkend);
           longy= longx +nl_cnt +ff_cnt;
           longx= fcb.y -longx;
           fcb.fpos= fcb.blkbegin;
           fcb.bpos= fcb.blkend;
         }

         if (fcb.blkbegin<=fcb.pagestrt  &&  fcb.pagestrt<fcb.blkend)
             fcb.pagestrt= last_ln(fcb.blkbegin);
         else
         if (fcb.blkend < fcb.pagestrt)
             fcb.pagestrt-= (integer)(fcb.blkend -fcb.blkbegin);

         if (fcb.blkbegin<=fcb.linestrt  &&  fcb.linestrt<fcb.blkend)
             fcb.linestrt= last_ln(fcb.blkbegin);
         else
         if (fcb.blkend < fcb.linestrt)
             fcb.linestrt-= (integer)(fcb.blkend -fcb.blkbegin);

         longy+= longx;           /* longx: Adr. 1. modifizierte Zeile     */
         if (longy >= fcb.minY)   /* longy: Adr. letzte modifizierte Zeile */
         { if (longx < fcb.minY) longx= fcb.minY; }
         if (longx <= fcb.maxY)
         { if (longy > fcb.maxY) longy= fcb.maxY; }
         if (longx>=fcb.minY  &&  longy<=fcb.maxY)
         { if (longx<fcb.y  &&  fcb.y<=longy) fcb.y= (short)longx;
           else
           if (longy < fcb.y) fcb.y-= (short)(longy -longx);
           _display[longy]= TRUE;
           if ((longy-=longx)>fcb.maxY-fcb.minY-2) display();
           else
           { _dely  = longx;
             _delcnt= longy;
           }
         }
         fcb.modifyed= TRUE;
         fcb.blkbegin= fcb.blkend= (byte far *)0;
         if (_delcnt!=0 && _dely+_delcnt<=fcb.maxY)
           _display[_delcnt+_dely]= TRUE;
         clrerr();
         break;

    case RETAKE:
         if (oldfpos == (byte far *)0)
         { bell(); bell(); break;
         }
         _display[fcb.y]= TRUE;
         count_ln(fcb.fpos,oldfpos);
         _insy= fcb.y+1;
         if ((_inscnt= nl_cnt +ff_cnt) != 0)
         { fcb.linestrt= fcb.fpos;
           fcb.y++;
         }
         fcb.linecnt += nl_cnt;
         count_ln(oldbpos,fcb.bpos);
         _inscnt += nl_cnt+ff_cnt;
         fcb.fpos= oldfpos;
         fcb.bpos= oldbpos;
         break;
         
    case FINDOLD:
         if (*muster)
         { i= step();
           if (*repl)  b= REPLACE;
           goto find_1;
         }

    case FIND:
         *repl= 0;
         
    case REPLACE:
         putwind("Find string: ");
         ask(muster,sizeof(muster));
         if (*muster == 0) break;
         if (b==REPLACE)
         { putwind("Replace: ");
           ask(repl,sizeof(repl));
           p= repl;
           while (*p) ++p;
           rl= p-repl;
         }
         p= muster;
         while (*p)  ++p;
         ml= p-muster;
         fw_opt  = TRUE;
         cnt_opt = 0;
         ask_opt = TRUE;
         disp_opt= FALSE;
         u_opt   = FALSE;
         ff= FALSE;
         if (b==REPLACE)
         { i=0;
           while (muster[i])
           { if (muster[i]=='\n' || muster[i]=='\f') ff= TRUE;
             ++i;
           }
           i=0; ffr=0;
           while (repl[i])
           { if (repl[i]=='\n' || repl[i]=='\f') {++ffr; ff= TRUE; }
             ++i;
           }
           putwind("Opt: (n)o ask (b)ack (d)isp (u) <num/*>");
         }
         else  putwind("Opt: (b)ackward (u)pcase");
         putwind(": ");
         ask(scratch,10);
         p= scratch;
         while ((c= *(p++)) != 0)
         { if (c=='n' || c=='N')  {ask_opt = FALSE;    continue;  }
           if (c=='b' || c=='B')  {fw_opt  = FALSE;    continue;  }
           if (c=='*'          )  {cnt_opt = BUFFSIZE; continue;  }
           if (c=='d' || c=='D')  {disp_opt= TRUE;     continue;  }
           if (c=='u' || c=='U')  {u_opt   = TRUE;     continue;  }
           if (c>='0' && c<='9')  {if (cnt_opt < BUFFSIZE) cnt_opt= cnt_opt*10 +c-'0'; }
         }
         if (cnt_opt == 0) cnt_opt= 1;
         i= TRUE;
         find_1:
         longx= cnt_opt;
         found= FALSE;
         if (u_opt)
         { p= muster;
           while (*p)
           { if (*p>='a' && *p<='z')
             *p -= 'a'-'A';
             ++p;
           }
         }
         if (b==REPLACE) Qwindow= 0;
         while (longx &&  i)
         { --longx;
           if (fw_opt)
           { if (u_opt) { if ( !findfwu()) break; }
             else       { if ( !findfw() ) break; }
           }
           else
           { if (u_opt) { if ( !findbwu()) break; }
             else       { if ( !findbw() ) break; }
           }
           found= TRUE;
           if (b != REPLACE) break;
           c= 'y';
           if (ask_opt  ||  disp_opt)
           { if (fcb.oldlncnt != fcb.linecnt)
             { fcb.oldlncnt= fcb.linecnt;
               gotoXY(TTYCOLUM-6,fcb.minY-1);
               decout(fcb.linecnt);
             }
             if (ff==TRUE) display();
             if (ff== -1) ff= TRUE;
             buildscr();
           }
           if (ask_opt)
           { gotoXY(40,fcb.minY-1);
             if (parm._vmod /* VENIX */) ttystr(parm._video);
             ttystr("replace? (y/n/c/*)");
             if (parm._vmod /* VENIX */) ttystr(parm._nvideo);
             gotoXY(X+1,Y);
             while ((c=getc())!='n'  &&  c!='j'  &&  c!='y'  &&  c!='c' && c!='*')
             { bell();
             }
             gotoXY(40,fcb.minY-1);
             if (parm._vmod /* VENIX */)  ttystr(parm._video);
             ttystr("                  ");
             if (parm._vmod /* VENIX */) ttystr(parm._nvideo);
             if (c=='*')
             { ask_opt= FALSE;
               disp_opt= FALSE;
               longx= BUFFSIZE;
               c= 'y';
             }
             if (c=='c') goto replexit;
           }
           ttyflush();
           if (c!='n')
           { if (rl>ml  &&  (integer)(rl-ml)> (integer)(fcb.bpos-fcb.fpos))
             { error(E_TOOBIG); break;
             }
             fcb.modifyed= TRUE;
             fcb.bpos+= ml;
             if (fw_opt)
             { memcpy(fcb.fpos,repl,(integer)rl);
               fcb.fpos+=rl; i= TRUE;
               lndiff+= ffr; fcb.linecnt+= ffr;
             }
             else
             { memcpy(fcb.bpos-=rl,repl,(integer)rl);
               i= step();
             }
             longy= fcb.y+ lndiff;
             if (longy>=fcb.minY  &&  longy<=fcb.maxY)
             _display[longy]= TRUE;
           }
           else
           { if (ff) ff= -1;
             i= step();
           }
         }
         if ( !found) error(E_BADFND);
         else if (ff) display();
         replexit:
         break;

    case CHEDMODE:
         fcb.insert ^= 1;  /* d.h. fcb.insert= !fcb.insert */
         --helpmode;
         break;

    case AUTOTAB:
         fcb.autotab ^=1;  /* d.h. fcb.autotab= !fcb.autotab */

  }
  if (shiftcnt > (long) ((integer)(fcb.memend-fcb.bpos)) ) /* Kursor soll um mehr Stellen in Richtung  */
      shiftcnt = (long) ((integer)(fcb.memend-fcb.bpos));  /* Dateiende gerueckt werden, als die Datei */
                                                           /* noch Zeichen hinter dem Kursor hat       */
  else
  if (shiftcnt < -((long) ((integer)(fcb.fpos-fcb.membegin))) ) /* analog in Richtung Dateianfang */
      shiftcnt = -((long) ((integer)(fcb.fpos-fcb.membegin)));
  shift(shiftcnt);
  if ((*fcb.bpos & fcb.bytemask)=='\n' && (*(fcb.fpos-1) & fcb.bytemask)=='\r')
  shift( (shiftcnt != 1)? (long)-1 : (long)1  );
  
  if (fcb.fpos<=fcb.blkbegin  && fcb.blkbegin<fcb.bpos)  fcb.blkbegin= fcb.bpos;
  if (fcb.fpos<=fcb.blkend    && fcb.blkend  <fcb.bpos)  fcb.blkend  = fcb.bpos;
  if (fcb.blkbegin==fcb.blkend  && fcb.blkend)
  { _display[fcb.y]= TRUE;
    fcb.blkbegin= fcb.blkend= (byte far *)0;
  }
}
