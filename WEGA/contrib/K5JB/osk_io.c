/* OS- and machine-dependent stuff for OSK machines
 * Written by Bob, Brose, N0QBJ, 974 Hampden Ave, St Paul MN 55114-1108
 *

Version Log
-----------
012093 - First OSK release.
022493 - Fixed bug in smtp, wasn't doing cr/lf correctly and SOME nos hosts
                 objected.
                 Fixed various FTP bugs (also caused by cr/lf conversions.
                 Fixed problem in ftpserver ls command response which should now
                  allow mgets from a remote host to work correctly (directory
                  access routines in this file altered).
081293 - Fixed bug in rename which was causing an abort on mailbox reads.
081693 - Fixed memory allocation bugs, space used by make_path was not
                  being freed.
                 Added the code for getcwd and cd.
                 
*/

#include "global.h"
#include <time.h>
#include <stdio.h>
#include <sgstat.h>
#include <modes.h>
#include <dir.h>
#include <strings.h>

#include "osk.h"
#include "mbuf.h"
#include "internet.h"
#include "iface.h"
#include "cmdparse.h"


#define MAXCMD 80
unsigned nasy;
int IORser[ASY_MAX];
struct sgbuf orgsgbufi, orgsgbufo;

#ifndef _UCC
#define MAXTMP 26
struct FILREC {
        FILE *filptr;
        char tmpnam[32];
};

struct FILREC tmpfils[MAXTMP];

#endif

restore()
{
}

disable()
{
}

kbread()
{
        unsigned char c;

    if (getstat(1,0) < 0)
      return -1;

        read(0, &c, 1);
        return c;
}


struct asy asy[ASY_MAX];

/* Interface list header */
struct interface *ifaces = NULL;


/* Called at startup time to set up console I/O, memory heap */
ioinit()
{
        char termname[40];
        struct sgbuf sbuf;
        int i;
        
/* tmp file tracking array init, non-ucc */

#ifndef _UCC
        for (i=0;i<MAXTMP;i++)
                tmpfils[i].filptr=NULL;
#endif
        _gs_devn(2, termname+1);
        termname[0] = '/';

#ifdef STRANGECONS
        fclose(stdin);
        if (fopen(termname, "r+") != stdin)
          exit(1000);
          
        fclose(stdout);
        if (fopen(termname, "w+") != stdout)
          exit(2000);
        
    if (fileno(stdin) != 0)
      exit(3000);
      
    if (fileno(stdout) != 1)
      exit(4000);
#endif

        _gs_opt(0,&sbuf);
        memcpy(&orgsgbufi,&sbuf,sizeof(sbuf));
    sbuf.sg_case = 0;
    sbuf.sg_backsp = 0;
    sbuf.sg_delete = 0;
    sbuf.sg_echo = 0;
    sbuf.sg_alf = 1;
    sbuf.sg_nulls = 0;
    sbuf.sg_pause = 0;
    sbuf.sg_page = 0;
    sbuf.sg_bspch = 0;
    sbuf.sg_dlnch = 0;
    sbuf.sg_eorch = 0;
    sbuf.sg_eofch = 0;
    sbuf.sg_rlnch = 0;
    sbuf.sg_dulnch = 0;
    sbuf.sg_psch = 0;
    sbuf.sg_bsech = 0;
    sbuf.sg_bellch = 0;
    sbuf.sg_xon = 0;
    sbuf.sg_xoff = 0;
    sbuf.sg_tabcr = 0;
    sbuf.sg_tabsiz = 0;
        sbuf.sg_kbich = 0;
        sbuf.sg_kbach = 0;
        _ss_opt(0,&sbuf);
        
#ifdef STRANGECONS
        _gs_opt(1,&sbuf);
    memcpy(&orgsgbufo,&sbuf,sizeof(sbuf));
    sbuf.sg_case = 0;
    sbuf.sg_backsp = 0;
    sbuf.sg_delete = 0;
    sbuf.sg_echo = 0;
    sbuf.sg_alf = 1;
    sbuf.sg_nulls = 0;
    sbuf.sg_pause = 0;
    sbuf.sg_page = 0;
    sbuf.sg_bspch = 0;
    sbuf.sg_dlnch = 0;
    sbuf.sg_eorch = 0;
    sbuf.sg_eofch = 0;
    sbuf.sg_rlnch = 0;
    sbuf.sg_dulnch = 0;
    sbuf.sg_psch = 0;
    sbuf.sg_bsech = 0;
    sbuf.sg_bellch = 0;
    sbuf.sg_xon = 0;
    sbuf.sg_xoff = 0;
    sbuf.sg_tabcr = 0;
    sbuf.sg_tabsiz = 0;
    sbuf.sg_kbich = 0;
	sbuf.sg_kbach = 0;
	_ss_opt(1,&sbuf);
#endif

	setbuf(stdin,NULL);
}

