#include <sys/param.h>
#include <sys/sysparm.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/map.h>

char mksegwstr[]="@@[$]mkseg.c@i@iRev : 4.1@i08/27/83 11:56:13";

mkseg()
{
	register unsigned segno;
	register unsigned size;
	register struct proc *pp;
	struct a {
		unsigned segno;
		unsigned size;
	} 
	*uap;
	char i, j;

	pp = u.u_procp;
	segno = ((struct a *)u.u_ap)->segno & 0x7f;
	size = ((struct a *)u.u_ap)->size;
	if (!segno){
loop:	
		i = 4;
		for (j=0; j<u.u_nsegs; j++)
			if ((u.u_segno[j]&0x7f) == i)
				if (++i > STAK){
					u.u_error = ENOSEG;
					return;
				}
				else
					goto loop;
		segno = i;
	}
	else
		for (j=0; j<u.u_nsegs; j++)
			if (segno == u.u_segno[j]){
				u.u_error = ENOSEG;
				return;
			}
	j = u.u_nsegs;
	if (u.u_tsize+u.u_dsize+u.u_ssize+(unsigned)USIZE > umemory){
		u.u_error = ENOMEM;
		return;
	}
	u.u_segno[j] = segno;
	u.u_nsegs++;
	u.u_dsize += size;
	sestabur(size, j, j, 0);
	if (u.u_error)
		return;
	shufflecore(pp->p_size+size, pp->p_size, u.u_ssize);
	u.u_r.r_val1 = segno<<8;
	u.u_r.r_val2 = 0;
}

/* sgstat is defined here; it is directly never used */

#define NSEGS	10
sgstat()
{
	cnt_t i,j;
	struct {
		unsigned segno;
		unsigned limit;
	} segstat[10];
	register paddr_t *saddr;

	saddr = (paddr_t *)u.u_ap;
	j = 0;
	if (u.u_sep)
		for (i=0; ; ){
			if (i< u.u_nsegs){
				if (u.u_segno[i] & 0x80){
					hibyte(segstat[j].segno) = u.u_segno[i] & 0x7f;
					if (u.u_segmts[i].sg_limit == 0xff)
						segstat[j].limit = 0xfffe;
					else
						segstat[j].limit = ((unsigned int)(u.u_segmts[i].sg_limit)+1)<<8;
				}
			}
			if (j++ != NSEGS)
				i++;
			else
				break;
		}
	else {
		hibyte(segstat[0].segno) = u.u_tseg;
		segstat[0].limit = u.u_toff<<8;
		j++;
	}
	for(i=j; i<NSEGS; i++){
		hibyte(segstat[i].segno) = 0xff;
		segstat[i].limit = 0;
	}
	if (copyout(segstat, *saddr, sizeof(segstat)) < 0)
		u.u_error = EFAULT;
}