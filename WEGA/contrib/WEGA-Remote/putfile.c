
/*  putfile ...  this program downloads a file to an local             */
/*               system running the P8000 / Development System         */
/*               Communication Package.                                */

#define SOH	0x01	/* ascii start-of-header character */
#define STX	0x02	/* ascii start-of-text character */
#define ETX	0x03	/* ascii end-of-text character */
#define ACK	0x06	/* ascii acknowledge character */
#define NAK	0x15	/* ascii non-acknowledge character */
#define CAN	0x18	/* ascii cancel character (ctrl-x) */
#define ESC	0x1b	/* ascii escape character */
#define RETRIES	10	/* number of allowable transmission errors */
#define XFER_SIZE 64	/* max number of characters sent as a block */

#define BINARY	0x01	/* flag byte bit settings */
#define QUERY	0x02
#define FATAL	0x04
#define ALLBIN	0x08
#define EXEC	0x10
#define SECONDS 20	/* timeout interval */

#include <signal.h>
#ifdef __FreeBSD__
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <term.h>
struct termios targ;
#define RAW	ICANON
#define CRMOD	OPOST
#define XTABS	OXTABS
#define TANDEM	IXON|IXOFF
#else
#include <sgtty.h>
struct sgttyb targ;
#endif

#include <sys/types.h>
#include <sys/stat.h>
struct stat stbuf;

char *tty, *ttyname();

char acknak, checksum, buffer[XFER_SIZE], high_count, low_count;
int i, n_read, fd, tflags, success, unsuccess;
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
  if (chmod(tty, 0600) == -1)		/* write protect terminal */
    printf ("\nputfile: unable to write protect the terminal\n");

#ifdef __FreeBSD__
  tcgetattr(0, &targ);			/* get current terminal parameters */
  tflags = targ.c_oflag;		/* save flag word */
  targ.c_oflag |= RAW;			/* set 1-char input, no parity output */
  targ.c_oflag &= ~(ECHO|CRMOD|XTABS|TANDEM);  /* turn off echo, carr. return */
					/* subst. and tab replacement */
  tcsetattr(0, TCSADRAIN, &targ);
#else
  gtty (0, &targ);			/* get current terminal parameters */
  tflags = targ.sg_flags;		/* save flag word */
  targ.sg_flags |= RAW;			/* set 1-char input, no parity output */
  targ.sg_flags &= ~(ECHO|CRMOD|XTABS|TANDEM); /* turn off echo, carr. return */
                                        /* subst. and tab replacement */
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
        case 'B':			/* all files are binary */
          flags |= ALLBIN;
          break;
      }
    else				/* argument is a file name */
    {
      if ((fd = open(*argv, 0)) == -1)	/* open file and test for error */
      {
        printf ("\n%s\n\nunable to open file on remote system\r\n", *argv);
	unsuccess++;
      }
      else
      {
      fstat (fd, &stbuf);
      if (stbuf.st_mode & S_IFDIR)      /* is the directory file ? */
      { 
         printf ("\n%s\n\nunable to transfer directory file from remote system\r\n", *argv);
	 unsuccess++;
      }
      else
      {
	if(stbuf.st_mode & S_IEXEC)     /* is the file executable?    */
          flags |= EXEC;		/* if yes, set executable bit */

	if(stbuf.st_size == 0L)
	  printf("\n%s on remote system has length of zero\n",*argv);
	else
	{
          printf ("%c%cL", SOH, ESC);	/* SOH, escape, L - sequence for load */
          scanf ("%c", &acknak);	/* wait for local sys to get set up */
  
          if (flags & ALLBIN)		/* set binary if all files binary */
            flags |= BINARY;
          putchar (flags);		/* send the flag byte */
          scanf ("%c", &acknak);	/* wait for local sys to acknowledge */
  
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
            printf ("%c%s%c", STX, fname, ETX); /* stx, file name, etx */
            putchar (checksum);		/* send checksum */
            scanf ("%c", &acknak);	/* should get acknowledge now */
            if (acknak != ACK)
              if (acknak == CAN)	/* abort this file */
                break;
          }
  
        /* filename received, now send file */
          if (acknak == ACK)
          {
  
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
                printf ("%c", STX);	/* start-of-text */
                putchar (high_count);	/* send data length */
                putchar (low_count);
                write (1, buffer, n_read);/* send buffer */
                printf ("%c", ETX);	/* end-of-text */
                putchar (checksum);	/* send checksum */
		timeout = 0;		/* assume no timeout */
		alarm(SECONDS);		/* set up alarm */
                scanf ("%c", &acknak);	/* should get acknowledge now */
		alarm(0);		/* reset alarm */
		if(timeout) continue;	/* timed out, resend buffer */
                if ((acknak==CAN) || (acknak==ESC)) /* abort requested */
                  break;
              }
              if (acknak != ACK)	/* send local sys the abort message */
              {
                printf ("%c%s ... transfer aborted\r\n%c", ESC, fname, ETX);
                break;			/* abort this file, go to next */
              }
            }
            if (acknak == ACK) success++;	/* count successful transfers */
            else unsuccess++;
          }
          printf ("%c", CAN);		/* ctrl-X signals end-of-file */
          close (fd);			/* close file */
          if (acknak == ESC)		/* abort everything */
          {
            scanf ("%c", &acknak);	/* wait for local sys to be ready */
            break;
          }
          scanf ("%c", &acknak);	/* wait for local sys to finish up */
          flags &= ~(BINARY | EXEC);	/* default to ascii type next file */
	}
      }
      }
    }
  }
#ifdef __FreeBSD__
  targ.c_oflag = tflags;		/* restore initial terminal flags */
  tcsetattr(0, TCSADRAIN, &targ);
#else
  targ.sg_flags = tflags;		/* restore initial terminal flags */
  stty (0, &targ);
#endif
  printf ("\nputfile: %d successful transfers  %d unsuccessful transfers\n",
          success, unsuccess);

  if (chmod(tty, 0622) == -1)		/* un-write protect */
    printf ("\nputfile: unable to un-write protect the terminal\n");
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
