/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 1.1     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Steuerzeichen */
/*                                                   */
/*       Stand: 03.01.89                             */
/*                                                   */
/*****************************************************/

#include "fse.h"

char dmpbuff[33];
int  dmp_j= 0;

void dumpline(linestrt)
long linestrt;
{ register byte far *_p;
  short    i,video;
  byte     b;
  byte far *lnstrt;

  if ( (unsigned)((fcb.fpos-fcb.membegin)+(fcb.memend-fcb.bpos)) <linestrt)
  { clreol();
    return;
  }
  if ((_p= fcb.membegin+linestrt) >= fcb.fpos)  _p +=(integer)(fcb.bpos-fcb.fpos);
  lnstrt=_p;
  if (_p <= fcb.memend)
  { i= 16;
    ttych(' ');
    hex2out( (byte)(linestrt>>8)       );
    hex2out( (byte)(linestrt & 0x00ff) );
    ttystr(" |");
    video= FALSE;
    b= (fcb.blkend>_p  &&  _p>=fcb.blkbegin)? 1:0;   /*  b=0  Leerzeichen ausgeben */
    while (i && _p<fcb.memend)                       /*  b=1  invers setzen        */
    { i--;                                           /*  b=2  lvideo setzen        */
      if (_p==fcb.blkbegin  &&  fcb.blkend!=(byte *)0  &&  _p!=fcb.blkend) b=1;
      if (b==0 || parm._vmod==1 /* VENIX */)
                    { ttych(' ');                   } else
      if (b==1)     { invers(); b=0;  video= TRUE;   } else
                    { nvideo(); b=0;  video= FALSE;  }
      hex2out( *(_p++));
      if (_p==fcb.fpos) _p= fcb.bpos;
      if (_p==fcb.blkend) b=2;
    }
    if (video && parm._vmod!=1 /* != VENIX */ )  nvideo();
    else ttych(' ');
    while (i--) ttystr("   ");
    ttystr(" | >");
    i= 16;
    _p= lnstrt;
    while (i && _p<fcb.memend)
    { ttych( normch( *(_p++)) );
      --i;
      if (_p==fcb.fpos) _p= fcb.bpos;
    }
    while (i--) ttych(' ');
    ttych('<');
    }
  clreol();
}

void dumppage()
{ short y;

  y= fcb.minY;
  do
  { if (_display[y])
    { _display[y]= FALSE;
      gotoXY(0,y);
      dumpline(fcb.dumpstrt +(y-fcb.minY)*16);
    }
  } while (++y <= fcb.maxY);
}

long findhex(length)
int length;
{ byte far *p, *end;
  unsigned int  i;
  long cnt;
  
  p  = fcb.bpos;
  end= fcb.memend- length;
  while (p<=end)
  { if (*p == dmpbuff[0])
    { i= length;
      while (--i && p[i]==dmpbuff[i]);
      if ( !i)
      { cnt= (unsigned long)(p-fcb.bpos);
        if (cnt<0) cnt += 0x10000;
        return cnt;
      }
    }
    ++p;
  }
  error(E_BADFND);
  return (long) -1;
}


/*
 * Der DUMP -Editor
 * ****************
 */