/* Called just before exiting to restore console state */
iostop()
{
        _ss_opt(0,&orgsgbufi);
#ifdef STRANGECONS
        _ss_opt(0,&orgsgbufo);
#endif
}

eihalt()
{
    tsleep(1);
}

#ifdef  SLOWCHECK

int clksec()
{
        time_t tim;
        (void) time(&tim);
/* 1 second resolution, return hundredths */
        return (int)(tim * 100);
}

#else

#ifdef  _UCC
_asm("clksec:   moveq   #3,d0");
_asm("          os9     F$Time");
_asm("          move.l  #0,d0");
_asm("          move.w  d3,d0");
_asm("          rts");
#else
#include "clksec.a"
#endif
#endif

void
check_time()
{
        int32 iss();
        void tickle();

        static long clkval;
        long ntime, offset;

        /* read elapsed real time (typ. 100 Hz) */
        ntime=clksec(); 

        /* resynchronize if the error is large (10 seconds or more) */
        offset = ntime - clkval;
        if (offset > (10000/MSPTICK) || offset < 0)
                clkval = ntime;

        /* Handle possibility of several missed ticks */
        while (ntime != clkval){
                fflush(NULL);   
        ++clkval;
                icmpclk();
                tickle();
                (void) iss();
        }
}
/* Initialize asynch port "dev" */
int
asy_init(dev,arg1,arg2,bufsize)
char  *arg1,*arg2;
int16 dev;
unsigned bufsize;
{
        register unsigned base;
        register struct asy *ap;
        struct sgbuf sbuf;
        unsigned i=0; 
        if (dev >= nasy)
                return -1;
 
    ap = &asy[dev];

/*      ap->tty = malloc(strlen(arg2)+1);*/
        strcpy(ap->tty,arg2);

        if ( (IORser[dev]=open(ap->tty, S_IREAD+S_IWRITE)) < 0) 
        {       printf("asy %s: Open failed.\015\012", ap->tty);
                fflush(stdout);
                return -1;
        }
        _gs_opt(IORser[dev], &sbuf);
        sbuf.sg_case = 0;
        sbuf.sg_backsp = 0;
        sbuf.sg_delete = 0;
        sbuf.sg_echo = 0;
        sbuf.sg_alf = 0;
        sbuf.sg_nulls = 0;
        sbuf.sg_pause = 0;
        sbuf.sg_page = 0;
        sbuf.sg_bspch = 0;
        sbuf.sg_dlnch = 0;
        sbuf.sg_eorch = 0;
        sbuf.sg_eofch = 0;
        sbuf.sg_rlnch = 0;
        sbuf.sg_dulnch = 0;
        sbuf.sg_psch = 0;
        sbuf.sg_kbich = 0;
        sbuf.sg_kbach = 0;
        sbuf.sg_bsech = 0;
        sbuf.sg_bellch = 0;
        sbuf.sg_xon = 0;
        sbuf.sg_xoff = 0;
        sbuf.sg_tabcr = 0;
        sbuf.sg_tabsiz = 0;
        _ss_opt(IORser[dev], &sbuf);

        switch(sbuf.sg_baud)
               {
               case 0:      i = 50;
                              break;
               case 1:      i = 75;
                              break;
               case 2:     i = 110;
                              break;
               case 3:     i = 134;
                              break;
               case 4:     i = 150;
                              break;
               case 5:     i = 300;
                              break;
               case 6:     i = 600;
                              break;
               case 7:    i = 1200;
                              break;
               case 8:    i = 1800;
                              break;
               case 10:    i = 2400;
                              break;
               case 12:    i = 4800;
                              break;
               case 14:    i = 9600;
                              break;
               case 15:   i = 19200;
                              break;
               case 16:   i=38400;
                                        break;
              default:  i= 0;
                }
        printf("asy %s :Open %i Baud, Ok.\015",ap->tty,i);
        return 0;
}

