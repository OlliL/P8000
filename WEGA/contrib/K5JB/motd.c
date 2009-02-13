/* motd.c - a module for making a message of the day, function also
 * used by ax25 mbox if MB_BUTTIN is defined.  Note that actual line
 * length is constrained by ttydriv.  Maximum is 73 characters plus
 * end of line sequence.
 */

#include "config.h"
#if defined(TN_MOTD) || defined(MB_BUTTIN) || defined(AX_MOTD)
#include <stdio.h>

#define LEN_MOTD 80	/* line actually constrained by ttydriv to 73 + eols */

#ifdef MB_BUTTIN
char butt_msg1[LEN_MOTD];  /* used by ax_mbx.c */
#endif
#ifdef TN_MOTD
char tn_motd[LEN_MOTD]; /* one used by telnet, tnserv.c */
#endif
                        
#ifdef AX_MOTD         /* one used if someone initiates an ax.25 or netrom */
char ax_motd[LEN_MOTD] =  /* chat session; initialized for unsophisticates */
		"Chat session enabled.  If I'm not here, disconnect and send mail.\015";
#endif

int	/* match rest of commands in main.c */
motd(argc,argv)
int argc;
char **argv;
{
	int i,j=0;
	int for_telnet = 0;
	char *cp;

	switch(*argv[0]){
#ifdef TN_MOTD
		case 'm':	/* came from the motd command */
			cp = tn_motd;
			for_telnet = 1;
			break;
#endif
#ifdef AX_MOTD
		case 'a':	/* came from axmotd command */
			cp = ax_motd;
			break;
#endif
#ifdef MB_BUTTIN
		default:
			cp = butt_msg1;
#endif
	}

	if(argc > 1){	/* we have something to say */
		if(argv[1][0] == '-'){	/* clear it */
			cp[0] = '\0';
			return 0;
		}
		for(i=1;i<argc;i++){
			if(j)
				cp[j++] = ' ';
			do{
				cp[j++] = *argv[i]++;
				if(j > LEN_MOTD - 4) /* leave room for \r\n\0 */
					break;
			}while(*argv[i]);
			if(j > LEN_MOTD - 5)
				break;
		}
		cp[j++] = '\015';
		if(for_telnet)	/* gotta follow da rules */
			cp[j++] = '\012';
		cp[j] = '\0';
	}else
		printf("%s\n",cp);
	return 0;
}
#endif
