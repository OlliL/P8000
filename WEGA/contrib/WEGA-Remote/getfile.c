
/*  getfile ...  this program uploads a file from a local system     */

#define SOH	0x01	/* ascii start-of-header character */
#define STX	0x02	/* ascii start-of-text character */
#define ETX	0x03	/* ascii end-of-text character */
#define EOT	0x04	/* ascii end-of-transmission character */
#define ACK	0x06	/* ascii acknowledge character */
#define NAK	0x15	/* ascii non-acknowledge character */
#define CAN	0x18	/* ascii cancel character (ctrl-x) */
#define ESC	0x1b	/* ascii escape character */
#define RETRIES	10	/* number of allowable transmission errors */
#define XFER_SIZE 64	/* max number of characters sent as a block */
#define CMODE	0644	/* creation mode for ascii files */
#define	EMODE	0755	/* creation mode for executable files */

#define BINARY	0x01	/* flag byte bit settings */
#define QUERY	0x02
#define FATAL	0x04
#define ALLBIN	0x08
#define	EXEC	0x10	/* mode flag is sent after the filename */
#define SECONDS 20	/* timeout interval */

#include <signal.h>
#ifdef __FreeBSD__
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <term.h>
#include <sys/stat.h>
void look_for_etx();
void aread();
struct termios targ;
#define RAW	ICANON
#define CRMOD	OPOST
#define XTABS	OXTABS
#define TANDEM	IXON|IXOFF
#else
#include <sgtty.h>
struct sgttyb targ;
#endif

char *tty, *ttyname();

char c, acknak, checksum, buffer[XFER_SIZE];
int high_count, low_count;
int i, j, n_to_read, n_written, fd, tflags, success, unsuccess;
int pmode;
int timeout = 0;
char *fname;
char flags = 0;

#ifdef __FreeBSD__
int
#endif
main (argc, argv)

int argc;
char **argv;

