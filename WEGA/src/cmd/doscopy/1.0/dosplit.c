#include <stdio.h>
#include <ctype.h>
#define STANDARD "/etc/default/msdos"
#define MAX 50

/* dosplit
 * Teilt einen Pfad in die DCP- und
 * MUTOS-Bestandteile.
 * Dabei wird das Verzeichnis /etc/default/msdos
 * ausgewertet und im DCP-Pfad werden
 * Klein- in Grossbuchstaben umgewandelt.
 * Bei Fehler liefert dosplit -1
*/


dosplit(path,mutos,dcp)
char *path,*mutos,*dcp;

{char *p,line[MAX],lw,*strchr();
 FILE *fp;
 int found=0;

 strcpy(mutos,path);
 *dcp = '\0';

 if((p=strchr(path,':')) == '\0')  
   /* kein DCP-Pfad */
   return(2); /* keine DCP-Datei angegeben */

 if(strlen(path) - strlen(p)  ==  1)  {
     /* vor ':' ist Laufwerksbuchstabe angegeben */

     if((fp=fopen(STANDARD,"r")) == '\0')  {
       fprintf(stderr,"%s   nicht eingerichtet \n",STANDARD);
       return(-1);
     }
     p--;
     lw = toupper(*p);
     p += 2;
     if(*p == '/')
       p++;
     while((fgets(line,MAX,fp)) != '\0'  && !found)
       if(*line == lw)  {
         line[strlen(line)-1] = '\0';
         strcpy(mutos,line+2);
         found++;
       }
     if(!found)  {
       fprintf(stderr,"Laufwerk %c: nicht definiert\n",lw);
       return(-1);
     }
  }
  else  {
     mutos[strlen(path)-strlen(p)] = '\0';
     ++p;
     if(*p == '/')
       p++;           
  }
   strcpy(dcp,p);
   while(*dcp != '\0')
     *dcp++ = islower(*dcp) ? toupper(*dcp) : *dcp;
   /* Der DCP-Pfad enthaelt mindestens das Stammverzeichnis */
   return(1);
}
