/* getud	P8000 - Datei Laden von 8-Bit-Teil unter UDOS */ 

#define SOH	0x01	/* ascii start-of-header character */
#define STX	0x02	/* ascii start-of-text character */
#define ETX	0x03	/* ascii end-of-text character */
#define EOT	0x04	/* ascii end-of-transmission character */
#define ACK	0x06	/* ascii acknowledge character */
#define NAK	0x15	/* ascii non-acknowledge character */
#define CAN	0x18	/* ascii cancel character (ctrl-x) */
#define ESC	0x1b	/* ascii escape character */
#define RETRIES	3	/* number of allowable transmission errors */
#define XFER_SIZE 64	/* max number of characters sent as a block */
#define CMODE	0644	/* creation mode for transmitted files */

#define BINARY	0x01	/* flag byte bit settings */
#define FATAL	0x04
#define ALLBIN	0x08

#include <stdio.h>
#include <signal.h>

char bufw[20], bufr[2];
int dd, k;

char c, acknak, checksum, buffer[XFER_SIZE];
int high_count, low_count;
int i, j, n_to_read, n_written, fd, tflags, success, unsuccess;

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

  if((dd=open("/dev/ud",2))==-1) {
    printf("\nunable to open /dev/ud\n"); goto ende; }
 
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
        case 'B':
          flags |= ALLBIN;		/* all files are binary */
	  break;
	case 'v':
	  verbose = 1;
	  break;
      }
    else				/* argument is a file name */
    {
	bufw[0]='S'; write(dd,bufw,1);
	read(dd,bufr,1); acknak=bufr[0];
	acknak &= 0177;

        if (flags & ALLBIN)		/* set binary if all files binary */
          flags |= BINARY;
	bufw[0]=flags; write(dd,bufw,1);
	read(dd,bufr,1); acknak=bufr[0];
	acknak &= 0177;

        checksum = 0;			/* compute checksum to send */
        fname = *argv;			/* pointer to file name */
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

        if (acknak != ACK) {		/* abort this file, go to next */
	  bufw[0]=CAN; write(dd,bufw,1);
	  }
	else				/* filename rcvd, now get the file */
        {
          if ((fd = creat(fname, CMODE)) == - 1) /* create or truncate file */
	  {
	    look_for_etx();		/* flush chars coming from local sys */
	    bufw[0]=CAN; write(dd,bufw,1);
            printf ("\nunable to open file\n");
	    read(dd,bufr,1); c=bufr[0];
	    c &= 0177;
	  }

          else				/* file was opened with no error */
	  {

	    if(verbose) printf("\nfile %s is read from udos-disk\n", fname);

            /* local will send EOT at eof, CAN for error, or ESC to end all */
	    read(dd,bufr,1); c=bufr[0]&0177;
            while ((c!=CAN) && (c!=ESC) && (c!=EOT))
            {
            /* try RETRIES times to get a good record */
              acknak = NAK;		/* no acknowledge so far */

              for (i=0; i<RETRIES && acknak!=ACK;)
              {
                if (c != STX) look_for_etx();	/* first char should be stx */

                else			/* stx found */
                {


                  read(dd,bufr,2);
                  high_count = bufr[0]; low_count = bufr[1];
                  n_to_read = (high_count*256) + low_count;


                  checksum = 0;		/* start checksum at zero */
                  for (j=0; j<n_to_read; j++)
                  {
                    read(dd,bufr,1); buffer[j]=bufr[0];
                    checksum ^= buffer[j]; /* checksum is xor of buffer chars */
                    if (buffer[j] == '\r') /* replace cr's with newlines */
                      if (!(flags & (BINARY | ALLBIN)))
                        buffer[j] = '\n';   /* do not change binary file */
                  }

		    aread();			/* next char should be etx */
                    if (c != ETX)
			look_for_etx();		/* flush rest of chars */
                    else			/* etx found */
                    {
			aread();	/* next char should be chksum */
			if((char)c == checksum) /* compare */
                    	{
                          n_written = write (fd, buffer, n_to_read); /* write */
                          acknak = ACK;	/* only now will an ACK be sent */
			  if(verbose) putchar('.');
			}
                    }
                }
                i++;				/* increment try number */

		if(del)
		{
		   bufw[0]=CAN; write(dd,bufw,1);
		   c=ESC; break;
		}

                if (acknak == ACK)
                {  bufw[0]=acknak; write(dd,bufw,1); }
                else
                {
                   bufw[0]=CAN; write(dd,bufw,1);
                }
		read(dd,bufr,1); c=bufr[0]&0177;
              }
            }
	  }
          close (fd);			/* close the file */
          if (c == EOT) success++;	/* update the success count */
          else unsuccess++;

          if (c == ESC)			/* user wants to end the whole thing */
          {
            read(dd,bufr,1); c=bufr[0];
            break;
          }
        }
        read(dd,bufr,1); c=bufr[0];

      flags &= ~BINARY;			/* default to ascii type next file */
      if(verbose) putchar('\n');
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

look_for_etx()		/* look for etx and chksum */
{
   while (c != ETX)
   {
	aread();
   }
   aread();			/* look for chksum */
}

aread()
{
   read(dd,bufr,1); c=bufr[0];
}
