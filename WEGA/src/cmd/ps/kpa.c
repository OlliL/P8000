/*
 * kpa.c: a physical address of a crash dump (ps -k) to the place in the dump file: behind the 1024 bytes of the
 * MMU table. Written from /bin/ps (WEGA).
 */

static char sccsid[] = "@[$]kpa.c\t1.1  08/25/83 09:29:20 - 87wega3.2";

long
kptop(addr)
long addr;
{
	return(addr + 1024);
}

/* lseek to the physical address addr of the dump */
long
kpseek(fd, addr)
long addr;
{
	long lseek();

	return(lseek(fd, kptop(addr), 0));
}
