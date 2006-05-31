/*
 *	remote
 *
 *	This is the wega version of the program "remote" in
 *	the P8000 / Development System Communication Package.
 *	In its current version, it provides a semi-transparent
 *	(translucent?) mode through a wega system to a remote
 *	system which enables a user to download files from
 *	the remote system. This connection is made through a file
 *	which looks like a normal terminal but is actually a line
 *	to the second system. This device is determined by a file
 *	(DATABASE) which contains entries linking system names to
 *	devices.  Each entry is on a separate line and has the system
 *	name followed by a space or tab, and then the device to be used.
 *	The first line in the database is the default device.
 *	Remote locks other people out by creating a lock file. LOCKROOT is
 *	appended with the name of the device (excluding path) to come up
 *	with a lock file.
 *
 *	The program forks a copy of itself. The parent handles one
 *	side of the communication and the child the other so that
 *	asynchronous i/o may occur.
 *
 */

char *strcat();
#include <stdio.h> /* used for database accesses only ... all else uses
			wega calls */
#include <pwd.h>
#define DATABASE "/usr/spool/uucp/remotelines"
#define LOCKROOT "/usr/spool/uucp/LCK..\0xxxxxxxxxxxxxxxxxxxxxxxxxx"
			/* note: extra space required by strcat */
#define NEEDNULL 21	/* index where \0 is .. */

#define	FS	0x1C	/* quit from transparent mode */
#define SOH	0x01	/* ascii start-of-header character */	
#define STX	0x02	/* ascii start-of-text character */
#define ETX     0x03	/* ascii end-of-text character */
#define EOT	0x04	/* ascii end-of-transmission character */
#define ACK	0x06	/* ascii acknowledge character */
#define NAK	0x15	/* ascii non-acknowledge character */
#define CAN	0x18	/* ascii cancel character */
#define ESC	0x1b	/* ascii escape character */	
#define DEL	0x7f	/* ascii delete character */
#define NAMSIZ	14	/* maximum file name length */
#define RECSIZ	512	/* disk record size for P8000 */
#define XFERSIZ	64	/* transfer size for file upload */
#define SECONDS 10	/* seconds to wait before timing out a read */

#define BINARY	0x01	/* flag byte bit settings */
#define QUERY	0x02
#define FATAL	0x04
#define ALLBIN	0x08
#define EXEC	0x10

char stx = STX;		/* used where addresses are necessary */
char etx = ETX;
char eot = EOT;
char ack = ACK;
char nak = NAK;
char can = CAN;
char esc = ESC;
char exc = EXEC;

#include <termio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

int allstop, onestop, timeout;	/* signal flags */
int printmsg;			/* flag to print non-fatal error messages */
int parent, child;		/* process ids of parent and child processes */
static char *device,*locknm;	/* pointer to device name and lockfile name */
static char *speed;		/* pointer to speed */
static char line[100],*p;	/* buffer for reading in database */
static int lockfd,pterm;
static struct termio targ;	/* defined in termio.h */
static struct termio tflags, ptflags;	/* original tty flags */
static struct termio txlflags, ptxlflags;	/* xlucent flags */
int (*curint)();		/* initial SIGINT value */

xlucent (term, targp, savterm)	/* make terminal interface translucent and */
int term;		/* return the previous configuration */
struct termio *targp, *savterm;
{
  int iospeed, i;

  ioctl (term, TCGETA, targp);	/* get terminal parameters */
  savterm->c_iflag = targp->c_iflag;
  savterm->c_oflag = targp->c_oflag;
  savterm->c_cflag = targp->c_cflag;
  savterm->c_lflag = targp->c_lflag;
  savterm->c_line = targp->c_line;
  for (i = 0; i < NCC; i++)
  	savterm->c_cc[i] = targp->c_cc[i];