void dump_ed(command)
byte command;
{ long   cnt,col,pagelen;
  short  y,i,j;
  byte   value,b;
  char   displayc;

  value  = b= command;
  pagelen= (long) ((fcb.maxY -fcb.minY +1) <<4);
  y      = fcb.minY+ (short) (( (integer)(fcb.fpos-fcb.membegin) -fcb.dumpstrt) >> 4);
  col    = (long) ( ((integer)(fcb.fpos-fcb.membegin) -fcb.dumpstrt) & 0x0000000f);
  if (fcb.dmpascii)
  { if (command == ADDCH)  displayc= normch( value= (byte)getc() );
    else displayc= (command<'\040' || command>OPCODE)? '\000' : (char)command;
    if (displayc != '\000')
    { ttych(displayc);
      gotoXY( (short)(8+col+col+col),y);
      hex2out(value);
      command= ADDCH;
    }
  }
  else
  { if ((b= numeral(command)) != '\020')
    { while ((value= numeral(getkey())) == '\020') bell();
      gotoXY(60+(short)col,y);
      ttych( normch(value+= b<<4) );
      command= ADDCH;
    }
  }
  cnt= 0;
  switch  (command)
  { case CHEDMODE : fcb.dmpascii= ~fcb.dmpascii;  break;
    case WRDRIGHT :
    case LINEEND  : if ((cnt= 15 -col) != 0) break;
    case RIGHT    : cnt= 1; break;
    case DOWN     : if ((integer)(fcb.memend-fcb.bpos) > 15-col) cnt=16; break;
    case PAGEEND  : if ((cnt= (fcb.maxY -y)*16 +15 -col) == 0)
                         cnt= pagelen;
                    break;
    case FILEEND  : cnt= (long) ((integer)(fcb.memend-fcb.bpos)); break;
    case WRDLEFT  :
    case LINESTRT : if ((cnt= -col) != 0) break;
    case LEFT     : cnt= -1; break;
    case UP       : if ((integer)(fcb.fpos-fcb.membegin) > 15) cnt= -16; break;
    case PAGESTRT : if ((cnt= -col -(y-fcb.minY)*16) == 0)
                         cnt= -pagelen;
                    break;
    case FILESTRT : cnt= -( (long) ((integer)(fcb.fpos-fcb.membegin)) ); break;
    case ADDCH    :
                    if (fcb.bpos == fcb.memend)
                    { if (fcb.bpos<=fcb.fpos)
                      { error(E_TOOBIG);
                        _display[y]= TRUE;
                        break;
                      }
                      --fcb.bpos;
                      if (col==15) _display[y+1]= TRUE;
                      fcb.modifyed= TRUE;
                      if (fcb.blkend==fcb.memend  &&  fcb.blkend!=fcb.blkbegin
                          && parm._vmod!=1 /* != VENIX */)
                      { b= (byte) col;
                        col+= col+col+7;
                        gotoXY( (short)(col+3),y); nvideo();
                        gotoXY( (short)col,y); 
                        if (b==0) invers(); else ttych(' ');
                      }
                    }
                    cnt= 1;
                    fcb.modifyed= TRUE;
                    *(fcb.bpos)= value;
                    break;
    case POSITION : putwind("Enter address in hexadecimal");
                    ++Qwindow;
                    putwind("Address: ");
                    gotoWIND();
                    while ((command=getkey())!= RETURN)
                    { if ((b= numeral(command)) != '\020')
                           cnt= cnt*16 +(long)b;
                      else bell();
                    }
                    cnt -= (long) ((integer)(fcb.fpos-fcb.membegin));
                    break;
    case FINDOLD  : if ( dmp_j)
                    { fcb.bpos++;
                      cnt= findhex(dmp_j)+1;
                      fcb.bpos--; 
                      break;
                    }
    case FIND     : putwind("Enter hexadecimal string");
                    putwind(": ");
                    gotoWIND();
                    b= 0;
                    while ((command= getkey())!=RETURN)
                    { if (command==DELCHRL ||  command==LEFT)
                      { if ( !b)
                        { bell();
                          continue;
                        }
                        --b;
                        backstep(); ttych(' '); backstep();
                        continue;
                      }
                      if (b>=32)
                      { bell();
                        continue;
                      }
                      if (command>='a' && command<='f') command-= 'a'-'A';
                      value= command;
                      if (value>='A' && value<='F') value-= 'A'-10;
                      else
                      if (value>='0' && value<='9') value-= '0';
                      else
                      { bell();
                        continue;
                      }
                      ttych(command);
                      dmpbuff[b++]= value;
                    }
                    i=dmp_j= 0;
                    dmpbuff[b]= 0;
                    while (i<b)
                    { dmpbuff[dmp_j++]= dmpbuff[i]*16 + dmpbuff[i+1];
                      i+= 2;
                    }
                    if (dmp_j)  if ((cnt= findhex(dmp_j))== -1) cnt= 0;
                    break;
    default       : bell();
  }
  if (cnt > (long) ((integer)(fcb.memend-fcb.bpos)) ) /* Kursor soll um mehr Stellen in Richtung  */
      cnt=  (long) ((integer)(fcb.memend-fcb.bpos)) ; /* Dateiende gerueckt werden, als die Datei */
                                                      /* noch Zeichen hinter dem Kursor hat       */
  else
  if (cnt < -((long) ((integer)(fcb.fpos-fcb.membegin))) )/* analog in Richtung Dateianfang */
      cnt=  -((long) ((integer)(fcb.fpos-fcb.membegin)));
  shift(cnt); 
}
