/*
 * putfile - send files to a remote system over the terminal line (see remote(1), getfile(1))
 *
 * WEGA (no source; written from /bin/putfile 1.5, 11/83). The names are guesses. The protocol is that of getfile: SOH ESC
 * 'L' and the option byte, per file STX name ETX and the check byte until ACK, then records STX, the length (two bytes),
 * the bytes, ETX, the check byte, each until ACK (10 tries); CAN ends a file, ESC the whole transfer.
 */

static char sccsid[] = "@[$]putfile.c\t1.5  11/18/83 14:13:24 - 87wega3.2";

#include <sgtty.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define	SOH	001
#define	STX	002
#define	ETX	003
#define	ACK	006
#define	NAK	025
#define	CAN	030
#define	ESC	033

int	tflag = 0;
char	flags = 0;

struct	sgttyb	tb;
struct	stat	sb;
char	*ttyn;
char	ack;
char	cksum;
char	buf[64];
char	hi;
char	lo;
int	i;
int	n;
int	fd;
int	savflags;
int	succ;
int	unsucc;
char	*name;

int	onalarm();
char	*ttyname();

main(argc, argv)
char **argv;
{
	signal(SIGALRM, onalarm);
	ttyn = ttyname(1);
	if (chmod(ttyn, 0600) == -1)
		printf("\nputfile: unable to write protect the terminal\n");
	gtty(0, &tb);
	savflags = tb.sg_flags;
	tb.sg_flags |= RAW;
	tb.sg_flags &= ~(ECHO|TANDEM|CRMOD|XTABS);
	stty(0, &tb);
	while (--argc > 0) {
		if (**++argv == '-') {
			switch ((*argv)[1]) {
			case 'b':
				flags |= 01;
				break;
			case 'q':
				flags |= 02;
				break;
			case 'f':
				flags |= 04;
				break;
			case 'B':
				flags |= 010;
				break;
			}
		} else {
			if ((fd = open(*argv, 0)) == -1) {
				printf("\n%s\n\nunable to open file on remote system\r\n", *argv);
				unsucc++;
				continue;
			}
			fstat(fd, &sb);
			if (sb.st_mode & 0100)
				flags |= 020;
			if (sb.st_size == 0) {
				printf("\n%s on remote system has length of zero\n", *argv);
				continue;
			}
			printf("%c%cL", SOH, ESC);
			scanf("%c", &ack);
			if (flags & 010)
				flags |= 01;
			putchar(flags);
			scanf("%c", &ack);
			cksum = 0;
			for (name = *argv; **argv; (*argv)++) {
				if (**argv == '/') {
					name = *argv;
					name++;
					cksum = 0;
				} else
					cksum ^= **argv;
			}
			ack = NAK;
			for (i = 0; ack != ACK; ) {
				printf("%c%s%c", STX, name, ETX);
				putchar(cksum);
				scanf("%c", &ack);
				if (ack != ACK && ack == CAN)
					break;
				if (++i >= 10)
					break;
			}
			if (ack == ACK) {
				do {
					if ((n = read(fd, buf, 64)) <= 0)
						goto eof;
					hi = n / 256;
					lo = n - (hi << 8);
					ack = NAK;
					cksum = 0;
					for (i = 0; i < n; i++) {
						if (buf[i] == '\n')
							if (!(flags & 011))
								buf[i] = '\r';
						cksum ^= buf[i];
					}
					for (i = 0; ack != ACK; ) {
						printf("%c", STX);
						putchar(hi);
						putchar(lo);
						write(1, buf, n);
						printf("%c", ETX);
						putchar(cksum);
						tflag = 0;
						alarm(20);
						scanf("%c", &ack);
						alarm(0);
						if (tflag == 0 && (ack == CAN || ack == ESC))
							break;
						if (++i >= 10)
							break;
					}
				} while (ack == ACK);
				printf("%c%s ... transfer aborted\r\n%c", ESC, name, ETX);
			eof:
				if (ack == ACK)
					succ++;
				else
					unsucc++;
			}
			printf("%c", CAN);
			close(fd);
			if (ack == ESC) {
				scanf("%c", &ack);
				break;
			}
			scanf("%c", &ack);
			flags &= ~021;
		}
	}
	tb.sg_flags = savflags;
	stty(0, &tb);
	printf("\nputfile: %d successful transfers  %d unsuccessful transfers\n", succ, unsucc);
	if (chmod(ttyn, 0622) == -1)
		printf("\nputfile: unable to un-write protect the terminal\n");
}

onalarm()
{
	signal(SIGALRM, onalarm);
	tflag = 1;
}