int
asy_stop(iface)
struct interface *iface;
{
        return 0;
}

/* Asynchronous line I/O control (speed change not implemented */
asy_ioctl(interface,argc,argv)
struct interface *interface;
int argc;
char *argv[];
{
        if(argc < 1){
                printf("%d\015\012",asy[interface->dev].speed);
                return 0;
        }
        return asy_speed(interface->dev,atoi(argv[0]));
}


/* Set asynch line speed */
int
asy_speed(dev,speed)
int dev;
int speed;
{
        return 0;
}


/* Send a buffer to serial transmitter */
asy_output(dev,buf,cnt)
unsigned dev;
char *buf;
unsigned short cnt;
{
        if(dev >= nasy)
            return 0;

        write(IORser[dev], buf, cnt);
}

/* Receive characters from asynch line
 * Returns count of characters read
 */
unsigned
asy_recv(dev,buf,cnt)
int dev;
char *buf;
unsigned cnt;
{
        register struct asy *ap;
        register int charpend;

        ap = &asy[dev];

        if ( (charpend=_gs_rdy(IORser[dev])) < 0)
          return 0;

        charpend = read(IORser[dev], buf, min(charpend, cnt));
        return charpend < 0 ? 0 : charpend;
}


int stxrdy(dev)
int dev;
{
        return 1;
}

/* dir in temp file (a lot of crap to do this in the 3.2 osk compiler).
   Short/long listings. For FTP */
char *getenv();
void free();

FILE *
dir(path,full)
char *path;
int full;
{
        FILE *fp, *fpo;
        char *name = "dirfile", *cp;
        char cmd[256], *fnamstr=cmd;
        char disk[256];
                FILE *tmpfile();
                unsigned char ch;
                int i,k,fp1;
                register int j;
                char *make_path();
                char *ep, *np;
                void filedir();
                DIR *dt;
                int dofree=0;
                                                
                if ((ep=getenv("TMPDIR"))!=NULL)
                {
                        np=make_path(ep,name,1);
                        dofree=1;
                }
                else
                        np=name;
                                                
        strcpy(disk, path);
        for (cp=disk; *cp && *++cp!='/'; )
          ;
        *cp = '\0';

        if(full){
                sprintf(cmd,"(dir -de %s; echo; free %s; echo)>-%s",path,disk,np);
                system(cmd);
        } else {

                        fp1=creat(np,S_IREAD+S_IWRITE);
                                if ((dt=opendir(path))!=NULL)
                                {
                                        closedir(dt);
                                        strcat(path,"/*");
                                }

                        filedir(path,0,fnamstr);
                        if (*fnamstr=='\0')
                            close(fp1);
                        else
                        {
                                        write(fp1,fnamstr,strlen(fnamstr));
                                        write(fp1,"\015\012",2);
                                
                                        while(1)
                                        {       filedir(path,1,fnamstr);
                                                if (*fnamstr=='\0')
                                                {
                                                        close(fp1);
                                                        break;
                                                }
                                                else
                                                {
                                                        write(fp1,fnamstr,strlen(fnamstr));
                                                        write(fp1,"\015\012",2);
                                                }
                                        }
                                }
/*
                                NOTE, this ALMOST worked but added the path on a wildcard
                                 dir which messed up remote mgets.
                                 
                sprintf(cmd,"(dir -d %s; echo; free %s; echo)>-%s",path,disk,np);
                system(cmd);
*/
               }
        fp = fopen(np,"r");
        if((fpo = tmpfile())==NULLFILE)
                return NULLFILE;
                while ((i=fread(cmd,1,128,fp)))
                { 
                        k=0;
                        for(j=0;j<i;j++)
                        {
                                disk[k]=cmd[j];
                                k++;
                                if (cmd[j]=='\015')
                                {       disk[k]='\012';
                                        k++;
                                }
                        }       
                        fwrite(disk,1,k,fpo);
                }
                fflush(fpo);
                rewind(fpo);
                fclose(fp);
                unlink(np);
                if (dofree)
                        free(np);               
        return fpo;
}

