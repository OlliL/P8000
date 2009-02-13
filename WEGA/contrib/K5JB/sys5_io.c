/* OS- and machine-dependent stuff for SYS5 */
/* This file was revised with k5jb version k24 11-9-92 */
/* V.k34: When used with telunix on my brain-damaged pty, telunix.c2 or
 * telunix.c3 must be used.  Provides for telnet client stub in telserv.c
 * and forked child that does shell command. - K5JB
 */

/*
	FILE: UNIX.io.c
	
	Routines:
		onquit()	used to stop console input for shell layer - K5JB
		report_quit() is the deferred report of quit receipt
		ioinit()
		iostop()
		asy_init()
		asy_stop()
		asy_speed()
		asy_output()
		asy_recv()
		dir()
	Written or converted by Mikel Matthews, N9DVG
	SYS5 added by Jere Sandidge, K4FUM
	Directory pipe added by Ed DeHart, WA3YOA
	Numerous changes by Charles Hedrick and John Limpert, N3DMC
	Hell, *I* even hacked on it... :-)  Bdale, N3EUA
	Converted to blocking input to let CPU breathe.  Note that Coherent
	doesn't have timeout on blocking input so put sleep2() on asy_recv().
	Also re-worked to handle bad serial port connection on 3B2, probably 
	hardware dependent -- K5JB
	
	If you (have and) want to use the select code, define SELECT in the 
	makefile or in this file.
*/
#include <stdio.h>
#include <sys/types.h>
#include <termio.h>
#include <signal.h>
#ifndef COH386
#include <sys/file.h>
#endif
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include "global.h"
#include "asy.h"
#include "mbuf.h"
#include "internet.h"
#include "iface.h"
#include "unix.h"
#include "cmdparse.h"
#include "unixopt.h"

#ifndef	B19200
#define	B19200	EXTA
#endif

#ifndef	B38400
#define	B38400	EXTB
#endif

struct asy asy[ASY_MAX];
struct interface *ifaces;
struct termio mysavetty, savecon;
int saveconfl;
int	IORser[ASY_MAX];
char *ttbuf;
int slipisopen;

#ifdef USE_QUIT	/* the forked shell depends on quit signals too */
int reportquit_flag = 0;

/* note that when we do shell escape, this will be replaced by a
 * user signal so quit can be ignored
 */
int
onquit()
{
	extern int backgrd;
#ifdef FORKSHELL
	extern int shellpid;
	extern char prompt[];
	int stat_loc;
#endif

	signal(SIGQUIT,onquit);	/* SIGQUIT is a Ctrl \ */

#ifdef FORKSHELL
	if(shellpid){  /* we only want to do this once */
		kill(shellpid,SIGKILL);
		wait(&stat_loc);	/* revisit this.  We will have more than one child */
		printf(prompt);
		fflush(stdout);
		shellpid = 0;
		return 0;
	}
#endif

	backgrd = !backgrd;
	reportquit_flag = 1;
	return(0);
}

/* this is called only if we need it for shell layers */
void
report_quit()
{
extern int backgrd;

	reportquit_flag = 0;	/* flag that brought us here */
	printf("\nKeyboard input is ");
	if(backgrd)
		printf("IGNORED.  Inactive shell OK now.  Ctrl-\\ to restore.\n");
	else
		printf("ACCEPTED.  Don't make shell inactive.\n");
}
#endif

/* Called at startup time to set up console I/O, memory heap */
ioinit()
{
	struct termio ttybuf;
	extern int iostop();
#ifdef USE_QUIT
	extern int onquit();
#endif

	(void) signal(SIGHUP, iostop);
	(void) signal(SIGINT, iostop);
#ifdef USE_QUIT
	(void) signal(SIGQUIT,onquit);	/* SIGQUIT is a Ctrl \ */
#else
	(void) signal(SIGQUIT, iostop);
#endif
	(void) signal(SIGTERM, iostop);

	ioctl(0, TCGETA, &ttybuf);
	savecon = ttybuf;

	ttybuf.c_iflag &= ~IXON;
	ttybuf.c_lflag &= ~(ICANON|ECHO);	/* removed ISIG -- K5JB */
	/* note that in Coherent, the following is ineffective */
	ttybuf.c_cc[VTIME] = 1;	/* effective with ~ICANON -- was '\0' - K5JB */
	ttybuf.c_cc[VMIN] = 0; /* ditto, was '\01' - K5JB */
	if ((saveconfl = fcntl(0, F_GETFL, 0)) == -1) {
		perror("Could not read console flags");
		return -1;
	}

#if defined(COH386) && !defined(COH4)
	/* remove this if Coherent adds VTIME and VMIN functions */
	fcntl(0, F_SETFL, saveconfl | O_NDELAY); /* non-blocking Input */
#else
	/* following had | O_NDELAY, was non-blocking - K5JB */
	fcntl(0, F_SETFL, saveconfl & ~O_NDELAY); /* blocking Input */
#endif
	ioctl(0, TCSETAW, &ttybuf);
}

