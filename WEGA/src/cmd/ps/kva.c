/*
 * kva.c: a kernel virtual address of a crash dump (ps -k) to the place in the dump file, through the MMU table
 * that the dump holds in its first 1024 bytes (the segment descriptors at 256). Written from /bin/ps (WEGA).
 */

static char sccsid[] = "@[$]kva.c\t1.1  08/25/83 09:28:57 - 87wega3.2";

#include <sys/mmu.h>

static char mmubuf[1024];
static int mmuok;
#define	sd	((struct segd *)&mmubuf[256])
long	lseek();

/* read the MMU table of the dump */
getmmu(fd)
{
	register long n;

	if ((n = lseek(fd, 0L, 0)) == -1)
		return(0);
	if ((n = read(fd, mmubuf, 1024)) != 1024)
		return(0);
	mmuok++;
	return(1);
}

/* the physical address of the kernel address addr, -1 if it is not mapped */
long
kvtop(addr)
long addr;
{
	register int seg;
	register int attr;
	register long off;
	register long base;
	long lim;

	if (!mmuok)
		return(-1L);
	seg = (addr >> 24) & 0xff;
	off = addr & 0xffff;
	attr = sd[seg].sg_attr & 0xff;
	if (seg > 62 || (attr & CPUI_AT))
		return(-1L);
	base = (long)sd[seg].sg_base << 8;
	if (attr & DIRW_AT) {
		lim = (256L - (sd[seg].sg_limit & 0xff)) << 8;
		if (off < lim)
			return(-1L);
	} else {
		lim = ((long)(sd[seg].sg_limit & 0xff) + 1) << 8;
		if (off > lim)
			return(-1L);
	}
	return(base + 1024 + off);
}

/* lseek to the kernel address addr of the dump */
long
kvseek(fd, addr)
long addr;
{
	register long pa;

	if (!mmuok && getmmu(fd) == -1)
		return(-1L);
	if ((pa = kvtop(addr)) == -1)
		return(pa);
	return(lseek(fd, pa, 0));
}
