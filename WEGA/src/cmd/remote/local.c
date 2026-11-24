static char Version[] = "@[$]aaaaaaaaaaaabbbb/local.c	1.5  02/28/84 14:21:55 - ............";

/*    local - this program returns operating system control        */
/*          to a local system which is running the S8000 /         */
/*          Development System Communication Package               */
/*                                                                 */
/*          The -l option sends a kill request to the local        */
/*          system, which then sends a "logout" command to Unix.   */
/*                                                                 */
/*    references - 1. Design Objectives and Requirements for       */
/*                    the S8000/Development System Communic-       */
/*                    ation Package (Document # 00016)             */
/*                 2. External Reference Specifications for        */
/*                    same (Document # 00021)                      */

#define SOH	0x01	/* ascii start-of-header character */
#define STX	0x02	/* ascii start-of-text character */
#define ETX	0x03	/* ascii end-of-text character */
#define ENQ	0x05	/* ascii enquiry character */
#define ACK	0x06	/* ascii acknowledge character */
#define NAK	0x15	/* ascii non-acknowledge character */
#define CAN	0x18	/* ascii cancel character */
#define ESC	0x1b	/* ascii escape character */
#define RETRIES	10	/* number of allowable transmission errors */

#include <stdio.h>
#include <ctype.h>
#include <sgtty.h>
struct sgttyb targ, targsave;
int i;
char acknak, checksum;
char *commnd;
int rc = 0;

main (argc, argv)
int argc;
char *argv[];
{
  gtty (0, &targ);		/* get current terminal parameters */
  targsave = targ;			/* save them */
  targ.sg_flags |= RAW;		/* set RAW mode for program duration */
  targ.sg_flags &= ~ECHO;	/* turn off character echo */
  stty (0, &targ);

  if (argc <= 1)		/* no argument, send quit request */
  {
    printf ("%c%cQ", SOH, ESC);
    acknak = getchar();		/* wait for acknowledge */
  }
  else if (argv[1][0] == '-' && argv[1][1] == 'l' && argv[1][2] == 0)
  {
    printf ("%c%cK", SOH, ESC);	/* send kill request for logout */
    acknak = getchar();		/* wait for acknowledge */
    putchar(acknak);			/* echo it back */
  }
  else				/* a command for the local system */
  {
    argcat (&argc, &argv);
    printf ("%c%cX", SOH, ESC);
    scanf ("%c", &acknak);		/* wait for local sys to get set up */
    argv++;
    commnd = *argv;
    checksum = 0;
    for (; **argv; (*argv)++)
      checksum ^= **argv;
    acknak = NAK;
    for (i = 0; acknak == NAK; )
    {
      printf ("%c%s%c", STX, commnd, ETX);
      putchar (checksum);
      scanf ("%c", &acknak);
      if (++i >= RETRIES)
        break;
    }
    if (acknak != ACK)
      if (acknak == ENQ)
      {
        rc = getchar() << 8;
        rc += getchar();
      }
      else
      {
        rc = 0x8000;
        printf ("%c", CAN);
      }
  }

  targ = targsave;			/* restore initial terminal mode */
  stty (0, &targ);
  exit (rc);
}

static char buf[1024];
static char *nargv[3];

argcat (argcp, argvp)
int *argcp;
char ***argvp;
{
  char *s, *p = buf, *t;	/* s, t: never used (frame words of the original) */
  char **av;
  int first = 1, pct = 0, plus = 0;

  av = *argvp;
  nargv[0] = *av;
  nargv[1] = buf;
  nargv[2] = 0;
  if (**av == '+')
  {
    plus = 1;
    (*av)++;
  }
  while (*++av)
  {
    if (first)
    {
      if (**av == '+')
      {
        plus = 1;
        (*av)++;
      }
      first = 0;
    }
    else
    {
      pct = 0;
      if (**av == '%')
      {
        pct = 1;
        (*av)++;
      }
      *p++ = ' ';
    }
    while (i = *(*av)++)
    {
      if (pct && !plus)
        i = islower(i) ? toupper(i) : i;
      if (!pct && plus)
        i = islower(i) ? toupper(i) : i;
      *p++ = i;
    }
  }
  *p = 0;
  *argvp = nargv;
  *argcp = 2;
}