/* dirutil.c
 *        OSK directory reading stuff.
 *        Matthias Bohlen
 *        Copyright 1987 Matthias Bohlen, All Rights Reserved.
 *        Permission granted for non-commercial copying and use, provided
 *        this notice is retained.
 */

static DIR *d;
static struct direct *r;

/* wildcard filename lookup */
void filedir (dirname, times, ret_str)
char *dirname;
int times;
char *ret_str;
{
        DIR *d2;
        register int masklen;
        register char *cp;
        int i;
        char dname[80], filemask[80];
        
        for (cp=dirname+strlen(dirname); cp>dirname && *--cp != '/'; )
          ;
        strncpy(dname, dirname, cp-dirname);
        dname[cp-dirname] = '\0';
        strcpy(filemask, cp+1);
        
    masklen = strlen(filemask);
    
        if (times==0) {   /* first time */
                if ( (d=opendir(dname)) == NULL ) {
                        *ret_str = '\0';
                        return;
                }
        }
        
        while (1) {
                if ( (r=readdir(d)) == NULL ) {
                        *ret_str = '\0';
                        closedir(d);
                        return;
                }
                if (_cmpnam(r->d_name, filemask, masklen) == 0)
                        if (r->d_name[0] != '.') {
                                strcpy(ret_str, r->d_name);
                                
                                strcat(dname,"/");
                                strcat(dname,ret_str);
                                if ((i=open(dname,S_IREAD))==-1)
                                {
                                        if ((d2=opendir(dname))!=NULL)
                                        {       closedir(d2);
                                                strcat(ret_str,"/");
                                        }
                                }
                                else
                                        close(i);
                                return;
                        }
        }
}

dodir(argc, argv)
int     argc;
char    **argv;
{
        int     i, stat;
        char    str[MAXCMD];

        strcpy(str, "dir -e ");
        for (i = 1; i < argc; i++) {
                strcat(str, argv[i]);
                strcat(str, " ");
        }


        stat = system(str);

        return stat;
}
sysreset()
{
        printf("\n no system reset function \n");
}
doshell(argc,argv)
int argc;
char **argv;
{
        struct sgbuf sgbufi, sgbufo;
        _gs_opt(0,&sgbufi);
#ifdef STRANGECONS
        _gs_opt(1,&sgbufo); 
#endif
        _ss_opt(0,&orgsgbufi);
#ifdef STRANGECONS
        _ss_opt(1,&orgsgbufo);
#endif
        system("");
        _ss_opt(0,&sgbufi);
#ifdef STRANGECONS
        _ss_opt(1,&sgbufo);
#endif
}

/* close a pipe (not different in osk)? */
int pclose(i)
FILE *i;
{
        fclose(i);
}

mkdir(s, m)
char    *s;
int     m;
{
        char tmp[MAXCMD];

        sprintf(tmp, "makdir %s", s);
        if (system(tmp))
                return -1;
/*      if (chmod(s, m) < 0)
                return -1;
*/
        return 0;
}


rmdir(s)
char    *s;
{
        char tmp[MAXCMD];

        sprintf(tmp, "deldir -f -q %s", s);
        if (system(tmp))
                return -1;

        return 0;
}

OpenPty()
{
        return -1;
}

