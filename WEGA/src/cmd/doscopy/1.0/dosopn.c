
#include "dos.h"


/* dosopn
 * Durchmustert ein DCP-Verzeichnis ,
 * uebergibt die Adresse  einer Struktur dostat
 * fuer die durch path beschriebene Datei.
 * Liefert -1 bei Fehler und 0 wenn nicht gefunden
*/


dosopn(path,discp,statp)

char *path;
struct dosdisc *discp;
struct dostat *statp;

{
 char n[9],*p,*dir;
 long dadr;
 unsigned next=0,dosnxt();
 int res,dn,i,z=1;
 char *strchr();

 dn = discp->dirnum;
 dadr = discp->maindir;

 while((p=strchr(path,'/')) != 0) {
      dir = path;
      dir[strlen(path)-strlen(p)] = '\0';
      path = ++p;
      strcpy(n,dir);
      for(i=0;i<z;i++)  {
        if((res=dosrde(discp->fd,n,dn,dadr,statp)) < 0)
           return(res);
        if(next != 0) {
         if((next=dosnxt(discp->fd,next,discp->fat1,discp->fatelen)) < 1)
          break;
          dadr = DOSADR(next,discp->clustsiz)+discp->data;
        }
      }
      if(res == 0)
        return(res);

      dadr = DOSADR(statp->first,discp->clustsiz)+discp->data;
      next = statp->first;

      z = 1;
      while((next=dosnxt(discp->fd,next,discp->fat1,discp->fatelen)) > 0)
          z++;
      dn = discp->clustsiz/32;
 }
 
 for(i=0;i<z;i++) {
    if((res=dosrde(discp->fd,path,dn,dadr,statp)) < 0)
      return(res);
    if((next=dosnxt(discp->fd,next,discp->fat1,discp->fatelen)) < 1)
      break;
    dadr = DOSADR(next,discp->clustsiz)+discp->data;
 }
 return(res);
}

 

/* dosrde 
 * Sucht einen Verzeichniseintrag und       
 * belegt eine Struktur dostat mit Werten
*/

dosrde(fd,fn,dnum,adr,stat_b)
int fd;               
char *fn;      /* Name des zu suchenden DCP-Files */
int dnum;      /* max. moegliche Anzahl von Verzeichniseintraegn */
long adr;      /* Startadresse des Verzeichnisses */
struct dostat *stat_b;

{
 long pos;
 int length;
 int i,z=1;
 int cmp();
 char *strchr(),*ext,*sbext,sbe[4],nam[9],c='\0';

 pos=lseek(fd,(long)(adr),0);
 
 ext=strchr(fn,'.');
 if (ext != 0)  {
    ext++;
    fn[strlen(fn)-strlen(ext)-1]='\0';
 }
 else
    ext = &c;

 sbext=sbe;

 while((length=getde(fd,stat_b)) > 0  && z++<=dnum )   {
   for(i=0;i<8;i++)
      nam[i]=stat_b->name[i];
   nam[8]='\0';
   for(i=8;i<12;i++)
      sbext[i-8]=stat_b->name[i];
   if(!cmp(fn,nam) && !cmp(ext,sbext)) 
      return(length);
   
   
 }
   return(length);  

}




/* getde
 * liest einen 32-Byte Verzeichniseintrag und belegt
 * dostat mit den Dateiparametern.
 * Beachte: Der Bytezaehler muss vorher auf das erste
 * Byte des Eintrages zeigen !
 * Die Funktion liefert 0, wenn keine gueltigen Eintraege
 * mehr im Verzeichnis stehen und -1 bei Lesefehler.
*/


getde(fd,stat_buf)

int fd;
struct dostat *stat_buf;

{
 unsigned char dirbuf[32];
 char nam[13];
 int len,i;
 unsigned date,time;

 if ((len=read(fd,dirbuf,32))!=32) 
    return(-1);
  

 stat_buf->name = nam;

 for(i=0;i<12;i++) 
   nam[i]=dirbuf[i];
 

 if(nam[0]=='\0')
   return(0);

 nam[11] = '\0';

 if(dirbuf[11]&0x01)
   stat_buf->flag = RO;
 else
   stat_buf->flag = RW;
 stat_buf->attr = dirbuf[11];
 if(dirbuf[11]&0x02)
   stat_buf->mode = _BLOCKED;
 else if(dirbuf[11]&0x04)
   stat_buf->mode = _SYSDAT;
 else if(dirbuf[11]&0x10)
   stat_buf->mode = _SDIR;
 else
 	stat_buf->mode = _FILE;

 stat_buf->first=dirbuf[27]*256+dirbuf[26];
 stat_buf->dim=dirbuf[31]*16777216+dirbuf[30]*65536+dirbuf[29]*256+dirbuf[28];
 date=dirbuf[25]*256 + dirbuf[24];
 time=dirbuf[23]*256 + dirbuf[22];

 stat_buf->date.year=date/512 + 1980;
 stat_buf->date.month=(date&0x01ff)/32;
 stat_buf->date.day=date&0x001f;

 stat_buf->time.hour=time/2048;
 stat_buf->time.min=(time&0x07ff)/32;
 stat_buf->time.sec=(time&0x001f)*2;


 return(1);
}


cmp(a,b)
char *a,*b;
{
 if(*a == '\0' && *b == 0x20)
   return(0);
 while(*a++ == *b++)  {
     if(*a == '\0' &&  (*b == '\0' || *b == 0x20))
        return(0);
 }
 return(1);
}






/* Bereitstellen der naechsten Clusternummer            
 * liefert 0 bei Dateiende
*/

unsigned dosnxt(fd,first,fat1,fatlen)
int fd;
unsigned first,fat1;
int fatlen;

{
 unsigned r,nxt1,nxt2,nxt;
 long pos;
 unsigned char b[3],d[2];

if (fatlen == 12) {

     r = first * 1.5;
     if(first % 2 == 0)
        pos = lseek(fd,(long)fat1+r,0);
     else
        pos = lseek(fd,(long)fat1+r-1,0);
     read(fd,b,3);
     nxt1 = (b[1]&0x0f)*256 + b[0];
     nxt2 = b[2]*16 + b[1]/16;

     if (first % 2 == 0)                  
        nxt = nxt1;
     else  
       nxt = nxt2;
    if (nxt >= 0x0ff8 && nxt <= 0x0fff)
       nxt = 0;
     
 }
 else  {
    r = first * 2;
    pos = lseek(fd,(long)fat1+r,0);

    read(fd,d,2);
    nxt = d[1]*256+d[0];
    if (nxt >= 0xfff8 && nxt <= 0xffff)
        nxt = 0;
 }
 return(nxt);
}