{
#ifdef __FreeBSD__
  void time_out();
#else
  int time_out();
#endif

  signal(SIGALRM,time_out);		/* set up alarm handling */
  tty = ttyname(1);			/* get name of the output file */
  if (chmod(tty, 0600) == -1)		/* write protect the terminal */
    printf ("\ngetfile: unable to write protect the terminal\n");

#ifdef __FreeBSD__
  tcgetattr(0, &targ);			/* get current terminal parameters */
  tflags = targ.c_oflag;		/* save flag word */
  targ.c_oflag |= RAW;			/* set 1-char input, no parity output */
  targ.c_oflag &= ~(ECHO | TANDEM);	/* turn off echo, xon-xoff handling */
  tcsetattr(0, TCSADRAIN, &targ);
#else
  gtty (0, &targ);			/* get current terminal parameters */
  tflags = targ.sg_flags;		/* save flag word */
  targ.sg_flags |= RAW ;		/* set 1-char input, no parity output */
  targ.sg_flags &= ~(ECHO | TANDEM);	/* turn off echo, xon-xoff handling */
  stty (0, &targ);
#endif

  while (--argc > 0)			/* process all command line args */
  {
    if ((*++argv)[0] == '-')		/* test for command line option */
      switch ((*argv)[1])
      {
        case 'b':
          flags |= BINARY;		/* set binary file type */
          break;
        case 'q':
          flags |= QUERY;		/* query file replacement */
          break;
        case 'f':
          flags |= FATAL;		/* suppress non-fatal messages */
          break;
        case 'B':
          flags |= ALLBIN;		/* all files are binary */
      }
    else				/* argument is a file name */
    {
      c = 'y';				/* assume we are proceeding */

      if (flags & QUERY)		/* test for existing file */
        if ((fd = open(*argv, 0)) != -1) /* file exists */
        {
          printf ("replace %s (y/n)?", *argv);
          c = getchar() & 0177;		/* could have parity so mask bit 7 */
          printf ("%c\r\n", c);
          close (fd);
        }

      if (c == 'y')			/* replace the file */
      {
        printf ("%c%cS", SOH, ESC);	/* SOH, escape, S - sequence for send */
        scanf ("%c", &acknak);	/* wait for local sys to get set up */

        if (flags & ALLBIN)		/* set binary if all files binary */
          flags |= BINARY;
        putchar (flags);		/* send the flag byte */
        scanf ("%c", &acknak);	/* wait for acknowledge */

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

        for (i=0; i<RETRIES && (acknak!=ACK && acknak!=EXEC); i++)
        {
          printf ("%c%s%c", STX, fname, ETX);  /* stx, file name, etx */
          putchar (checksum);		/* send checksum */
          scanf ("%c", &acknak);	/* local sys should acknowledge now */
          if (acknak == CAN)		/* abort this file */
              break;
        }

        if (acknak != ACK && acknak != EXEC) /* abort this file, go to next */
	{
	  printf("%c", CAN);		/* ctrl-X cancels the request */
	  unsuccess++;
	}
	else				/* filename rcvd, now get the file */
        {
	  pmode = (acknak & EXEC) ? EMODE : CMODE;

          if ((fd = creat(fname, pmode)) == - 1) /* create or truncate file */
	  {
	    look_for_etx();		/* flush chars coming from local sys */
	    printf("%c", CAN);		/* send cancel */
            printf ("\n\nunable to open file on remote system\r\n");
#ifdef __FreeBSD__
            scanf("%c",&c);		/* wait for CAN from local sys */
#else
	    scanf("%c",c);		/* wait for CAN from local sys */
#endif
	  }

          else				/* file was opened with no error */
	  {
            /* local will send EOT at eof, CAN for error, or ESC to end all */
            while (((c=getchar())!=CAN) && (c!=ESC) && (c!=EOT))
            {
            /* try RETRIES times to get a good record */
              acknak = NAK;		/* no acknowledge so far */

              for (i=0; i<RETRIES && acknak!=ACK;)
              {
                if (c != STX) look_for_etx();	/* first char should be stx */

                else			/* stx found */
                {
                  high_count = getchar(); /* get the count characters */
                  low_count = getchar();
                  n_to_read = (high_count*256) + low_count;

                  checksum = 0;		/* start checksum at zero */
                  for (j=0; j<n_to_read; j++)
                  {
		    timeout = 0;	/* assume no timeout */
		    alarm(SECONDS);	/* set up alarm */
                    buffer[j] = getchar(); /* get a character */
		    alarm(0);		/* reset alarm */
		    if(timeout) break;	/* timed out */
                    checksum ^= buffer[j]; /* checksum is xor of buffer chars */
                    if (buffer[j] == '\r') /* replace cr's with newlines */
                      if (!(flags & (BINARY | ALLBIN)))
                        buffer[j] = '\n';   /* do not change binary file */
                  }

		  if(!timeout)
		  {
		    aread();			/* next char should be etx */
                    if (!timeout && (c != ETX))
			look_for_etx();		/* flush rest of chars */

                    else			/* etx found */
                    {
			aread();	/* next char should be chksum */
			if(!timeout && ((char)c == checksum)) /* compare */
                    	{
                          if ((n_written = write (fd, buffer, n_to_read)) != n_to_read)	/* write */
			  {
				printf("%c", CAN);
				printf("disk write error\n\r");
				break;
			  }
			  else
                          	acknak = ACK;	/* only now will an ACK be sent */
			}
                    }
                  }
                }
                i++;				/* increment try number */
                if (acknak == ACK)
                  printf ("%c", acknak);	/* send ACK */
                else
                {
                  if (i<RETRIES)
                  {
		    do	/* keep sending NAKs until char received */
		    {
                      printf ("%c", acknak);	/* send NAK */
		      aread();			/* read a char */
		    }
		    while (timeout);
                  }
                  else
                    printf ("%c", CAN);	/* send cancel after RETRIES */
                }
              }
            }
	  }
          close (fd);			/* close the file */
          if (c == EOT) success++;	/* update the success count */
          else unsuccess++;

          if (c == ESC)			/* user wants to end the whole thing */
          {
            scanf ("%c", &c);		/* wait for local sys to be ready */
            break;
          }
        }
        scanf ("%c", &c);		/* wait for local sys to finish up */
      }
      flags &= ~BINARY;			/* default to ascii type next file */
    }
  }
#ifdef __FreeBSD__
  targ.c_oflag = tflags;		/* restore initial terminal flags */
  tcsetattr(0, TCSADRAIN, &targ);
#else
  targ.sg_flags = tflags;		/* restore initial terminal flags */
  stty (0, &targ);
#endif
  printf ("\ngetfile: %d successful transfers  %d unsuccessful transfers\n",
          success, unsuccess);

  if (chmod(tty, 0622) == -1)		/* un-write protect the terminal */
    printf ("\ngetfile: unable to un-write protect the terminal\n");
#ifdef __FreeBSD__
  return 0;
#endif
}

#ifdef __FreeBSD__
void
#endif
time_out()
{
	signal(SIGALRM,time_out);
	timeout = 1;
}

#ifdef __FreeBSD__
void
#endif
look_for_etx()		/* look for etx and chksum */
{
   while (c != ETX)
   {
	aread();
	if(timeout) return;	/* timed out, send NAK */
   }
   aread();			/* look for chksum */
   if(timeout) return;
}

#ifdef __FreeBSD__
void
#endif
aread()			/* alarm read */
{
   timeout = 0;		/* assume no timeout */
   alarm(SECONDS);	/* set up alarm */
   c=getchar();		/* read next char */
   alarm(0);		/* cancel alarm request */
}