char *getcwd(tmp,siz)
char *tmp;
int siz;
{
        static int lock=0;
        char *name="pdfile";
        char *ep, *np, *make_path();
        FILE *fil;
        int dofree=0;
        
        while (lock);
        
        lock=1;
        if ((ep=getenv("TMPDIR"))!=NULLCHAR)
        {
                dofree=1;
                np=make_path(ep,name,1);
        }
        else
                np = name;
        sprintf(tmp,"pd >-%s",np);
        system(tmp);
        if ((fil=fopen(np,"r"))==NULL)
        {
                lock=0;
                return NULL;
        }
        fgets(tmp,siz,fil);
        fclose(fil);
        unlink(np);
        if (dofree) 
                free(np);
        lock=0;
        return name;
}       


/* enabled this command with k28 -- it was incomplete.  Tried to do it
in most portable way, though in SysV, it would be appropriate to feed
getcwd a null pointer and free the memory pointed to by getcwd's return
value.  This command is used for "pwd" and "cd".  - K5JB
*/
int
docd(argc, argv)
int argc;
char **argv;
{
        static char tmp[MAXCMD];
        char *getcwd();
        extern char *homedir;

        if(*argv[0] == 'c')
                if(chdir(argc > 1 ? argv[1] : homedir) == -1){
                        printf("Can't change directory\n");
                        return 1;
                }

        if (getcwd(tmp, sizeof(tmp) - 2) != NULL)
                printf("%s\n", tmp);

        return 0;
}

/***************************************************************************/
/*                                                                         */
/* _secsince70() : Unix library (OS9/68000)                                */
/* =============                                                           */
/*                                                                         */
/* Author:     K. Schmitt                                                  */
/* Compiler:   Microware C Vers. 3.0                                       */
/* OS:         OS9/68000 Vers. 2.2                                         */
/*                                                                         */
/* Edition History                                                         */
/* ===============                                                         */
/*                                                                         */
/* Ed. 0.00  Date 11/16/88                                                 */
/*           First version                                                 */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Description                                                             */
/*                                                                         */
/*
       returns # of secs past since 01/01/1970 GMT

*/

time_t _secsince70 (date)
          char date []; /* yy/mm/dd/hh/mm/ss */
                        /*  0  1  2  3  4  5 */
          {
          struct tm time70, time_;
          time_t mktime();


          time70.tm_sec   = 0;
          time70.tm_min   = 0;
          time70.tm_hour  = 0;
          time70.tm_mday  = 1;
          time70.tm_mon   = 0;
          time70.tm_year  = 70;
          time70.tm_isdst = -1;
          time_.tm_sec   = (int) date[5];
          time_.tm_min   = (int) date[4];
          time_.tm_hour  = (int) date[3];
          time_.tm_mday  = (int) date[2];
          time_.tm_mon   = (int) date[1] - 1;
          time_.tm_year  = (int) date[0];
          time_.tm_isdst = -1;
          return (mktime(&time_.tm_sec) - mktime(&time70.tm_sec));
          
          } /* end of _secsince70 */

