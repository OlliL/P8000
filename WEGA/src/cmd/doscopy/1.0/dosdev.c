/* Bereitstellen von DCP-Disk Informationen */
#include "dos.h"

/* dosdev() oeffnet DCP-Geraetefiles und 
 * belegt Struktur dosdisc mit gueltigen Werten
*/

dosdev(fn,mode,parm_buf)
char *fn;
int mode;
struct dosdisc *parm_buf;
{
 int length,bps;
 long pos;
 unsigned char bootinf[17];       /*Boot-Record-Informationen*/
 
 if ((parm_buf->fd=open(fn,mode)) < 0)  
    return(-1);
 
 
 pos = lseek(parm_buf->fd,(long)11,0);
 if((length = read(parm_buf->fd,bootinf,17)) != 17)  
    return(-1);
 

 parm_buf->fat1 = (bootinf[4]*256+bootinf[3])*(bootinf[1]*256+bootinf[0]);
 parm_buf->fatnum = bootinf[5];
 parm_buf->maindir = ((bootinf[4]*256+bootinf[3])+(bootinf[12]*256+bootinf[11])                     *parm_buf->fatnum)*(bootinf[1]*256+bootinf[0]);
 parm_buf->dirnum = bootinf[7]*256+bootinf[6];
 parm_buf->data = parm_buf->maindir+(parm_buf->dirnum * 32);
 parm_buf->clustsiz = bootinf[2]*(bootinf[1]*256+bootinf[0]);
 parm_buf->vol = bootinf[9]*256 + bootinf[8];
 
 bps = bootinf[1]*256+bootinf[0];
 if((parm_buf->vol-(parm_buf->data / bps)/bootinf[2]) > 4095)
      parm_buf->fatelen =16;
 else parm_buf->fatelen =12;

 return(0);
}



