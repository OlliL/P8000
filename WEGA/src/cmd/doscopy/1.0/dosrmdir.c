/*   Loeschen von DOS-Verzeichnissen
*/

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
    fprintf(stderr,"usage: dosrmdir dosdev:dirname\n");
    exit(1);
 }
 while (argc-- > 0)   {
       if((res=dosplit(*++argv,mutos,dcp)) < 0)  {  
         status = 2;
         continue;
       }
       if(res == 2) {
         status = system(strcat("rmdir ",mutos));
         continue;
       }
       if(*dcp == '\0') {
         fprintf(stderr,"%s -falsches Verzeichnis angegeben \n",*arg0);
         status = 2;
         continue;
       }
           
       if((res=dosdev(mutos,2,&disc)) < 0) {
         fprintf(stderr,"%s -Fehler im Laufwerk %s \n",*arg0,mutos);
         status = 2;
         continue;
       }
         
       strcpy(h,dcp);
       if((res = deldir(dcp,&disc)) < 1)
           switch (res) {
            case  0: fprintf(stderr,"%s -%s nicht gefunden \n",*arg0,h);
                     status = 2;
                     break;
            case -1: fprintf(stderr,"%s -%s -Lesefehler \n",*arg0,mutos);
                     status = 2;
                     break;
            case -2: fprintf(stderr,"%s -%s -Schriebfehler \n",*arg0,mutos);
                     status = 2;
                     break;
            case -3: fprintf(stderr,"%s -%s ist kein Verzeichnis oder schreibgeschuetzt \n",*arg0,h);
                     status = 2;
                     break;
            case -4: fprintf(stderr,"%s -%s ist nicht leer \n",*arg0,h);
                     status = 2;
                     break;
           }
             

 }
 exit(status);
}



/* deldir
 * Loescht das DCP-Verzeichnis dir auf dem Datentraeger discp .
 * Liefert 0 ,wenn nicht gefuden ,
 *        -1 ,bei Lesefehler ,
 *        -2 ,bei Schreibfehler 
 *        -3 ,falls dir kein Verzeichnis bzw
 *            schreibgeschuetzt  und
 *        -4 ,falls dir nicht leer ist.
*/

deldir(dir,discp)
char *dir;
struct dosdisc *discp;

{int fatdel(),res,i,z=1,empty=1;
 struct dostat fnrec,dirrec;
 long adr,pos,eadr;
 unsigned char c,delsign=0xe5;
 char s[50],*file,*p,*strchr();
 unsigned dnum,nxt,dosnxt();

 strcpy(s,dir);
 if((res=dosopn(dir,discp,&fnrec))<1)
   return(res);
 if(fnrec.mode != _SDIR || fnrec.flag == RO)
   return(-3);
 strcpy(dir,s);
 file = dir;
 while((p=strchr(file,'/')) != 0) 
    file = ++p;
 dir[strlen(dir)-strlen(file)-1] = '\0';

 if(file == dir) {
   dnum = discp->dirnum;
   adr = discp->maindir;
   nxt = 0;
 }
 else  { /* Das uebergeordnete Verzeichnis ist nicht das
          * Stammverzeichnis
         */
   if((res=dosopn(dir,discp,&dirrec)) < 1)
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
    if((res=dosrde(discp->fd,file,dnum,adr,&dirrec)) < 0)
      return(res);
    else 
      eadr = lseek(discp->fd,(long)-32,1);
    if(nxt != 0) {
       if((nxt=dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen)) < 1)
          break;
       /* naechsten Cluster lesen */
       adr = DOSADR(nxt,discp->clustsiz)+discp->data;
    }
 }

 adr = DOSADR(fnrec.first,discp->clustsiz)+discp->data;
 nxt = fnrec.first;
 z = 1;
 while((nxt=dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen)) > 0)
      z++;
 pos = lseek(discp->fd,adr,0);
 for(i=0;i<z;i++) {
    while((res=getde(discp->fd,&dirrec)) > 0 && empty)
         if((c=dirrec.name[0]) != 0xe5 && c != '.')
           empty--;
    if(res < 0)
      return(res);
    if(!empty)
      return(-4);
    if(res == 0)
      break;
    if((nxt=dosnxt(discp->fd,nxt,discp->fat1,discp->fatelen)) < 1)
      break;
    else {
      adr = DOSADR(nxt,discp->clustsiz)+discp->data;
      pos = lseek(discp->fd,adr,0);
    }
 }
 
 /* Verzeichnis ist leer -> als geloescht markieren */
 pos = lseek(discp->fd,eadr,0);

 if((res=write(discp->fd,&delsign,1)) != 1)
   return(-2);
 return(fatdel(&fnrec,discp));

}