/***************************************************************************/
/*                                                                         */
/* stat() : Unix library (OS9/68000)                                       */
/* ======                                                                  */
/*                                                                         */
/* Author:     K. Schmitt                                                  */
/* Compiler:   Microware C Vers. 3.0                                       */
/* OS:         OS9/68000 Vers. 2.2                                         */
/*                                                                         */
/* Edition History                                                         */
/* ===============                                                         */
/*                                                                         */
/* Ed. 0.00  Date 11/16/88                                                 */
/*           First version                                                 */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Description                                                             */
/*                                                                         */
/*

     NAME  
          stat, fstat - get file status

     SYNOPSIS
          #include <UNIX/stat.h>

          int stat (path, buf)
          char *path;  
          struct stat *buf;

          int fstat (fildes, buf)  
          int fildes;  
          struct stat *buf;

     DESCRIPTION
          Path points to a path name naming a file.  Read, write or
          execute permission of the named file is not required, but
          all directories listed in the path name leading to the file  
          must be searchable.  Stat obtains information about the  
          named file.  

          Fstat obtains information about an open file known by the
          file descriptor fildes, obtained from a successful open, 
          creat, dup, fcntl, or pipe system call.  

          Buf is a pointer to a stat structure into which information
          is placed concerning the file.

          The contents of the structure pointed to by buf include the
          following members:

           ushort     st_mode;     File mode
           ino_t      st_ino;      Inode number
           dev_t      st_dev;      ID of device containing a directory
                                   entry for this file
           dev_t      st_rdev;     ID of device. This entry is defined
                                   only for character special or block
                                   special files
           short      st_nlink;    Number of links
           ushort     st_uid;      User ID of the file's owner
           ushort     st_gid;      Group ID of the file's group
           off_t      st_size;     File size in bytes
           time_t     st_atime;    Time of last access
           time_t     st_mtime;    Time of last data modification
           time_t     st_ctime;    Time of last file status change
                                   Times measured in seconds since
                                   00:00:00 GMT, Jan. 1, 1970

          The following parameters are affected by different system
          calls:

          st_atime  Time when file data was last accessed.
                    Changed by the following system calls:  creat,
                    mknod, pipe, utime, and read.

          st_mtime  Time when data was last modified.
                    Changed by the following system calls:  creat,
                    mknod, pipe, utime, and write.

          st_ctime  Time when file status was last changed.
                    Changed by the following system calls:  chmod,
                    chown, creat, link, mknod, pipe, unlink, utime,
                    and write.

          Stat will fail if one or more of the following are true:

          [ENOTDIR]      A component of the path prefix is not a
                         directory.

          [ENOENT]       The named file does not exist.

          [EACCES]       Search permission is denied for a component
                         of the path prefix.

          [EFAULT]       Buf or path points to an invalid address.

          Fstat will fail if one or more of the following are true:

          [EBADF]        Fildes is not a valid open file descriptor.

          [EFAULT]       Buf points to an invalid address.

     RETURN VALUE
          Upon successful completion a value of 0 is returned.
          Otherwise, a value of -1 is returned and errno is set to
          indicate the error.

*/

#include "os9stat.h"

#include <direct.h>

extern time_t _secsince70 ();

int stat (fname, buf)
          char fname [];
          struct stat *buf;
          {
          int fd, ret;
          DIR *dirp;

          if ((fd = open (fname, S_IREAD)) == -1)
               {
               if ((dirp = opendir (fname)) == 0)
                    return (-1);
               fd = dirp -> dd_fd;
               }
          else
               {
               dirp = 0;
               }

          ret = fstat (fd, buf);
          if (dirp == 0) close (fd);
          else           closedir (dirp);

          return (ret);

          } /* end of stat */



int fstat (fd, buf)
          int fd;
          register struct stat *buf;
          {
          struct fildes fildes;
          register int i;
          register char *p;
          char *malloc();

          if (_gs_gfd (fd, &fildes, sizeof (struct fildes)) == -1)
               return (-1);

          buf -> st_uid = (unsigned short) (fildes.fd_own [0]);
          buf -> st_gid = (unsigned short) (fildes.fd_own [1]);
          buf -> st_mode = (unsigned short) (fildes.fd_att) & 0377;
          buf -> st_nlink = (unsigned short) (fildes.fd_link);

          for (i = 0, p = (char *) (&buf -> st_size); i < 4; i++)
               {
               *p++ = fildes.fd_fsize [i];
               }

          buf -> st_atime = buf -> st_mtime = _secsince70 (fildes.fd_date);
          fildes.fd_dcr [3] = 0;
          fildes.fd_dcr [4] = 0;
          buf -> st_ctime = _secsince70 (fildes.fd_dcr);
          if ((buf -> st_rdev = buf -> st_dev = malloc(30)) != (char *) 0)
               {
               _gs_devn (fd, buf -> st_dev);
               }

          return (0);

          } /* end of fstat */

#ifndef _UCC


