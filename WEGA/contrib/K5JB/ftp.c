/* Stuff common to both the FTP server and client */
#include <stdio.h>
#include "config.h"
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "timer.h"
#include "tcp.h"
#include "ftp.h"
#include "telnet.h"
#include "iface.h"
#include "ax25.h"
#include "lapb.h"
#include "finger.h"
#include "session.h"
#ifdef	SYS5
#include <sys/types.h>
#include <sys/stat.h>
#ifdef	hp9000s500
#include <sys/param.h>
#endif
#include <sys/inode.h>
#endif
#include "nr4.h"

#ifndef TRUE	/* k28 */
#define TRUE 1
#define FALSE 0
#endif

#if	(ATARI_ST && MWC)
#define	fclose	vclose		/* Take care of temp files -- hyc */
#endif

void free();

/* FTP Data channel Receive upcall handler */
void
ftpdr(tcb,cnt)
struct tcb *tcb;
int16 cnt;
{
	register struct ftp *ftp;
	struct mbuf *bp;
	char c;

	ftp = (struct ftp *)tcb->user;
	if(ftp->state != RECEIVING_STATE){
		close_tcp(tcb);
		return;
	}
	/* This will likely also generate an ACK with window rotation */
	recv_tcp(tcb,&bp,cnt);

#if (UNIX || MAC || AMIGA || ATARI_ST || _OSK)
	if(ftp->type == ASCII_TYPE){
		while(pullup(&bp,&c,1) == 1){
#ifdef _OSK
			if(c != '\012')	/* linefeed */
#else
			if(c != '\r')
#endif
				putc(c,ftp->fp);
		}
		return;
	}
#endif
	while(bp != NULLBUF){
		if(bp->cnt != 0)
			fwrite(bp->data,1,(unsigned)bp->cnt,ftp->fp);
		bp = free_mbuf(bp);
	}

 	if(ftp->fp != stdout && ferror(ftp->fp)){ /* write error (dsk full?) */
 		fclose(ftp->fp);
 		ftp->fp = NULLFILE;
 		close_self(tcb,RESET);
 	}
}
/* FTP Data channel Transmit upcall handler */
void
ftpdt(tcb,cnt)
struct tcb *tcb;
int16 cnt;
{
	struct ftp *ftp;
	struct mbuf *bp;
	register char *cp;
	register int c;
#ifdef	SYS5
	struct stat ss_buf;
#endif
#if (UNIX || MAC || AMIGA || ATARI_ST || _OSK)
	static int savedch = FALSE;	/* added k28 - this mod costs 80 bytes when
			applied to the Unix version */
#endif
	ftp = (struct ftp *)tcb->user;
	if(ftp->state != SENDING_STATE){
		close_tcp(tcb);
		return;
	}
	if((bp = alloc_mbuf(cnt)) == NULLBUF){
		/* Hard to know what to do here - You bet!  And this could be a real
		 * memory leak if memory is in yellow zone - K5JB */
		return;
	}
	c = 0;	/* double duty as character and flag */
	if(ftp->type == IMAGE_TYPE){
		bp->cnt = fread(bp->data,1,cnt,ftp->fp);
		if(bp->cnt != cnt)
			c = EOF;	/* flag to get outa here */
	} else { 	/* is ASCII type */
		cp = bp->data;
		/* lifted this out routine I wrote for finger so we wouldn't
		 * have a chronic queue of (window - 1).  If necessary to handle
		 * ill formed MS-DOS text files, just remove the #ifs -- K5JB k28
		 */
#if (UNIX || MAC || AMIGA || ATARI_ST || _OSK)
		if(savedch){	/* unlikely case */
			*cp++ = '\015';
			*cp++ = '\012';
			bp->cnt += 2;
			cnt -= 2;
			savedch = FALSE;
		}
#endif
		while(cnt > 0 && (c = getc(ftp->fp)) != EOF){
			switch((char)c) {
				case '\032':	/* NO ^Z's! - they clear screens */
					break;
#if (UNIX || MAC || AMIGA || ATARI_ST || _OSK)
#ifdef _OSK
				case '\012':	/* ignore \n - OS9/OSK and maybe Mac use \r as */
								/*	EOL character */
					break;
				case '\015':	/* convert CR to CR/LF */
					c = '\012';
#else
				case '\r':	/* Unix and the others use \n as */
								/*	EOL character */
					break;
				case '\n':	/* convert LF to CR/LF */
#endif
					if(cnt == 1){ /* unlikely case, no room for 2 chars */
						savedch = TRUE;
						cnt = 0;	/* make it a short one - drop out */
						break;
					}
					*cp++ = '\015';	/* CR */
					bp->cnt++;
					cnt--;
						/* and fall through */
#endif
				default:
					*cp++ = (char)c;
					bp->cnt++;
					cnt--;
					break;
			}
		} /* while !feof() && cnt != 0 */
	} /* else not image type */
	if(bp->cnt != 0)
		send_tcp(tcb,bp);
	else
		free_p(bp);
	if(c == EOF){	/* EOF seen in either ASCII or IMAGE types */
#ifdef	UNIX
#ifdef	SYS5
#ifndef	hp9000s500
/* If ftp->fp points to an open pipe (from dir()) it must be closed with */
/* pclose().  System V fstat() can tell us if this was a pipe or not. */
		if (fstat(fileno(ftp->fp), &ss_buf) < 0)
			perror("ftpdt: fstat");
#ifdef COH386
		if ((ss_buf.st_mode & S_IFPIP) == S_IFPIP)
#else
		if ((ss_buf.st_mode & IFIFO) == IFIFO)
#endif
			pclose(ftp->fp);	/* close pipe from dir */
		else
			fclose(ftp->fp);
#else	/* hp9000s500 */
/* HP-UX 5.21 on the 500 doesn't understand IFIFO, since we probably don't *.
/* care anyway, treat it like BSD is treated... */
		if (pclose(ftp->fp) < 0)
			fclose(ftp->fp);
#endif	/* hp9000s500 */
#else	/* SYS5 */
/* Berkeley Unix can't tell if this was a pipe or not.  Try a pclose() */
/* first.  If this fails, it must have been an open file. */
#if defined(_OSK) && !defined(_UCC)
		tmpclose(ftp->fp);
#else
		if (pclose(ftp->fp) < 0)
			fclose(ftp->fp);
#endif  /* OSK */
#endif	/* SYS5 */
#else	/* UNIX */
/* Anything other than Unix */
		fclose(ftp->fp);
#endif	/* UNIX */
		ftp->fp = NULLFILE;
		close_tcp(tcb);
	}	/* if EOF */
}
/* Allocate an FTP control block */
struct ftp *
ftp_create(bufsize)
unsigned bufsize;
{
	void ftp_delete();
	register struct ftp *ftp;

	if((ftp = (struct ftp *)calloc(1,sizeof (struct ftp))) == NULLFTP)
		return NULLFTP;
	if(bufsize != 0 && (ftp->buf = malloc(bufsize)) == NULLCHAR){
#ifdef DEBUG
		printf("called by ftp_create\n");fflush(stdout);
#endif
		ftp_delete(ftp);
#ifdef DEBUG
		printf("called by ftp_create\n");fflush(stdout);
#endif
		return NULLFTP;
	}
	ftp->state = COMMAND_STATE;
	ftp->type = ASCII_TYPE;	/* Default transfer type */
	return ftp;
}
/* Free resources, delete control block */
void
ftp_delete(ftp)
register struct ftp *ftp;
{
	int i;
	void freesession();

	if(ftp->fp != NULLFILE && ftp->fp != stdout)
		fclose(ftp->fp);
	if(ftp->data != NULLTCB)
		del_tcp(ftp->data);
	if(ftp->username != NULLCHAR)
		free(ftp->username);
        for (i = 0; i < MAXPATH; i++)
	  if(ftp->path[i] != NULLCHAR)
	    free(ftp->path[i]);
	if(ftp->buf != NULLCHAR)
		free(ftp->buf);
	if(ftp->cd != NULLCHAR)
		free(ftp->cd);
	if(ftp->session != NULLSESSION)
		freesession(ftp->session);
	free((char *)ftp);
}