  if (term != 0)	/* change speed of remote line */
  {
	switch (atoi(speed))
	{
		case 300:	iospeed = B300;
				break;
		case 1200:	iospeed = B1200;
				break;
		case 2400:	iospeed = B2400;
				break;
		case 4800:	iospeed = B4800;
				break;
		case 9600:	iospeed = B9600;
				break;
		case 19200:	iospeed = B19200;
				break;
		default:	fprintf(stderr, "remote: invalid baud rate\n\r");
				logout();
	}
	targp->c_cflag &= ~CBAUD;
	targp->c_cflag |= iospeed;
  }
  targp->c_iflag &= ~(ICRNL | ISTRIP);  /* set new configuration */
  targp->c_iflag |= (IXOFF|IXON);
  targp->c_oflag &= ~(OCRNL | TAB3 | OPOST | ONLCR);
  targp->c_cflag &= ~PARENB;
  targp->c_cflag |= (CS8 | CSTOPB | CLOCAL);
  targp->c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL);
  targp->c_cc[VMIN] = 1;
  targp->c_cc[VTIME] = 0;
  ioctl (term, TCSETA, targp);
}


xparent (term, targp, savterm)	/* make terminal interface transparent and */
int term;		/* return the previous configuration */
struct termio *targp, *savterm;
{
/* we assume that the interface is already translucent--we need only turn off */
/* flow control */

  int i;

  ioctl (term, TCGETA, targp);	/* get terminal parameters */
  savterm->c_iflag = targp->c_iflag;
  savterm->c_oflag = targp->c_oflag;
  savterm->c_cflag = targp->c_cflag;
  savterm->c_lflag = targp->c_lflag;
  savterm->c_line = targp->c_line;
  for (i = 0; i < NCC; i++)
  	savterm->c_cc[i] = targp->c_cc[i];

  targp->c_iflag &= ~(IXOFF|IXON);

  ioctl (term, TCSETA, targp);
}

/* return terminal interface to its initial state */
tinit (term, savterm, targp)
int term;
struct termio *savterm, *targp;
{
  int i;

  ioctl (term, TCGETA, targp);	/* get current terminal parameters */
  targp->c_iflag = savterm->c_iflag;  /* replace terminal configuration */
  targp->c_oflag = savterm->c_oflag;
  targp->c_cflag = savterm->c_cflag;
  targp->c_lflag = savterm->c_lflag;
  targp->c_line = savterm->c_line;
  for (i = 0; i < NCC; i++)
  	targp->c_cc[i] = savterm->c_cc[i];
  ioctl (term, TCSETA, targp);

  return;
}

term_to_wega (pterm)	/* send all input from terminal to wega */
int pterm;
{
  char c;

  while (1)			/* do forever */
  {
    c = getchar();
/*	Do we want out of remote
*/
    if ( (c & 0177) == FS )
    {
	kill(parent,SIGHUP);
	cleanup();
    }
    else
    write (pterm, &c, 1);
  }
}


char wega_to_term (pterm)	/* send all input from wega to terminal */
int pterm;
{
  char c;

  while (1)			/* do forever */
  {
    read (pterm, &c, 1);		/* get a character from wega */
    if (c == SOH)		/* first char of special sequence */
    {
      read (pterm, &c, 1);
      if (c == ESC)		/* second char of special sequence */
      {
        read (pterm, &c, 1);	/* possibly third char of special sequence */
        if ((c == 'Q') || (c == 'L') || (c == 'S') || (c == 'K'))
          return (c);
        else
        {
          putchar (SOH);
          putchar (ESC);
        }
      }
      else
        putchar (SOH);
    }
    putchar (c);		/* send character from wega to terminal */
  }
}

watch_for_abort (parent)	/* child executes this routine during file */
int parent;			/* transfers to detect user abort requests */
{
  char c;

  signal (SIGQUIT, SIG_IGN);	/* so we don't catch our own signals */
  signal (SIGTERM, SIG_IGN);

  while (1)		/* terminated by a kill from parent process */
  {
    if (((c = getchar()) & 0177) == ESC)	/* kill all transfers */
      kill (parent, SIGQUIT);
    if ((c & 0177) == CAN)		/* kill only the transfer in progress */
      kill (parent, SIGTERM);
  }
}


total_abort ()			/* set flag to indicate all xfers to stop */
{
  signal (SIGQUIT, total_abort);	/* set up the signal call again */
  allstop = 1;
}


single_abort ()			/* set flag to stop xfer in progress */
{
  signal (SIGTERM, single_abort); /* set up the signal call again */
  onestop = 1;
}

