/*   Erzeugen von DCP-Verzeichnissen
*/
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dos.h"


main(argc,argv)
char **argv;
{
 char **arg0;
 int status = 0,res;
 char mutos[40],dcp[50],h[50];
 struct dosdisc disc;
 struct dostat dstat;


 arg0 = argv;
 argc--;    
   
 if (argc == 0)   {
    fprintf(stderr,"usage: dosmkdir dosdev:dirname\n");
    exit(1);
 }
 while (argc-- > 0)   {
       if((res=dosplit(*++argv,mutos,dcp)) < 0)  {  
         status = 2;
         continue;
       }
       if(*dcp == '\0') {
         fprintf(stderr,"%s -kein DCP-Pfad angegeben \n",*arg0);
         status = 2;
         continue;
       }
           
       if((res=dosdev(mutos,2,&disc)) < 0) {
         fprintf(stderr,"%s -Fehler im Laufwerk %s \n",*arg0,mutos);
         status = 2;
         continue;
       }
         
       strcpy(h,dcp);
       if((res = msubdir(dcp,&disc)) < 1)
           switch (res) {
            case  0: fprintf(stderr,"%s -uebergeordnetes Verzeichnis fehlt \n",*arg0);
                     status = 2;
                     break;
            case -1: fprintf(stderr,"%s -%s -existiert bereits \n",*arg0,h);
                     status = 2;
                     break;
            case -2: fprintf(stderr,"%s -%s -Schriebfehler \n",*arg0,mutos);
                     status = 2;
                     break;
           }
             

 }
 exit(status);
}








/* msubdir
 * Traegt das Unterverzeichnis subdir  
 * auf dem Datentraeger discp ein.        
 * Liefert 0 falls das uebergeordnete 
 * Verzeichnis nicht existiert ,
 *        -1 falls subdir bereits existiert und
 *        -2 bei Schreibfehler
*/

msubdir(subdir,discp)

char *subdir;
struct dosdisc *discp;

{struct dostat fnrec,dirrec,pkt,dpkt;
 int res,z=1,i,j,io=1;
 char *p,*file,s[60],nam[13],namp[13],namdp[13];
 char *strchr();
 long adr,depos,pos,time(),zeit;
 unsigned dnum,nxt,free,dosnxt(),fatfree(),dpktfirst;   
 unsigned char c,buf[512];  
 struct tm *sttime,*localtime();
 int dmax,dcnt=0;

 strcpy(s,subdir);
 if((res=dosopn(subdir,discp,&fnrec)) < 0)
   return(-2);
 if(res > 0)  
     return(-1);
 

 strcpy(subdir,s);
 file=subdir;
 while((p=strchr(file,'/')) != 0) 
      file = ++p;
 
 subdir[strlen(subdir)-strlen(file)-1] = '\0';

