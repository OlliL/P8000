/*
 * readsym, as embedded in ranlib (older libc than the shipped libc.a).
 * Reconstructed from the object code of /usr/bin/ranlib (0x724..0x79b).
 *
 * Unlike the readsym of the shipped libc.a (gen/readsym.c) it reads the
 * continuation pieces of a long name over the symbol itself: there is no
 * "sym++" before the second fread.  The caller then sees the first piece of
 * the name text in sym, not the symbol; ranlib depends on this (its
 * __.SYMDEF lacks most long names).
 */
#include <stdio.h>
#include <s.out.h>

readsym(sym, stream)
struct s_nlist *sym;
FILE *stream;
{
	register int foo2;
	char foo1;

	if(fread(sym,14,1,stream)!=1)
		return(0xffff);

	if(!((*sym->sn_name)&128))
		return(0);

	foo1 = ((*sym->sn_name)&127)-8;
	foo2 = (foo1+13)/14;

	if(fread(sym,14,foo2,stream) != foo2)
		foo2 = 0xffff;

	return(foo2);
}