/* Called just before exiting to restore console state */
iostop()
{
	extern int exitval;
#if defined(PTY_PIPE) && defined(TELUNIX)
	extern int pipepid;
#endif
/*	setbuf(stdout,NULLCHAR); we aren't doing a setbuf anywhere on setup */

	while (ifaces != NULLIF) {
		if (ifaces->stop != NULLFP)
			(*ifaces->stop)(ifaces->dev);
		ifaces = ifaces->next;
	}

	ioctl(0, TCSETAW, &savecon);
	fcntl(0, F_SETFL, saveconfl);
#if defined(PTY_PIPE) && defined(TELUNIX)
	if(pipepid)
		kill(pipepid,SIGKILL);
#endif
	exit(exitval);
}

/* Initialize asynch port "dev" */
int
asy_init(dev, arg1, arg2, bufsize)
int16 dev;
char *arg1, *arg2;
unsigned bufsize;
{
	register struct asy *ap;
	struct termio	sgttyb;
	int tempttyfl;

#ifdef	SYS5_DEBUG
	printf("asy_init: called\n");
#endif	/* SYS5_DEBUG */

	if (dev >= nasy)
		return -1;

	ap = &asy[dev];
	ap->tty = malloc(strlen(arg2)+1);
	strcpy(ap->tty, arg2);
	printf("asy_init: tty name = %s\n", ap->tty);

	/* open with no block because port may not have DCD asserted */
	if ((IORser[dev] = open(ap->tty, (O_RDWR | O_NDELAY), 0)) < 0) {
		perror("Could not open device IORser");
		return -1;
	}

	if ((tempttyfl = fcntl(IORser[dev], F_GETFL, 0)) == -1) {
		perror("Could not read asy flags");
		return -1;
	}

	/* Older Coherent versions can't break a blocking read */

#if !(defined(COH386) && !defined(COH4))
	/* now make port blocking - first write will report error if so */
	fcntl(IORser[dev], F_SETFL, tempttyfl & ~O_NDELAY);
#endif

 /* 
  * get the stty structure and save it 
  */

	if (ioctl(IORser[dev], TCGETA, &mysavetty) < 0)	{
		perror("ioctl failed on device");
		return -1;
	}

 /* 
  * copy over the structure 
  */

	sgttyb = mysavetty;
	sgttyb.c_iflag = (IGNBRK | IGNPAR);
	sgttyb.c_oflag = 0;
	sgttyb.c_lflag = 0;
	sgttyb.c_cflag = (B9600 | CS8 | CREAD);
	/* note that in Coherent, the following is ineffective */
	sgttyb.c_cc[VTIME] = 1;	/* was 0 - K5JB */
	sgttyb.c_cc[VMIN] = 0;

	if (ioctl(IORser[dev], TCSETAF, &sgttyb) < 0) {
		perror("ioctl could not set parameters for IORser");
		return -1;
	}

	return 0;
}

int
asy_stop(dev)
int dev;
{
}


/* Set asynch line speed */
int
asy_speed(dev, speed)
int dev;
int speed;
{
	struct termio sgttyb;

	if (speed == 0 || dev >= nasy)
		return -1;

#ifdef	SYS5_DEBUG
	printf("asy_speed: Setting speed for device %d to %d\n",dev, speed);
#endif

	asy[dev].speed = speed;

	if (ioctl(IORser[dev], TCGETA, &sgttyb) < 0) {
		perror("ioctl could not get parameters");
		return -1;
	}

	sgttyb.c_cflag &= ~CBAUD;

	switch ((unsigned)speed) {
	case 0:
		sgttyb.c_cflag |= B0;
		break;
	case 50:
		sgttyb.c_cflag |= B50;
		break;
	case 75:
		sgttyb.c_cflag |= B75;
		break;
	case 110:
		sgttyb.c_cflag |= B110;
		break;
	case 134:
		sgttyb.c_cflag |= B134;
		break;
	case 150:
		sgttyb.c_cflag |= B150;
		break;
	case 200:
		sgttyb.c_cflag |= B200;
		break;
	case 300:
		sgttyb.c_cflag |= B300;
		break;
	case 600:
		sgttyb.c_cflag |= B600;
		break;
	case 1200:
		sgttyb.c_cflag |= B1200;
		break;
	case 1800:
		sgttyb.c_cflag |= B1800;
		break;
	case 2400:
		sgttyb.c_cflag |= B2400;
		break;
	case 4800:
		sgttyb.c_cflag |= B4800;
		break;
	case 9600:
		sgttyb.c_cflag |= B9600;
		break;
	case 19200:
		sgttyb.c_cflag |= B19200;
		break;
	case 38400:
		sgttyb.c_cflag |= B38400;
		break;
	default:
		printf("asy_speed: Unknown speed (%d)\n", speed);
		break;
	}

#ifdef	SYS5_DEBUG
	printf("speed = %d\n", sgttyb.sg_ispeed);
#endif	/* SYS5_DEBUG */

	if (ioctl(IORser[dev], TCSETAW, &sgttyb) < 0) {
		perror("ioctl could not set parameters for IORser");
		return -1;
	}

	return 0;
}


