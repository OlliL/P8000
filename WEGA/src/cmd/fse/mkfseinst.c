#include "system"


#define writestr(X) { static char text[]= X; write(1,text,sizeof(text)-1); }

#include <stdio.h>
#define BUFFSIZE 55000

unsigned char *buff;

main()
{ int           f,l,j,mask,incr;
  unsigned int  ll,ii,jj;
  FILE          *F;
  unsigned char *malloc();
  
  buff= malloc(BUFFSIZE);
  if (buff== (unsigned char *)0)
  { writestr("Memory too small\n");
    exit(1);
  }
  writestr("Make a new fse -insatallation program (ID=1)\n\n");
  if (system("make"))
  { writestr("make stoped with errors\n");
    exit(1);
  }
  f= open("fsen",0 /* O_RDONLY */ );
  if (f<=0)
  { writestr("can't open fsen\n");
    exit(1);
  }
  F= fopen("fseinst.h","w");
  if (F==NULL)
  { close(f);
    writestr("can't create fseinst.h\n");
    exit(1);
  }
  ll= read(f,buff,10000);
  mask= 0x3f;  incr= 0x03;
  fprintf(F,"unsigned char pgm1[] = {\n'Q','U','i','T',0,%d,%d,%d,%d\n",
            mask ^ 0xe5,
            incr ^ 0xe5,
            (ll/256) ^ 0xe5,
            (ll%256) ^ 0xe5  );
  jj= 0;
  while (jj<ll)
  { fprintf(F,",%d",buff[jj] ^ mask);
    mask += incr;
    if (((++jj) % 10) == 0) fprintf(F,"\n");
  }
  ll= read(f, buff, BUFFSIZE);
  mask= 0x71;
  incr= 0x07;
  jj= 0;
  fprintf(F,"};\n unsigned char pgm2[]= {\n'F','A','i','L',0,%d,%d,%d,%d\n",
            mask ^ 0xe5,
            incr ^ 0xe5,
            (ll/256) ^ 0xe5,
            (ll%256) ^ 0xe5   );
  while (jj<ll)
  { fprintf(F,",%d",buff[jj] ^ mask);
    mask += incr;
    if (((++jj)%10)==0) fprintf(F,"\n");
  }
  fprintf(F,"};\n");
  fclose(F);
  close(f);
#ifdef WEGA
  f= system("scc -O -s -i -o fseinst fseinst.c /usr/lib/slibtermlib.a");
#endif
#ifdef VENIX
  f= system("cc -O -s -i -o fseinst fseinst.c /usr/lib/libtermlib.a");
#endif
  unlink("fseinst.h");
  if (f!=0) { writestr("Error during compilation\n"); }
  else      { writestr("fseinst (ID =1) created\n");  }
}