/***************************************************************************/
/*                                                                         */
/* perror() : Unix library (OS9/68000)                                     */
/* ========                                                                */
/*                                                                         */
/* Author:     K. Schmitt / Ch. Engel                                      */
/* Compiler:   Microware C Vers. 3.0                                       */
/* OS:         OS9/68000 Vers. 2.2                                         */
/*                                                                         */
/* Edition History                                                         */
/* ===============                                                         */
/*                                                                         */
/* Ed. 0.00  Date 11/11/88                                                 */
/*           First version                                                 */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Description                                                             */
/*                                                                         */
/*

     NAME
          perror, sys_errlist, sys_nerr, errno - system error messages

     SYNOPSIS
          perror (s)
          char *s;

          int sys_nerr;
          char *sys_errlist[ ];

          int errno;

     DESCRIPTION
          Perror produces a short error message on the standard error,
          describing the last error encountered during a system call
          from a C program.  First the argument string s is printed,
          then a colon, then the message and a new-line.  To be of
          most use, the argument string should be the name of the
          program that incurred the error.  The error number is taken
          from the external variable errno, which is set when errors
          occur and cleared when non-erroneous calls are made.

          To simplify variant formatting of messages, the vector of
          message strings sys_errlist is provided; errno can be used
          as an index in this table to get the message string without
          the new-line.  Sys_nerr is the largest message number
          provided for in the table; it should be checked because new
          error codes may be added to the system before they are added
          to the table.

*/

#include <ctype.h>


#define YES  1
#define NO   0
#define OK   0
#define ERR  -1

#define ERRFILE "/dd/SYS/errmsg"
#define MAXERR  256

extern int errno;

int sys_nerr = MAXERR;
char *sys_errlist [MAXERR];
static int errstat = OK;

char *malloc ();

perror (s)
          char *s;
          {
          static int first = YES;
          FILE *errfile;

          if (first)
               {
               first = NO;
               if ((errfile = fopen (ERRFILE, "r")) == NULL)
                    {
                    errstat = ERR;
                    }
               else
                    {
                    _build_errlist (errfile);
                    fclose (errfile);
                    }
               }

          if (errstat)
               fprintf (stderr, "%s: %03d:%03d\n",
                    s, errno >> 8, errno & 0x0ff);
          else
               fprintf (stderr, "%s: %s\n", s, sys_errlist [errno&(MAXERR-1)]);

          } /* end of perror */

#ifdef STATIC
static
#endif
_build_errlist (errfile)
          FILE *errfile;
          {
          char message [1024], line [80];
          register unsigned i;
          register char *p;

          for (i = 0; i < MAXERR ; i++)
               sys_errlist [i] = NULL;

          for (fgets (line, 80, errfile); ! feof (errfile); )
               {
               if (! isdigit (line [0]) || strlen (line) < 7)
                    {
                    fgets (line, 80, errfile);
                    continue;
                    }

               i = (atoi (line) << 8) | atoi (line + 4);
               for (p = line + 7; *p && isspace (*p); p++)
                    ;

               strcpy (message, p);
               fgets (line, 80, errfile);
               for ( ; line [0] != '\0' && ! isdigit (line [0]);
                    fgets (line, 80, errfile))
                    {
                    for (p = line; *p && isspace (*p); p++)
                         ;

                    strcat (message, p);
                    }

               if (i >= MAXERR) break;

               if ((sys_errlist [i] = malloc (strlen (message) + 1)) ==
                    NULL)
                    {
                    errstat = ERR;
                    return;
                    }
               else
                    {
                    strcpy (sys_errlist [i], message);
                    }
               }
          } /* end of _build_errlist */

FILE *tmpfile()
{
        char *ep;
        char fname[80];
        int done=0;
        int indx;
        char *make_path();
        char *tmp;
        extern char *getenv();
        FILE *fil, *fopen();
        int access();
                
        fname[0]='/';
        fname[1]='a';
        
        if ((ep = getenv("TMPDIR")) == NULLCHAR)
                        if ((ep = getenv("NETSPOOL")) == NULLCHAR)
                                perror("must set NETSPOOL env var\n");
        done=0;
        indx=0;
        while (fname[1]<'z' && !done)
        {   fname[2]='\0';
                
                strcat(fname,".net_tmp");
                tmp = make_path(ep,fname,0);
                if      (!access(tmp,0))
                {       indx=indx+1;
                        fname[1]=fname[1]+1;
                        free(tmp);
                }
                else
                {
                        if ((fil=fopen(tmp,"w+"))!=NULL)
                        {
                                tmpfils[indx].filptr=fil;
                                strcpy(tmpfils[indx].tmpnam,tmp);
                                done=1;
                        }
                        else
                        {
                                indx=indx+1;
                                fname[1]=fname[1]+1;
                        }
                }
        }
        if (done)
        {
                free(tmp);
                return fil;
        }
        else
                return NULLFILE;
}

