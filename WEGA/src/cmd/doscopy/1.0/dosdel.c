
#include "dos.h"


/* dosdel
 * Loescht die DCP-Datei fn auf dem Datentraeger discp .
 * Liefert 0 ,wenn nicht gefuden ,
 *        -1 ,bei Lesefehler ,
 *        -2 ,bei Schreibfehler und
 *        -3 ,falls fn ein Verzeichnis bzw
 *            schreibgeschuetzt ist .
*/

dosdel(fn,discp)
char *fn;
struct dosdisc *discp;

{int fatdel(),res,i,j,z=1;
 struct dostat fnrec,dirrec;
 long adr,pos;
 unsigned char delsign=0xe5;
 char s[50],*file,*p,*strchr();
 unsigned dnum,nxt,dosnxt();

 strcpy(s,fn);
 if((res=dosopn(fn,discp,&fnrec))<1)
   return(res);
 if(fnrec.mode == _SDIR || fnrec.flag == RO)
   return(-3);
 strcpy(fn,s);
 file = fn;
 while((p=strchr(file,'/')) != 0) 
    file = ++p;
 fn[strlen(fn)-strlen(file)-1] = '\0';

 if(file == fn) {
   dnum = discp->dirnum;
   adr = discp->maindir;
   nxt = 0;
 }
 else  { /* Das uebergeordnete Verzeichnis ist nicht das
          * Stammverzeichnis
         */
   if((res=dosopn(fn,discp,&dirrec)) < 1)
     return(res);
   adr = DOSADR(dirrec.first,discp->clustsiz)+discp->data;
   nxt = dirrec.first;
   /* Bestimmen der Anzahl von Verzeichnisclustern */
   while((nxt=dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen)) > 0)
      z++;
   dnum = discp->clustsiz/32;

 }
 /* Suchen des zu loeschenden Eintrages */
 for(i=0;i<z;i++) {
    if((res=dosrde(discp->fd,file,dnum,adr,&fnrec)) < 0)
      return(res);
    else {
      pos = lseek(discp->fd,(long)-32,1);
      if((res=write(discp->fd,&delsign,1)) != 1)
         return(-2);
      return(fatdel(&fnrec,discp));
    }
    if(res < 0)
      return(res);
    if(nxt != 0) {
       if((nxt=dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen)) < 1)
          break;
       /* naechsten Cluster lesen */
       adr = DOSADR(nxt,discp->clustsiz)+discp->data;
    }
 }

}



/* fatdel
 * Gibt die Belegungskette der DCP-Datei statp
 * frei .Liefert -1 bei Lese- und -2 bei Schreib-
 * fehler .  
*/

fatdel(statp,discp)
struct dostat *statp;
struct dosdisc *discp;

{int res,fatput();
 unsigned nxt;
 unsigned zero,last=0;
 unsigned go; 

 while((go=dosnxt(discp->fd,statp->first,discp->fat1,discp->fatelen))!=0) {

  nxt = statp->first;
  while(nxt) {
    zero = last;
    last = nxt;
    nxt = dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen);
  }
 
  if(zero == 0) {
   if((res=fatput(last,0x0000,discp))<0)
     return(res);
   if((res=fatput(statp->first,0xffff,discp))<0)
     return(res);
  }
  else  {
   if((res=fatput(zero,0xffff,discp))<0)
     return(res);
   if((res=fatput(last,0x0000,discp))<0)
     return(res);
  }
 last = 0;
 }

 return(fatput(statp->first,0x0000,discp));

}


/* fatput
 * Schreibt in alle FAT-Versionen fuer den Wert          
 * des Eintrages ent den Wert val .
 * Liefert -1 bei Lesefehler
 * und -2 bei Schreibfehler .     
*/

fatput(ent,val,discp)
unsigned ent,val;
struct dosdisc *discp;

{
 unsigned r,nxt1,nxt2,nxt;
 long pos,fat=discp->fat1;
 unsigned char b[3],b16[2],d[3];
 int n,len;

for(n=discp->fatnum;n>=1;n--) {

 if(discp->fatelen == 12) {
     val &= 0x0fff;
   
     r = ent * 1.5;
     if(ent % 2 == 0)
        pos = lseek(discp->fd,fat+r,0);
     else
        pos = lseek(discp->fd,fat+r-1,0);
     len=read(discp->fd,b,3);
     if (len<3)
       return(-1);
     nxt1 = (b[1]%16)*256 + b[0];
     nxt2 = b[2]*16 + b[1]/16;

     pos = lseek(discp->fd,(long)-3,1);
     if(ent % 2 == 0) {
	 b[0] = val % 256;
         b[1] = (b[1]/16)*16+(val/256);
     }
     else  {
         b[2] = val/16;
	 b[1] = (val % 16)*16+(b[1]&0x0f);
     }
	r=write(discp->fd,b,3);
     if( r != 3 )
            return(-2);
 }
 else  {
    r = ent * 2;
    pos = lseek(discp->fd,fat+r,0);
    len=read(discp->fd,b16,2);
    if( len <2)
        return(-1);
    nxt = b16[1]*256+b16[0];
    pos = lseek(discp->fd,(long)-2,1);
    read(discp->fd,d,2);
   d[0] = val% 256;
   d[1] = val/256;
 }
 fat = (discp->maindir-fat)/n+fat;
 }
 return(1);
}