 if(file == subdir) {
   dmax = dnum = discp->dirnum;
   adr = discp->maindir;
   dpktfirst = nxt = 0;
 }
 else { /* Das uebergeordnete Verzeichnis ist nicht das
         * Stammverzeichnis !
        */
   if((res=dosopn(subdir,discp,&dirrec)) < 1)
     return(res);
   if(dirrec.mode != _SDIR)
     return(0);
   dpktfirst = dirrec.first;
   adr = DOSADR(dirrec.first,discp->clustsiz)+discp->data;
   /* Bestimmen der Anzahl von Verzeichniseintraegen */
   nxt = dirrec.first;
   while((nxt=dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen)) > 0)
         z++;
   dnum = discp->clustsiz/32;
   dmax = z*dnum;
   nxt = dirrec.first;
 }

 pos = lseek(discp->fd,adr,0);
 /* Suche des ersten freien Eintrages */
 for(i=0;i<z;i++) {
    j = 1;
    while(j++ <= dnum && (res=getde(discp->fd,&fnrec)) > 0 ) {
       dcnt++;
       if((c=fnrec.name[0]) == 0xe5) { 
         i = z;
         depos = lseek(discp->fd,(long)-32,1);
         break;
       }
    }
    if(res == 0) {
      depos = lseek(discp->fd,(long)-32,1);
      break;
    }
    if(nxt != 0) {
      if((nxt=dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen)) < 1)
        break;
      else { /* Naechsten Cluster lesen */
        adr = DOSADR(nxt,discp->clustsiz)+discp->data;
        pos = lseek(discp->fd,adr,0);
        dirrec.first = nxt;
      }
    }
 }
 
 if(res < 0)
   return(-2);
 if(subdir == file && dcnt >= dmax)
   return(-2);       /* Stammverzeichnis voll */
 if(dcnt >= dmax) {  /* Verzeichniscluster voll */
   free = fatfree( discp );
   if(free < 0)
     return(-2);
   if(fatput(dirrec.first,free,discp) < 0)
     return(-2);
   if(fatput(free,0xffff,discp) < 0)
     return(-2);
   adr = DOSADR(free,discp->clustsiz)+discp->data;
   for(i=0;i<512;i++)
      buf[i] = 0x00;
   pos = lseek(discp->fd,adr,0);
   z = discp->clustsiz/512;
   for(i=0;i<z;i++)
      if((res=write(discp->fd,buf,512)) != 512) 
        return(-2);
 /* depos zeigt auf den zu belegenden Eintrag */
   depos = lseek(discp->fd,adr,0);
 }
 


 /* fnrec wird mit Werten von subdir belegt */
 if((p=strchr(file,'.')) != 0) {
   p++;
   
   file[strlen(file)-strlen(p)-1] = '\0';
   for(i=0;i<8;i++)
    if(i >= strlen(file))
      fnrec.name[i] = 0x20;
    else
      fnrec.name[i] = file[i];
   for(i=8;i<11;i++)
    if(i-7 > strlen(p))
      fnrec.name[i] = 0x20;
    else
      fnrec.name[i] = p[i-8];
 }
 else
    for(i=0;i<11;i++)
     if(i >= strlen(file) || i > 7)
       fnrec.name[i] = 0x20;
     else
       fnrec.name[i] = file[i];

 strcpy(nam,fnrec.name);
 fnrec.name=nam;

 fnrec.mode = _SDIR;
 fnrec.flag = RW;
 zeit = time(0);
 sttime = localtime(&zeit);

 fnrec.time.sec = sttime->tm_sec;
 fnrec.time.min = sttime->tm_min;
 fnrec.time.hour = sttime->tm_hour;

 fnrec.date.day = sttime->tm_mday;
 fnrec.date.month = sttime->tm_mon+1;
 fnrec.date.year = sttime->tm_year;

   free = fatfree( discp );
 if(free < 0)
   return(-2);
 fnrec.first = free;
 fatput(free,0xffff,discp); 
 fnrec.dim = 0;

 /* Uebernehmen der Eintraege . und .. */
 adr = DOSADR(free,discp->clustsiz)+discp->data;
 pkt.name = namp;
 for(i=0;i<11;i++)
  if(i == 0)
    pkt.name[i] = 0x2e;
  else
    pkt.name[i] = 0x20;
 pkt.name[11] = '\0';
 pkt.mode = _SDIR;
 pkt.flag = RW;
 dpkt.time.sec = pkt.time.sec = fnrec.time.sec;
 dpkt.time.min = pkt.time.min = fnrec.time.min;
 dpkt.time.hour = pkt.time.hour = fnrec.time.hour;
 dpkt.date.day = pkt.date.day = fnrec.date.day;
 dpkt.date.month = pkt.date.month = fnrec.date.month;
 dpkt.date.year = pkt.date.year = pkt.date.year;
 pkt.dim = 0;
 pkt.first = fnrec.first;

 dpkt.name = namdp;
 dpkt.name[0] = 0x2e;
 dpkt.name[1] = 0x2e;
 for(i=2;i<11;i++)
    dpkt.name[i] = 0x20;
 dpkt.name[11] = '\0';
 dpkt.mode = _SDIR;
 dpkt.flag = RW;
 dpkt.dim = 0;
 dpkt.first = dpktfirst;

 pos = lseek(discp->fd,adr,0);
 for(i=0;i<512;i++)
    buf[i] = 0x00;
 z = discp->clustsiz/512;
 for(i=0;i<z;i++)
    if((res=write(discp->fd,buf,512)) != 512)
      return(-2);

 pos = lseek(discp->fd,adr,0);
 if((res=putdir(&pkt,discp)) < 0)
   io--;
 if(io)
   if((res=putdir(&dpkt,discp)) < 0)
     io--;
 if(!io)  {
   res=fatdel(&fnrec,discp);
   return(-2);
 }

 /* Screiben des Verzeichniseintrages subdir */
 pos = lseek(discp->fd,depos,0);
 if((res=putdir(&fnrec,discp))<0)
    return(-2);
 return(1);
}





putdir(statp,discp)
struct dostat *statp;
struct dosdisc *discp;

{int res,i;
 unsigned char attrib=0x10,*h,buf[32];
 unsigned tv;

 for(i=0;i<11;i++)
    buf[i] = statp->name[i];
 buf[11] = attrib;
 for(i=12;i<22;i++) 
    buf[i] = '\0';

 tv = statp->time.hour*2045 + statp->time.min*32 + statp->time.sec/2;
/*
 h = &tv;
 buf[22] = *h++;
 buf[23] = *h;
*/
	buf[22] = tv % 256;
	buf[23] = tv / 256;
 tv = (statp->date.year-80)*512 + statp->date.month*32 + statp->date.day;
/*
 h = &tv;
 buf[24] = *h++;
 buf[25] = *h;
*/
	buf[24] = tv % 256;
	buf[25] = tv / 256;
/*
 h = &(statp->first);
 buf[26] = *h++;
 buf[27] = *h;
*/
	buf[26] = statp->first % 256;
	buf[27] = statp->first / 256;
/*
 h = &(statp->dim);
 buf[28] = *h++;
 buf[29] = *h++;
 buf[30] = *h++;
 buf[31] = *h;
*/
	buf[28] = statp->dim % 256;
	buf[29] = ( statp->dim % 65536 ) / 256;
	buf[30] = ( statp->dim % 16777216 ) / 65536;
	buf[31] = ( statp->dim / 16777216 );
 if((res=write(discp->fd,buf,32)) != 32)
   return(-2);
 return(1);
}
