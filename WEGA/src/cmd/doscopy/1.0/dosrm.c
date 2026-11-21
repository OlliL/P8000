/*   Loeschen von DOS-Dateien
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
    fprintf(stderr,"usage: dosrm dosdev:filename [dosdev:filename] [...]\n");
    exit(1);
 }
 while (argc-- > 0)   {
       if((res=dosplit(*++argv,mutos,dcp)) < 0)  {  
         status = 2;
         continue;
       }
       if(*dcp == '\0') {
         status = system(strcat("rm ",mutos));
         continue;
       }
           
       if((res=dosdev(mutos,2,&disc)) < 0) {
         fprintf(stderr,"%s -Fehler im Laufwerk %s \n",*arg0,mutos);
         status = 2;
         continue;
       }
         
       strcpy(h,dcp);
       if((res = dosdel(dcp,&disc)) < 1)
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
            case -3: fprintf(stderr,"%s -%s ist Verzeichnis oder schreibgeschuetzt \n",*arg0,h);
                     status = 2;
                     break;
           }
             

 }
 exit(status);
}