time_out ()			/* set flag to indicate a read timed out */
{
  signal (SIGALRM, time_out);	/* set up the signal call again */
  timeout = 1;
}

message (pterm)		/* get a message from the remote system */
int pterm;		/* an ETX ends the message */
{
  char c;

  while (1)
  {
    read (pterm, &c, 1);
    if (c == ETX) return;
    putchar (c);
  }
}


aread (pterm, cptr)	/* time out a read using the alarm clock */
int pterm;
char *cptr;
{
  timeout = 0;		/* reset the timeout flag */
  alarm (SECONDS);	/* we only wait SECONDS for the read to complete */
  read (pterm, cptr, 1); /* read one character */
  alarm (0);		/* cancel the timeout call */
}


wait_etx (pterm)	/* wait for etx, checksum from remote system */
int pterm;
{
  char c;

  while (c != ETX)	/* wait for etx */
    read (pterm, &c, 1);
  read (pterm, &c, 1);	/* throw away checksum */

  return;
}    

getname (pterm, name)	/* get a file name from the remote system */
int pterm;
char *name;
{
  char c;
  char csum;		/* computed checksum */
  int i;

  while (1)		/* repeat if message or checksum error */
  {
    csum = '\0';	/* start checksum at zero */
    read (pterm, &c, 1); /* first character of name string */
    switch (c)
    {
      case CAN:		/* cancel signifies remote abort */
        return (-1);
      case ESC:		/* a message from the remote system */
        message (pterm);
        break;
      case STX:		/* true start of the file name */
        for (i=0; i<=NAMSIZ; i++)
        {
          read (pterm, &c, 1);	/* a file name character */
          if (c == ETX)	/* end of file name */
          {
            name[i] = '\0';	/* end of file name string */
            read (pterm, &c, 1);	/* checksum */

            if (c == csum) return (0);	/* good file name */
            else		/* checksum error */
            {
              if(printmsg) printf ("checksum error ... retry\n\r");
              write (pterm, &nak, 1);
              break;		/* get name again */
            }
          }
          else
          {
            name[i] = c;	/* store character in name */
            csum ^= c;		/* update checksum */
          }
        }
        if (i <= NAMSIZ) break;	/* checksum error */
        {
          printf ("\n\rfile name too long\n\r");  
          wait_etx (pterm);	/* wait for etx, checksum */
          write (pterm, &can, 1); /* send a cancel to remote system */
        }
        break;
 
     default:		/* illegal first character */
        printf ("\n\rillegal first character of file name sequence\n\r");
        wait_etx (pterm);	/* wait for etx, checksum */
        write (pterm, &can, 1);	/* send a cancel to the remote system */
        break;			/* remote system should echo the cancel */
    }
  }
}

