/*****************************************************/
/*                                                   */
/*       WEGA/VENIX/XENIX -Variante  Version 1.1     */
/*       - mit Kopierschutz                          */
/*       - Bildschirmansteuerung ueber Termcap       */
/*                                                   */
/*       Stand: 03.01.89                             */
/*                                                   */
/*****************************************************/


  if (strcmp(p=(byte *)getenv("TERM"),parm.TERM))
  { if (tgetent(fcb.membegin,p) != 1)
    { write(1,termerr,sizeof(termerr));
      exit(1);
    }
    parm.gotowait= 0;
    parm.clrwait = 0;
    p= (unsigned char far *)fcb.membegin +1100;
    i= tgetnum("sg");
    if (i<0)  i= tgetnum("ug");
    parm._vmod= (i<=0);    /*  1 --> 0 (P8000)           */
                           /*  0 --> 1 (VENIX)           */
                           /* -1 --> 1 (VENIX = default) */
                           
    if ((pp=(byte *)tgetstr("cm",&p)) != (unsigned char *)0)
    { while (*pp>='0' && *pp<='9')
      { parm.gotowait= parm.gotowait*10 + *(pp++) -'0';
      }
      memcpy(scratch,pp,30);
    }
    else scratch[0]= 0;

    if ((pp=(byte *)tgetstr("cl",&p)) != (byte *)0)
    { while (*pp>='0' && *pp<='9')
      { parm.clrwait= parm.clrwait*10 + *(pp++) -'0';
      }
      if (*pp=='*') ++pp;
      memcpy(parm._clrscr ,pp,9);
    }
    else parm._clrscr[0]= 0;

    if ((pp=(byte *)tgetstr("ce",&p)) != (byte *)0)
    { while (*pp>='0' && *pp<='9') ++pp;
      memcpy(parm._clreol ,pp,9);
    }
    else parm._clreol[0]= 0;

    if ((pp=(byte *)tgetstr("al",&p)) != (byte *)0)
    { while (*pp>='0' && *pp<='9')  ++pp;
      memcpy(parm._insline,pp,9);
    }
    else parm._insline[0]= 0;

    if ((pp=(byte *)tgetstr("dl",&p)) != (byte *)0)
    { while (*pp>='0' && *pp<='9')  ++pp;
      memcpy(parm._delline,pp,9);
    }
    else parm._delline[0]= 0;

    if ((pp=(byte *)tgetstr("ic",&p)) != (byte *)0)
    { while (*pp>='0' && *pp<='9')  ++pp;
      memcpy(parm._ins_ch ,pp,9);
    }
    else parm._ins_ch[0]= 0;

    if ((pp=(byte *)tgetstr("so",&p)) != (byte *)0)
    { while (*pp>='0' && *pp<='9')  ++pp;
      memcpy(parm._video  ,pp,9);
    }
    else  parm._video[0]= 0;

    if ((pp=(byte *)tgetstr("se",&p)) != (byte *)0)
    { while (*pp>='0' && *pp<='9')  ++pp;
      memcpy(parm._nvideo ,pp,9);
    }
    else  parm._video[0]= 0;

    if (parm._video[0]==0)  /* Keine Videoattribute */
    { parm._video[0]= parm._nvideo[0]= ' ';
      parm._video[1]= parm._nvideo[1]= 0;
      parm._vmod= 0;  /* P8000 */
    }
    
    { g= (byte *)parm._gotoXY;
      p= (byte *)scratch;
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
          if (*p == 'd')  ++p;
          *(g++)= GOTOSOUT; continue;
        }
        if (p[0]=='\\')
        { *(g++)= (p[1]-'0')*64 +
                  (p[2]-'0')*8  +
                  (p[3]    );
          p+= 4; continue;
        }
        *(g++)= *(p++);
        if (p> (byte *)(scratch+28))
        { parm._gotoXY[0]= 0;
          break;
        }
      }
      *g= GOTOEND;
    }
  }

  if (parm._delline[0]==0) parm._insline[0]= 0;

  if (parm._clrscr[0] ==0 ||
      parm._gotoXY[0] ==0 ||
      parm._clreol[0] ==0 )
  { write(1,termerr,sizeof(termerr));
    exit(1);
  }


