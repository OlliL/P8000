/*   Kopieren von DOS-Dateien auf die Standardausgabe
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dos.h"


main(argc,argv)
char **argv;
{
 char **arg0;
 int conv = 1;      /*Newlinekonvertierung*/
 int status = 0,res;
 char mutos[40],dcp[50],h[50];
 struct dosdisc disc;
 struct dostat dstat;


 arg0 = argv;
 argc--;    
 for ( ; argc >0 && argv[1][0] == '-';argc--,argv++)
    switch (argv[1][1])  {
       case 0 : break;
       case 'r' : --conv;  /* keine Newlinekonv.*/
                  break;
       default : fprintf(stderr,"%s -falsche Option %c \n",*arg0,argv[1][1]);
                 exit(1);
    } 
   
 if (argc == 0)   {
    fprintf(stderr,"usage: doscat [-r] dosdev:filename [dosdev:filename] [...]\n");
    exit(1);
 }
 while (argc-- > 0)   {
       if((res=dosplit(*++argv,mutos,dcp)) < 0)  {  
         status = 2;
         continue;
       }
       if(*dcp == '\0') {
         status = system(strcat("cat ",mutos));
         continue;
       }
           
       if((res=dosdev(mutos,0,&disc)) < 0) {
         fprintf(stderr,"%s -Fehler im Laufwerk %s \n",*arg0,mutos);
         status = 2;
         continue;
       }
         
       strcpy(h,dcp);
       if((res = dosopn(dcp,&disc,&dstat)) < 1)
         if(res == 0) {
           fprintf(stderr,"%s -%s nicht gefunden \n",*arg0,h);
           status = 2;
         }
         else {
           fprintf(stderr,"%s -%s -Lesefehler \n",*arg0,mutos);
           status = 2;
         }
       else
	 if((res=dosout(&dstat,1,&disc,conv)) < 1)
           switch (res) {
            case -1: fprintf(stderr,"%s -%s -Lesefehler \n",*arg0,mutos);
                     status = 2;
                     break;
            case -2: fprintf(stderr,"%s -Fehler bei der Standardausgabe \n",*arg0);
                     status = 2;
                     break;
           }
             

 }
 exit(status);
}