getrec (pterm, fd)		/* get a file record from remote system */
int pterm;
int fd;				/* file descriptor of local system file */
{
  static char buffer [2*RECSIZ]; /* holds characters, 2* ensures no ovflow */
  static int bufptr = 0;

  char c, csum;
  int i, high_count, low_count, n_to_read, n_read, n_written;

  read (pterm, &c, 1);	/* read first char of record format from remote */
  switch (c)
  {
    default:		/* unexpected (and meaningless) character */
      printf("\n\rillegal first character in block transfer sequence\n\r");
      return(4);	/* cancel this transfer (incomplete protocol) */

    case ESC:
      message (pterm);	/* get a message from remote system */
      return (2);	/* return message status */

    case CAN:		/* end of file */
      if (bufptr > 0)   /* write remaining data to file */
      {
        write (fd, buffer, bufptr);
        bufptr = 0;
      }
      return (3);	/* return eof status */

    case STX:		/* a real data record is coming */
      csum = '\0';	/* start checksum at zero */
      read (pterm, &c, 1);	/* get number of chars to follow */
      high_count = c;
      read (pterm, &c, 1);
      low_count = c;
      n_to_read = (high_count*256) + low_count;

      n_read = 0;	/* no characters read so far */
      for (i=0; i<n_to_read; i++)  /* get data from remote system */
      {
        aread (pterm, &c);	/* time out the read */
	if (timeout)		/* read was not completed */
	{
	  bufptr -= n_read;	/* readjust buffer pointer */
	  /*printf ("timeout ... retry\n\r");*/
	  return (1);		/* return error status and try to re-receive */
	}
	n_read++;	/* increment number of characters received */
        buffer[bufptr++] = c;
        csum ^= c;		/* compute checksum */
      }
      aread (pterm, &c);	/* get etx from remote system */
      if (timeout)		/* read was not completed */
      {
	bufptr -= n_read;	/* adjust buffer pointer */
	/*printf ("timeout ... retry\n\r");*/
	return (1);
      }
      if (c != ETX)
      {
        bufptr -= n_read;	/* readjust buffer pointer */
	/*printf ("illegal end-of-text ... retry\n\r");*/
        return (1);		/* return error status */
      }
      aread (pterm, &c);	/* get checksum from remote system */
      if (timeout)		/* read was not completed */
      {
	bufptr -= n_read;	/* adjust buffer pointer */
	/*printf ("timeout ... retry\n\r");*/
	return (1);
      }
#ifdef debug
	  printf(" remote sum = %x local sum = %x\n",c,csum);
#endif
      if (c != csum)
      {
        bufptr -= n_to_read;	/* readjust buffer pointer */
        if (printmsg) printf ("checksum error ... retry\n\r");
        return (1);		/* return error status */
      }
      if (bufptr > RECSIZ)	/* a disk write is necessary */
      {
        if ((n_written = write (fd, buffer, RECSIZ)) != RECSIZ)
        {
          printf ("disk write error\n\r");
          return (4);		/* return cancel status */
        }
        for (i=0; i<RECSIZ; i++)  /* move remaining data up in buffer */
          buffer[i] = buffer[i+RECSIZ];
        bufptr -= RECSIZ;	/* adjust buffer pointer */
      }
      if (onestop) return (4);	/* test for one file abort from user */
      if (allstop) return (5);	/* test for general abort from user */
      return (0);		/* otherwise return ok status */
  } 
}

load (pterm)			/* download a file from the remote system */
int pterm;
{
  char c;
  char fname[NAMSIZ+1];		/* file name */
  int pmode;			/* protection mode for new file */
  char flags;
  int dnfd;			/* file descriptor for downloaded file */

  write (pterm, &ack, 1);
  read (pterm, &flags, 1);	/* read flag byte */
  write (pterm, &ack, 1);

  printmsg = (flags & FATAL) ? 0 : 1;	/* print or suppress non-fatal msgs */

  if (getname (pterm, fname) < 0) return;	/* get the file name */

  c =  'y';
  if(flags & QUERY)		/* test for existing file */
    if((dnfd = open(fname, 0)) != -1)	/* file exists */
    {
      printf("replace %s (y/n)?",fname);
      c = getchar() & 0177;
      printf("%c\n\r",c);
      close(dnfd);
    }

  if(c != 'y')			/* do not replace */
  {
    write(pterm, &can, 1);	/* cancel the transfer */
    aread(pterm, &c);	/* wait for CAN from remote sys */
    write(pterm &ack, 1);	/* tell remote sys to proceed */
    return;
  }

  printf ("\n\r%s", fname);	/* print file name */
  if ((child = fork()) == 0)
    watch_for_abort (parent);	/* child watches user input */
  pmode = (flags & EXEC) ? 0755 : 0644;	/* set execution bits of file mode */

  if ((dnfd = creat (fname, pmode)) < 0)	/* create the file */
  {
    printf (" ... unable to open file on local system");
    write (pterm, &can, 1);	/* send abort to remote system */
  }
  else
    write (pterm, &ack, 1);	/* send ack to remote system */

  printf ("\n\r");
  allstop = 0;			/* reset abort flags */
  onestop = 0;
  while (1)			/* loop until getrec returns quit status  */
  {
    timeout = 0;		/* reset timeout flag */
    switch (getrec (pterm, dnfd))
    {
      case 0:
        putchar ('.');		/* user feedback */
        write (pterm, &ack, 1);
        break;
      case 1:
        write (pterm, &nak, 1);	/* record incorrect */
        break;
      case 2:
        break;			/* message recvd, do not acknowledge */
      case 3:
        close (dnfd);		/* end of file sent */
        printf ("\n\r");
        write (pterm, &ack, 1);
        return;
      case 4:
        write (pterm, &can, 1);	/* user requests cancellation */
        break;                  /* remote system should return a cancel */
      case 5:
        write (pterm, &esc, 1);	/* user requests abort of all files */
        break;			/* remote system should return a cancel */
    }
  }
}

