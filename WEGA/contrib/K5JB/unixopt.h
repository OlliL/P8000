/* unixopt.h -- Unix specific options -- Moved here from Makefile
 * for dependency management purposes - K5JB
 */

/* if you have the mkdir() function, define the following: */
#ifndef WEGA
#define HAVEMKDIR
#endif

/* This is a wait value for the Coherent sleep2() function */
#define COHWAIT 50	/* 50 ms */

/* If you have Coherent version 4.xx, define this.  Also, in the
 * Makefile, make MSPTICK=10.  (It is 1000 for older versions)
 */
#undef COH4

/* if you want remote users to be able to log on to your machine with
 * telnet - Requires having pseudo tty ports on the computer. This works
 * well with Coherent but my 3B2 master pty blocks on read and I have
 * to take extraordinary measures, spawning a child process and a pipe.
 * Contact me for source code if you have same problem.
 */

#ifndef WEGA
#define TELUNIX
#endif

/* On k5jb3b2 only, has to do with brain damaged pty on my machine.
 * Don't define this unless you have the file that I call telunix.c3
 * linked to telunix.c in your set.
 */
#ifndef WEGA
#define PTY_PIPE
#endif

/* a client stub with PORT 87.  Uses IPC to transact with a server.
 * Contact me for a sample server if you want to try this.
 */
#ifndef WEGA
#define TELSERV
#define SERVNAME "telserv"	/* suggest 7 max for screen formatting */
#endif

#ifdef _OSK
#undef TELUNIX
#undef TELSERV
#undef PTY_PIPE
#endif

/* the start and stop options for MSDOS are defined in config.h.  Edit
 * these to suit your installation.  Here is a full set of options for
 * reference:

#define STARTOPT \
	"start: discard, echo, finger, ftp, remote, smtp, telnet, telserv, telunix"
#undef STOPOPT
#define STOPOPT \
	"stop: discard, echo, finger, ftp, remote, smtp, telnet, telserv, telunix"
*/

#define STARTOPT \
	"start: discard, echo, finger, ftp, remote, smtp, telnet, telserv, telunix"
#undef STOPOPT
#define STOPOPT \
	"stop: discard, echo, finger, ftp, remote, smtp, telnet, telserv, telunix"


/* We can either USE_QUIT and the shell layer manager (shl) or USE_QUIT
 * and FORKSHELL to cause NET to spawn a child process when the shell
 * command is called.  FORKSHELL depends on USE_QUIT
 */

#define USE_QUIT /* shell layer capability - quit sets reportquit_flag */
					/* check for that flag in main loop and if set call */
					/* report_quit() - A signal handler shouldn't printf */

#define FORKSHELL	/* Causes NET to spawn a child process with the ! command */

#if defined(FORKSHELL) && !defined(USE_QUIT)
#define USE_QUIT
#endif

