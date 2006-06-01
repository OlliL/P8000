
/*    local - this program returns operating system control        */
/*          to a local system which is running the P8000 /         */
/*          Development System Communication Package               */
/*                                                                 */
/*          The -l option sends a kill request to the local        */
/*          system, which then sends a "logout" command to WEGA.   */

#define SOH	0x01	/* ascii start-of-header character */
#define ESC	0x1b	/* ascii escape character */

#include <stdio.h>
#ifdef __FreeBSD__
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <term.h>
struct termios targ;
#define RAW	ICANON
#else
#include <sgtty.h>
struct sgttyb targ;
#endif

#include <pwd.h>
int tflags;

char c;

int
main (argc, argv)
int argc;
char *argv[];
{
  char *getlogin();
  struct passwd *getpwuid();

  if (strcmp(getlogin(), getpwuid(getuid())->pw_name))
  {
    printf("Not login shell\n");
    exit(0);
  }

#ifdef __FreeBSD__
  tcgetattr(0, &targ);		/* get current terminal parameters */
  tflags = targ.c_oflag;	/* save flag word */
  targ.c_oflag |= RAW;		/* set 1-char input, no parity output */
  targ.c_oflag &= ~ECHO;	/* turn off echo, xon-xoff handling */
  tcsetattr(0, TCSADRAIN, &targ);
#else
  gtty (0, &targ);		/* get current terminal parameters */
  tflags = targ.sg_flags;	/* save flag word */
  targ.sg_flags |= RAW;		/* set RAW mode for program duration */
  targ.sg_flags &= ~ECHO;	/* turn off character echo */
  stty (0, &targ);
#endif

  printf ("%c%c", SOH, ESC);	/* soh, escape - code for a special request */

  if (argc > 1)
    if ((argv[1][0] == '-') && (argv[1][1] == 'l'))
    {
      printf ("K");		/* send kill request for logout */
      c = getchar();		/* wait for acknowledge */
      putchar(c);		/* echo it back */
    }
    else			/* no "-l", send quit request */
    {
      printf ("Q");
      c = getchar();		/* wait for acknowledge */
    }
  else				/* no argument, send quit request */
  {
    printf ("Q");
    c = getchar();		/* wait for acknowledge */
  }

#ifdef __FreeBSD__
  targ.c_oflag = tflags;
  tcsetattr(0, TCSADRAIN, &targ);
#else
  targ.sg_flags = tflags;	/* restore initial terminal mode */
  stty (0, &targ);
#endif
  return 0;
}