sendrec (pterm, fd, fname)	/* upload one file record */
int pterm;			/* send the record here */
int fd;				/* get the record from this file */
char *fname;			/* file name used for message only */
{
  char buffer[XFERSIZ];		/* temp storage for file record */
  char c, csum;
  char high_count, low_count;
  int i, n_read, n_written;

  if ((n_read = read (fd, buffer, XFERSIZ)) > 0)	/* test for eof */
  {
    high_count = n_read/256;	/* compute number of chars to send */
    low_count = n_read - (high_count*256);
    csum = '\0';
    for (i=0; i<n_read; i++)	/* compute checksum */
      csum ^= buffer[i];

    c = NAK;			/* no acknowledge so far */
    while (c == NAK)		/* send the record */
    {
      write (pterm, &stx, 1);	/* start-of-text */
      write (pterm, &high_count, 1);	/* number of data bytes to follow */
      write (pterm, &low_count, 1);
      if ((n_written = write (pterm, buffer, n_read)) != n_read)
        printf ("data written doesn't match data read\n\r");
      write (pterm, &etx, 1);	/* end-of-text */
      write (pterm, &csum, 1);	/* checksum */

      read (pterm, &c, 1);	/* get acknowledge from remote system */
      if (c == NAK && printmsg)
        printf ("checksum error ... retry\n\r");
    }

    if ((c == CAN) || onestop)	/* test for error or user abort */
    {
      printf ("\n\r%s ... transfer aborted\n\r", fname);
      write (pterm, &can, 1);	/* tell remote system to abort this file */
      return (1);
    }

    if (allstop)		/* user wants to abort all files */
    {
      printf ("\n\r%s ... transfer aborted\n\r", fname);
      write (pterm, &esc, 1);
      return (1);
    }

    if (c != ACK)
    {
      printf ("illegal acknowledge character received\n\r");
      write(pterm, &can, 1);
      return(1);
    }
    return (0);
  }
  else				/* end-of-file status returned from read */
  {
    write (pterm, &eot, 1);
    return (1);
  }
}

send (pterm)			/* upload a file to the remote system */
int pterm;
{
  char c;
  char fname[NAMSIZ+1];		/* name of file to upload */
  char flags;
  int upfd;			/* file descriptor of fname */
  struct stat stbuf;		/* structure used for fstat call */

  if((child = fork()) == 0)
    watch_for_abort(parent);	/* child watches user input */
  write (pterm, &ack, 1);	/* tell remote system to proceed */
  read (pterm, &flags, 1);	/* read flag byte */
  write (pterm, &ack, 1);

  printmsg = (flags & FATAL) ? 0 : 1;	/* print or suppress non-fatal msgs */

  if (getname (pterm, fname) < 0) return;	/* get the file name */

  printf ("\n\r%s", fname);	/* print file name */
  if ((upfd = open (fname, 0)) < 0)		/* open the file */
  {
    printf ("\n\nunable to open file on local system\n\r");
    write (pterm, &can, 1);	/* cancel the transfer */
    read (pterm, &c, 1);
    if (c != CAN)
      printf ("unexpected character received from remote system\n\r");
  }
  else
  {
    fstat(upfd, &stbuf);		/* get file status */
    if(stbuf.st_size > 0L)		/* proceed if non-zero length */
    {
	/* acknowledge the file name */
	/* if the file is executable, send the mode flag */
	if ((stbuf.st_mode & S_IEXEC) == S_IEXEC)
		write(pterm, &exc, 1);
	else
		write(pterm, &ack, 1);

	printf ("\n\r");
	allstop = 0;		/* clear error flags */
	onestop = 0;
  
	while (sendrec (pterm, upfd, fname) == 0)
	  putchar ('.');		/* positive feedback for user */
  
	close (upfd);		/* close the file */
	printf ("\n\r");
    }
    else
    {
	printf(" on local system has length of zero\n");
	write(pterm, &can, 1);	/* cancel the transfer */
	read(pterm, &c, 1);
	if(c != CAN)
	  printf("unexpected character received from remote system\n");
    }
  }

  write (pterm, &ack, 1);	/* tell remote system to proceed */
  return;
}


