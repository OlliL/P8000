/* putud	P8000	Datei Schreiben auf 8-Bit-Teil unter UDOS */ 


#define SOH	0x01	/* ascii start-of-header character */
#define STX	0x02	/* ascii start-of-text character */
#define ETX	0x03	/* ascii end-of-text character */
#define ACK	0x06	/* ascii acknowledge character */
#define NAK	0x15	/* ascii non-acknowledge character */
#define CAN	0x18	/* ascii cancel character (ctrl-x) */
#define ESC	0x1b	/* ascii escape character */
#define RETRIES	3	/* number of allowable transmission errors */
#define XFER_SIZE 64	/* max number of characters sent as a block */

#define BINARY	0x01	/* flag byte bit settings */
#define FATAL	0x04
#define ALLBIN	0x08
#define EXEC	0x10

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat stbuf;

char bufw[20], bufr[2];
int dd, k;

char acknak, checksum, buffer[XFER_SIZE], high_count, low_count;
int i, n_read, fd, tflags, success, unsuccess;

int verbose=0;
int del=0;
int del_pro();

char *fname;
char flags = 0;

main (argc, argv)

int argc;
char **argv;

{
 
  signal(SIGINT, del_pro);  /* Interrupt-Signal (DEL-Taste) einfangen */
  signal(SIGHUP, SIG_IGN);  /* weitere Signale ignorieren */
  signal(SIGQUIT,SIG_IGN);
  signal(SIGILL, SIG_IGN);
  signal(SIGTRAP,SIG_IGN);
  signal(SIGIOT, SIG_IGN);
  signal(SIGSEGV,SIG_IGN);
  signal(SIGSYS, SIG_IGN);
  signal(SIGPIPE,SIG_IGN);
  signal(SIGALRM,SIG_IGN);
  signal(SIGTERM,SIG_IGN);

  if((dd=open("/dev/ud",2))==-1)
    {printf("\nunable to open /dev/ud\n");goto ende;}

  while (--argc > 0)			/* process all command line args */
  {
    if ((*++argv)[0] == '-')		/* test for command line option */
      switch ((*argv)[1])
      {
        case 'b':
          flags |= BINARY;		/* set binary file type */
          break;
        case 'f':
          flags |= FATAL;		/* udos-procedure file */
          break;
        case 'B':			/* all files are binary */
          flags |= ALLBIN;
          break;
	case 'v':
	  verbose = 1;
	  break;
      }
    else				/* argument is a file name */
    {
      if ((fd = open(*argv, 0)) == -1)	/* open file and test for error */
        printf ("\n%s unable to open file\n", *argv);
      else				/* file was opened with no error */
      {
        fstat (fd, &stbuf);		/* is the file executable? */
        if (stbuf.st_mode & S_IEXEC)
          flags |= EXEC;		/* if yes, set executable bit */

	if(stbuf.st_size == 0L)
	  printf("\nfile %s has length of zero\n",*argv);
	else
	{
	  bufw[0]='L';write(dd,bufw,1);
	  read(dd,bufr,1); acknak=bufr[0];
	  acknak &= 0177;
  
          if (flags & ALLBIN)		/* set binary if all files binary */
            flags |= BINARY;
	  bufw[0]=flags; write(dd,bufw,1);
	  read(dd,bufr,1); acknak=bufr[0];
	  acknak &= 0177;
  
          checksum = 0;			/* compute checksum to send */
          fname = *argv;		/* pointer to file name */
          for (; **argv; (*argv)++)	/* loop until end of name string */
            if (**argv == '/')		/* strip path name from string */
            {
              fname = *argv;		/* fname points to char after '/' */
              fname++;
              checksum = 0;		/* start checksum over */
            }
            else
              checksum ^= **argv;	/* checksum is xor of filename chars */
          acknak = NAK;			/* no acknowledge so far */
  
        /* send the filename first and get an ACK. Try RETRIES times */

          for (i=0; i<RETRIES && acknak!=ACK; i++)
          {
	    bufw[0]=STX; k=1;
	    while((bufw[k++]=(fname[k-1]))!='\0');
	    bufw[k-1]=ETX; write(dd,bufw,k);
	    bufw[0]=checksum; write(dd,bufw,1);
	    read(dd,bufr,1); acknak=bufr[0];
	    acknak &= 0177;
            if (acknak != ACK)
              if (acknak == CAN)	/* abort this file */
                break;
          }
  
        /* filename received, now send file */
          if (acknak == ACK)
          {

	    if(verbose) printf("\nfile %s is written to udos-disk\n", fname);
  
        /* read the file in blocks of XFER_SIZE chars. format record and send */
            while ((n_read = read(fd, buffer, XFER_SIZE)) > 0)
            {
              high_count = n_read/256;	/* compute the two bytes containing */
              low_count = n_read - (high_count*256);  /* the data length */
              acknak = NAK;		/* no acknowledge so far */
              checksum = 0;		/* compute checksum to send */
              for (i=0; i<n_read; i++)	/* checksum is xor of buffer chars */
              {
                if (buffer[i] == '\n')	/* turn newlines into returns */
                  if (!(flags & (BINARY | ALLBIN)))
                    buffer[i] = '\r';	/* do not change binary file */
                checksum ^= buffer[i];
              }

            /* send one block of characters and try RETRIES times to get ACK */
  
              for (i=0; i<RETRIES && acknak!=ACK; i++)
              {
		bufw[0]=STX; bufw[1]=high_count; bufw[2]=low_count;
		write(dd,bufw,3);
		write(dd,buffer,n_read);
		bufw[0]=ETX; bufw[1]=checksum;
		write(dd,bufw,2);
		if(verbose) putchar('.');

		read(dd,bufr,1); acknak=bufr[0];
		acknak &= 0177;

		if (del) acknak=ESC;
                if ((acknak==CAN) || (acknak==ESC)) /* abort requested */
                  break;
              }
              if (acknak != ACK)	/* send local sys the abort message */
                break;			/* abort this file, go to next */
            }
            if (acknak == ACK) success++;	/* count successful transfers */
            else unsuccess++;
          }
	  bufw[0]=CAN; write(dd,bufw,1);
          close (fd);			/* close file */
          if (acknak == ESC)		/* abort everything */
          {
            read(dd,bufr,1); acknak=bufr[0];
	    acknak &= 0177;
            break;
          }
          read(dd,bufr,1); acknak=bufr[0];
	  acknak &= 0177;
          flags &= ~(BINARY | EXEC);	/* default to ascii type next file */
	  if(verbose) putchar('\n');
	}
      }
    }
  }
  close(dd);
ende:
 return(unsuccess);

}

del_pro()
{
	signal(SIGINT, del_pro);
	del=1;
}