/* Send a buffer to serial transmitter */
asy_output(dev, buf, cnt)
unsigned dev;
char *buf;
unsigned short cnt;
{
#ifdef	SYS5_DEBUG
	printf("asy_output called. dev = %d, cnt = %d\n", dev, cnt);
	printf("buf = %lx\n", (long) buf);
	fflush(stdout);
#endif
	
	if (dev >= nasy)
		return -1;

	if (write(IORser[dev], buf, cnt) < cnt)	{
		perror("asy_output");
		printf("asy_output: error in writing to device %d\n", dev);
		printf("asy_output: Check serial port cabling\n");
		return -1;
	}

	return 0;
}

#if defined(COH386) && !defined(COH4)
#include <poll.h>

/* this always returns 0 unless interval is -1.  It returns either when
 * interval, in ms, elapses, or input available on stdin .  This can be
 * removed if Coherent comes up with the VMIN VTIME thing.
 */

int
sleep2(interval)
int interval;
{
	struct pollfd pollcat[1];
	pollcat[0].fd = 0;	/* stdin */
	pollcat[0].events = POLLIN;
	return(poll(pollcat,1,interval));
}
#endif

/* Receive characters from asynch line
 * Returns count of characters read
 */
unsigned
asy_recv(dev,buf,cnt)
int dev;
char *buf;
unsigned cnt;
{
#define	IOBUFLEN	330	/* max ax.25 frame, was 256 */
	unsigned tot;
	int r;
	static struct	{
		char	buf[IOBUFLEN];
		char	*data;
		int	cnt;
	}	IOBUF[ASY_MAX];

#ifdef SELECT
	int	mask;
	int	writemask;
	int	ok;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 35;
	mask = (1<<IORser[dev]);
	writemask = (1<<IORser[dev]);
	ok = 0;
	tot = 0;
	ok = select(mask, &mask, 0, 0, &timeout);
	if ( mask & (1<<IORser[dev]))
	{
		tot = read(IORser[dev], buf, cnt);
	}
	return (tot);
#else
	if(IORser[dev] < 0) {
		printf("asy_recv: bad file descriptor passed for device %d\n",
			dev);
		return(0);
	}
	tot = 0;
	/* fill the read ahead buffer */
	if(IOBUF[dev].cnt == 0) {
		IOBUF[dev].data = IOBUF[dev].buf;
#if defined(COH386) && !defined(COH4)
		sleep2(COHWAIT);	/* a 10 ms sleep to give up CPU cycles */
#endif
		r = read(IORser[dev], IOBUF[dev].data, IOBUFLEN);
		/* check the read */
		if (r == -1) {
#ifndef COH386	/* apparently read() in coherent returns -1 when nothing */
				/* available from port */
			IOBUF[dev].cnt = 0;	/* bad read */
#ifdef USE_QUIT
			if(errno != EINTR){
				perror("asy_recv");
				printf("asy_recv: error in reading from device %d\n", dev);
			}
#else
			perror("asy_recv");
			printf("asy_recv: error in reading from device %d\n", dev);
#endif
#endif
			return(0);
		} else
			IOBUF[dev].cnt = r;
	} 
	r = 0;	/* return count */
	/* fetch what you need with no system call overhead */
	if(IOBUF[dev].cnt > 0) {
		if(cnt == 1) { /* single byte copy, do it here */
			*buf = *IOBUF[dev].data++;
			IOBUF[dev].cnt--;
			r = 1;
		} else { /* multi-byte copy, left memcpy do the work */
			unsigned n = min(cnt, IOBUF[dev].cnt);
			memcpy(buf, IOBUF[dev].data, n);
			IOBUF[dev].cnt -= n;
			IOBUF[dev].data += n;
			r = n;
		}
	}
	tot = (unsigned int) r;
	return (tot);
#endif	/* not SELECT */
}

/* Generate a directory listing by opening a pipe to /bin/ls.
 * If full == 1, give a full listing; else return just a list of names.
 */
FILE *
dir(path,full)
char *path;
int full;
{
	FILE *fp;
	char cmd[1024];

	if (path == NULLCHAR || path[0] == '\0')
		path = ".";

#ifdef	SYS5_DEBUG
	printf("DIR: path = %s\n", path);
#endif	/* SYS5_DEBUG */
/* added 2>&1 to this pipe to direct errors to file - K5JB */
	if (full)
		sprintf(cmd,"ls -l %s 2>&1", path);
	else
		sprintf(cmd, "ls %s 2>&1", path);

	if ((fp = popen(cmd,"r")) == NULLFILE) {
		perror("popen");
		return NULLFILE;
	}

	return fp;
}


asy_ioctl(interface, argc, argv)
struct interface *interface;
int	argc;
char	*argv[];
{
	if (argc < 1) {
		printf("%d\r\n", asy[interface->dev].speed);
		return 0;
	}

	return asy_speed(interface->dev, atoi(argv[0]));
}