main (argc, argv)
int argc;
char **argv;
{
  char c, wega_to_term();
  FILE *dbf;	/* file descriptor for database */
  int total_abort(), single_abort(), time_out(), cleanup(), intrupt();
  int inithookup;	/* initial hookup flag */
  char *eof,*nxtdev;	/* end of file on database */

  inithookup = 1;
  signal(SIGHUP, cleanup);	/* watch for hangups */
  /* get current SIGINT value, watch for interrupts before going to raw mode */
  curint = signal(SIGINT, intrupt);
  signal(SIGALRM, time_out);	/* set up timeout for initial hookup */

	/* attempt to open database file */
  if ((dbf = fopen(DATABASE,"r")) == 0)
  {  fprintf(stderr,"remote: cannot access remote database\n");
     exit(-1);
  }
	/* read database and parse into system (line), device and speed */
  device = 0;
  speed = "9600";
  argc--;
  eof = (char *)1;
  while(eof  && !(device))
  {  eof = fgets(line,100,dbf);
     if (!(eof))
	continue;
     for(p=line ; p-line<=100 && *p ; p++)
        if ((*p == '\t') || (*p == ' '))
	{	device = p+1;
		*(p--) = 0;
	}
     for(p=p+1; p-line<=100 && *p; p++)
        if ((*p == '\t') || (*p == ' '))
	{
		speed = p+1;
		*(p--) = 0;
	}
     if (!(device) || (device == line))
     {	fprintf(stderr,"remote: error in parsing database.\n");
	exit(-1);
     }
	/* check command line option if present */
     if (argc)
     	if (strcmp(line,argv[1]))
		device = 0;
  }
  fclose(dbf);
  if (!(device))
  {  if (argc)
       fprintf(stderr,"remote: no remote line to system %s\n",argv[1]);
     else
       fprintf(stderr,"remote: no remote line\n");
     exit(-1);
  }
	/* find device portion of lockfile */
  for(p=device;*p && *p != '\n' && *p != '\t' && *p != ' ';p++);
  if (*p != '\n' && *p)
    nxtdev = p+1;
  else
    nxtdev = 0;
  *p = '\0';
  while (device && !(pterm))
  { try(device);
    if (!(pterm))
    { device = nxtdev;
      if (device)
      { for(p=nxtdev;*p && *p != '\n' && *p != '\t' && *p != ' ';p++);
        if (*p != '\n' && *p)
          nxtdev = p+1;
        else
          nxtdev = 0;
        *p = '\0';
      }
    }
  }
  if (!(pterm))
  {
	char buf[20];
	char tbuf[10];

	dbf = fopen(locknm, "r");
	fgets(buf, 20, dbf);
	buf[strlen(buf) - 1] = '\0';
	fgets(tbuf, 10, dbf);
	tbuf[strlen(tbuf) - 1] = '\0';
	fprintf(stderr,"remote: %s at %s is using the remote line to %s\n", buf, tbuf, line);
	fclose(dbf);
	exit(0);
  }

  c = '\n';
  write (pterm, &c, 1);
  while (1)			/* loop until exit is taken */
  {
    parent = getpid();	/* get id of this process */
    if ((child = fork()) == 0)
    {
      if (inithookup)
      {
	timeout = 0;		/* assume no timeout */
	alarm(20);		/* give the remote sys time to respond */
	c = getchar();
	alarm(0);
	if (timeout || ((c & 0177) == DEL))/* remote sys is probably down */
	{
	  ioctl(pterm, TCFLSH, 2);
	  tinit(pterm, &ptflags, &targ);
	  close(pterm);
	  tinit(0, &tflags, &targ);
	  if (timeout)
		fprintf(stderr, "remote line to %s timed out\n", line);
	  else
		fprintf(stderr, "remote: interrupted\n");
	  intrupt();
	}
	write(pterm, &c, 1);
      }
      term_to_wega (pterm);	/* child handles this direction */
    }
    else
    {
      inithookup = 0;		/* hookup is done */
      c = wega_to_term (pterm); /* parent handles this direction */
      kill (child, SIGINT);	/* send termination signal to child */
      wait (0);			/* wait for child to die */

      switch (c)		/* special request from 2nd wega system */
      {
        case 'L':		/* load file */
        case 'S':		/* send file */
	  printf ("id=%d\n\r", parent);  /* so we can kill manually */
	  signal (SIGQUIT, total_abort); /* child may send these signals */
	  signal (SIGTERM, single_abort); /* if user requests an abort */
	  signal (SIGALRM, time_out);    /* if a read request times out */
	  xparent(pterm, &targ, &ptxlflags);	/* disable XON/XOFF */
	  if (c == 'L')
            load (pterm);	/* download a file */
          else
            send (pterm);	/* upload a file */
	  tinit(pterm, &ptxlflags, &targ);		/* reenable XON/XOFF */
          kill (child, SIGINT);	/* stop watching for user abort */
	  wait (0);			/* wait for child to die */
          break;

        case 'Q':		/* quit */
          write (pterm, &c, 1);	/* echo it back to kill the local */
	  sleep(5);		/* give the logout a chance to get there */
	  logout();
        case 'K':		/* quit with logout */
          write (pterm, &c, 1);	/* echo it back to kill the local */
          read (pterm, &c, 1);	/* accept locals final acknowledge */
	  cleanup();
      }
    }
  }
}

