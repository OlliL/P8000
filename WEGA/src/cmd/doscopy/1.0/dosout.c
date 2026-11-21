#include "dos.h"
#define bufsize 1024

/* dosout
 * Gibt die Datei ,welche durch 'file'
 * gekennzeichnet wird , in das File 
 * 'out' aus .
 * 'opt' zeigt Newlinekonvertierung an.
 * dosout liefert -1 bei Lesefehler und
 *                -2 bei Schreibefehler
*/


static unsigned char buf[bufsize],outbuf[bufsize];

dosout(file,out,disc,opt)
struct dostat *file;
int out;
struct dosdisc *disc;
int opt;

{
 long z; 
 unsigned i,j,length;
 long adr,pos,clcnt=1;
 unsigned next=file->first;
 int r;

 z=1;
 
 adr = DOSADR(next,disc->clustsiz) + disc->data;
 pos = lseek(disc->fd,adr,0);

 while(next != 0)  {

	length=read(disc->fd,buf,bufsize);
    if(length < 0)
      return(-1);

    j=0;
    for(i=0;i<=bufsize-1 && z <= file->dim && z++ <= clcnt*disc->clustsiz;i++)  {      
     if(opt)  {
       if(i == bufsize-1  &&  buf[i] == 0x0d)
          buf[i] = 0x20;
       if(buf[i] == 0x0d)
          if(buf[++i] == 0x0a)  {
              outbuf[j] = buf[i];
              z++;
          }
          else 
              outbuf[j] = buf[--i];
       else
          outbuf[j] = buf[i];
     }
     else   /* keine Newlinekonvertierung */
       outbuf[j] = buf[i];
     j++;
    }

    if((r=write(out,outbuf,j)) != j)
      return(-2);

    if(z > clcnt*disc->clustsiz || z > file->dim) {
      next = dosnxt(disc->fd,next,disc->fat1,disc->fatelen);
      adr = DOSADR(next,disc->clustsiz) + disc->data;
      pos = lseek(disc->fd,adr,0);

      clcnt++;
    } 
 }
    return(1);
}   
 
 
