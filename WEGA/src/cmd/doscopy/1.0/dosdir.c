/* dosdir
 * Listet DCP-Dateien und -Verzeichnisse
 * in DCP-Format auf
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
 int z=1,i,j,dcnt;
 unsigned char mutos[40],dcp[50],h[50],c;
 char label[12];
 struct dosdisc disc;
 struct dostat dstat,estat;
 long adr,pos;
 unsigned dnum,dosnxt(),nxt;

 arg0 = argv;
 argc--;
 if(argc == 0) {
   fprintf(stderr,"usage: dosdir dosdev:[dirname] [dosdev:[dirname]] [...]\n");
   exit(1);
 }
 while (argc-- > 0)  {
     dcnt = 0;
     label[0] = '\0';
     if((res=dosplit(*++argv,mutos,dcp)) < 0) {
       status = 2;
       continue;
     }
     
     if(res == 2) {     
       fprintf(stderr,"%s -kein DCP-Pfad \n",*arg0);
       continue;
     }

     if((res=dosdev(mutos,0,&disc)) < 0) {
       fprintf(stderr,"%s -Fehler im Laufwerk %s \n",*arg0,mutos);
       status = 2;
       continue;
     }


       adr = disc.maindir;
       dnum = disc.dirnum;
     
     pos = lseek(disc.fd,adr,0);
    
     nxt = dstat.first;
        j = 1;
        while(j++ <= dnum && (res=getde(disc.fd,&estat)) > 0)  
          if((c=estat.name[0]) != 0xe5 && (estat.attr&VOLUME_LABEL) != 0) {
             strcpy(label,estat.name);
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
         pkopf(mutos,h,label);
         pdir(&dstat);
         bytefree(1,&disc);
         continue;
       }
       if(dstat.mode == _SDIR) {
         pkopf(mutos,h,label);
         adr = DOSADR(dstat.first,disc.clustsiz)+disc.data;
         /* Bestimmen Anzahl von Verzeichniseintraegen */
         nxt = dstat.first;
         while((nxt=dosnxt(disc.fd,nxt,disc.fat1,disc.fatelen)) > 0)
              z++;
         dnum = disc.clustsiz/32;
       }
     }
     else { /* Ausgabe des Stammverzeichnisses */
       pkopf(mutos, '\0', label);
       adr = disc.maindir;
       dnum = disc.dirnum;
     }
     pos = lseek(disc.fd,adr,0);
     /* Ausgabe aller gueltigen Eintraege */
     nxt = dstat.first;
     for(i=0;i<z;i++) {
        j = 1;
        while(j++ <= dnum && (res=getde(disc.fd,&estat)) > 0)  
          if((c=estat.name[0]) != 0xe5 && (estat.attr&VOLUME_LABEL) == 0) {
             strcpy(h,estat.name);
             estat.name = h;
             pdir(&estat);
             dcnt += 1;
          }
        if((nxt=dosnxt(disc.fd,nxt,disc.fat1,disc.fatelen)) < 1)
          break;
        else { /* Naechsten Cluster bereitstellen */
          adr = DOSADR(nxt,disc.clustsiz)+disc.data;
          pos = lseek(disc.fd,adr,0);
        }
     }
     bytefree(dcnt,&disc);

 } /* while */

 exit(status);
}





pkopf(lw,dcp, label)
char *lw,*dcp, *label;
{
 printf("\n Volume in Laufwerk %s hat Label: %s \n",lw, label);
 if(dcp != '\0')
   printf(" Verzeichnis von %s:/%s \n \n",lw,dcp);
 else
   printf(" Verzeichnis von %s:/ \n \n",lw);
}

pdir(pstat)
struct dostat *pstat;
{
 if(pstat->mode == _SDIR) {
   printf("%8.8s%3.3s ",pstat->name, &pstat->name[8]);
   printf("  <DIR>   ");
 } else {
   printf("%8.8s %3.3s",pstat->name, &pstat->name[8]);
   printf("%9ld ",pstat->dim);
 }
 printf("%2d-%02d-%2d ",pstat->date.month,pstat->date.day,pstat->date.year-1900);
 printf("%2d:%2d \n",pstat->time.hour,pstat->time.min);

}
 


      
/* bytefree
 * Summiert alle FAT-Eintraege groesser Null
 * und gibt die so ermittelte Anzahl freier
 * Byte auf dem Bildschirm aus
*/

bytefree(dcnt,discp) 
int dcnt;  /* Anz. Dateien im Verzeichnis */
struct dosdisc *discp;
{
 long pos,free;
 unsigned char n12[3],n16[2];
 unsigned n1,n2,fatsiz,z=1,used=0;
 int len;

 if(discp->fatelen == 12) {
   fatsiz = (discp->maindir - discp->fat1)/discp->fatnum -3;
   /* Anz. gueltiger Bytes je FAT */

   pos = lseek(discp->fd,(long)discp->fat1+3,0); 

   while(fatsiz/3 > z++)  {
     if((len=read(discp->fd,n12,3)) < 3)
       return; 
     n1 = (n12[1]&0x0f)*256 + n12[0];
     n2 = n12[2]*16 + n12[1]/16;
     if(n1 != 0)
       used++;
     if(n2 != 0)
       used++;
   }
 }
 else  {
   fatsiz = (discp->maindir - discp->fat1)/discp->fatnum - 4;
   /* Anz. gueltiger Bytes je FAT */

   pos = lseek(discp->fd,(long)discp->fat1+4,0);
 
   while(fatsiz/2 > z++)  {
     if((len = read(discp->fd,n16,2)) < 2)
       return;
     n1 = (n16[1]*256) + n16[0];
     if(n1 != 0)
       used++;
   }
 }
 free = (long)discp->vol*512 - (discp->data + (long)discp->clustsiz*used);    
 if(free < 0)
   printf("       %d Datei(en)    ??? Bytes frei \n",dcnt);
 else      
   printf("       %d Datei(en)    %ld Bytes frei \n",dcnt,free);
}
