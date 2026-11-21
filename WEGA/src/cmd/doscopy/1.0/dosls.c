/* dosls
 * Listet DCP-Dateien und -Verzeichnisse
 * in MUTOS-Format auf
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
 int z=1,i,j;
 unsigned char mutos[40],dcp[50],h[50],c;
 struct dosdisc disc;
 struct dostat dstat,estat;
 long adr,pos;
 unsigned dnum,dosnxt(),nxt;

 arg0 = argv;
 argc--;
 if(argc == 0) {
   fprintf(stderr,"usage: dosls dosdev:[dirname] [dosdev:[dirname]] [...]\n");
   exit(1);
 }
 while (argc-- > 0)  {
     if((res=dosplit(*++argv,mutos,dcp)) < 0) {
       status = 2;
       continue;
     }
     
     if(res == 2) {     
       status = system(strcat("l ",mutos));
       continue;
     }

     if((res=dosdev(mutos,0,&disc)) < 0) {
       fprintf(stderr,"%s -Fehler im Laufwerk %s \n",*arg0,mutos);
       status = 2;
       continue;
     }
     
     if(*dcp != '\0')  {
       /* Es handelt sich nicht um das Stammverzeichnis */
       strcpy(h,dcp);
       if((res=dosopn(dcp,&disc,&dstat)) < 1) {
         if(res == 0) {
           fprintf(stderr,"%s -%s nicht gefunden \n",*arg0,h);
           status = 2;
         }
         else {
           fprintf(stderr,"%s -%s -Lesefehler \n",*arg0,mutos);
           status = 2;
         }
         continue;
       }

       if(dstat.mode == _FILE) {
         pls(&dstat);
         continue;
       }
       if(dstat.mode == _SDIR) {
         printf("\n%s \n",h);
         adr = DOSADR(dstat.first,disc.clustsiz)+disc.data;
         /* Bestimmen Anzahl von Verzeichniseintraegen */
         nxt = dstat.first;
         while((nxt=dosnxt(disc.fd,nxt,disc.fat1,disc.fatelen)) > 0)
              z++;
         dnum = disc.clustsiz/32;
       }
     }
     else { /* Ausgabe des Stammverzeichnisses */
       printf("\n%s \n",mutos);
       adr = disc.maindir;
       dnum = disc.dirnum;
     }
     pos = lseek(disc.fd,adr,0);
     /* Ausgabe aller gueltigen Eintraege */
     nxt = dstat.first;
     for(i=0;i<z;i++) {
        j = 1;
        while(j++ <= dnum && (res=getde(disc.fd,&estat)) > 0)  
          if((c=estat.name[0]) != 0xe5) {
             strcpy(h,estat.name);
             estat.name = h;
             pls(&estat);
          }
        if((nxt=dosnxt(disc.fd,nxt,disc.fat1,disc.fatelen)) < 1)
          break;
        else { /* Naechsten Cluster bereitstellen */
          adr = DOSADR(nxt,disc.clustsiz)+disc.data;
          pos = lseek(disc.fd,adr,0);
        }
     }
 

 } /* while */
 exit(status);
}


pls(pstat)
struct dostat *pstat;
{
 if(pstat->mode == _SDIR)
   printf("d");
 else
   printf("-");
 printf("rw-rw-r-- 1 dcp        ");
 printf("%6ld ",pstat->dim);
 printf("%2d %2d %2d:%2d ",pstat->date.month,pstat->date.day,pstat->time.hour,pstat->time.min);
 printf("%s \n",pstat->name);

}
       
