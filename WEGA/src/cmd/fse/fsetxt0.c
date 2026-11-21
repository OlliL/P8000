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


void textline(curr_pos)            /* curr_pos ist die Adresse des Zeilentrennzeichens */
register byte far *curr_pos;
{ register byte     b;
  register long     column;
           long     col,lastcol;
           char     firstch;
           byte far *p;

  col= column= 0;
  lastcol= fcb.offset+ (TTYCOLUM-2);
  firstch= ((*curr_pos & fcb.bytemask) == '\f')? '>' : ' ';
  while (1)
  { if (++curr_pos == fcb.fpos) curr_pos= fcb.bpos;
    if (curr_pos == fcb.blkbegin  &&  fcb.blkend!= (byte far *)0)
                                  { if (column==fcb.offset)  ttych(firstch);
                                    if (++column > fcb.offset) invers();
                                    if (column>lastcol) break;           }
    if ((b= *curr_pos & fcb.bytemask)<='\f'  &&  b>='\n'  &&  b!=CNTRL('K'))  break;
    if (curr_pos == fcb.blkend  ) { if (column==fcb.offset) ttych(firstch);
                                    if (++column > fcb.offset) nvideo();
                                    if (column>lastcol) break;           }
    if (b<'\040')
    { if (b == '\r')
      { if ((p=curr_pos+1) == fcb.fpos) p= fcb.bpos;
        if ((*p & fcb.bytemask) == '\n') break;
      }
      if (b == '\t')
      { do
        { if (column == fcb.offset)
          { if (fcb.blkbegin<curr_pos  &&  curr_pos<fcb.blkend) invers();
            else                                                ttych(firstch);
          }
          if (++column > fcb.offset) ttych(firstch);
          ++col;
          if (column > lastcol) goto txtln_1;
        } while (column & 0x07);
        continue;
      }
      if (fcb.cntrmode == 0) continue;
      if (fcb.cntrmode == 1) b= ' '; else
      if (fcb.cntrmode == 2) b= '.'; else
      { if (column == fcb.offset)
        { if (fcb.blkbegin<curr_pos  &&  curr_pos<fcb.blkend) invers();
          else                                                ttych(firstch);
        }
        if (++column > fcb.offset) ttych('^');
        if (column > lastcol)  break;
        b+= '@';
      }
    }
    if (column==fcb.offset)
    { if (fcb.blkbegin<curr_pos  &&  curr_pos<fcb.blkend) invers();
      else                                                ttych(firstch);
    }
    if (++column>fcb.offset) ttych( (char)b );
    if (column>lastcol)  break;
    ++col;
  }
  txtln_1:
  if (column<=fcb.offset   &&   fcb.blkbegin<=curr_pos    &&
      fcb.blkend>=curr_pos)  invers();
  clreol();
  if (parm._vmod) ttystr(parm._nvideo);
}