int tmpclose(t)
FILE *t;
{
        int i=0;
        
        while(i<26)
        {
                if (tmpfils[i].filptr==t)
                {   
                        fclose(t);
                        unlink(tmpfils[i].tmpnam);
                        tmpfils[i].filptr=NULL;
                        return 0;
                }
                i++;
        }
/*      printf("attempted to delete non-tempfile\n"); unavoidable but not important */
        fclose(t);
        return 0;
}       
int rename(s,d)
char *s;
char *d;
{
        char *d2;
        char tmp[MAXCMD];
        int access();
        
        if (!access(d,0))
        {
                if (unlink(d))
                {
                        printf("Can't rename file (no permission to unlink dup dest)\n");
                        return -1;
                }
        }                       
    else
/* strip path from destination */
        {
                d2=d;
                while (*d != '\0')
                {
                        if(*d++ == '/')
                                d2=d;
                }
                sprintf(tmp,"rename %s %s",s,d2);
                system(tmp);
                return 0;
        }       
}

int fputc(c, fp)
int c;
FILE *fp;
{
        putc(c,fp);
}


/***************************************************************************/
/*                                                                         */
/* strtok() : Unix library (OS9/68000)                                     */
/* ========                                                                */
/*                                                                         */
/* Author:     Henry Spencer (UNIX stringlib from BIX)                     */
/* Compiler:   Microware C Vers. 3.0                                       */
/* OS:         OS9/68000 Vers. 2.2                                         */
/*                                                                         */
/* Edition History                                                         */
/* ===============                                                         */
/*                                                                         */
/* Ed. 0.00  Date 11/15/88                                                 */
/*           First version                                                 */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Description                                                             */
/*                                                                         */
/*

          char *strpbrk(s1, s2)
          char *s1, *s2;

          Strtok considers the string s1 to consist of a sequence of
          zero or more text tokens separated by spans of one or more
          characters from the separator string s2.  The first call 
          (with pointer s1 specified) returns a pointer to the first
          character of the first token, and will have written a NULL
          character into s1 immediately following the returned token.  
          Subsequent calls with zero for the first argument, will work 
          through the string s1 in this way until no tokens remain.
          The separator string s2 may be different from call to call.  
          When no token remains in s1, a NULL is returned. 
*/

/*#define NULL 0 */

char *scanpoint = NULL;
char *                         /* NULL if no token left */
strtok(s, delim)
          char *s;
          register char *delim;
          {
          register char *scan;
          char *tok;
          register char *dscan;

          if ((s == NULL) && (scanpoint == NULL))
               return(NULL);

          if (s != NULL)
               scan = s;
          else
               scan = scanpoint;

          /*
           * Scan leading delimiters.
           */
          for (; *scan != '\0'; scan++)
               {
               for (dscan = delim; *dscan != '\0'; dscan++)
                    {
                    if (*scan == *dscan)
                         break;
                    }

               if (*dscan == '\0')
                    break;
               }

          if (*scan == '\0')
               {
               scanpoint = NULL;
               return(NULL);
               }

          tok = scan;

          /*
           * Scan token.
           */
          for (; *scan != '\0'; scan++)
               {
               for (dscan = delim; *dscan != '\0';)    /* ++ moved down. */
                    {
                    if (*scan == *dscan++)
                         {
                         scanpoint = scan+1;
                         *scan = '\0';
                         return(tok);
                         }
                    }
               }

          /*
           * Reached end of string.
           */
          scanpoint = NULL;
          return(tok);

          } /* end of strtok */


#endif
/* E O F */
