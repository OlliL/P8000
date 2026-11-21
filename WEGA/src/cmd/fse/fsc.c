#include "system"

#define writestr(X) { static char text[]= X; write(1,text,sizeof(text)-1); }


#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#include "fseerror.h"
                                      /* return:  0 compilation ok      */
                                      /*          1 compilation aborted */
unsigned char buff[2048];             /*          2 call error (scc)    */
unsigned char *sccfile;              
int i,l,ll;
unsigned char **args;


int getc()
{ int c;

  c= '\n';
  if (ll!=l)  c= buff[l++];
  if (c=='\000') { l=ll; c= '\n'; };
  return c;
}



int compute()
{ int            j, errfile, errcnt, c;
  unsigned char  fname[30],fsefile[30];
  unsigned char  *pp;
  struct errlist err[MAXERR];

  for (;;)
  { writestr("start of compilation\n");
    lseek(i, (long)0, 0);
#ifdef WEGA
    if (fork()==0)
    { execv("/bin/scc",args);
      return 2;
    }
#endif
#ifdef VENIX
    if (fork()==0)
    { execv("/bin/cc",args);
      return 2;
    }
#endif
    wait(&j);
    if (j == 0)
    { writestr("compilation ok\n");
      return 0;
    }
    write(i,"\000",1);
    writestr("compilation ended with errors\n");
    lseek(i, (long)0, 0);
    ll= read(i, buff, sizeof(buff));
    l= 0;
    fsefile[0]= 0;
    errfile= creat("errmsg",0666);
    errcnt= 0;
    j= 0;
    while (j<MAXERR)
    { err[j].line   = 0;
      err[j].column = 0;
      err[j].text[0]= 0;
      ++j;
    }
    while (l!=ll  &&  errcnt<MAXERR)
    { if (getc()!='"')
      { while (getc()!='\n');
        continue;
      }
      j= 0;
      while ((c=getc())!='"'  &&  c!='\n') if (j<29) fname[j++]= c;
      fname[j]= 0;
      if (fsefile[0]==0)
      { do
        { fsefile[j]= fname[j];
        } while (--j>=0);
      }
      else
      { if (strncmp(fname,fsefile,j)) break;
      }
#ifdef WEGA
      getc();  /*  ':'  ueberlesen */
      j= 0;
      while ((c=getc())>='0'  &&  c<='9')  j= 10*j+c-'0';
      err[errcnt].line= j;
      j= 0;
      while ((c=getc())!='\n')
      { if (j<sizeof(err[0].text)-1) err[errcnt].text[j++]= c;
      }
      err[errcnt].text[j]= 0;
      /* if (errcnt==0  ||  err[errcnt-1].line!=err[errcnt].line)   */
      errcnt++;
#endif
#ifdef VENIX
      while ((c=getc())!='\n'  &&  (c<'0' || c>'9'));
      j= 0;
      while (c>='0'  &&  c<='9')
      { j= 10*j+c-'0';
        c= getc();
      }
      err[errcnt].line= j;
      j= 0;
      while ((c=getc())!='\n')
      { if (j<sizeof(err[0].text)-1) err[errcnt].text[j++]= c;
      }
      err[errcnt].text[j]= 0;
      /* if (errcnt==0  ||  err[errcnt-1].line!=err[errcnt].line)   */
      errcnt++;
#endif
    }
    write(errfile,err,sizeof(err));
    close(errfile);
    if (fsefile[0]==0)
    { pp= buff;
      while (*pp) write(1,pp++,1);
      return 1;
    }
    if (fork()==0)
    { execl("/bin/fse","/bin/fse","-e",fsefile, (unsigned char *)0);
      return 2;
    }
    wait(&j);
    if ((j/256)!=0)
    { writestr("fsc stopped\n");
      return 1;
    }
  }
}

main(argc, argv)
int argc;
unsigned char **argv;
{ int j;

  if ((sccfile= argv[1])== (unsigned char *)0)
  { writestr("usage:  fsc [options] filename\n");
    j= 4;
  }
  else
  { args= argv;
    close(creat("errlist",0666));   /* das ist portabel, open nicht */
    i= open("errlist",O_RDWR);
    if (i<=0)
    { writestr("can't open errlist\n");
      j= 5;
    }
    else
    { close(2);
      dup2(i,2);
      j= compute();
      close(2);
      close(i);
      unlink("errmsg");
      unlink("errlist");
      unlink("errlist~");
    }
  }
  exit(j);
}

