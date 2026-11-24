/*
 * getfile - get files from a remote system over the terminal line (see remote(1), putfile(1))
 *
 * WEGA 1.6 (11/83). The structure of main follows getfile.c 1.7 of Zilog (ZEUS 3.21, tmp/zeus321_root/usr/src/getfile.c),
 * with the code of 1.6; the names are guesses (1.7: acknak, c, timeout, time_out, look_for_etx, aread). The protocol:
 * SOH ESC 'S' and the option byte start a transfer; per file STX name ETX and the check byte (the names xored) are sent
 * until ACK (or DLE: an executable file); then records STX, the length (two bytes), the bytes, ETX, the check byte; ACK,
 * NAK or CAN answer.
 */

static char sccsid[] = "@[$]getfile.c\t1.6  11/18/83 13:18:30 - 87wega3.2";

#include <sgtty.h>
#include <signal.h>

#define	SOH	001
#define	STX	002
#define	ETX	003
#define	EOT	004
#define	ACK	006
#define	DLE	020
#define	NAK	025
#define	CAN	030
#define	ESC	033

int	tflag = 0;
char	flags = 0;

struct	sgttyb	tb;
char	*ttyn;
char	answer;
char	ack;
char	cksum;
char	buf[64];
int	hi;
int	lo;
int	retry;
int	i;
int	len;
int	n;
int	fd;
int	savflags;
int	succ;
int	unsucc;
int	mode;
char	*name;

int	onalarm();
char	*ttyname();

main(argc, argv)
char **argv;
{
	signal(SIGALRM, onalarm);
	ttyn = ttyname(1);
	if (chmod(ttyn, 0600) == -1)
		printf("\ngetfile: unable to write protect the terminal\n");
	gtty(0, &tb);
	savflags = tb.sg_flags;
	tb.sg_flags |= RAW;
	tb.sg_flags &= ~(ECHO|TANDEM);
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
			continue;
		}
		answer = 'y';
		if (flags & 02) {
			if ((fd = open(*argv, 0)) != -1) {
				printf("replace %s (y/n)?", *argv);
				answer = getchar() & 0177;
				printf("%c\r\n", answer);
				close(fd);
			}
		}
		if (answer != 'y')
			goto nextfile;
		printf("%c%cS", SOH, ESC);
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
		for (retry = 0; ack != ACK && ack != DLE; ) {
			printf("%c%s%c", STX, name, ETX);
			putchar(cksum);
			scanf("%c", &ack);
			if (ack == CAN)
				break;
			if (++retry >= 10)
				break;
		}
		if (ack != ACK && ack != DLE) {
			printf("%c", CAN);
			unsucc++;
		} else {
			mode = (ack & DLE) ? 0755 : 0644;
			if ((fd = creat(name, mode)) == -1) {
				drain();
				printf("%c", CAN);
				printf("\n\nunable to open file on remote system\r\n");
				scanf("%c", answer);	/* sic: the value, not its address */
			} else {
				while ((answer = getchar()) != CAN && answer != ESC && answer != EOT) {
					ack = NAK;
					for (retry = 0; retry < 10 && ack != ACK; ) {
						if (answer != STX)
							drain();
						else {
							hi = getchar();
							lo = getchar();
							len = (hi << 8) + lo;
							cksum = 0;
							for (i = 0; i < len; i++) {
								tflag = 0;
								alarm(20);
								buf[i] = getchar();
								alarm(0);
								if (tflag)
									break;
								cksum ^= buf[i];
								if (buf[i] == '\r')
									if (!(flags & 011))
										buf[i] = '\n';
							}
							if (!tflag) {
								getc1();
								if (!tflag && answer != ETX)
									drain();
								else {
									getc1();
									if (!tflag && answer == cksum) {
										if ((n = write(fd, buf, len)) != len) {
											printf("%c", CAN);
											printf("disk write error\n\r");
											break;
										} else
											ack = ACK;
									}
								}
							}
						}
						retry++;
						if (ack == ACK)
							printf("%c", ack);
						else {
							if (retry < 10) {
								do {
									printf("%c", ack);
									getc1();
								} while (tflag);
							} else
								printf("%c", CAN);
						}
					}
				}
			}
			close(fd);
			if (answer == EOT)
				succ++;
			else
				unsucc++;
			if (answer == ESC) {
				scanf("%c", &answer);
				break;
			}
		}
		scanf("%c", answer);	/* sic: the value, not its address */
	nextfile:
		flags &= ~01;
	}
	tb.sg_flags = savflags;
	stty(0, &tb);
	printf("\ngetfile: %d successful transfers  %d unsuccessful transfers\n", succ, unsucc);
	if (chmod(ttyn, 0622) == -1)
		printf("\ngetfile: unable to un-write protect the terminal\n");
}

onalarm()
{
	signal(SIGALRM, onalarm);
	tflag = 1;
}

drain()
{
	while (answer != ETX) {
		getc1();
		if (tflag)
			return;
	}
	getc1();
	if (tflag)
		return;
}

getc1()
{
	tflag = 0;
	alarm(20);
	answer = getchar();
	alarm(0);
}