try(device)
char *device;
{
  static char lockfl[] = LOCKROOT;
  int mask;
  char c;
  char *p;
  char *n;
  char *getlogin(), *ttyname();

  pterm = 0;
  for(p=device;*p;p++);
  while(*p != '/' && p != device)
	p--;
  if (*p == '/')
	p++;
  lockfl[NEEDNULL] = '\0';
  locknm = strcat(lockfl,p);
  if (!(access (locknm, 0)))	/* modified to use stat (makes more sense) */
    return 1;
  mask = umask (0);		/* set file create mask and save previous */
  lockfd = creat (locknm, 0644); /* create the lock file*/
  umask (mask);			/* restore original mask */

  if (lockfd < 0)
  {
    printf ("unable to create lock file for system %s device %s\n",line,device);
    return 1;
  }

  n = getlogin();		/* put login and tty name in the lockfile  */
  write(lockfd, n, strlen(n));
  c = '\n';
  write(lockfd, &c, 1);
  n = ttyname(0);
  write(lockfd, &n[5], strlen(&n[5]));
  write(lockfd, &c, 1);
  close(lockfd);

  if ((pterm = open (device, 2)) < 0)  /* open 2nd system for read/write */
  {
    pterm = 0;
    printf ("remote: unable to open device for system %s device %s\n",line,device);
    unlink (locknm);  /* remove the lock file */
    exit(-1);
  }
  ioctl(pterm, TCFLSH, 2);
  xlucent (0, &targ, &tflags);		/* set new terminal parameters */
  xlucent (pterm, &targ, &ptflags);	/* set new pseudo terminal parameters */
  signal(SIGINT, curint);		/* reset to original SIGINT value */
}

cleanup()	/* interrupt or hang up, so exit gracefully */
{
  write(pterm, "logout\n", 7);
  sleep(5);
  logout();
}

logout()	/* logout processing */
{
  ioctl(pterm, TCFLSH, 2);	/* flush remote line */
  tinit (pterm, &ptflags, &targ);/* restore pseudo terminal settings */
  close(pterm);			/* explicit close */
  tinit (0, &tflags, &targ);	/* restore initial terminal settings */
  unlink (locknm);		/* remove the lock file */
  exit (0);			/* end parent process */
}

/*
 *  this routine is used by both parent and child
 */
intrupt()
{
  unlink(locknm);
  if (child)
  {
    kill(child, SIGINT);
    wait(0);
  }
  else
    kill(parent, SIGINT);
  exit(0);
}
